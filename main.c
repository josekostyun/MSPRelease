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
        if(new_sentence)
        {
            new_sentence = 0;

            // Check if it's a GGA sentence
            if(strstr(gps_buffer, "GGA"))
            {
                parse_GPGGA(gps_buffer);

                // Send GPS data to Bluetooth
                Bluetooth_sendString("GPS: ");
                Bluetooth_sendString(gps_buffer);
                Bluetooth_sendString("\r\n");

                // Send parsed latitude and fix status
                if(fix_status != '0')
                {
                    Bluetooth_sendString("LAT: ");
                    Bluetooth_sendString(latitude);
                    Bluetooth_sendString(" FIX: ");
                    Bluetooth_sendChar(fix_status);
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
