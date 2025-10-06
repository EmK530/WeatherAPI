/**
 * @file app_state.c
 * @brief Implementation of application state management functions
 *
 * Provides core functionality for managing the weather application's state,
 * including city location management, API request timing, and weather data
 * retrieval and display.
 */

#include "app_state.h"
#include "city.h"
#include "constants.h"
#include "jansson_helper.h"
#include "linked_list.h"
#include "stdio.h"
#include "tinydir.h"
#include <stdlib.h>
#include <sys/stat.h>

/**
 * @brief Populate known_locations with hardcoded city data
 *
 * Internal function that loads a predefined set of 16 cities
 * with their coordinates. Used as fallback when no cached city data exists.
 * Clears any existing locations before populating.
 *
 * @param _app Pointer to app_state to populate
 * @return Always returns 0 (success)
 */
int app_state_populate_known_locations_from_testdata(app_state *_app) {
  const char test_data[] = "Stockholm:59.3293:18.0686\n"
                           "Göteborg:57.7089:11.9746\n"
                           "Malmö:55.6050:13.0038\n"
                           "Uppsala:59.8586:17.6389\n"
                           "Västerås:59.6099:16.5448\n"
                           "Örebro:59.2741:15.2066\n"
                           "Linköping:58.4109:15.6216\n"
                           "Helsingborg:56.0465:12.6945\n"
                           "Jönköping:57.7815:14.1562\n"
                           "Norrköping:58.5877:16.1924\n"
                           "Lund:55.7047:13.1910\n"
                           "Gävle:60.6749:17.1413\n"
                           "Sundsvall:62.3908:17.3069\n"
                           "Umeå:63.8258:20.2630\n"
                           "Luleå:65.5848:22.1567\n"
                           "Kiruna:67.8558:20.2253\n";

  // Clean up existing locations if any
  if (_app->known_locations != NULL) {
    LinkedList_dispose(_app->known_locations, City_dispose);
  }

  _app->known_locations = LinkedList_create();
  City_parse_cities(_app->known_locations, (char *)test_data);

  return 0;
}

app_state *app_create() {
  app_state *app = (app_state *)malloc(sizeof(app_state));
  if (!app) {
    fprintf(stderr, "Failed to allocate memory for app_state\n");
    return NULL;
  }
  memset(app, 0, sizeof(app_state));
  return app;
}

void app_dispose(app_state *_app) {
  if (!_app)
    return;

  if (_app->known_locations != NULL) {
    LinkedList_dispose(_app->known_locations, City_dispose);
  }
  curl_dispose(&_app->curl_handle);

  free(_app);
}

int app_init_defaults(app_state *_app) {
  if (!_app)
    return -1;

  // Zero out the structure to ensure clean initialization
  memset(_app, 0, sizeof(app_state));

  // Initialize HTTP client
  if (!curl_init(&_app->curl_handle)) {
    return -1; /* Error message is handled in curl_init */
  }

  // Create cache directory if it doesn't exist
  struct stat sb;
  if (!(stat("cache", &sb) == 0 && S_ISDIR(sb.st_mode))) {
    mkdir("cache", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }

  _app->known_locations = LinkedList_create();

  // Attempt to load cities from cache file
  read_cities_from_file("cache/cities.json", _app->known_locations);

  // Populate with default test data if cache is empty
  if (_app->known_locations->size == 0) {
    app_state_populate_known_locations_from_testdata(_app);
    write_cities_to_file("cache/cities.json", _app->known_locations);
  }

  return 0;
}

int app_seconds_to_next_api_update(cURL *_curl, city *_city) {
  size_t prev_call = (size_t)_city->timestamp_prev_api_call;

  // If no previous call was made, allow immediate update
  if (prev_call <= 0)
    return 0;

  size_t now = (size_t)time(NULL);
  size_t interval = (size_t)API_DATA_RELEASE_ITERVAL_SECONDS;
  size_t seconds_left = 0;

  // Calculate which API update interval we're currently in
  size_t quarters_from_now = now / interval;
  size_t quarters_from_prev_call = prev_call / interval;

  // If we've moved to a new interval, data is available
  if (quarters_from_now > quarters_from_prev_call)
    return 0;

  // Calculate when the next interval starts
  size_t next_interval_start = (quarters_from_now + 1) * interval;
  seconds_left = next_interval_start - now;

  // todo remove debug print
  printf("No API call sent, %02zum %02zus left until new data is released\n",
         seconds_left / 60, seconds_left % 60);

  return seconds_left;
}

void app_list_cities(app_state *_app) {
  // Display all cities with 1-based numbering
  for (int i = 0; i < (int)_app->known_locations->size; i++) {
    city *item = (city *)(LinkedList_get_index(_app->known_locations, i)->item);
    printf("%3d: %s \n", i + 1, item->name);
  }
  printf("  0: Exit\n");
}

int app_get_weather_by_index(app_state *_app, size_t _index) {
  // Validate index bounds
  if (_index >= _app->known_locations->size) {
    return -1;
  }

  city *item =
      (city *)(LinkedList_get_index(_app->known_locations, _index)->item);

  int result = City_get_weather(&_app->curl_handle, item);
  return result;
}

void app_print_weather(app_state *_app, int _index) {
  if (!_app || _index < 0 || _index >= (int)_app->known_locations->size) {
    fprintf(stderr, "Invalid index or app_state\n");
    return;
  }
  city *item =
      (city *)(LinkedList_get_index(_app->known_locations, _index)->item);
  weather *data = item->current_weather;

  printf("\t\t\t%s %.1lf°C\n", item->name, data->temperature);
}