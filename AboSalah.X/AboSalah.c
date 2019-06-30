//AboSalah.c
//Motor control: forward, reverse
#include <xc.h>     //important definitions by the compiler
#pragma config OSC=HS,WDT=OFF     //configure for: High Speed osc., no watchdog timer
void interrupt isr()     //interrupt service routine
{
    unsigned char in;
    if(INTCON & 0x01)     //if a change on portB
    {
        in = PORTB;     //read portB
        INTCON &= 0xFE;     //clear the interrupt flag
        if((in & 0xC0) != 0xC0)
        {
            TRISD = 0xFF;     //make portD input for EPWM not to be active
            PR2 = 0x28;     //set the period of EPWM as 25 KHz
            ECCPR1L = 0x05;     //write the high significant 8 bits of the duty cycle
            if(!(PORTB & 0x80))
            {
                ECCP1CON = 0x4C;     //configure ECCP module: PWM, full-bridge forward, total duty cycle of 50%
            }
            else if(!(PORTB & 0x40))
            {
                ECCP1CON = 0xCC;     //configure ECCP module: PWM, full-bridge reverse, total duty cycle of 50%
            }
            PIR1 = 0x00;     //clear timer2 interrupt flag
            T2CON = 0x04;     //enable timer2, no prescalar nor postscalar
            while(!PIR1);     //wait for ECCP to comlete one PWM cycle
            TRISD &= 0x0F;     //make P1A, P1B, P1C, and P1D pins as output for PWM to start
        }
    }
}
void main()
{
    RCON &= 0xEF;     //disable interrupt priority feature
    INTCON = 0xC8;     //enable interrupts, peripherals interrupts, portB change interrupt
    INTCON2 = 0x01;     //turn on portB pull-up resistors, set portB on change interrupt priority as high
    TRISB = 0xFF;     //make portB as input for portB interrupt on change to work
    while(1);
}