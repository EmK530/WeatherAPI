#define _GNU_SOURCE
#include "city.h"
#include "jansson_helper.h"
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

void list_cities() {
  const char *scan1 = cities;
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

int find_city(char *input, char *name, double *lat, double *lon) {
  const char *scan1 = cities;
  while (*scan1 != '\0') {
    sscanf(scan1, "%16[^:]:%lf:%lf", name, lat, lon);
    if (strcmp(name, input) == 0) {
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

void City_parse_cities(LinkedList *_city_list, const char *_city_data) {

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

weather *City_get_weather(cURL *_curl, city *_city) {
  char file_name[50];
  snprintf(file_name, 49, "cache/%s_%.2f_%.2f.json", _city->name,
           _city->latitude, _city->longitude);

  weather *weather_data = NULL;

  /* todo jj: This works on my other project but here i dont really know how we
   * save timestamps per city so i just leave this in a comment.
   It also needs to be integrated as a check in the check cache logic below.
   */
  /*   size_t seconds_to_next_api_update =
        app_seconds_to_next_api_update(_curl, _city);
   */

  /* todo we cant return from here because cache might exist but be stale */
  /* check cache */
  if (check_weather_cache(file_name) == 0) {
    char *data = read_weather_cache(file_name);
    if (data != NULL) {
      weather_data = parse_weather_json(data);
      free(data);
      if (weather_data != NULL) {

        return weather_data;
      }
    }
  }

  /* if cache does not exist ( todo or is stale) call API */
  char url[200];
  snprintf(url, 199, template, _city->latitude, _city->longitude);
  curl_perform(_curl, url);
  CURLcode result = curl_get_result(_curl);
  if (result == CURLE_OK) {
    weather_data = parse_weather_json(_curl->data);
    if (weather_data != NULL) {
      write_weather_cache(file_name, _curl->data);
    }
  }
  return weather_data;
}