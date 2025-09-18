#include "curl_helper.h"
#include "weather.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  /*
  suggestions (JJ):
  - lets place all global state here
  - i think we must call curl_global_init here, right now its called implicitly
  by curl_easy_init at every API call. That will cause errors if we multithread.
  And its supposed to be called only once per program run.
  */

  cURL curl;
  if (!curl_init(&curl)) {
    return 1; // error print is handled in curl_init
  }

  printf("Välkommen!\n\n");

  list_cities();

  /* JJ Longest place name in the world is
   * "Taumata­whakatangihanga­koauau­o­tamatea­turi­pukaka­piki­maunga­horo­nuku­pokai­whenua­ki­tana­tahu"
   * (85 letters) */
  char input[16];

  /* JJ lets make currently selected location a global object, like a location
   * struct or something. We will need to reference it a lot. */
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

      curl_perform(&curl, url);
      CURLcode result = curl_get_result(&curl);
      if (result == CURLE_OK) {
        char* resp = NULL;
        size_t size = 0;
        curl_get_response(&curl, &resp, &size);
        if(resp != NULL)
        {
          printf("\n%s\n", resp);
        } else {
          printf("\nKunde inte ladda data från cURL!\n");
        }
      } else {
        printf("\nKunde inte ladda vädret!\n");
      }
    }
  }

  /* todo: this is unreachable, add a way to exit */
  curl_dispose(&curl);
  return 0;
}