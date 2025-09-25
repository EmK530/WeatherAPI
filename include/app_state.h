#ifndef APP_STATE_H
#define APP_STATE_H

#include "constants.h"
#include "linked_list.h"
#include <stddef.h>

typedef struct {
  char time[32];   /* ISO 8601 format, YYYY-MM-DDTHH:MM */
  size_t interval; /* 900 seconds = 15 min interval between measurements */
  double celsius;  /* double needed for wind chill or heat index calculations */
  double windspeed;
  float winddirection;
  char is_day; /* todo perhaps use bool? */
  int weathercode;
} weather;

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
  char default_temp_unit; /* C or F */
  char *previous_api_call;
  int exit;
  int error_code; /* 0 == OK */
  char *error_message;
} app_state;

/* FUNCTIONS  */
app_state *app_create();

void app_init_defaults(app_state *);

void set_current_location(app_state *, int);

#endif /* APP_STATE_H */