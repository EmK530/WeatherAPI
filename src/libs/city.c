#define _GNU_SOURCE
#include "city.h"
#include "constants.h"
#include "jansson_helper.h"
#include "linked_list.h"
#include <app_state.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *template = METEO_FORECAST_URL;

city *City_create(char *_name, float _latitude, float _longitude) {
  // allocate space for the city struct and zero it
  city *new_city = (city *)malloc(sizeof(city));
  memset(new_city, 0, sizeof(city));

  // initialize values
  new_city->name = strdup(_name);
  new_city->latitude = _latitude;
  new_city->longitude = _longitude;

  return new_city;
}

void City_dispose(void *_city) {
  // cast pointer to a city pointer
  city *pcity = (city *)_city;
  // free the name and weather, if any, before freeing the struct
  free(pcity->name);
  if (pcity->current_weather) {
    free(pcity->current_weather);
  }
  free(pcity);
}

void City_parse_cities(LinkedList *_city_list, const char *_city_data) {

  // The city data is modified in-place, so it is copied to prevent side-effects
  char *city_data_copy = strdup(_city_data);

  char *current = city_data_copy;
  char *name = NULL;
  char *lat_str = NULL;
  char *lon_str = NULL;

  // loop over the characters in the data string
  do {
    // name starts at the first character
    if (name == NULL) {
      name = current;
    } else if (lat_str == NULL) {
      // latitude starts after the first ':'
      if (*current == ':') {
        // change the ':' for a '\0' NULL terminator, ending the name string
        *current = '\0';
        lat_str = current + 1;
      }
      // longitude starts after the second ':'
    } else if (lon_str == NULL) {
      if (*current == ':') {
        // change the ':' for a '\0' NULL terminator, ending the latitude string
        *current = '\0';
        lon_str = current + 1;
      }
      // '\n' separates the information about each city
    } else if (*current == '\n') {
      // change the '\n' for a '\0' NULL terminator, ending the longitude string
      *current = '\0';

      // create a new city struct with the name, latitude and longitude parsed
      city *new_city = City_create(name, atof(lat_str), atof(lon_str));

      // append it to the list
      LinkedList_append(_city_list, new_city);

      // reset the variables
      name = NULL;
      lat_str = NULL;
      lon_str = NULL;
    }
    // step the current character forward
    current++;
    // stop once we reach the end of the string
  } while (*current != '\0');

  // free the copy of the city data
  free(city_data_copy);
}

int City_get_weather(cURL *_curl, city *_city) {
  // create string for the file name of the cache
  char file_name[50];
  snprintf(file_name, 49, "cache/%s_%.2f_%.2f.json", _city->name,
           _city->latitude, _city->longitude);

  weather *weather_data = NULL;

  // check cache exists and is current
  if (check_weather_cache(file_name) == 0) {
    char *data = read_weather_cache(file_name);
    if (data != NULL) {
      // parse weather data from json into weather struct
      weather_data = parse_weather_json(data);
      if (weather_data != NULL) {
        // free current weather if any
        if (_city->current_weather) {
          free(_city->current_weather);
        }
        // update the city's weather
        _city->current_weather = weather_data;
      }
      // free json string
      free(data);
      printf("\tWeather data for %s loaded from cache.\n", _city->name);
      return 0;
    }
  }
  // if cache does not exist, call API
  char url[200];
  snprintf(url, 199, template, _city->latitude, _city->longitude);
  curl_perform(_curl, url);

  // check curl error code
  CURLcode result = curl_get_result(_curl);
  if (result == CURLE_OK) {
    // parse weather data from json into weather struct
    weather_data = parse_weather_json(_curl->data);
    if (weather_data != NULL) {
      // update the cache with new data
      write_weather_cache(file_name, _curl->data);
    }
  } else {
    // return -1 if curl failed
    return -1;
  }
  if (weather_data != NULL) {
    // free current weather if any
    if (_city->current_weather) {
      free(_city->current_weather);
    }
    // update the city's weather
    _city->current_weather = weather_data;
  }
  printf("\tWeather data for %s loaded from API.\n", _city->name);
  return 0;
}