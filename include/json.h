#ifdef JSON_H

# define weather_exists jansson_weather_exists
# define weather_is_stale jansson_weather_is_stale
# define weather_write jansson_weather_write
# define weather_print jansson_weather_print
# define weather_fetch jansson_weather_fetch

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
  PARAM_TIME = 1,
  PARAM_INTERVAL = 2,
  PARAM_TEMPERATURE = 3,
  PARAM_WIND_SPEED = 4,
  PARAM_WIND_DIRECTION = 5,
  PARAM_IS_DAY = 6,
  PARAM_WEATHER_CODE = 7
}

// Jansson:
int jansson_weather_exists(const char *cityName);
int jansson_weather_is_stale(const char *cityName);
int jansson_weather_write(const char *cityName, const char *data);
int jansson_weather_print(const char *cityName, int parameter);
current_weather jansson_weather_fetch(const char *cityName);

#endif