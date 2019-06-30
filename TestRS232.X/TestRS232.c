//TestRS232.c
//Receives one byte frome PC or ohter MCU, then echoes the byte to the source
#include <xc.h>     //important for the compiler
#pragma config OSC=HS,WDT=OFF     //high speed oscillator, no WDT
void interrupt isr()     //inerrupt service routine
{
    volatile unsigned char rcb;     //received byte
    if(RCIF)     //if USART receive interrupt flag set
    {
        RCIF = 0b0;     //clear the flag
        rcb = RCREG;     //read the received byte
        LATD = rcb;     //display the byte sent on portD
        if(OERR)     //if over-run error occured
            CREN = 0b0;     //diable receive
        TX9 = 0b0;     //choose 8-bit mode for transmission
        TXEN = 0b1;     //enable transmission
        TXREG = rcb;     //send the received byte again to the source
        if(!CREN)     //if receive was disabled
            CREN = 0b1;     //enable it
    }
}
void main()
{
    SPBRG = 0x19;     //baude rate generator value to produce 9600 bps
    BRGH = 0b1;     //enable high baud rate
    SPEN = 0b1;     //enable USART
    SYNC = 0b0;     //asynchronous mode
    IPEN = 0b0;     //disable interrupt priority
    GIE = 0b1;     //enable interrupts globally
    PEIE = 0b1;     //enable peripheral interrupts
    RCIE = 0b1;     //enable receive interrupt
    RX9 = 0b0;     //8-bit mode receive
    ADDEN = 0b0;     //diable adderss detect
    CREN = 0b1;     //enable continuous receive
    TXIE = 0b0;     //diable receive interrupt
    while(1);     //wait for the host to send something
}