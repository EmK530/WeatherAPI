#ifndef JANSON_HELPER_H
#define JANSON_HELPER_H

#include "city.h"
#include "linked_list.h"

int check_weather_cache(char *cityName);
void print_temperature(char *cityName);

typedef struct {
  char time[20]; /* todo how long can a ISO 8601 time string be? 20 might be to
                    small */
  int interval;
  double temperature;
  double windspeed;
  int winddirection;
  int is_day;
  int weathercode;
} current_weather;

int does_json_exist(char *cityName);
int is_weather_old(char *cityName);
// int print_weather(char *cityName);
current_weather get_weather();
int read_cities_from_file(char *file_name, LinkedList *list);
int write_cities_to_file(char *file_name, LinkedList *list);
int write_cache(char *file_name, char *json);
int read_cache(char *file_anem, char *buffer);
weather *parse_weather_json(char *json);
int write_weather_cache(char *file_name, char *data);
char *read_weather_cache(char *file_name);

#endif