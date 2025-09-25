#ifndef WEATHER_H
#define WEATHER_H
#include "linked_list.h"

// extern const char *cities;
extern char *template;

typedef struct {
  char *name;
  float latitude;
  float longitude;
} city;

void list_cities();
int find_city(char *input, char *name, double *lat, double *lon);

city *City_create(char *_name, float _latitude, float _longitude);
void City_dispose(city *_city);
void City_parse_cities(LinkedList *_city_list, char *_city_data);

#endif