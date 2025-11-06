#include <string.h>
#include "parser.h"
#include "gps_buffer.h"

char utc_time[12] = "000000";
char latitude[12] = "0000000000";
char longitude[12] = "0000000000";
char fix_status = 'V';

void parse_GPRMC(char *sentence)
{
    char *ptr = sentence;
    int field = 0, idx = 0;
    char temp_time[12] = {0}, temp_lat[12] = {0}, temp_lon[12] = {0}, temp_fix = 'V';

    while (*ptr)
    {
        if (*ptr == ',') { field++; idx = 0; ptr++; continue; }

        switch (field)
        {
            case 1: if (idx < 11) temp_time[idx++] = *ptr; temp_time[idx] = '\0'; break; // UTC
            case 2: temp_fix = *ptr; break;                                             // Status
            case 3: if (idx < 11) temp_lat[idx++] = *ptr; temp_lat[idx] = '\0'; break;   // Latitude
            case 5: if (idx < 11) temp_lon[idx++] = *ptr; temp_lon[idx] = '\0'; break;   // Longitude
        }
        ptr++;
    }

    if (temp_fix == 'A') { // Only store valid GPS fixes
        strcpy(utc_time, temp_time);
        strcpy(latitude, temp_lat);
        strcpy(longitude, temp_lon);
        fix_status = temp_fix;
        GPSBuffer_add(utc_time, latitude, longitude);
    }
    else {
        fix_status = 'V';
    }
}
