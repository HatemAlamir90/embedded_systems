//TestSPI.c
//Tests the operation of the MSSP module in SPI mode, displaying data of portB on portD
#include <xc.h>     //imporatant declarations to the compiler
#define XTAL_FREQ 4000000
#pragma config OSC=HS,WDT=OFF     //configure the device for high speed oscillator, no WDT
void interrupt isr()     //interrupt service routine
{
    volatile unsigned char temp;     //holds data received
    if(PIR1 & 0x08)     //if MSSP interrupt occured
    {
        PIR1 &= 0xF7;     //clear the flag
        temp = SSPBUF;     //read the data from the buffer
        LATD = temp;     //output data to portD
    }
}
void main()
{
    volatile unsigned char in,tem;     //holds previous portB value, temporary storage
    RCON &= 0x7F;     //clear interrupt priority enable bit
    INTCON = 0xC0;     //enable interrupts globally, enable peripheral interrupts
    PIR1 = 0x00;     //clear MSSP interrupt flag
    IPR1 = 0xFF;     //set MSSP interrupt priority to high
    PIE1 = 0x08;     //enable MSSP interrupt
    INTCON2 = 0x00;     //enable portB pull-up resistors
    TRISB = 0xFF;     //make portB pins input for pull-ups to work
    TRISD = 0x00;     //make portD pins output to control output LEDs
    SSPSTAT &= 0x3F;     //MSSP: sample the input at the middle of the output time, phase zero
    SSPCON1 = 0x22;     //MSSP: no collision (yet), no overflow (yet), enable MSSP module, Master mode with Fosc/64
    TRISC = 0xD3;     //portC: SDO as output, SDI as input, SCK as output, pin C2 as output
    while(1)
    {
        if(in != PORTB)     //if a change occured in input
        {
            in = PORTB;     //read input port
            if(SSPSTAT & 0x01)     //if for any reason there was still the buffer full
            {
                tem = SSPBUF;     //read the buffer
                LATD = tem;     //output it to portD
            }
            SSPBUF = in;     //send it via SPI
            while(SSPCON1 & 0x80)     //if a collision occured
            {
                SSPCON1 &= 0x7F;     //remove the flag
                SSPBUF = in;     //try to send again
            }
            LATC &= 0xFB;     //light indication LED
            __delay_ms(100);     //delay 0.2 sec
            __delay_ms(100);
            LATC |= 0x04;     //extinguish the LED
            __delay_ms(100);     //delay 0.2 sec
            __delay_ms(100);
        }
    }
}
