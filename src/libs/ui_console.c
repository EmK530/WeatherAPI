#include "app_state.h"
#include <stdio.h>

void print_available_locations(location_list *_known_locations) {
  {
    printf("  0: Exit program\n");

    size_t i = 0;
    for (; i < _known_locations->number_of_locations; i++) {
      printf("%3zu: %s\n", i + 1, _known_locations->locations[i].name);
    }
  }
}

void clear_buffer_stdin() {
  int current_char;
  while ((current_char = getchar()) != '\n' && current_char != EOF)
    ;
}

int prompt_user_for_current_selection(app_state *_app) {
  int selection = 0;

  do {
    printf("SELECT: ");
    if (scanf("%i", &selection) != 1) {
      clear_buffer_stdin();
      printf("invalid selection\n");
      continue;
    }

    clear_buffer_stdin();

    /* exit program on negative and zero */
    if (selection <= 0) {
      _app->exit = 1;
      break;
    }

    /* exit loop if selection is in range */
    if (selection <= (int)_app->known_locations->size) {
      break;
    }
    printf("invalid selection\n");

  } while (1);

  return selection;
}

void print_current_location(location *_current_location) {
  printf("-------------------------------------------------------------------"
         "---------\nSELECTED: %s, "
         "latitude: %lf, longitude: %lf\n",
         _current_location->name, _current_location->latitude,
         _current_location->longitude);
}