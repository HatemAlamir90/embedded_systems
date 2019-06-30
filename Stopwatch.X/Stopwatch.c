#include <xc.h>
#pragma config OSC=HS, WDT=OFF
unsigned char qr=0,sec=0;
void interrupt isr()
{
    if(PIR1 & 0x01)
    {
        PIR1 &= 0xFE;
        TMR1H=0x0B;
        TMR1L=0xDC;
        ++qr;
        if(qr >= 4)
        {
            ++sec;
            if(sec >= 60)
                sec=0;
            qr=0;
        }
    }
    if(INTCON3 & 0x01)
    {
        INTCON3 &= 0xFE;
        LATD=(~((sec<<2)|qr));
    }
}
void main()
{
    RCON &= 0x7F;
    INTCON = 0xC0;
    INTCON2 = 0x80;
    INTCON3 = 0x48;
    PIR1 = 0x00;
    PIE1 = 0x01;
    IPR1 = 0x01;
    T1CON = 0xA5;
    TMR1H=0x0B;
    TMR1L=0xDC;
    TRISB &= 0xFF;
    TRISD = 0x00;
    LATD=0xFF;
    while(1);
}