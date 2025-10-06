#define _GNU_SOURCE
#include "city.h"
#include "jansson_helper.h"
#include "linked_list.h"
#include "constants.h"
#include <app_state.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *template = METEO_FORECAST_URL;

city *City_create(char *_name, float _latitude, float _longitude) {
  city *new_city = (city *)malloc(sizeof(city));

  new_city->name = strdup(_name);
  new_city->latitude = _latitude;
  new_city->longitude = _longitude;
  new_city->current_weather = NULL;

  return new_city;
}

void City_dispose(void *_city) {
  city *pcity = (city *)_city;
  free(pcity->name);
  if (pcity->current_weather) {
    free(pcity->current_weather);
  }
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

int City_get_weather(cURL *_curl, city *_city) {
  char file_name[50];
  snprintf(file_name, 49, "cache/%s_%.2f_%.2f.json", _city->name,
           _city->latitude, _city->longitude);

  weather *weather_data = NULL;

  /* check cache */
  if (check_weather_cache(file_name) == 0) {
    char *data = read_weather_cache(file_name);
    if (data != NULL) {
      weather_data = parse_weather_json(data);
      if (weather_data != NULL) {
        if (_city->current_weather) {
          free(_city->current_weather);
        }
        _city->current_weather = weather_data;
      }
      free(data);
      printf("\tWeather data for %s loaded from cache.\n", _city->name);
      return 0;
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
  } else {
    return -1;
  }
  if (weather_data != NULL) {
    if (_city->current_weather) {
      free(_city->current_weather);
    }
    _city->current_weather = weather_data;
  }
  printf("\tWeather data for %s loaded from API.\n", _city->name);
  return 0;
}