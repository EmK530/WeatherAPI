#include "app_state.h"
#include "constants.h"

void app_state_init_defaults(app_state *_state) {

  if (!_state)
    return;

  location default_location = {
      .name = "Stockholm",
      .latitude = 59.3293,
      .longitude = 18.0686,
  };

  _state->current_location = default_location;
  _state->default_temp_unit = 'C';
  _state->known_locations = NULL;
  _state->previous_api_call = NULL;
  _state->error_code = 0;
}
