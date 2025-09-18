#include "Meteo.h"
#include "HTTP.h"

int Meteo_init(Meteo* meteo, const char* api_url)
{
	memset(meteo, 0, sizeof(Meteo));

	meteo->api_url = api_url;
	
	return 0;
}

int Meteo_getTemperature(Meteo* meteo, float lat, float lon, float* _Temperature)
{
	HTTP http;
	if(HTTP_init(&http) != 0)
	{
		printf("Failed to initialize HTTP\n");
		return -1;
	}

	char url[256];
	snprintf(url, sizeof(url), "%s/v1/forecast?latitude=%2.2f&longitude=%2.2f&hourly=temperature_2m", meteo->api_url, lat, lon);

	int result = HTTP_get(&http, url);
	if(result != 0)
	{
		printf("HTTP GET request failed. Errorcode: %i\n", result);
		HTTP_dispose(&http);
		return -2;
	}

	*(_Temperature) = 0;

	return 0;
}

void Meteo_dispose(Meteo* m)
{

}

