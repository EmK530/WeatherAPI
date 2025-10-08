#ifndef CITY_H
#define CITY_H
#include "http.h"
#include "linked_list.h"

extern char *template;
/**
 * @struct weather
 * @brief Struct for weather data
 *
 * Struct for weather data, as well as when it was created, and for how
 * long it is valid.
 */
typedef struct {
  char time[32];   /* ISO 8601 format, YYYY-MM-DDTHH:MM */
  size_t interval; /* 900 seconds = 15 min interval between measurements */
  double
      temperature; /* double needed for wind chill or heat index calculations */
  double windspeed;
  float winddirection;
  char is_day;
  int weathercode;
} weather;

/**
 * @struct city
 * @brief Struct for city data
 *
 * Struct for city data, such as name, coordinates and most recent weather, if
 * any.
 */
typedef struct {
  char *name;
  float latitude;
  float longitude;
  size_t timestamp_prev_api_call;
  weather *current_weather;
} city;

/**
 * @brief Create a new city struct
 *
 * Creates a heap allocated city struct, and initializes it's name, latitude and
 * longitude. current_weather is set to NULL.
 *
 * @param _name Name of the city
 * @param _latitude Latitude of the city
 * @param _longitude Longitude of the city
 * @return Pointer to heap allocated city struct
 */
city *City_create(char *_name, float _latitude, float _longitude);

/**
 * @brief Dispose a city struct
 *
 * Frees memory used by a city struct, including memory used by current_weather.
 *
 * @param _city The city to be disposed
 */
void City_dispose(void *_city);

/**
 * @brief Parse formated city data string into linked list
 *
 * Parses a formatted string into city structs and appends them to the linked
 * list.
 *
 * @param _city_list Linked list into which to store the cities created
 * @param _city_data The formatted string to be parsed
 */
void City_parse_cities(LinkedList *_city_list, const char *_city_data);

/**
 * @brief Get current weather data for a given city
 *
 * Checks the cache for up-to-date weather data for this city. If the data is
 * not found, or it is expired, the function makes an API call to get new data,
 * and caches it. The weather data is then stored in the city struct.
 *
 * @param _curl The curl handle to be used for the API-call
 * @param _city The city for which to get weather data
 * @return An error code
 */
int City_get_weather(cURL *_curl, city *_city);

#endif