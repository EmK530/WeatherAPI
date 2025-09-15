#include "curl_helper.h"
#include "essentials.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

CURL *curl = NULL;
CURLcode result = CURLCODE_UNSET;
struct Response resp = {0};
char curHash[SHA256_HASH_SIZE];
/* add 4th void* user pointer for using CURLOPT_WRITEDATA */
size_t write_response(void *ptr, size_t size, size_t nmemb) {
  size_t total = size * nmemb;

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
    return 0;
  }
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
  return 1;
}

int http_perform(const char *url) {
  if (curl == NULL) {
    printf("[HTTP] Cannot run http_perform with no cURL instance\n");
    return 0;
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
  return 1;
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