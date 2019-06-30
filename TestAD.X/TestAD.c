//TestAD.c
//Tests the operation of the A/D module; display the value on portB and portD
#include <xc.h>     //important definitions and declarations
#define _XTAL_FREQ 4000000
#pragma config OSC=HS,WDT=OFF     //high speed oscillator, no WatchDogTimer
void interrupt isr()     //interrupt service routine
{
    if(ADIF)
    {
        ADIF = 0b0;     //clear the flag
        LATB = ~ADRESH;     //display the high byte on portB
        LATD = ~ADRESL;     //display the low byte on portD
    }
}
void main()
{
    ADCON1 &= 0xF0;
    ADCON1 |= 0x0E;     //configure AN0 as digital input
    ADCON0 &= 0xC7;     //select channel0
    ADCS0 = 0b1;     //select internal
    ADCS1 = 0b0;     //clock with
    ADCS2 = 0b1;     //Tad = 16 * Tosc
    ADFM = 0b0;     //lift-justify the result
    TRISA = 0xFF;     //portA input, for analog input to be represented right
    TRISE |= 0x07;     //portE input
    TRISB = 0x00;     //portB output to control the LEDs
    TRISD = 0x3F;      //pins D7 and D6 input to control the LEDs
    ADON = 0b1;     //activate A/D module
    IPEN = 0b0;     //disable interrupt priority feature
    ADIF = 0b0;     //clear A/D interrupt flag
    ADIE = 0b1;     //enable A/D interrupt
    ADIP = 0b1;     //set A/D interrupt priority as high
    PIR2 = 0x00;     //clear EEPROM, comparator, and ECCP interrupt flags,
    PIE2 = 0x00;     //and disable their interrupts,
    IPR2 = 0x00;     //with low priority for them
    PEIE = 0b1;     //enable peripheral interrupts
    GIE = 0b1;     //enable interrupts globally
    LATB = 0xFF;     //extinguish the LEDs initially
    LATD = 0xFF;
    GO_nDONE = 0b0;
    while(1)
    {
        while(GO_nDONE);     //wait until the previous conversion finishes
        __delay_us(15);     //wait for aquisition time
        GO_nDONE = 0b1;     //start a new conversion
    }
}