#ifndef INIT_H
#define INIT_H

#define GPS_BUFFER_SIZE 100

extern char gps_buffer[GPS_BUFFER_SIZE];
extern volatile int gps_idx;
extern volatile char new_sentence;

void Initialize_Clock(void);
void Initialize_GPS_UART(void);
void Initialize_Bluetooth_UART(void);
void Configure_MTK3339(void);
void Bluetooth_sendChar(char c);
void Bluetooth_sendString(const char* str);

#endif
