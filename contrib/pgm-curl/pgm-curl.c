#include <string.h>
#include <stdio.h>
#include <postgres.h>
#include <fmgr.h>
#include <curl/curl.h>
#include "utils/builtins.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(pgm_curl_c);
PG_FUNCTION_INFO_V1(pgm_curl_post);

typedef struct
{
    char *data;
    size_t size;
} Response;

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    Response *res = (Response *)userdata;
    size_t new_size = res->size + size * nmemb;
    res->data = (char *)malloc(new_size + 1);
    memcpy(res->data + res->size, ptr, size * nmemb);
    res->size = new_size;
    res->data[new_size] = '\0';
    return size * nmemb;
}

Datum pgm_curl_c(PG_FUNCTION_ARGS)
{
    text *url = PG_GETARG_TEXT_P(0);
    CURL *curl = curl_easy_init();
    CURLcode res;
    Response response;
    response.data = NULL;
    response.size = 0;
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, text_to_cstring(url));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res != CURLE_OK)
            ereport(ERROR, (errmsg("cURL Error: %s", curl_easy_strerror(res))));
    }
    else
    {
        ereport(ERROR, (errmsg("Failed to initialize cURL")));
    }
    PG_RETURN_TEXT_P(cstring_to_text(response.data));
}

Datum pgm_curl_post(PG_FUNCTION_ARGS)
{
    // if (PG_ARGISNULL(0) || PG_ARGISNULL(1) || PG_ARGISNULL(2))
    //     ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("NULL values not allowed")));

    text *url = PG_GETARG_TEXT_P(0);
    text *token = PG_GETARG_TEXT_P(1);

    char auth_header[1024];
    char *auth_token = text_to_cstring(token);
    long response_code = 0;
    struct curl_slist *headers = NULL;
    CURL *curl = curl_easy_init();
    CURLcode res;
    Response response;
    response.data = NULL;
    response.size = 0;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, text_to_cstring(url));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, text_to_cstring(PG_GETARG_TEXT_P(2)));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        headers = curl_slist_append(headers, "Content-Type: application/json");
        snprintf(auth_header, sizeof(auth_header), "x-functions-key: %s", auth_token);
        headers = curl_slist_append(headers, auth_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK)
        {
            ereport(ERROR, (errmsg("cURL Error: %s", curl_easy_strerror(res))));
        }
        else
        {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            if (!(response_code >= 200 && response_code <= 205))
            {
                ereport(ERROR, (errmsg("Call returned error with response code %ld. Please chech your azure function logs.", response_code)));
            }
        }
    }
    else
    {
        ereport(ERROR, (errmsg("Failed to initialize cURL")));
    }
    PG_RETURN_TEXT_P(cstring_to_text(response.data));
}
