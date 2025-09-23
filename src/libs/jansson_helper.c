#include "jansson/jansson.h"
#include <time.h>

int check_weather_cache(char *cityName) {
  /* Check local cache first */
  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  /*  Finns staden lokalt? */
  json_error_t error;
  json_t *root = json_load_file(cityFile, 0, &error);
  if (!root) {
    fprintf(stderr, "Error loading JSON: %s (line %d, col %d)\n", error.text,
            error.line, error.column);
    return 1; /* Staden finns inte lokalt */
  }

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

void print_temperature(char *cityName) {
  /* Check local cache first */
  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  /*  Finns staden lokalt? */
  json_error_t error;
  json_t *root = json_load_file(cityFile, 0, &error);
  if (!root) {
    fprintf(stderr, "Error loading JSON: %s (line %d, col %d)\n", error.text,
            error.line, error.column);
    return; /* Staden finns inte lokalt */
  }

  /* Staden finns lokalt, är den gammal? */
  json_t *current_weather = json_object_get(root, "current_weather");
  if (!json_is_object(current_weather)) {
    json_decref(root);
    return;
  }

  json_t *temp_val = json_object_get(current_weather, "temperature");
  json_t *wind_val = json_object_get(current_weather, "windspeed");

  if (json_is_number(temp_val) && json_is_number(wind_val)) {
    double temp = json_number_value(temp_val);
    double wind = json_number_value(wind_val);

    printf("\n  Temperatur: %.1f °C\n", temp);
    printf("  Vind: %.1f km/h\n", wind);
  }
  json_decref(root);
}

int does_json_exist(char *cityName) {
  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  json_error_t error;
  json_t *root = json_load_file(cityFile, 0, &error);
  if (!root) {
    /* fprintf(stderr, "Error loading JSON: %s (line %d, col %d)\n",
    error.text, error.line, error.column); */
    return 1; /* Staden finns inte lokalt */
  }

  json_decref(root);
  return 0; /* Staden finns lokalt */
}

int is_weather_old(char *cityName) {
  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  json_error_t error;
  json_t *root = json_load_file(cityFile, 0, &error);

  json_t *current_weather = json_object_get(root, "current_weather");
  if (!json_is_object(current_weather)) {
    json_decref(root);
    return -1;
  }

  json_t *time_val = json_object_get(current_weather, "time");
  if (!json_is_string(time_val)) {
    json_decref(root);
    return -1;
  }
  const char *time_str = json_string_value(time_val);

  struct tm tm_time = {0};
  if (strptime(time_str, "%Y-%m-%dT%H:%M", &tm_time) == 0) {
    fprintf(stderr, "Failed to parse time string: %s\n", time_str);
    json_decref(root);
    return -1;
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
    return 0; /* Vädret är gammalt */
  }

  json_decref(root);
  return 1; /* Vädret är inte gammalt */
}

int print_weather(char *cityName) {
  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  json_error_t error;
  json_t *root = json_load_file(cityFile, 0, &error);
  if (!root) {
    fprintf(stderr, "Error loading JSON: %s (line %d, col %d)\n", error.text,
            error.line, error.column);
    return -1;
  }

  json_t *current_weather = json_object_get(root, "current_weather");
  if (!json_is_object(current_weather)) {
    json_decref(root);
    return -1;
  }

  json_t *temp_val = json_object_get(current_weather, "temperature");
  json_t *wind_val = json_object_get(current_weather, "windspeed");

  if (json_is_number(temp_val) && json_is_number(wind_val)) {
    double temp = json_number_value(temp_val);
    double wind = json_number_value(wind_val);

    printf("\nTemperature: %.1f °C\n", temp);
    printf("Wind: %.1f km/h\n", wind);
  }

  json_decref(root);
  return 0;
}