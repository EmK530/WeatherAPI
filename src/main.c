#include "curl_helper.h"
#include "weather.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    return 1; /* error print is handled in curl_init */
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
      /* Check local cache first */
      char cityFile[55];
      snprintf(cityFile, sizeof(cityFile), "cache/%s.json", input);
      /*  Finns staden lokalt? */
      json_error_t error;
      json_t *root = json_load_file(cityFile, 0, &error);
      if (!root) { /* Staden finns inte lokalt */
        fprintf(stderr, "Error loading JSON: %s (line %d, col %d)\n",
                error.text, error.line, error.column);
      } else { /* Staden finns lokalt */
        json_t *current_weather = json_object_get(root, "current_weather");
        if (!json_is_object(current_weather)) {
          json_decref(root);
        }

        json_t *time_val = json_object_get(current_weather, "time");
        if (!json_is_string(time_val)) {
          json_decref(root);
        }
        const char *time_str = json_string_value(time_val);

        struct tm tm_time = {0};
        if (strptime(time_str, "%Y-%m-%dT%H:%M", &tm_time) == 0) {
          fprintf(stderr, "Failed to parse time string: %s\n", time_str);
          json_decref(root);
        }

        time_t weather_time = timegm(&tm_time);
        time_t now = time(NULL);

        json_t *interval_val = json_object_get(current_weather, "interval");
        int interval = (json_is_integer(interval_val))
                           ? (int)json_integer_value(interval_val)
                           : 900;

        double diff = difftime(now, weather_time);

        /* // Är vädret gammalt? */
        if (diff > interval) { /* // Vädret är gammalt */
          printf("Vädret är gammalt. Hämtar nytt från meteo...\n");
          json_decref(root);
        } else { /* Vädret är inte gammalt */
          json_t *temp_val = json_object_get(current_weather, "temperature");
          json_t *wind_val = json_object_get(current_weather, "windspeed");

          if (json_is_number(temp_val) && json_is_number(wind_val)) {
            double temp = json_number_value(temp_val);
            double wind = json_number_value(wind_val);

            printf("Vädret är inte gammalt!\n");
            printf("  Temperatur: %.1f °C\n", temp);
            printf("  Vind: %.1f km/h\n", wind);
          }
          json_decref(root);
          continue; /* jump to next main loop iteration if we find up to date
                       weather in the cache */
        }
      }

      char url[100];
      sprintf(url, template, lat, lon);
      printf("URL: \"%s\"\n", url);

      curl_perform(&curl, url);
      CURLcode result = curl_get_result(&curl);
      if (result == CURLE_OK) {
        if (curl.data != NULL) {
          printf("%s\n", curl.data);

          json_error_t error;
          json_t *root = json_loads(curl.data, 0, &error);

          if (!root) {
            fprintf(stderr, "JSON parsing error: %s (line %d, column %d)\n",
                    error.text, error.line, error.column);
          } else {
            if (json_dump_file(root, cityFile, JSON_INDENT(4)) != 0) {
              fprintf(stderr, "Error writing JSON to file\n");
            } else {
              printf("JSON sparad till %s\n", cityFile);
            }
            json_decref(root);
          }

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