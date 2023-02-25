#include <string.h>
#include <stdio.h>
#include <postgres.h>
#include <fmgr.h>
#include <curl/curl.h>
#include "utils/builtins.h"
#include "utils/memutils.h"
#include "access/htup_details.h"
#include "executor/spi.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(pgm_curl_post);

typedef struct
{
    char *memory;
    size_t size;
} MemoryStruct;

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    mem->memory = MemoryContextAlloc(TopMemoryContext, mem->size + realsize + 1);
    if (mem->memory == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

Datum pgm_curl_post(PG_FUNCTION_ARGS)
{
    int ret;
    char *url, *token;
    text *url_text, *token_text;
    text *function_alias = PG_GETARG_TEXT_P(0);
    long response_code = 0;
    CURL *curl;
    CURLcode res;
    MemoryStruct chunk;
    char header[1024], query[1024];
    struct curl_slist *headers = NULL;
    text *response;

    ret = SPI_connect();
    if (ret != SPI_OK_CONNECT)
        elog(ERROR, "SPI_connect failed: error code %d", ret);

    snprintf(query, sizeof(query), "SELECT uri, auth_token FROM azure_functions WHERE function_alias = %s", function_alias);
    ret = SPI_execute(query, true, 0);
    if (ret != SPI_OK_SELECT)
        elog(ERROR, "SPI_execute failed: error code %d", ret);

    if (SPI_processed > 0)
    {
        HeapTuple tuple = SPI_tuptable->vals[0];
        url_text = DatumGetTextP(heap_getattr(tuple, SPI_tuptable->tupdesc, 1, &ret));
        token_text = DatumGetTextP(heap_getattr(tuple, SPI_tuptable->tupdesc, 2, &ret));
        url = text_to_cstring(url_text);
        token = text_to_cstring(token_text);
    }
    else
    {
        elog(ERROR, "No API credentials found in the database");
        PG_RETURN_NULL();
    }

    SPI_finish();

    chunk.memory = MemoryContextAlloc(TopMemoryContext, 1);
    chunk.size = 0;

    // curl_global_init(CURL_GLOBAL_ALL);
    /* init the curl session */
    curl = curl_easy_init();

    if (curl)
    {
        /* specify URL to get */
        curl_easy_setopt(curl, CURLOPT_URL, url);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, text_to_cstring(PG_GETARG_TEXT_P(1)));

        /* send all data to this function */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        headers = curl_slist_append(headers, "Content-Type: application/json");
        snprintf(header, sizeof(header), "x-functions-key: %s", token);
        headers = curl_slist_append(headers, header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        /* get it! */
        res = curl_easy_perform(curl);
        /* check for errors */
        if (res != CURLE_OK)
        {
            elog(ERROR, "curl_easy_perform() failed: %s\n",
                 curl_easy_strerror(res));
            PG_RETURN_NULL();
        }
        else
        {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            if (!(response_code >= 200 && response_code <= 205))
            {
                ereport(ERROR, (errmsg("Call returned error with response code %ld. Please chech your azure function logs.", response_code)));
            }
        }
        /* cleanup curl stuff */
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    response = cstring_to_text(chunk.memory);
    MemoryContextFree(chunk.memory);
    PG_RETURN_TEXT_P(response);
}
