#pragma once

int check_weather_cache(char* cityName);
void print_temperature(char* cityName);
/**/
typedef struct {
    char time[20];
    int interval;
    double temperature;
    double windspeed;
    int winddirection;
    int is_day;
    int weathercode;
} current_weather;

int does_json_exist(char* cityName);
int is_weather_old(char* cityName);
int print_weather(char* cityName);
current_weather get_weather();