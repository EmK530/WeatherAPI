#include "app_state.h"
#include "city.h"
#include "jansson_helper.h"
#include "libs/jansson/jansson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
  app_state *app = app_create();
  app_init_defaults(app);

  printf("Välkommen!\n\n");

  char input[16];
  int selection;

  do {
    app_list_cities(app);
    scanf("%15s", input);
    selection = atoi(input);
    if (app_get_weather_by_index(app, selection - 1) >= 0) {
      app_print_current_weather(app);
    }

  } while (selection > 0);

  app_dispose(app);
  return 0;

  list_cities();

  char name[16];
  double lat, lon;

  while (1) {
    printf(
        "\nSkriv in en stad att kolla vädret på (\"exit\" för att avsluta): ");
    scanf("%15s", input);

    if (strcmp(input, "exit") == 0) {
      break;
    }

    int found = find_city(input, name, &lat, &lon);

    if (found == 0) {
      printf("Felaktig stad!\n");
    } else {
      if (check_weather_cache(name) == 0) /* If cache is loaded */
      {
        print_temperature(name);
        continue;
      }

      char url[100];
      sprintf(url, template, lat, lon);
      printf("URL: \"%s\"\n", url);

      // curl_perform(&curl, url);
      // CURLcode result = curl_get_result(&curl);
      /*
      if (result == CURLE_OK) {
        if (curl.data != NULL) {
          printf("%s\n", curl.data);

          json_error_t error;
          json_t *root = json_loads(curl.data, 0, &error);

          if (!root) {
            fprintf(stderr, "JSON parsing error: %s (line %d, column %d)\n",
                    error.text, error.line, error.column);
          } else {
            char cityFile[55];
            snprintf(cityFile, sizeof(cityFile), "cache/%s.json", name);
            if (json_dump_file(root, cityFile, JSON_INDENT(4)) != 0) {
              fprintf(stderr, "Error writing JSON to file\n");
            } else {
              printf("JSON sparad till %s\n", cityFile);
              print_temperature(name);
            }
            json_decref(root);
          }

        } else {
          printf("\nKunde inte ladda data från cURL!\n");
        }
      } else {
        printf("\nKunde inte ladda vädret!\n");
      }
      */
    }
  }

  /* todo fix cleanup since we now get here is user selects 0
   curl_dispose(app->curl_handle);
   */

  return 0;
}