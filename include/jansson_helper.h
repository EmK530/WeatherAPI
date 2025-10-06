#ifndef JANSON_HELPER_H
#define JANSON_HELPER_H

#include "city.h"
#include "linked_list.h"

// Structure to hold current weather data
typedef struct {
  char time[20];
  int interval;
  double temperature;
  double windspeed;
  int winddirection;
  int is_day;
  int weathercode;
} current_weather;

// Check if weather cache exists and if it's stale for a city
// Returns 0 if cache exists and is fresh, 1 if it doesn't exist or is stale
int check_weather_cache(char *cityName);

// Check if JSON cache file exists for a city
// Returns 1 if exists, 0 otherwise
int does_json_exist(char *cityName);

// Read cities from a file into a linked list
// Returns 0 on success, 1 on failure
int read_cities_from_file(char *file_name, LinkedList *list);

// Write cities from a linked list to a file
// Returns 0 on success, non-zero on failure
int write_cities_to_file(char *file_name, LinkedList *list);

// Parse weather data from a JSON string
// Returns a pointer to a weather struct, or NULL on failure
weather *parse_weather_json(char *json);

// Read weather data from a cache file
// Returns the data as a string, or NULL on failure
char *read_weather_cache(char *file_name);

// Write weather data to a cache file
// Returns 0 on success, non-zero on failure
int write_weather_cache(char *file_name, char *data);

#endif