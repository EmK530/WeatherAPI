#ifndef APP_STATE_H
#define APP_STATE_H

#include "city.h"
#include "constants.h"
#include "http.h"
#include "linked_list.h"
#include <stddef.h>

typedef struct {
  LinkedList *known_locations;
  cURL curl_handle;
} app_state;

/* FUNCTIONS  */

int app_seconds_to_next_api_update(cURL *_curl, city *_city);

app_state *app_create();
void app_dispose(app_state *_app);

int app_init_defaults(app_state *_app);

void app_list_cities(app_state *_app);

int app_get_weather_by_index(app_state *_app, int _index);
void app_print_current_weather(app_state *_app);
// void set_current_location(app_state *_app, int _selection);

#endif /* APP_STATE_H */