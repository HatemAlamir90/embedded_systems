//TestWDT.c
//Uses watchdog timer to restart the processor hangs waiting for pin B7 to be pulled high
#include <xc.h>     //declarations used by the compiler
#pragma config OSC=HS,WDT=ON,WDTPS=128     //high speed oscillator, enable WDT with postscalar of 4
#define _XTAL_FREQ 4000000     //define processor frequency variable used by delay macros
void main()
{
    TRISB = 0xFF;     //make portB pins as input for pullup resistors to work
    INTCON2 = 0x00;     //enable portB pullup resistors
    TRISD = 0xFE;     //make pin D0 as output to control the LED
    LATD = 0xFE;          //light the LED for
    __delay_ms(197);     //2 seconds after reset
    __delay_ms(197);
    __delay_ms(197);
    __delay_ms(197);
    __delay_ms(197);
    __delay_ms(197);
    __delay_ms(197);
    __delay_ms(197);
    __delay_ms(197);
    CLRWDT();
    __delay_ms(197);
    LATD = 0xFF;     //extinguish the LED
    while(1)
    {
        CLRWDT();
        while(!(PORTB & 0x80));     //wait for pin B7 to be pulled high
        LATD = 0xFE;         //light the LED
        __delay_ms(100);     //for 0.2
        __delay_ms(100);     //second
        LATD = 0xFF;         //extinguish the LED
        __delay_ms(100);     //for 0.2
        __delay_ms(100);     //second
    }
}

