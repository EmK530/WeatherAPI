#ifndef APP_STATE_H
#define APP_STATE_H

#include "city.h"
#include "constants.h"
#include "curl_helper.h"
#include "linked_list.h"
#include <stddef.h>

typedef struct {
  char name[MAX_LOCATION_NAME_LENGTH];
  size_t id;
  double latitude;
  double longitude;
  int timezone_offset;
  weather current_weather;
} location;

typedef struct {
  location *locations;
  size_t number_of_locations;
} location_list;

typedef struct {
  location current_location;
  LinkedList *known_locations;
  cURL curl_handle;
  char default_temp_unit; /* C or F */
  char *api_result;
  int exit;
  int error_code; /* 0 == OK */
  char *error_message;
} app_state;

/* FUNCTIONS  */
app_state *app_create();
void app_dispose(app_state *_app);

int app_init_defaults(app_state *);

void app_list_cities(app_state *);

int app_get_weather_by_index(app_state *_app, int _index);
// void set_current_location(app_state *, int);

#endif /* APP_STATE_H */