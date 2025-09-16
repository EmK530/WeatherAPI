#ifndef APP_STATE_H
#define APP_STATE_H

#include "constants.h"
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
  double latitude;
  double longitude;
  int timezone_offset;
  weather current_weather;
} location;

typedef struct {
  location current_location;
  location *known_locations;
  char default_temp_unit; /* C or F */
  char *previous_api_call;
  int error_code; /* 0 == OK */
} app_state;

void app_state_init_defaults(app_state *);

#endif /* APP_STATE_H */