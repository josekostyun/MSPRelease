#include <msp430.h>
#include <string.h>
#include "init.h"
#include "parser.h"

// Parsed GPS fields (remain here)
char latitude[12] = "0000000000";
char fix_status = '0';

volatile uint8_t ble_ready = 0; //testing only

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;        // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;             // Unlock GPIO pins

    Initialize_Clock();
    Initialize_GPS_UART();
    Initialize_Bluetooth_UART();
    Configure_MTK3339();

    __enable_interrupt();

    //Testing Only
    while (!ble_ready);

    GPS_queryConfig(); //GPS Configue test
    Bluetooth_sendString("GPS Ready\r\n");


    while(1)
    {
        if (new_sentence)
        {
            new_sentence = 0;

            if (strstr(gps_buffer, "RMC"))
            {
                parse_GPRMC(gps_buffer);

                if (fix_status == 'A')
                {
                    Bluetooth_sendString("UTC: ");
                    Bluetooth_sendString(utc_time);
                    Bluetooth_sendString(" LAT: ");
                    Bluetooth_sendString(latitude);
                    Bluetooth_sendString(" LON: ");
                    Bluetooth_sendString(longitude);
                    Bluetooth_sendString("\r\n");
                }
                else
                {
                    Bluetooth_sendString("NO FIX\r\n");
                }
            }

            gps_idx = 0;
        }
    }

    return 0;
}
