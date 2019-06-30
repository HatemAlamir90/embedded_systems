/*
 * File:   auto_washing_machine.c
 * Author: Hatem Al-Amir
 *
 * Created on May 9, 2016, 19:16 PM
 * Comments: This file implements a control system for "Ideal Zanussi
 *           Z 650 XS" automatic washing machine. This project uses Microchip
 *           PIC16F877A microcontroller.
 */


#include <xc.h>
#include "auto_washing_machine.h"

/*Microcontroller configuration*/

/*Oscillator selection, external oscillator*/
#pragma config FOSC = XT
/*enable watchdog timer, timeout is about 17.5 ms, without pre-scalar
 * at 25 degree Celsius*/
#pragma config WDTE = OFF
/*enable power-up timer, fixed 72 ms*/
#pragma config PWRTE = ON
/*enable brown-out reset, when Vdd is at 4.0 volts or lower for 100 us or more*/
#pragma config BOREN = ON
/*disable low voltage in-circuit serial programming*/
#pragma config LVP = OFF
/*protect EEPROM memory from reading from the outside, refer to the device
 * data sheet for more information*/
#pragma config CPD = OFF
/*EEPROM write enable, only from 1000h to 1FFF can be written by code*/
#pragma config WRT = OFF
/*disable in-circuit debugger*/
#pragma config DEBUG = OFF
/*protect code program memory from reading from the outside, refer to the device
 * data sheet for more information*/
#pragma config CP = OFF

/*set to rotate the motor*/
volatile bit mot_rot;
/*set to select high speed, otherwise low speed*/
volatile bit mot_high;
/*used to count seconds*/
volatile unsigned char seconds;
/*initialize the program*/
void init(void);

/*its clear, what is the following function*/
void main(void)
{
    di(); //disable interrupts
    /*Configure interrupts
     * GIE = 0b0, disable interrupts until finishing configuration
     * PEIE = 0b1, enable peripheral interrupts
     * TMR0IE = 0b0, disable timer 0 overflow interrupt
     * INTE = 0b1, enable RB0/INT external interrupt
     * TMR0IF = 0b0, clear timer 0 overflow interrupt flag
     * INTF = 0b0, clear RB0/INT interrupt flag
     * RBIF = 0b0, clear port B change interrupt flag
     */
    INTCON = 0x58;
    /*
     * TMR1IE = 0b1, enable timer 1 overflow interrupt
     * TMR2IE = 0b1, enable timer 2 overflow interrupt
     * The rest of peripheral interrupts are disabled
     */
    PIE1 = 0x03;
    PIE2 = 0x00;
    /*Clear peripheral interrupt flags*/
    PIR1 = 0x00;
    PIR2 = 0x00;
    
    /*Configure option register
     * nRBPU = 0b0, enable port B internal pull-up resistors
     * INTEDGE = 0b0, external interrupt is on falling edge
     * T0CS = 0b0, timer 0 is on the internal instruction cycle
     * T0SE = 0b0, irrelevant here
     * PSA = 0b1, assign the following pre-scalar to the watchdog timer
     * PS2:PS0 = 0b111, watchdog timer pre-scalar rate is 1:128
     */
    OPTION_REG = 0x0F;
    
    /*Configure port A and its associated peripherals */
    ADCON0 = 0x00;  //shut off A/D module
    ADCON1 = 0x07;  //all port A pins are digital i/o
    CMCON = 0x07;   //shut off analog comparator module
    CVRCON = 0x00;  //power down comparator voltage reference module
    SSPEN = 0b0;    //disable MSSP module
    TRISA = 0x00;   //set port A pins as output
    
    /*Configure port B*/
    TRISB = 0xFF;   //all port B pins are input
    
    /*Configure port C and its associated peripherals*/
    CCP1CON = 0x00; //disable CCP1 module
    CCP2CON = 0x00; //disable CCP2 module
    SPEN = 0b0; //disable USART module
    TRISC = 0x00;   //all port C pins are output
    
    /*Configure port D*/
    TRISD = 0x00;   //all port D pins are output
    
    /*Configure port E and its associated peripherals*/
    TRISE = 0x07;   //all port E pins are input; disable PSP module
    
    TMR0 = 0x00;    //clear timer 0
    
    /*Configure timer 1
     * T1CKPS1:T1CKPS0 = 0b11, 1:8 pre-scalar
     * T1OSCEN = 0b0, shut-off timer 1 oscillator (working on the internal one)
     * nT1SYNC: irrelevant
     * TMR1SC = 0b0, internal clock
     * TMR1ON = 0b0, stop timer 1 until we finish the configuration
     */
    T1CON = 0x30;
    
    /*Configure timer 2*/
    T2CON = 0x7B;   //timer 2 is off
    PR2 = 0xFF;
    
    init();    //initialize the system
    ei(); //enable interrupts
    /*check for a selected state, foreverrrrrrr*/
    while(1);
}

