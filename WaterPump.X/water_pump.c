/*
 * File:   water_pump.c
 * Author: Hatem Al-Amir
 *
 * Created on May 26, 2016, 7:12 PM
 */

/*device configuration*/
/*internal clock, with io functionality on RA4, and RA5*/
#pragma config FOSC = INTOSCIO
/*disable watch-dog timer*/
#pragma config WDTE = OFF
/*enable power-up timer*/
#pragma config PWRTE = ON
/*enable master-clear functionality on RA3*/
#pragma config MCLRE = ON
/*disable code protection functionality*/
#pragma config CP = OFF
/*disable data protection functionality*/
#pragma config CPD = OFF
/*enable brown-out detection during operation, and disable it during sleep*/
#pragma config BOREN = NSLEEP
/*disable internal-external switch-over*/
#pragma config IESO = OFF
/*disable fail-safe clock monitor*/
#pragma config FCMEN = OFF

#include <xc.h>
#include "water_pump.h"
/*how many times timer 1 overflows every minute*/
#define TMR1OVS_MIN 60u
volatile unsigned char mode; //selected mode
volatile unsigned char mot_time; //motor on-time, depending on the mode
volatile unsigned char ovf; //number of timer 1 overflows
volatile unsigned char seconds; //no comment
volatile unsigned char minutes; //for how minutes the motor has been on
volatile bit mot_on; //is the motor off
void main(void) {
    /*configuration of port A*/
    di(); //disable interrupts while configuring the device
    /*interrupt control register
     * GIE, global interrupt enable = 0b0, disable until finishing configuration
     * PEIE, peripheral interrupt enable = 0b1, enable all unmasked ones
     * T0IE, timer 0 overflow interrupt = 0b0, disable
     * INTE, external interrupt on RA2 = 0b1, enable
     * RAIE, port A interrupt on change = 0b1, enable
     * T0IF, timer 0 interrupt flag = 0b0, clear
     * INTF, external interrupt flag = 0b0, clear
     * RAIF, port A interrupt flag = 0b0, clear
     */
    INTCON = 0x48;
    /*peripheral interrupt enable register
     * TMR1IE (PIE1<0>), timer 1 interrupt = 0b1, enable
     * disable all other interrupts
     */
    PIE1 = 0x01;
    /*peripheral interrupt request register
     * clear all requests
     */
    PIR1 = 0x00;
    /*power control register
     * ULPWUE (PCON<5>), ultra low-power wake-up = 0b0, disable
     */
    PCON = 0x10;
    /*option register
     * nRAPU, port A pull-ups = 0b0, enable
     * INTEDG, external interrupt edge = 0b0, falling
     * T0CS, timer 0 clock select = 0b0, internal instruction cycle
     * PSA, pre-scalar assignment = 0b0, to timer 0
     * PS2:0, pre-scalar rate = 0b111, 1:256
     */
    OPTION_REG = 0x07;
    TRISA = 0x3F; //all port A is input
    ANSEL = 0x00; //all analog channels are disabled
    WPUA = 0x03; //enable weak pull-ups for RA0 ,and RA2 only
    IOCA = 0x03; //enable interrupt-on-change for RA0, and RA2 only
    CMCON0 = 0x07; //configure RA0, RA1, RC0, and RC1 as digital io
    /*configure port C*/
    TRISC = 0x00; //all port C pins are output
    /*timer 1 control register
     * T1GINV, timer 1 gate invert bit = 0b1, active high
     * TMR1GE, timer gate enable = 0b0, timer 1 is on regardless of the gate
     * T1CKPS<1:0>, timer 1 input clock pre-scalar = 0b00, 1:1 pre-scalar
     * T1OSCEN, LP oscillator enable = 0b1, disable
     * nT1SYNC, external clock synchronization = 0b0, irrelevant here
     * TMR1CS, clock source selection = 0b0, internal instruction cycle
     * TIMER1ON, timer 1 enable = 0b0, disable until the user begins the motor
     */
    T1CON = 0x80;
    /*comparator configuration register
     * T1GSS (CMCON1<1>), timer 1 gate source select = 0b0, comparator C2 output
     */
    CMCON1 = 0x00;
    /*clear timer registers before enabling interrupts*/
    TMR1H = 0x00;
    TMR1L = 0x00;
    mode = 0;
    mot_time = MODE1_DUR_min;
    ovf = 0;
    seconds = 0;
    minutes = 0;
    mot_on = 0b0;
    PUMP = !PUMP_ON;
    START_LED = !START_LED_ON;
    MODE1_LED = MODE1_LED_ON;
    MODE2_LED = !MODE2_LED_ON;
    MODE3_LED = !MODE3_LED_ON;
    POWER_LED = POWER_LED_ON;
    ei(); //enable interrupts upon finishing configuration
    while(1);
}

/*interrupt service routine*/
void interrupt isr(void)
{
    /*when the user pushes mode button, select the next mode*/
    if(RAIF)
    {
        if(MODE_SIG == MODE)
        {
            mode = (++mode) % MODES;
            switch(mode)
            {
                case 0: //mode 1
                    mot_time = MODE1_DUR_min;
                    MODE2_LED = !MODE2_LED_ON;
                    MODE3_LED = !MODE3_LED_ON;
                    MODE1_LED = MODE1_LED_ON;
                    break;
                case 1: //mode 2
                    mot_time = MODE2_DUR_min;
                    MODE1_LED = !MODE1_LED_ON;
                    MODE3_LED = !MODE3_LED_ON;
                    MODE2_LED = MODE2_LED_ON;
                    break;
                case 2: //mode 3
                    mot_time = MODE3_DUR_min;
                    MODE1_LED = !MODE1_LED_ON;
                    MODE2_LED = !MODE2_LED_ON;
                    MODE3_LED = MODE3_LED_ON;
                    break;
                default:
                    /*in case of any error, shut down the motor, and extinguish all 
                     *mode-LEDs, to inform the user that an error occurred
                     */
                    mot_time = 0;
                    MODE1_LED = !MODE1_LED_ON;
                    MODE2_LED = !MODE2_LED_ON;
                    MODE3_LED = !MODE3_LED_ON;
            }
        }
        /*if the user pushes start/stop button, reverse the state of the motor*/
        if(START_STOP_SIG == START_STOP)
        {
            if(mot_on)
            {
                PUMP = !PUMP_ON;
                TMR1ON = 0b0;
                mot_on = 0b0;
                START_LED = !START_LED_ON;
            }
            else
            {
                ovf = 0x00;
                seconds = 0x00;
                minutes = 0x00;
                TMR1L = 0x00;
                TMR1H = 0x00;
                PUMP = PUMP_ON;
                TMR1ON = 0b1;
                mot_on = 0b1;
                START_LED = START_LED_ON;
            }
        }
        __delay_ms(BOUNCE_DELAY_ms);
        PORTA; //read port A to end mismatch condition, refer to the data-sheet
        RAIF = 0b0; //clear interrupt flag, to avoid 
    }
    /*when timer 1 overflows, count seconds, and minutes*/
    if(TMR1IF)
    {
        ovf++;
        if(ovf >= OVF_SEC)
            seconds++;
        if(seconds >= TMR1OVS_MIN)
        {
            minutes++;
            if(minutes >= mot_time)
            {
                PUMP = !PUMP_ON;
                START_LED = !START_LED_ON;
                mot_on = 0b0;
            }
        }
        TMR1IF = 0b0;
    }
}