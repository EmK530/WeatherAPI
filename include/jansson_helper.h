#ifndef JANSON_HELPER_H
#define JANSON_HELPER_H

#include "city.h"
#include "linked_list.h"

/**
 * @brief Checks if the weather cache for a given city is valid.
 * A cache is considered valid if it exists and is not outdated.
 * @param cityName The name of the city to check.
 * @return 0 if the cache exists and is fresh, 1 if it doesn't exist or is stale.
 */
int check_weather_cache(char *cityName);

/**
 * @brief Reads a list of cities from a JSON file and populates a linked list.
 * @param file_name The path to the JSON file containing city data.
 * @param list A pointer to a LinkedList to populate with city data.
 * @return 0 on success, 1 on failure.
 */
int read_cities_from_file(char *file_name, LinkedList *list);

/**
 * @brief Writes a list of cities to a JSON file from a linked list.
 * @param file_name The path to the JSON file to write city data to.
 * @param list A pointer to a LinkedList containing city data.
 * @return 0 on success, -1 on failure.
 */
int write_cities_to_file(char *file_name, LinkedList *list);

/**
 * @brief Parses weather data from a JSON string.
 * @param json A string containing the JSON weather data.
 * @return A pointer to a weather struct with parsed data, or NULL on failure.
 */
weather *parse_weather_json(char *json);

/**
 * @brief Reads weather data from a cache file.
 * @param file_name The path to the cache file.
 * @return A string containing the raw JSON data, or NULL on failure.
 */
char *read_weather_cache(char *file_name);

/**
 * @brief Writes weather data to a cache file.
 * @param file_name The path to the cache file.
 * @param data A string containing the JSON weather data to write.
 * @return 0 on success, -1 on failure.
 */
int write_weather_cache(char *file_name, char *data);

#endif