#include "jansson_helper.h"
#include "jansson/jansson.h"
#include <time.h>
/**/
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

current_weather get_weather(char *cityName) {
  current_weather cw = {0}; // initialize all fields to safe defaults

  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  json_error_t error;
  json_t *root = json_load_file(cityFile, 0, &error);
  if (!root) {
    fprintf(stderr, "Error loading JSON: %s (line %d, col %d)\n", error.text,
            error.line, error.column);
    return cw; // return empty struct on error
  }

  json_t *current_weather = json_object_get(root, "current_weather");
  if (!json_is_object(current_weather)) {
    json_decref(root);
    return cw; // return empty struct if missing
  }

  // Extract values
  json_t *time_val = json_object_get(current_weather, "time");
  json_t *interval_val = json_object_get(current_weather, "interval");
  json_t *temp_val = json_object_get(current_weather, "temperature");
  json_t *wind_val = json_object_get(current_weather, "windspeed");
  json_t *winddir_val = json_object_get(current_weather, "winddirection");
  json_t *isday_val = json_object_get(current_weather, "is_day");
  json_t *wcode_val = json_object_get(current_weather, "weathercode");

  if (json_is_string(time_val)) {
    strncpy(cw.time, json_string_value(time_val), sizeof(cw.time) - 1);
  }
  if (json_is_integer(interval_val)) {
    cw.interval = (int)json_integer_value(interval_val);
  }
  if (json_is_number(temp_val)) {
    cw.temperature = json_number_value(temp_val);
  }
  if (json_is_number(wind_val)) {
    cw.windspeed = json_number_value(wind_val);
  }
  if (json_is_integer(winddir_val)) {
    cw.winddirection = (int)json_integer_value(winddir_val);
  }
  if (json_is_integer(isday_val)) {
    cw.is_day = (int)json_integer_value(isday_val);
  }
  if (json_is_integer(wcode_val)) {
    cw.weathercode = (int)json_integer_value(wcode_val);
  }

  json_decref(root);
  return cw;
}