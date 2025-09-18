#ifndef __Meteo_h__
#define __Meteo_h__

typedef struct
{
	const char* api_url;

} Meteo;

int Meteo_init(Meteo* meteo, const char* api_url);

int Meteo_getTemperature(Meteo* meteo, float lat, float lon, float* _Temperature);

void Meteo_dispose(Meteo* meteo);


#endif // __Meteo_h__
