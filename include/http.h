#pragma once

#include <curl/curl.h>
#include <time.h>

#define CURLCODE_UNSET ((CURLcode)0xFFFFFFFF)

/**
 * @struct cURL
 * @brief Struct for storing information used by http.c functions
 *
 * curlInternal: the CURL struct used by curl
 * result: the curl code from the most recent curl operation
 * call_cooldown: cooldown timer to prevent excess API-calls
 * size: the size of the data
 * data: the data recieved from the most recent API-call
 */
typedef struct {
  CURL *curlInternal;
  CURLcode result;
  time_t call_cooldown;
  size_t size;
  char *data;
} cURL;

/**
 * @brief Initialize a curl struct
 *
 * @param curl the curl struct to initialize
 * @return error code
 */
int curl_init(cURL *curl);

/**
 * @brief Perform a curl request to the given url
 *
 * Perform a curl request to the given url, storing the resulting data and error
 * code in the curl struct. If a request was made too recently the function will
 * first sleep until the cooldown runs out.
 *
 * @param curl the curl struct with which to make the request
 * @param url the url to make the curl request to
 * @return error code
 */
int curl_perform(cURL *curl, const char *url);

/**
 * @brief Get the error code from a given curl struct
 *
 * @param curl the curl struct
 * @return The error code from the curl struct
 */
CURLcode curl_get_result(cURL *curl);

/**
 * @brief Get the response data from a given curl struct
 *
 * @param curl the curl struct
 * @param data location to store the data string
 * @param size location to store the size
 */
void curl_get_response(cURL *curl, char **data, size_t *size);

/**
 * @brief Dispose a give curl struct
 *
 * Free the space used by a given curl struct
 *
 * @param curl the curl struct to be disposed
 */
void curl_dispose(cURL *curl);
