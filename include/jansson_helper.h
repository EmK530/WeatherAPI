#pragma once

int check_weather_cache(char* cityName);
void print_temperature(char* cityName);

int does_json_exist(char* cityName);
int is_weather_old(char* cityName);
int print_weather(char* cityName);