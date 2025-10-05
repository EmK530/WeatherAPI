#include "app_state.h"
#include "city.h"
#include "constants.h"
#include "jansson_helper.h"
#include "linked_list.h"
#include "stdio.h"
#include "tinydir.h"
#include <stdlib.h>
#include <sys/stat.h>

/* INTERNAL FUNCTIONS */
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

  if (_app->known_locations != NULL) {
    LinkedList_dispose(_app->known_locations, City_dispose);
  }

  _app->known_locations = LinkedList_create();
  City_parse_cities(_app->known_locations, (char *)test_data);

  return 0;
}
/* EXTERNAL FUNCTIONS */
app_state *app_create() {
  app_state *app = (app_state *)malloc(sizeof(app_state));

  return app;
}

void app_dispose(app_state *_app) {
  if (_app->known_locations != NULL) {
    LinkedList_dispose(_app->known_locations, City_dispose);
  }
  curl_dispose(&_app->curl_handle);
  if (_app->current_location.current_weather) {
    free(_app->current_location.current_weather);
  }
  free(_app);
}

int app_init_defaults(app_state *_app) {
  if (!_app)
    return -1;
  memset(_app, 0, sizeof(app_state));

  if (!curl_init(&_app->curl_handle)) {
    return -1; /* error print is handled in curl_init */
  }
  // create cache folder if missing
  struct stat sb;
  if (!(stat("cache", &sb) == 0 && S_ISDIR(sb.st_mode))) {
    mkdir("cache", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }

  _app->known_locations = LinkedList_create();

  // populate know_locations from cache
  read_cities_from_file("cache/cities.json", _app->known_locations);

  // populate from defaults if no cache
  if (_app->known_locations->size == 0) {
    app_state_populate_known_locations_from_testdata(_app);
    write_cities_to_file("cache/cities.json", _app->known_locations);
  }
  return 0;
}

void app_list_cities(app_state *_app) {

  for (int i = 0; i < (int)_app->known_locations->size; i++) {
    city *item = (city *)(LinkedList_get_index(_app->known_locations, i)->item);
    printf("%3d: %s) \n", i + 1, item->name);
  }
  printf("  0: exit\n");
}

int app_get_weather_by_index(app_state *_app, int _index) {
  if (_index >= (int)_app->known_locations->size || _index < 0) {
    return -1;
  }
  city *item =
      (city *)(LinkedList_get_index(_app->known_locations, _index)->item);

  weather *result = City_get_weather(&_app->curl_handle, item);
  if (result == NULL) {
    return -1;
  }
  if (_app->current_location.current_weather != NULL) {
    free(_app->current_location.current_weather);
  }
  _app->current_location.name = item->name; // maybe strdup, might be more clean
  _app->current_location.latitude = item->latitude;
  _app->current_location.longitude = item->longitude;
  _app->current_location.current_weather = result;
  return 0;
}

void app_print_current_weather(app_state *_app) {
  location loc = _app->current_location;
  printf("\tLocation:\t%s\n"
         "\tTemperature:\t%f\n"
         "\tWindspeed:\t%f\n\n",
         loc.name, loc.current_weather->temperature,
         loc.current_weather->windspeed);
}

void set_current_location(app_state *_app, int _selection) {
  _app++;
  _selection++;
}