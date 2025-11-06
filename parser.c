#include <string.h>
#include <stdio.h>
#include "parser.h"
#include "gps_buffer.h"

char utc_time[18] = "000000000000000"; // YYYYMMDDHHMMSSsss
char latitude[16]   = "0000000000";    // Changed from [12] to [16]
char longitude[16]  = "0000000000";    // Changed from [12] to [16]
char fix_status     = 'V';

// Simple zero-pad integer to string (no printf needed)
static void append_num(char *dst, int value, int digits)
{
    char buf[8];
    int i = 0;
    for (i = digits - 1; i >= 0; i--) {
        buf[i] = (value % 10) + '0';
        value /= 10;
    }
    buf[digits] = '\0';
    strcat(dst, buf);
}


void parse_GPRMC(char *sentence)
{
    char *ptr = sentence;
    int field = 0, idx = 0;
    char time_raw[12] = {0};
    char date_raw[8]  = {0};
    char frac[4]      = {'0','0','0','\0'};
    char lat_dir = 0;      // N or S
    char lon_dir = 0;      // E or W

    while (*ptr)
    {
        if (*ptr == ',') { field++; idx = 0; ptr++; continue; }

        switch (field)
        {
            case 1: // UTC time (hhmmss.sss)
                if (*ptr == '.') {
                    idx = 0;
                    ptr++;
                    int i;
                    for (i = 0; i < 3 && *ptr >= '0' && *ptr <= '9'; i++)
                        frac[i] = *ptr++;
                    continue;
                }
                if (idx < 10) time_raw[idx++] = *ptr;
                time_raw[idx] = '\0';
                break;

            case 2: fix_status = *ptr; break;

            case 3: // Latitude (DDMM.MMMM)
                if (idx < 14) latitude[idx++] = *ptr;
                latitude[idx] = '\0';
                break;

            case 4: // Latitude direction (N/S)
                lat_dir = *ptr;
                break;

            case 5: // Longitude (DDDMM.MMMM)
                if (idx < 14) longitude[idx++] = *ptr;
                longitude[idx] = '\0';
                break;

            case 6: // Longitude direction (E/W)
                lon_dir = *ptr;
                break;

            case 9: // Date (DDMMYY)
                if (idx < 6) date_raw[idx++] = *ptr;
                date_raw[idx] = '\0';
                break;
        }
        ptr++;
    }

    if (fix_status != 'A') return;

    // Append direction indicators to coordinates
    if (lat_dir) {
        int len = strlen(latitude);
        if (len < 15) {
            latitude[len] = lat_dir;
            latitude[len + 1] = '\0';
        }
    }

    if (lon_dir) {
        int len = strlen(longitude);
        if (len < 15) {
            longitude[len] = lon_dir;
            longitude[len + 1] = '\0';
        }
    }

    // Parse date and time
    int day   = (date_raw[0]-'0')*10 + (date_raw[1]-'0');
    int month = (date_raw[2]-'0')*10 + (date_raw[3]-'0');
    int year  = (date_raw[4]-'0')*10 + (date_raw[5]-'0') + 2000;
    int hour  = (time_raw[0]-'0')*10 + (time_raw[1]-'0');
    int min   = (time_raw[2]-'0')*10 + (time_raw[3]-'0');
    int sec   = (time_raw[4]-'0')*10 + (time_raw[5]-'0');

    utc_time[0] = '\0';               // clear buffer

    append_num(utc_time, year, 4);
    append_num(utc_time, month, 2);
    append_num(utc_time, day, 2);
    append_num(utc_time, hour, 2);
    append_num(utc_time, min, 2);
    append_num(utc_time, sec, 2);
    strcat(utc_time, frac);           // add fractional milliseconds

    GPSBuffer_add(utc_time, latitude, longitude);
}
