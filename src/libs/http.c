#include "http.h"
#include "constants.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Internal function
// Callback used to write data from curl response into curl struct
size_t write_response(void *response, size_t size, size_t nmemb, void *userp) {
  // calculate size of the data to write
  size_t total = size * nmemb;
  cURL *curl = (cURL *)userp;

  // allocate space for data, or reallocate if some data already exists
  char *ptr = NULL;
  int totalSize = curl->size + total + 1;
  if (curl->data == NULL) {
    ptr = (char *)malloc(totalSize);
  } else {
    ptr = realloc(curl->data, totalSize);
  }

  if (ptr == NULL) {
    return 0; /* Out of memory protection */
  }

  // update curl variables
  curl->data = ptr;
  memcpy(&(curl->data[curl->size]), response, total);
  curl->size += total;
  curl->data[curl->size] = 0;

  return total;
}

int curl_init(cURL *curl) {
  // zero curl struct
  memset(curl, 0, sizeof(cURL));
  // initialize curlInternal
  curl->curlInternal = curl_easy_init();
  if (curl->curlInternal == NULL) {
    printf("[HTTP] Could not initialize cURL!\n");
    return 0; /* todo flipped return values */
  }
  // set the write callback function for the curl internal and the write
  // destination. This makes the curl internal use our write_response function
  // instead of printing the response data to stdout
  curl_easy_setopt(curl->curlInternal, CURLOPT_WRITEFUNCTION, write_response);
  curl_easy_setopt(curl->curlInternal, CURLOPT_WRITEDATA, (void *)curl);

  /* reset members */
  curl->call_cooldown = time(NULL);
  curl->call_cooldown = 0;
  curl->size = 0;
  curl->data = NULL;

  return 1;
}

int curl_perform(cURL *curl, const char *url) {
  if (curl == NULL || curl->curlInternal == NULL) {
    printf("[HTTP] Cannot run http_perform with invalid cURL instance\n");
    return 0;
  }

  // free and reset previous data and size, if any
  if (curl->data != NULL) {
    free(curl->data);
    curl->data = NULL;
    curl->size = 0;
  }

  // set the url to be used
  curl_easy_setopt(curl->curlInternal, CURLOPT_URL, url);

  /* dont call more often than allowed */
  if (time(NULL) - curl->call_cooldown < MIN_SECONDS_BETWEEN_CALLS) {
    size_t wait =
        MIN_SECONDS_BETWEEN_CALLS - (time(NULL) - curl->call_cooldown);
    printf("Cooldown active. Waiting for %zu seconds...\n", wait);
    sleep(wait);
  }

  // perform the curl request and update cooldown timer
  curl->result = curl_easy_perform(curl->curlInternal);
  curl->call_cooldown = time(NULL);
  return 1;
}

// TODO: do we need this function?
CURLcode curl_get_result(cURL *curl) {
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

// TODO: do we need this function?
void curl_get_response(cURL *curl, char **data, size_t *size) {
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

void curl_dispose(cURL *curl) {
  if (curl != NULL) {
    // free and NULL curl internal
    if (curl->curlInternal != NULL) {
      curl_easy_cleanup(curl->curlInternal);
      curl->curlInternal = NULL;
    }
    // free and NULL data, set size to 0
    if (curl->data != NULL) {
      free(curl->data);
      curl->data = NULL;
      curl->size = 0;
    }
  }
}