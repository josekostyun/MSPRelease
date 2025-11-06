#ifndef GPS_BUFFER_H
#define GPS_BUFFER_H

#define GPS_HISTORY_SIZE 20
#define GPS_FIELD_LEN 16

typedef struct {
    char utc_time[GPS_FIELD_LEN];
    char latitude[GPS_FIELD_LEN];
    char longitude[GPS_FIELD_LEN];
} GPSData;

extern GPSData gps_history[GPS_HISTORY_SIZE];
extern unsigned int gps_head;

void GPSBuffer_add(const char* time, const char* lat, const char* lon);
void GPSBuffer_printAll(void);

#endif
