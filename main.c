#include <msp430.h>
#include <string.h>
#include <stdint.h>
#include "init.h"
#include "parser.h"
#include "gps_buffer.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;        // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;             // Unlock GPIO pins

    Initialize_Clock();
    Initialize_GPS_UART();
    Configure_MTK3339();
    Initialize_Bluetooth_UART();
    Initialize_SPI();

    __enable_interrupt();

    //Testing Only
    while (!ble_ready);

    GPS_queryConfig(); //GPS Configure test
    Bluetooth_sendString("GPS Ready\r\n");


    while(1){
        if (ph_ready){
            ph_ready = 0;
            Bluetooth_sendString("\r\n[PI DATA] ");
            Bluetooth_sendString((char*)ph_buffer);
            Bluetooth_sendString("\r\n");
        }
    }

    return 0;
}
