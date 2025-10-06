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

  char input[128];
  long selection;

  do {
    app_list_cities(app);

    scanf("%127s", input);
    selection = strtol(input, NULL, 10);
    if (app_get_weather_by_index(app, selection - 1) >= 0) {
      app_print_current_weather(app);
    }

  } while (selection > 0);

  app_dispose(app);
  return 0;
}