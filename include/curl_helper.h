#pragma once

#include <curl/curl.h>

#define CURLCODE_UNSET ((CURLcode)0xFFFFFFFF)

typedef struct
{
  CURL* curlInternal;
  CURLcode result;
  size_t size;
  char* data;
} cURL;

int curl_init(cURL* curl);
int curl_perform(cURL* curl, const char *url);
CURLcode curl_get_result(cURL* curl);
void curl_get_response(cURL* curl, char** data, size_t* size);
void curl_dispose(cURL* curl);