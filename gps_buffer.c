#include <stdio.h>
#include <string.h>
#include "gps_buffer.h"
#include "init.h" // for Bluetooth_sendString()

GPSData gps_history[GPS_HISTORY_SIZE];
unsigned int gps_head = 0;

void GPSBuffer_add(const char* time, const char* lat, const char* lon)
{
    snprintf(gps_history[gps_head].utc_time, GPS_FIELD_LEN, "%s", time);
    snprintf(gps_history[gps_head].latitude, GPS_FIELD_LEN, "%s", lat);
    snprintf(gps_history[gps_head].longitude, GPS_FIELD_LEN, "%s", lon);

    gps_head = (gps_head + 1) % GPS_HISTORY_SIZE;  // circular increment
}

void GPSBuffer_printAll(void)
{
    Bluetooth_sendString("\r\n--- GPS History ---\r\n");
    for (int i = 0; i < GPS_HISTORY_SIZE; i++) {
        Bluetooth_sendString(gps_history[i].utc_time);
        Bluetooth_sendString(" | ");
        Bluetooth_sendString(gps_history[i].latitude);
        Bluetooth_sendString(" | ");
        Bluetooth_sendString(gps_history[i].longitude);
        Bluetooth_sendString("\r\n");
    }
}
