#pragma once

#include <curl/curl.h>

#define CURLCODE_UNSET ((CURLcode)0xFFFFFFFF)
#define NULLTERM_RESPONSE 1

struct Response {
    char* data;
    size_t size;
};

int http_init();
int http_perform(const char* url);
CURLcode http_get_result();
char* http_get_response();
void http_dispose();