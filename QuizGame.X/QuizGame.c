#include <xc.h>
#pragma config OSC=HS,WDT=OFF
void interrupt isr()
{
    unsigned char in;
    unsigned char right=0xF7, wrong=0xEF;
    if(INTCON & 0x01)
    {
        in=PORTB;
        if((in & 0x0F) != 0x0F)
        {
            if((!(in & 0x01)) && (!(in & 0x80)))
                LATD=right;
            else if((!(in & 0x02)) && (!(in & 0x40)))
                LATD=right;
            else if((!(in & 0x04)) && (!(in & 0x20)))
                LATD=right;
            else if((!(in & 0x08)) && (!(in & 0x10)))
                LATD=right;
            else
                LATD=wrong;
        }
        INTCON &= 0xFE;
    }
}
void main()
{
    TRISD=0x00;
    TRISB = 0xFF;
    RCON &= 0x7F;
    INTCON = 0xC8;
    INTCON2 &= 0x01;
    while(1);
}