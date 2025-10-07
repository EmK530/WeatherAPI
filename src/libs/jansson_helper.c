#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#include "jansson_helper.h"
#include "city.h"
#include "jansson/jansson.h"
#include "linked_list.h"
#include <string.h>
#include <time.h>

int check_weather_cache(char *file_name) {

  /*  Finns staden lokalt? */
  json_error_t error;
  json_t *root = json_load_file(file_name, 0, &error);
  if (!root) {
    return 1;
  }
  /* todo JJ: We currently check if city exists and then we check if its data is
  old in a bunch of diffrent spots in this file. This makes it easy to
  accidentally get old and new data when querying diffrent fields. In my
  refactor i placed all the data in the linked list and the get_temp etc
  funcitons only fetch the existing data. Every function should not have to
  check. */
  /* Staden finns lokalt, är den gammal? */
  json_t *current_weather = json_object_get(root, "current_weather");
  if (!json_is_object(current_weather)) {
    json_decref(root);
    return 1;
  }

  json_t *time_val = json_object_get(current_weather, "time");
  if (!json_is_string(time_val)) {
    json_decref(root);
    return 1;
  }
  const char *time_str = json_string_value(time_val);

  struct tm tm_time = {0};
  if (strptime(time_str, "%Y-%m-%dT%H:%M", &tm_time) == 0) {
    fprintf(stderr, "Failed to parse time string: %s\n", time_str);
    json_decref(root);
    return 1;
  }

  time_t weather_time = timegm(&tm_time);
  time_t now = time(NULL);

  json_t *interval_val = json_object_get(current_weather, "interval");
  int interval = (json_is_integer(interval_val))
                     ? (int)json_integer_value(interval_val)
                     : 900;

  double diff = difftime(now, weather_time);

  if (diff > interval) {
    json_decref(root);
    return 1; /* Cache är gammal */
  }

  json_decref(root);
  return 0;
}

int read_cities_from_file(char *file_name, LinkedList *list) {

  json_error_t error;
  json_t *file = json_load_file(file_name, 0, &error);

  if (!file) {
    return 1;
  }

  if (!json_is_array(file)) {
    json_decref(file);
    return 1;
  }
  for (int i = 0; i < (int)json_array_size(file); i++) {
    json_t *item = json_array_get(file, i);
    json_t *name = json_object_get(item, "name");
    json_t *latitude = json_object_get(item, "latitude");
    json_t *longitude = json_object_get(item, "longitude");

    if (!json_is_string(name)) {
      json_decref(file);
      return 1;
    }
    if (!json_is_real(latitude)) {
      json_decref(file);
      return 1;
    }
    if (!json_is_real(longitude)) {
      json_decref(file);
      return 1;
    }

    city *new_city =
        City_create((char *)json_string_value(name), json_real_value(latitude),
                    json_real_value(longitude));

    LinkedList_append(list, new_city);
  }
  json_decref(file);
  return 0;
}

int write_cities_to_file(char *file_name, LinkedList *list) {

  json_t *city_list = json_array();

  for (int i = 0; i < (int)list->size; i++) {
    city *item = (city *)(LinkedList_get_index(list, i)->item);

    json_t *city_tuple = json_object();
    json_object_set_new(city_tuple, "name", json_string(item->name));
    json_object_set_new(city_tuple, "latitude", json_real(item->latitude));
    json_object_set_new(city_tuple, "longitude", json_real(item->longitude));

    json_array_append_new(city_list, city_tuple);
  }
  int error = json_dump_file(city_list, file_name, JSON_INDENT(4));
  json_decref(city_list);
  return error;
}

weather *parse_weather_json(char *json) {
  json_error_t error;
  json_t *weather_json = json_loads(json, 0, &error);

  if (!weather_json) {
    json_decref(weather_json);
    return NULL;
  }

  json_t *current_weather = json_object_get(weather_json, "current_weather");
  if (!current_weather) {
    json_decref(weather_json);
    return NULL;
  }
  json_t *time_val = json_object_get(current_weather, "time");
  json_t *interval_val = json_object_get(current_weather, "interval");
  json_t *temp_val = json_object_get(current_weather, "temperature");
  json_t *wind_val = json_object_get(current_weather, "windspeed");
  json_t *winddir_val = json_object_get(current_weather, "winddirection");
  json_t *isday_val = json_object_get(current_weather, "is_day");
  json_t *wcode_val = json_object_get(current_weather, "weathercode");

  weather *result = (weather *)malloc(sizeof(weather));
  if (json_is_string(time_val)) {
    strncpy(result->time, json_string_value(time_val),
            sizeof(result->time) - 1);
  }
  if (json_is_integer(interval_val)) {
    result->interval = (int)json_integer_value(interval_val);
  }
  if (json_is_number(temp_val)) {
    result->temperature = json_number_value(temp_val);
  }
  if (json_is_number(wind_val)) {
    result->windspeed = json_number_value(wind_val);
  }
  if (json_is_integer(winddir_val)) {
    result->winddirection = (int)json_integer_value(winddir_val);
  }
  if (json_is_integer(isday_val)) {
    result->is_day = (int)json_integer_value(isday_val);
  }
  if (json_is_integer(wcode_val)) {
    result->weathercode = (int)json_integer_value(wcode_val);
  }
  json_decref(weather_json);

  // check data validity?
  return result;
}

int write_weather_cache(char *file_name, char *data) {
  json_error_t error;
  json_t *weather_json = json_loads(data, 0, &error);

  if (!weather_json) {
    return -1;
  }

  json_dump_file(weather_json, file_name, JSON_INDENT(4));

  json_decref(weather_json);
  return 0;
}

char *read_weather_cache(char *file_name) {
  json_error_t error;
  json_t *weather_json = json_load_file(file_name, 0, &error);

  if (!weather_json) {
    return NULL;
  }

  char *raw_json = json_dumps(weather_json, 0);
  json_decref(weather_json);
  return raw_json;
}