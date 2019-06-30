//SpeedIndication.c
//Lighting a red LED if a motor speed exceeds 60000 rpm
//Last modified: 31-12-2013
#include <xc.h>     //Important definitions by the compiler
#pragma config OSC=HS,WDT=OFF     //High speed crystal, no watchdog timer
unsigned long int prev_t1;     //previous captured timer1 reading
unsigned char ovfs;     //number of timer1 overflows
void interrupt isr()
{
    unsigned long int cur_t1, tcks;     //current captured timer1 reading, total ticks
    unsigned char temp;     //temporary hold of CCPR1L register
    if(PIR1 & 0x01)     //timer1 overflow interrupt flag is set
    {
        PIR1 &= 0xFE;
        if(ovfs < 2)
            ++ovfs;
    }
    if(PIR1 & 0x04)     //CCP module interrupt flag is set
    {
        PIR1 &= 0xFB;
        if(ovfs < 2)
        {
            temp = CCPR1L;
            cur_t1 = CCPR1H;
            cur_t1 <<= 8;
            cur_t1 |= temp;
            tcks = (((unsigned long int)ovfs)*((unsigned long int)65536))-prev_t1+cur_t1;
            if(tcks < 1000)     //if less than 1 milli second elapsed
                PORTD &= 0xFE;     //light the LED
            else     //else
                PORTD |= 0x01;     //extinguish the LED
            prev_t1 = cur_t1;
            ovfs = 0;
        }
        else
        {
            PORTD |= 0x01;
            ovfs = 0;
        }
    }
}
void main()
{
    RCON &= 0x7F;     //disabling interrupt prioty
    INTCON = 0xC0;     //enable interrupts globally
    PIR1 = 0x00;     //reset CCP and timer1 interrupt flags
    PIE1 = 0x05;     //enable CCP and timer1 interrupts
    IPR1 = 0xFF;     //not needed as priority feature is disapled
    TRISD = 0xFE;     //set portD pin0 as output
    PORTD = 0xFF;     //extinguish the LED
    TRISC = 0xFB;     //pin D2 output,to not activate capture mode
    T1CON = 0x85;     //timer1: 16-bit, internal clock, activate
    T3CON = 0x00;     //timer3: usse timer1 as CCP time source, deactivate timer3
    ECCP1CON = 0x00;     //ECCP disabling
    CCP1CON = 0x05;     //CCP: capture mode every rising edge
    TRISC |= 0x04;     //make pin C2 as input to activate CCP capture mode
    while(1);     //wait for capture mode and timer1 interrupts to take over
}