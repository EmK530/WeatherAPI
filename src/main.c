#include "app_state.h" /* provides app_state struct and functions like
                   app_create(), app_init_defaults(),
                   app_list_cities(), app_get_weather_by_index(),
                   app_print_weather(), and app_dispose(). This header
                   centralizes application-level state and is used
                   throughout main to operate on the global state. */
#include "city.h" /* declares city- and weather-related types and functions
                (e.g., City_get_weather). Included here because main
                interacts with city indexes indirectly through
                app_state functions which operate on city objects. */
#include "jansson_helper.h" /* helpers for JSON parsing/serialization used by
                      city and app_state code when reading/writing
                      cached weather/city data; not directly used
                      in main but included for consistency with
                      higher-level project modules. */
#include "libs/jansson/jansson.h" /* third-party JSON library header —
                          included in some source files where
                          jansson-specific types are needed.
                          main doesn't call jansson directly but
                          including it here mirrors other files
                          and documents the dependency. */
#include <stdio.h>   /* standard I/O: printf, scanf — used directly in main */
#include <stdlib.h>  /* standard library: malloc/free, strtol, EXIT codes */
#include <string.h>  /* string helpers (not used directly in main now but
                 commonly used in I/O handling and present in many
                 source files) */
#include <time.h>    /* time functions used elsewhere (e.g., in app_state)
                 but included here for parity with other modules */

int main() {
  /* Create the main application state. app_create() allocates and returns
    an `app_state *` (see include/app_state.h). The returned pointer is the
    central handle used throughout the application to access known
    locations, current weather, and the HTTP/cURL wrapper. */
  app_state *app = app_create();

  /* Initialize application defaults. This sets up the curl/http wrapper,
    creates the cache folder if missing, reads cached city list into the
    known_locations linked list, and populates defaults if the cache is
    empty. See src/libs/app_state.c -> app_init_defaults for details. */
  app_init_defaults(app);

  /* Greet the user. printf is from <stdio.h>. The application communicates
    with the user via the console: listing cities and prompting for
    selection. */
  printf("Välkommen!\n\n");

  /* Buffer to hold raw user input from stdin. We use a fixed-size buffer
    and guard reads with a max width in scanf to avoid overflow. The size
    128 is chosen heuristically and is consistent with other string buffers
    in the codebase. */
  char input[128];

  /* Numeric selection parsed from `input`. Using `long` to match usage of
    strtol and to comfortably hold integer indices for lists. */
  long selection;

  /* The main interactive loop: list known cities, read user selection,
    fetch weather for the selected city (if valid), and print it. The loop
    continues until the user selects 0 or a non-positive number to exit. */
  do {
   /* Print the list of known locations. This function is defined in
     src/libs/app_state.c and uses the `known_locations` LinkedList to
     display each city's name and index. It depends on LinkedList and
     City structures. */
   app_list_cities(app);

   /* Read a user token from stdin. The format specifier limits the read to
     127 characters plus a null terminator to avoid buffer overflow.
     scanf is simple here; more robust apps might use fgets + parsing. */
   scanf("%127s", input);

   /* Convert the input string to a long integer. strtol handles numeric
     conversion and returns 0 on failure; we pass NULL for endptr since
     we don't need to inspect trailing characters here. The base 10 is
     used to parse normal decimal input. */
   selection = strtol(input, NULL, 10);

   /* Adjust selection to zero-based index and request weather for that
     index. app_get_weather_by_index will call into City_get_weather and
     the HTTP wrapper to retrieve data. It returns >= 0 on success, in
     which case we print the weather with app_print_weather (which uses
     the app_state current structures to present the data). */
   if (app_get_weather_by_index(app, selection - 1) >= 0) {
    app_print_weather(app, selection - 1);
   }

   /* Continue looping while the user chooses a positive selection.
     Selecting 0 is explicitly printed as the exit option in
     app_list_cities. */
  } while (selection > 0);

  /* Clean up the application state. app_dispose frees known_locations,
    disposes the curl/http wrapper, frees cached weather entries, and
    finally frees the `app_state` structure itself. */
  app_dispose(app);

  /* Return 0 to indicate success to the OS. */
  return 0;
}