#include <stdio.h>
#include <string.h>

const char *cities = "Stockholm:59.3293:18.0686\n"
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
char *template = "https://api.open-meteo.com/v1/"
                 "forecast?latitude=%lf&longitude=%lf&current_weather=true";

void list_cities() {
  const char *scan1 = cities;
  char name[16];
  while (*scan1 != '\0') {
    sscanf(scan1, "%16[^:]", name);
    printf("%s\n", name);
    scan1 = strchr(scan1, '\n');
    if (scan1 == NULL) {
      break;
    } else {
      scan1++;
    }
  }
}

int find_city(char *input, char *name, double *lat, double *lon) {
  const char *scan1 = cities;
  while (*scan1 != '\0') {
    sscanf(scan1, "%16[^:]:%lf:%lf", name, lat, lon);
    if (strcmp(name, input) == 0) {
      return 1;
    }
    scan1 = strchr(scan1, '\n');
    if (scan1 != NULL) {
      scan1++;
    } else {
      return 0;
    }
  }
  return 0;
}