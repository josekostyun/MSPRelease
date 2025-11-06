#include <msp430.h>
#include "init.h"

extern volatile unsigned char ble_ready; //testing only

// GPS buffer variables
char gps_buffer[GPS_BUFFER_SIZE];
volatile int gps_idx = 0;
volatile char new_sentence = 0;

// Bluetooth ready flag
volatile unsigned char ble_ready = 0;

// SPI buffer variables
volatile char ph_buffer[32];
volatile unsigned char ph_idx = 0;
volatile unsigned char ph_ready = 0;

// Initialize 8MHz clock
void Initialize_Clock(void)
{
    __bis_SR_register(SCG0);
    CSCTL3 = SELREF__REFOCLK;
    CSCTL0 = 0;
    CSCTL1 &= ~(DCORSEL_7);
    CSCTL1 |= DCORSEL_3;
    CSCTL2 = FLLD_0 + 243;
    __delay_cycles(3);
    __bic_SR_register(SCG0);

    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));
}

// Initialize GPS UART (UCA0 @ 9600 baud)
void Initialize_GPS_UART(void)
{
    P1SEL0 |= (BIT6 | BIT7);
    P1SEL1 &= ~(BIT6 | BIT7);

    UCA0CTLW0 = UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 52;
    UCA0MCTLW = 0x4911;
    UCA0CTLW0 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
}

// Initialize Bluetooth UART (UCA1 @ 115200 baud)
void Initialize_Bluetooth_UART(void)
{
    P4SEL0 |= (BIT2 | BIT3);
    P4SEL1 &= ~(BIT2 | BIT3);

    UCA1CTLW0 = UCSWRST;
    UCA1CTLW0 |= UCSSEL__SMCLK;

    UCA1BR0 = 4;
    UCA1BR1 = 0;
    UCA1MCTLW = UCOS16 | UCBRF_5 | 0x5500;

    UCA1CTLW0 &= ~UCSWRST;

    UCA1IE |= UCRXIE; //Testing only
}

void GPS_sendCommand(const char *cmd)
{
    const char *p = cmd;
    while (*p)
    {
        while (!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = *p++;
    }
}

void Initialize_SPI(void)
{
    // Configure SPI pins for UCB0 (Primary Function Mode)
    P1SEL0 |= BIT0 | BIT1 | BIT2 | BIT3;
    P1SEL1 &= ~(BIT0 | BIT1 | BIT2 | BIT3);

    /*
       P1.0 = STE   (Chip Select from Pi)
       P1.1 = CLK
       P1.2 = SIMO (Pi → MSP RX)
       P1.3 = SOMI (MSP → Pi TX)
    */

    UCB0CTLW0 = UCSWRST;        // hold in reset
    UCB0CTLW0 |= UCSYNC;        // synchronous (SPI)
    UCB0CTLW0 |= UCMODE_1;      // 4-pin SPI Slave w/ STE
    UCB0CTLW0 &= ~UCMST;        // Slave mode
    UCB0CTLW0 |= UCMSB;         // MSB first
    UCB0CTLW0 &= ~UCCKPH;       // Clock phase (Pi mode 0)
    UCB0CTLW0 &= ~UCCKPL;

    UCB0CTLW0 &= ~UCSWRST;      // release reset
    UCB0IE |= UCRXIE;           // enable RX interrupt
}


void GPS_queryConfig(void)
{
    // Query update rate (should respond $PMTK220,250*xx)
    GPS_sendCommand("$PMTK220?\r\n");
    __delay_cycles(8000000);

    // Query NMEA output settings (should respond $PMTK514,...)
    GPS_sendCommand("$PMTK414\r\n");
    __delay_cycles(8000000);

    // Query baud rate (should respond $PMTK251,115200*xx)
    GPS_sendCommand("$PMTK251?\r\n");
    __delay_cycles(8000000);
}

void Configure_MTK3339(void)
{
    // Step 1: Set update rate to 4 Hz (250 ms)
    GPS_sendCommand("$PMTK220,250*29\r\n");
    __delay_cycles(8000000); // ~1s delay @ 8MHz

    // Step 2: Enable only RMC sentences (position + time)
    GPS_sendCommand("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
    __delay_cycles(8000000);

    // Step 3: Change GPS baud rate to 115200
    GPS_sendCommand("$PMTK251,115200*1F\r\n");
    __delay_cycles(8000000);

    // Step 4: Reconfigure MSP UART to match new baud
    UCA0CTLW0 = UCSWRST;            // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;     // SMCLK source
    UCA0BRW = 4;                    // 8MHz / (16*115200) ≈ 4.34
    UCA0MCTLW = UCOS16 | UCBRF_5 | 0x5500;
    UCA0CTLW0 &= ~UCSWRST;          // Release for operation
    UCA0IE |= UCRXIE;               // Re-enable RX interrupt
}

void Bluetooth_sendChar(char c)
{
    while(!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = c;
}

void Bluetooth_sendString(const char* str)
{
    while(*str)
    {
        Bluetooth_sendChar(*str++);
    }
}

// GPS UART RX Interrupt
#pragma vector=EUSCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_UART_UCRXIFG:
        {
            char c = UCA0RXBUF;

            if(c == '$')
            {
                gps_idx = 0;
                gps_buffer[gps_idx++] = c;
            }
            else if(c == '\n' || c == '\r')
            {
                if(gps_idx > 5)
                {
                    gps_buffer[gps_idx] = '\0';
                    new_sentence = 1;
                }
            }
            else if(gps_idx < GPS_BUFFER_SIZE - 1)
            {
                gps_buffer[gps_idx++] = c;
            }
            break;
        }
        default: break;
    }
}

// Bluetooth UART RX Interrupt (UCA1) Testing only
#pragma vector=EUSCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_UART_UCRXIFG:
        {
            char rx = UCA1RXBUF; // Read the byte

            // When phone writes ANY byte → BLE link is officially ready
            if (!ble_ready)
            {
                ble_ready = 1;
            }
            break;
        }

        default: break;
    }
}

#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_ISR(void)
{
    switch(__even_in_range(UCB0IV, USCI_SPI_UCTXIFG))
    {
        case USCI_SPI_UCRXIFG:
        {
            char c = UCB0RXBUF;

            if (c == '$') {
                ph_idx = 0;
                ph_buffer[ph_idx++] = c;
            }
            else if (c == '#') {
                ph_buffer[ph_idx] = '\0';
                ph_ready = 1;
            }
            else if (ph_idx < sizeof(ph_buffer)-1) {
                ph_buffer[ph_idx++] = c;
            }
            break;
        }
    }
}

