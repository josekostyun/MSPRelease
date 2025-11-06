#include <msp430.h>
#include <string.h>
#include <stdint.h>
#include "init.h"
#include "parser.h"
#include "gps_buffer.h"  // ← ADD THIS LINE


volatile uint8_t ble_ready = 0; //testing only
volatile uint8_t buffer_printed = 0; // flag to print buffer only once

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

    GPS_queryConfig(); //GPS Configure test
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
                    // Print buffer after collecting 20 entries (5 seconds of data)
                    if (gps_head == 20 && !buffer_printed)
                    {
                        buffer_printed = 1; // Set flag so it only prints once
                        Bluetooth_sendString("\r\n=== First 5 Seconds of Data (20 entries) ===\r\n");
                        GPSBuffer_printAll();
                        Bluetooth_sendString("=== End of History ===\r\n\r\n");
                    }
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
