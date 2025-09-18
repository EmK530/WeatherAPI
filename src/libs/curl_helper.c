#include "curl_helper.h"
#include "essentials.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

/* add 4th void* user pointer for using CURLOPT_WRITEDATA */
size_t write_response(void *response, size_t size, size_t nmemb, void* userp) {
  size_t total = size * nmemb;
  cURL* curl = (cURL*)userp;

  char* ptr = NULL;
  int totalSize = curl->size + total + 1;
  if(curl->data == NULL)
  {
    ptr = (char*)malloc(totalSize);
  } else {
    ptr = realloc(curl->data, totalSize);
  }

  if(ptr == NULL)
  {
    return 0; /* Out of memory protection */
  }

  curl->data = ptr;
  memcpy(&(curl->data[curl->size]), response, total);
  curl->size += total;
  curl->data[curl->size] = 0;

  return total;
}

int curl_init(cURL* curl) {
  memset(curl, 0, sizeof(cURL));
  curl->curlInternal = curl_easy_init();
  if (curl->curlInternal == NULL) {
    printf("[HTTP] Could not initialize cURL!\n");
    return 0;
  }
  curl_easy_setopt(curl->curlInternal, CURLOPT_WRITEFUNCTION, write_response);
  curl_easy_setopt(curl->curlInternal, CURLOPT_WRITEDATA, (void*)curl);
  return 1;
}

int curl_perform(cURL* curl, const char *url) {
  if (curl == NULL || curl->curlInternal == NULL) {
    printf("[HTTP] Cannot run http_perform with invalid cURL instance\n");
    return 0;
  }

  if (curl->data != NULL) {
    free(curl->data);
    curl->data = NULL;
    curl->size = 0;
  }

  char curHash[SHA256_HASH_SIZE];
  sha256_hash(url, curHash);
  printf("[HTTP] Performing with hash: \"%s\"\n", curHash);

  curl_easy_setopt(curl->curlInternal, CURLOPT_URL, url);
  curl->result = curl_easy_perform(curl->curlInternal);
  return 1;
}

CURLcode curl_get_result(cURL* curl) {
  if (curl == NULL || curl->curlInternal == NULL) {
    printf("[HTTP] Cannot run http_get_result with invalid cURL instance\n");
    return 0;
  }
  if (curl->result == CURLCODE_UNSET) {
    printf("[HTTP] No result stored to return for http_get_result\n");
    return CURLCODE_UNSET;
  }
  return curl->result;
}

void curl_get_response(cURL* curl, char** data, size_t* size) {
  if (curl == NULL || curl->curlInternal == NULL) {
    printf("[HTTP] Cannot run http_get_response with invalid cURL instance\n");
    return;
  }
  if (curl->result == CURLCODE_UNSET) {
    printf("[HTTP] No result stored to return for http_get_response\n");
    return;
  }
  *data = curl->data;
  *size = curl->size;
}

void curl_dispose(cURL* curl) {
  if (curl != NULL) {
    if(curl->curlInternal != NULL)
    {
      curl_easy_cleanup(curl);
      curl = NULL;
    }
    if(curl->data != NULL)
    {
      free(curl->data);
      curl->data = NULL;
      curl->size = 0;
    }
  }
}