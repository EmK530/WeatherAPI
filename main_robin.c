#include <stdio.h>

#include "libs/Cities.h"

Cities g_cities;

int main()
{


	Cities_init(&g_cities);

	City* linköping;
	if(Cities_get(&g_cities, "Linköping", &linköping) == 0)
	{
		float temperature; 
		int result = Cities_getTemperature(&g_cities, linköping, &temperature);
		if(result == 0)
			printf("Temperature in Linköping: %.2f °C\n", temperature);
		else
			printf("Failed to get temperature for Linköping, error code: %d\n", result);


	}

	return 0;
}

