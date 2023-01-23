// #include <string.h>
// #include <stdio.h>
// #include <postgres.h>
// #include <fmgr.h>
// #include <curl/curl.h>
// #include "utils/builtins.h"

// PG_MODULE_MAGIC;

// PG_FUNCTION_INFO_V1(send_request);

// typedef struct {
//     char *data;
//     size_t size;
// } Response;

// static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
//     Response *res = (Response *)userdata;
//     size_t new_size = res->size + size * nmemb;
//     res->data = (char *)repalloc(res->data, new_size + 1);
//     memcpy(res->data + res->size, ptr, size * nmemb);
//     res->size = new_size;
//     res->data[new_size] = '\0';
//     return size * nmemb;
// }

// Datum
// send_request(PG_FUNCTION_ARGS) {
//     CURL *curl;
//     CURLcode res;
//     Response response;
//     response.data = NULL;
//     response.size = 0;
    
//     curl = curl_easy_init();
//     if(curl) {
//         curl_easy_setopt(curl, CURLOPT_URL, "https://pgm-test1.azurewebsites.net/api/httpexample?name=Meher");
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
//         res = curl_easy_perform(curl);
//         curl_easy_cleanup(curl);
//         if(res != CURLE_OK)
//             ereport(ERROR, (errmsg("cURL Error: %s", curl_easy_strerror(res))));
//     } else {
//         ereport(ERROR, (errmsg("Failed to initialize cURL")));
//     }
//     PG_RETURN_TEXT_P(cstring_to_text(response.data));
// }