/**
 * @file app_state.h
 * @brief Application state management for weather information system
 *
 * This module provides the core application state structure and functions
 * for managing weather data across multiple cities, including location
 * management, API request timing, and weather data retrieval.
 */

#ifndef APP_STATE_H
#define APP_STATE_H

#include "city.h"
#include "constants.h"
#include "http.h"
#include "linked_list.h"
#include <stddef.h>

/**
 * @struct app_state
 * @brief Main application state container
 *
 * Holds the application's runtime state including all known city locations
 * and the HTTP client handle for API communication.
 */
typedef struct {
  LinkedList *known_locations; /**< List of city objects with weather data */
  cURL curl_handle;            /**< HTTP client handle for API requests */
} app_state;

/**
 * @brief [work in progress] Calculate seconds until next API data update is
 * available
 *
 * Determines the time remaining until new weather data will be released by
 * the API based on the configured update interval and the last API call
 * timestamp.
 *
 * @param _curl Pointer to cURL handle (currently unused in implementation)
 * @param _city Pointer to city object with previous API call timestamp
 * @return Number of seconds until next update, or 0 if update is available now
 */
int app_seconds_to_next_api_update(cURL *_curl, city *_city);

/**
 * @brief Allocate memory for a new application state
 *
 * Creates an uninitialized app_state structure. Must be followed by
 * app_init_defaults() to properly initialize the state.
 *
 * @return Pointer to newly allocated app_state, or NULL on allocation failure
 */
app_state *app_create();

/**
 * @brief Clean up and free all resources associated with application state
 *
 * Disposes of all known locations, closes the cURL handle, and frees
 * the app_state structure itself.
 *
 * @param _app Pointer to app_state to dispose
 */
void app_dispose(app_state *_app);

/**
 * @brief Initialize application state with default values
 *
 * Sets up the application by:
 * - Initializing the cURL handle
 * - Creating the cache directory if it doesn't exist
 * - Loading cities from cache or populating with test data
 * - Writing initial city data to cache if needed
 *
 * @param _app Pointer to app_state to initialize
 * @return 0 on success, -1 on failure (invalid pointer or cURL init failure)
 */
int app_init_defaults(app_state *_app);

/**
 * @brief Display list of all known cities to stdout
 *
 * Prints a numbered list of all cities in the known_locations list,
 * along with an exit option (0).
 *
 * @param _app Pointer to app_state containing city list
 */
void app_list_cities(app_state *_app);

/**
 * @brief Retrieve weather data for a city by index
 *
 * Fetches current weather information from the API for the city at
 * the specified index in the known_locations list.
 *
 * @param _app Pointer to app_state containing city list and cURL handle
 * @param _index Zero-based index of city in known_locations list
 * @return 0 on success, -1 if index is out of bounds
 */
int app_get_weather_by_index(app_state *_app, size_t _index);

/**
 * @brief Print weather information for a city to stdout
 *
 * Displays the location name, temperature, and windspeed for the city
 * at the specified index.
 *
 * @param _app Pointer to app_state containing city list
 * @param _index Zero-based index of city in known_locations list
 */
void app_print_weather(app_state *_app, int _index);

#endif /* APP_STATE_H */