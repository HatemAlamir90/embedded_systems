#include <xc.h>
#pragma config OSC=HS,WDT=OFF
void interrupt isr()
{
    if(INTCON & 0x04)
    {
        //PORTD ^= 0xC0;
        LATD ^= 0xC0;
        INTCON &= 0xFB;
        TMR0H=0xFE;
        TMR0L=0x79;
        //T0CON |= 07;
    }
}
void main()
{
    TRISD = 0x00;
    RCON &= 0x7F;
    INTCON = 0xE0;
    T0CON = 0x97;
    TMR0H = 0xFE;
    //PORTD = 0x7F;
    TMR0L = 0x79;
    //T0CON |= 0x07;
    LATD = 0x7F;
    while(1);
}
