#include "curl_helper.h"
#include "essentials.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
/* JJ
Remove global state from here?
This file also have way to many funcitons IMO. WE
never want to init and not perform, and never want to perform and not
get_result etc. So it does not make sense to split it up. I would like to jsut
call something like this from main: get_current_temp( location ) or
get_forecast(location, days) or get_history(location, days) etc.
 */

CURL *curl = NULL;
CURLcode result = CURLCODE_UNSET;
struct Response resp = {0};
char curHash[SHA256_HASH_SIZE];

/* add 4th void* user pointer for using CURLOPT_WRITEDATA */
size_t write_response(void *ptr, size_t size, size_t nmemb) {
  size_t total = size * nmemb;

  /* JJ
  this either makes it impossible to recieve binary data or impossible to
  recieve string data.
  Lets have two separate functions,
  get_JSON_string_from_API and get_binary_from_API  (or something) so we can do
  both.
  */
#ifdef NULLTERM_RESPONSE
  resp.data = realloc(resp.data, resp.size + total + 1);
#else
  resp.data = realloc(resp.data, resp.size + total);
#endif
  memcpy(resp.data + resp.size, ptr, total);
  resp.size += total;

#ifdef NULLTERM_RESPONSE
  resp.data[resp.size] = '\0';
#endif
  return total;
}

int http_init() {
  curl = curl_easy_init();
  if (!curl) {
    printf("[HTTP] Could not initialize cURL!\n");
    return 1;
  }
  /* todo return 0 is success, return != 0 is error  */
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
  return 0;
}

int http_perform(const char *url) {
  if (curl == NULL) {
    printf("[HTTP] Cannot run http_perform with no cURL instance\n");
    return 1;
  }

  if (resp.data != NULL) {
    free(resp.data);
    resp.data = NULL;
    resp.size = 0;
  }

  sha256_hash(url, curHash);
  printf("[HTTP] Performing with hash: \"%s\"\n", curHash);

  curl_easy_setopt(curl, CURLOPT_URL, url);
  result = curl_easy_perform(curl);
  return 0;
}

CURLcode http_get_result() {
  if (result == CURLCODE_UNSET) {
    printf("[HTTP] No result stored to return for http_get_result\n");
    return CURLCODE_UNSET;
  }
  return result;
}

char *http_get_response() {
  if (result == CURLCODE_UNSET) {
    printf("[HTTP] No result stored to return for http_get_response\n");
    /* todo wrong return value */
    return '\0';
  }
  return resp.data;
}

void http_dispose() {
  if (curl != NULL) {
    curl_easy_cleanup(curl);
    curl = NULL;
    free(resp.data);
    resp.data = NULL;
    resp.size = 0;
  }
}