/*
 * void init(void);
 * terminates the sequence, and resets the program to its initial state.
 */
void init(void)
{
    
    seconds = 0x00;
    
    /*initialize the motor state*/
    MOTOR_POWER = !MOTOR_ON;
    MOTOR_SPEED = !MOTOR_HIGH;
    MOTOR_DIRECTION = MOTOR_RIGHT;
    mot_rot = 0b0;
    mot_high = 0b0;
    /*stop the drain pump*/
    DRAIN_PUMP = !DRAIN_PUMP_ON;
    /*extinguish the heater*/
    HEATER = !HEATER_ON;
    /*unlock the door*/
    LOCK_DOOR = !LOCK_DOOR_SIG;
    
    TMR1L = 0x00;   //clear timer 1 low significant byte
    TMR1H = 0x00;   //clear timer 1 high significant byte
    TMR1ON = 0b1;   //begin seconds timer
}

/*Interrupt service routine*/
void interrupt isr(void)
{
    static bit mot_is_on;
    static bit mot_is_left;
    /*if timer 1 has overflowed, so about a half of a second have elapsed*/
    if(TMR1IF == 0b1)
    {
        
        seconds++;
            /*if the motor is rotating on low speed, every ROT_DIR_TIMEOUT
              half-seconds, stop/change direction of the motor*/
            if(mot_rot && !mot_high && ((seconds % ROT_DIR_TIMEOUT) == 0))
            {
                        if(mot_is_on)
                        {
                            MOTOR_POWER = !MOTOR_ON;
                            mot_is_on = 0b0;
                        }
                        else
                        {
                            mot_is_left ^= 0b1;
                            if(mot_is_left)
                                MOTOR_DIRECTION = !MOTOR_RIGHT;
                            else
                                MOTOR_DIRECTION = MOTOR_RIGHT;
                            MOTOR_POWER = MOTOR_ON;
                            mot_is_on = 0b1;
                        }
            }
            else if(!mot_rot && mot_is_on)
            {
                MOTOR_POWER = !MOTOR_ON;
                mot_is_on = 0b0;
            }
            TMR1IF = 0b0;
    }
    /*handler of start/stop button*/
    if(INTF)
    {
        if(!mot_rot)
        {
            MOTOR_DIRECTION = MOTOR_RIGHT;
            mot_is_left = 0b0;
            MOTOR_POWER = MOTOR_ON;
            mot_rot = 0b1;
            mot_is_on = 0b1;
        }
        else
        {
            MOTOR_POWER = !MOTOR_ON;
            mot_rot = 0b0;
            mot_is_on = 0b0;
        }
        __delay_ms(BOUNCING_DELAY_ms);
        INTF = 0b0;
    }
    /*handler of half-load, no-spin, state-select, and skip-select button*/
    if(RBIF)
    {
        /*handler of no-spin button*/
        if(0b0 == RB4)
        {
            RC3 ^= 0b1;
            mot_high ^= 0b1;
            if(0b1 == mot_high)
                {
                    if(mot_is_left)
                    {
                        MOTOR_POWER = !MOTOR_ON;
                        __delay_ms(LOW_HIGH_DELAY_ms);
                    }
                    MOTOR_DIRECTION = MOTOR_RIGHT;
                    mot_is_left = 0b0;
                    MOTOR_SPEED = MOTOR_HIGH;
                    MOTOR_POWER = MOTOR_ON;
                    mot_rot = 0b1;
                    mot_is_on = 0b1;
                }
                else
                {
                    MOTOR_SPEED = !MOTOR_HIGH;
                    MOTOR_POWER = !MOTOR_ON;
                    mot_rot = 0b0;
                    mot_is_on = 0b0;
                }
        }
        RBIF = 0b0;
    }
}