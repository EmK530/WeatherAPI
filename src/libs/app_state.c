#include "app_state.h"
#include "constants.h"
#include <malloc.h>
#include <stdlib.h>

/* INTERNAL FUNCTIONS */
int app_state_populate_known_locations_from_testdata(app_state *_state) {
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

  if (_state->known_locations.locations != NULL) {
    free(_state->known_locations.locations);
    _state->known_locations.locations = NULL;
    _state->known_locations.number_of_locations = 0;
  }

  /* count locations before malloc */
  size_t locations_in_testdata = 0;
  size_t i;
  for (i = 0; test_data[i] != '\0'; i++) {
    if (test_data[i] == '\n') {
      locations_in_testdata++;
    }
  }

  /* allocate enough memory for the locations */
  location *temp_locations_pointer =
      malloc(sizeof(location) * locations_in_testdata);
  /* todo finish error handling */
  if (temp_locations_pointer == NULL) {
    _state->error_code = -1;
    _state->error_message = "malloc failed";
    exit(EXIT_FAILURE); /* dont hard exit here */
    return -1;
  }

  /* load data from string into memory. put data into state as long as we can
   * get 3 things at a time from sscanf */
  int length_of_read_string = 0;
  size_t offset = 0;
  i = 0;
  while (3 == sscanf(test_data + offset, "%[^:]:%lf:%lf%n",
                     temp_locations_pointer[i].name,
                     &temp_locations_pointer[i].latitude,
                     &temp_locations_pointer[i].longitude,
                     &length_of_read_string)) {

    _state->known_locations.number_of_locations++;

    /* +1 to skip over the '\n' at the end of each row */
    offset = offset + length_of_read_string + 1;
    i++;
  }

  _state->known_locations.locations = temp_locations_pointer;

  return 0;
}

/* EXTERNAL FUNCTIONS */
void app_init_defaults(app_state *_state) {
  if (!_state)
    return;

  location default_location = {
      .name = "Stockholm",
      .latitude = 59.3293,
      .longitude = 18.0686,
  };

  _state->current_location = default_location;
  _state->default_temp_unit = 'C';
  _state->previous_api_call = NULL;
  _state->known_locations.locations = NULL;
  _state->known_locations.number_of_locations = 0;
  _state->exit = 0;
  _state->error_code = 0;

  /* todo make a real load function that read from file or API */
  app_state_populate_known_locations_from_testdata(_state);
}

void set_current_location(app_state *_state, int _selection) {
  /* structs can be copied in C, even if they have arrays inside them */
  _state->current_location = _state->known_locations.locations[_selection - 1];
}