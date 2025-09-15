#include "curl_helper.h"
#include "weather.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  if (!http_init()) {
    printf("Failed to initialize cURL!\n");
    return 1;
  }

  printf("Välkommen!\n\n");

  list_cities();

  char input[16];

  char name[16];
  double lat, lon;
  while (1) {
    printf("\nSkriv in en stad att kolla vädret på: ");
    scanf("%15s", input);

    int found = find_city(input, name, &lat, &lon);
    if (found == 0) {
      printf("Felaktig stad!\n");
    } else {
      char url[100];
      sprintf(url, template, lat, lon);
      printf("URL: \"%s\"\n", url);

      http_perform(url);
      CURLcode result = http_get_result();
      if (result == CURLE_OK) {
        char *resp = http_get_response();
        printf("\n%s\n", resp);
      } else {
        printf("\nKunde inte ladda vädret!\n");
      }
    }
  }

  /* todo: this is unreachable, add a way to exit */
  http_dispose();
  return 0;
}