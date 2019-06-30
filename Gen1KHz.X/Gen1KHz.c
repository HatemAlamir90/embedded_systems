//Gen1KHz.c
//Generates 1 KHz output
//Last modified: 31/12/2013
#include <xc.h>     //important definitions by the compiler
#pragma config OSC=HS,WDT=OFF     //high speed crystal, no watchdog timer
void interrupt isr()
{
    if(PIR1 & 0x04)     //when 500 ticks, 500 micro seconds, elapsed
    {
        PIR1 &= 0xFB;
        TMR1H = 0x00;     //reset timer1
        TMR1L = 0x00;
    }
}
void main()
{
    RCON &= 0x7F;     //disaple interrupt priority feature
    INTCON = 0xC0;     //enable interrupts globally
    TRISC = 0xFF;     //portC as input, for compare mode not to be activated
    PIR1 = 0x00;     //reset CCP interrupt flag
    IPR1 = 0xFF;     //set CCP interrupt as high priority
    PIE1 = 0x04;     //enable CCP interrupt
    T1CON = 0x85;     //timer1: 16-bit, no prescalar, internal clock, activate
    T3CON = 0x00;     //timer3: use timer1 for compare, deactivate
    CCP1CON = 0x02;     //CCP: compare mode ,toggle output on match
    CCPR1H = 0x01;     //set CCP registers
    CCPR1L = 0xF4;     //to 500
    ECCP1CON = 0x00;     //disable ECCP module
    TRISD = 0xFE;     //pin D0 setting as output
    PORTD = 0x01;     //begin the output cycle high
    TRISC = 0xFB;     //pin C2 (CCP1) output, for compare mode to work
    while(1);     //let interrupts do the job
}
