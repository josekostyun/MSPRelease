#ifndef INIT_H
#define INIT_H

#define GPS_BUFFER_SIZE 100

// GPS buffer variables
extern char gps_buffer[GPS_BUFFER_SIZE];
extern volatile int gps_idx;
extern volatile char new_sentence;

// Bluetooth ready flag
extern volatile unsigned char ble_ready;

// SPI buffer variables
extern volatile char ph_buffer[32];
extern volatile unsigned char ph_idx;
extern volatile unsigned char ph_ready;

void Initialize_Clock(void);
void Initialize_GPS_UART(void);
void Initialize_Bluetooth_UART(void);
void Initialize_SPI(void);
void Configure_MTK3339(void);
void GPS_queryConfig(void);
void Bluetooth_sendChar(char c);
void Bluetooth_sendString(const char* str);

#endif
