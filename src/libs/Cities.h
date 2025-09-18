#ifndef __CITIES_H__
#define __CITIES_H__

#include "Meteo.h"

typedef struct City City;

typedef struct City
{
	City* prev;
	City* next;

	char* name;
	float latitude;
	float longitude;

};

typedef struct
{
	City* head;
	City* tail;

	Meteo meteo;

} Cities;



int Cities_init(Cities* c);

void Cities_print(Cities* _Cities);
int Cities_add(Cities* _Cities, char* _Name, float _Latitude, float _Longitude, City** _City);
int Cities_get(Cities* _Cities, const char* _Name, City** _CityPtr);
void Cities_remove(Cities* _Cities, City* _City);

void Cities_dispose(Cities* c);

#endif // __CITIES_H__