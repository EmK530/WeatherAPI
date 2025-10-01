#ifdef JSON_H

typedef struct {
  char time[32]; /* todo how long can a ISO 8601 time string be? 32 might be to small */
  int interval;
  double temperature;
  double windspeed;
  int winddirection;
  int is_day;
  int weathercode;
} current_weather;

enum {
  PARAM_TEMPERATURE = 1,
  PARAM_WIND_SPEED = 2,
  PARAM_WIND_DIRECTION = 3,
  PARAM_IS_DAY = 4,
  PARAM_WEATHER_CODE = 5
}

// Jansson:
int jansson_weather_exists(const char *cityName);
int jansson_weather_is_stale(const char *cityName);
int jansson_weather_write(const char *cityName, const char *data);
int jansson_weather_print(const char *cityName, int parameter);
current_weather jansson_weather_fetch(const char *cityName);

#endif