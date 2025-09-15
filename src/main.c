#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "weather.h"
#include "http.h"

int main()
{
    printf("Välkommen!\n\n");

    list_cities();

    char input[16];

    char name[16];
    double lat, lon;
    while(1)
    {
        printf("\nSkriv in en stad att kolla vädret på: ");
        scanf("%15s", input);

        int found = find_city(input, name, &lat, &lon);
        if(found == 0)
        {
            printf("Felaktig stad!\n");
        } else {
            char url[100];
            sprintf(url, template, lat, lon);
            /* printf("URL: \"%s\"\n", url); */

            if(!http_init())
            {
                continue;
            }
            http_perform(url);
            CURLcode result = http_get_result();
            if(result == CURLE_OK)
            {
                char* resp = http_get_response();
                printf("\n%s\n",resp);
            } else {
                printf("\nKunde inte ladda vädret!\n");
            }
            http_dispose();
        }
    }

    return 0;
}