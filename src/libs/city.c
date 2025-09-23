#define _GNU_SOURCE
#include "city.h"
#include "linked_list.h"
#include <app_state.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *cities = "Stockholm:59.3293:18.0686\n"
                     "Göteborg:57.7089:11.9746\n"
                     "Malmö:55.6050:13.0038\n"
                     "Uppsala:59.8586:17.6389\n"
                     "Västerås:59.6099:16.5448\n"
                     "Örebro:59.2741:15.2066\n"
                     "Linköping:58.4109:15.6216\n"
                     "Helsingborg:56.0465:12.6945\n"
                     "Jönköping:57.7815:14.1562\n"
                     "Norrköping:58.5877:16.1924\n"
                     "Lund:55.7047:13.1910\n"
                     "Gävle:60.6749:17.1413\n"
                     "Sundsvall:62.3908:17.3069\n"
                     "Umeå:63.8258:20.2630\n"
                     "Luleå:65.5848:22.1567\n"
                     "Kiruna:67.8558:20.2253\n";
char *template = "https://api.open-meteo.com/v1/"
                 "forecast?latitude=%lf&longitude=%lf&current_weather=true";

/* JJ
These two only work for the test data. We need to build a parser that gets all
the data into RAM so we only read from disk at startup.
*/

// TODO: void list_cities(linked_list* _cities)
void list_cities() {
  const char *scan1 = cities;
  /* todo 16 is to small for many location names */
  char name[16];
  while (*scan1 != '\0') {
    sscanf(scan1, "%16[^:]", name);
    printf("%s\n", name);
    scan1 = strchr(scan1, '\n');
    if (scan1 == NULL) {
      break;
    } else {
      scan1++;
    }
  }
}

// TODO: city* get_city(linked_list* _cities, char* name)
int find_city(char *input, char *name, double *lat, double *lon) {
  const char *scan1 = cities;
  while (*scan1 != '\0') {
    sscanf(scan1, "%16[^:]:%lf:%lf", name, lat, lon);
    /* todo write own strcmp that is not case sensitive */
    if (strcmp(name, input) == 0) {
      /* todo 0 is success, 1 is error */
      return 1;
    }
    scan1 = strchr(scan1, '\n');
    if (scan1 != NULL) {
      scan1++;
    } else {
      return 0;
    }
  }
  return 0;
}

city *City_create(char *_name, float _latitude, float _longitude) {
  city *new_city = (city *)malloc(sizeof(city));

  new_city->name = strdup(_name);
  new_city->latitude = _latitude;
  new_city->longitude = _longitude;

  return new_city;
}

void City_dispose(void *_city) {
  city *pcity = (city *)_city;
  free(pcity->name);
  free(pcity);
}

void City_parse_cities(LinkedList *_city_list, char *_city_data) {

  char *city_data_copy = strdup(_city_data);

  char *current = city_data_copy;
  char *name = NULL;
  char *lat_str = NULL;
  char *lon_str = NULL;

  do {
    if (name == NULL) {
      name = current;
    } else if (lat_str == NULL) {
      if (*current == ':') {
        *current = '\0';
        lat_str = current + 1;
      }
    } else if (lon_str == NULL) {
      if (*current == ':') {
        *current = '\0';
        lon_str = current + 1;
      }
    } else if (*current == '\n') {
      *current = '\0';

      city *new_city = City_create(name, atof(lat_str), atof(lon_str));

      LinkedList_append(_city_list, new_city);

      name = NULL;
      lat_str = NULL;
      lon_str = NULL;
    }
    current++;
  } while (*current != '\0');

  free(city_data_copy);
}