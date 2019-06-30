#include <xc.h>
#pragma config OSC=RC
#pragma config OSCS=OFF
#pragma config WDT=OFF
float cnt;
void interrupt isr()
{
    if((INTCON & 0b00000100) != 0)
    {
        cnt += 16.384;
        INTCON &= 0b11111011;
    }
}
void main()
{
    INTCON = 0b11100000;
    INTCON2 = 0x00;
    T0CON=0b11001000;
    TRISB = 0x00;
    LATB=0x00;
    PORTB=0x00;
    while(1)
    {
        /*if(cnt>=1000)
        {
            LATB ^= 0x03;
            cnt=0;
        }*/
        LATB=0x00;
        PORTB=0x00;
    }
}