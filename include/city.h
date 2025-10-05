#ifndef CITY_H
#define CITY_H
#include "curl_helper.h"
#include "linked_list.h"

extern char *template;
/* todo JJ måste betämma om vi ska använda city eller appstates location
 * struct */
typedef struct {
  char time[32];   /* ISO 8601 format, YYYY-MM-DDTHH:MM */
  size_t interval; /* 900 seconds = 15 min interval between measurements */
  double
      temperature; /* double needed for wind chill or heat index calculations */
  double windspeed;
  float winddirection;
  char is_day;
  int weathercode;
} weather;

typedef struct {
  char *name;
  float latitude;
  float longitude;
  size_t timestamp_prev_api_call;
} city;

void list_cities();
int find_city(char *input, char *name, double *lat, double *lon);

city *City_create(char *_name, float _latitude, float _longitude);
void City_dispose(void *_city);
void City_parse_cities(LinkedList *_city_list, const char *_city_data);

weather *City_get_weather(cURL *_curl, city *_city);

#endif