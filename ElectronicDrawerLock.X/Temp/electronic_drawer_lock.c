/*
 * electronic_drawer_lock.c
 * Created by: Hatem Alamir, Feb.-16-2016.
 * This file implements a program that controls a drawer lock electronic circuit.
 * The lock is password protected, up to 255 numbers; with the capability of
 * password changing.
 * This program is intended for Microchip 'PIC16F688' microcontroller.
 */

#include <xc.h>      //Basic declarations and definitions
#include "electronic_drawer_lock.h"  //project specific declarations

/*Device configurations*/
#pragma config FOSC = INTOSCIO  //internal oscillator with io function on
                                //RA4, and RA5
#pragma config WDTE = ON        //disable watchdog timer
#pragma config PWRTE = ON       //enable power-up timer, 72 ms
#pragma config MCLRE = OFF      //RA3 function is digital IO, MCLR is tied
                                //internally to Vdd
#pragma config BOREN = NSLEEP   //enable brown out detect during operation, and
                                //and disable it during sleep
#pragma config CP = ON          //disable program memory code protection
#pragma config CPD = OFF        //disable data memory code protection
#pragma config IESO = OFF       //disable interanal exteranl switchover
#pragma config FCMEN = OFF      //disable fail-safe clock monitor
__EEPROM_DATA(0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
bit changing;       //set if the user is changing the password
bit wrong;      //set if the user if case of wrong entry
bit open;   //set when the lock is open
bit sleeping; //set prior going to sleep
unsigned char digit_count;      //the number of digits of the saved password
unsigned char digits_entered;   //how many digits the user has entered
unsigned char digit;            //holds a number entered
unsigned char read_digit;            //holds a number read from the EEPROM
unsigned char last_pres_time;   //the time elapsed since the last button pressed
unsigned char w_led_time;       //how long the wrong-LED has been on
unsigned char buz_time; //how long the buzeer has been open
unsigned char ch_pres_cnt;      //how many times the user has pressed 'change'
unsigned short arr[3];

/*Interrupt service routine*/
void interrupt isr(void)
{
    if(TMR1IF)   //if timer 1 overflowed, so about one second elapsed
    {
        /*Reset timer 1*/
        TMR1L = 0x00;
        TMR1H = 0x00;
        if(WRONG_LED == WRONG_LED_ON)
        {
            w_led_time++;
            if(w_led_time >= MAX_W_LED_TIME_s)    //if the wrong-LED has been
            {                                   //on so long
                WRONG_LED = !WRONG_LED_ON;  //extinguish the LED
                w_led_time = 0;     //ready for the next failed trial
            }
        }
        if(BUZZER == BUZZER_ON)
        {
            buz_time++;
            if(buz_time >= BUZ_LO_TIME_s)
            {
                BUZZER = !BUZZER_ON;
                buz_time = 0;
            }
        }
        last_pres_time++;
        if(last_pres_time >= MAX_PRES_INT_s)  //if a long time has elapsed
        {                                   //since the last button pressed
            /*Reset every thing*/
            last_pres_time = 0;
            digits_entered = 0;
            wrong = 0b0;
            ch_pres_cnt = 0;
            TMR1ON = 0b0;
            OPEN_LED = !OPEN_LED_ON;
            CLOSE_LED = !CLOSE_LED_ON;
            WRONG_LED = !WRONG_LED_ON;
            BUZZER = !BUZZER_ON;
            ROW3 = BUT_PRES;
            sleeping = 0b1;
            SLEEP();
        }
        TMR1IF = 0b0;
    }
    if(RAIF)
    {
        __delay_ms(BOUNCE_DELAY_ms);    //wait until the bouncing finishes
        digit = 10;
        last_pres_time = 0;
        if(!TMR1ON)
            TMR1ON = 0b1; 
        if(sleeping)    //if the user has just awakened the device from sleep
        {
            sleeping = 0b0; //be awakened
            CLOSE_LED = CLOSE_LED_ON;
            return; //continue work in main
        }
        if(COL0 == BUT_PRES)
        {
            if(ROW0 == BUT_PRES)
            {
                digit = 1;
            }
            else if(ROW1 == BUT_PRES)
            {
                digit = 4;
            }
            else if(ROW2 == BUT_PRES)
            {
                digit = 7;
            }
            else if(ROW3 == BUT_PRES)   //the user presses *
            {
                /*Cancel whatever the user pressed before*/
                digits_entered = 0;
                wrong = 0b0;
                /*The user is allowed to change the password only if he entered the
                * current one before, this implies that the lock is open.
                */
                if(open)
                {
                    /*If the user is already in the change-state, so he/she wants
                    * to finish this state
                    */
                    if(changing)
                        changing = 0b0;     //clear the state
                    
                    /*If the user is not in the change-state, so make sure that
                    * he/she presses the required number of presses on 'change-
                    * button, to safeguard him/her from random pushes.
                    */
                    else
                    {
                        ch_pres_cnt++;
                        if(ch_pres_cnt >= CH_PRES)
                        {
                            ch_pres_cnt = 0;  //be ready for the next change session
                                            //the password
                            changing = 0b1;     //enter change-state
                            digit_count = 0;    //password digits equals zero until
                                            //the user enters digits
                            //eeprom_write(0, 0);   //save the new number of digits   
                        }
                    }
                    CLRWDT();
                    BUZZER = BUZZER_ON;
                    __delay_ms(BUZ_SH_TIME_ms);
                    BUZZER = !BUZZER_ON;
                }
                RAIF = 0b0; //clear interrupt flag
                __delay_ms(PRES_INT_ms);
                return;
            }
        }
        else if(COL1 == BUT_PRES)
        {
            if(ROW0 == BUT_PRES)
            {
                digit = 2;
            }
            else if(ROW1 == BUT_PRES)
            {
                digit = 5;
            }
            else if(ROW2 == BUT_PRES)
            {
                digit = 8;
            }
            else if(ROW3 == BUT_PRES)
            {
                digit = 0;
            }
        }
        else if(COL2 == BUT_PRES)
        {
            if(ROW0 == BUT_PRES)
            {
                digit = 3;
            }
            else if(ROW1 == BUT_PRES)
            {
                digit = 6;
            }
            else if(ROW2 == BUT_PRES)
            {
                digit = 9;
            }
            else if(ROW3 == BUT_PRES)   //the user presses #
            {
                /*If the user pressed 'open' while not changing the password,
                 * this guarantees that 'open' has no effect while the user
                 * is changing the password.
                 */
                if(!changing)
                {
                    /*if the lock is open, and the user pressed 'open' again*/
                    if(open)
                    {
                        LOCK = !LOCK_OPEN;  //close the lock immediately
                        open = 0b0;
                        OPEN_LED = !OPEN_LED_ON;
                        CLOSE_LED = CLOSE_LED_ON;
                    }
                    else if(wrong)    //if the user entered a wrong password
                    {
                        WRONG_LED = WRONG_LED_ON;   //light the wrong-LED
                        BUZZER = BUZZER_ON;
                    }  //lock is not open
                    else if(digits_entered == digit_count)  //if the user entered
                    {                       //the right number of the right digits
                        CLOSE_LED = !CLOSE_LED_ON;  //extinguish close-LED, if on
                        WRONG_LED = !WRONG_LED_ON;  //extinguish wrong-LED, if on
                        LOCK = LOCK_OPEN;   //open the lock
                        open = 0b1;
                        OPEN_LED = OPEN_LED_ON; //light open-LED
                        CLRWDT();
                        BUZZER = BUZZER_ON;
                        __delay_ms(BUZ_SH_TIME_ms);
                        BUZZER = !BUZZER_ON;
                        __delay_ms(BUZ_SH_TIME_ms);
                        BUZZER = BUZZER_ON;
                        __delay_ms(BUZ_SH_TIME_ms);
                        BUZZER = !BUZZER_ON;
                    }
                    else
                    {
                        WRONG_LED = WRONG_LED_ON;   //light the wrong-LED
                        BUZZER = BUZZER_ON;
                    }
                    wrong = 0b0;
                    digits_entered = 0;
                    ch_pres_cnt = 0;
                }
                RAIF = 0b0; //clear interrupt flag
                __delay_ms(PRES_INT_ms);
                return;
            }
        }
        if(digit < 10)
        {
            
            /*If the user is changing the password, every number press is
             * considered a new password digit, and is saved immediately to
             * the EEPROM.
             */
            if(changing)
            {
                arr[digit_count] = digit;
                digit_count++;
                /*
                if(digit_count < (_EEPROMSIZE - 1))
                {
                    eeprom_write(digit_count, digit);   //save the new digit
                    eeprom_write(0, digit_count);   //update digit count     
                }*/
                CLRWDT();
                BUZZER = BUZZER_ON;
                __delay_ms(BUZ_SH_TIME_ms);
                BUZZER = !BUZZER_ON;
            }
            /*if the user entered any wrong digits, discard the rest until he / she
             * presses #.
             */
            else if(!open)
            {
                if(!wrong)
                {
                    read_digit = arr[digits_entered];
                    digits_entered++;
                    /*read_digit = eeprom_read(digits_entered);
                    /*If the user entered a wrong digit, save your time with the
                     *next number presses*/
                    if(read_digit != digit)
                        wrong = 0b1;
                }
                CLRWDT();
                BUZZER = BUZZER_ON;
                __delay_ms(BUZ_SH_TIME_ms);
                BUZZER = !BUZZER_ON;
            }
            CLRWDT();
            __delay_ms(PRES_INT_ms);
        }
       RAIF = 0b0; //clear interrupt flag
    }
}

/*main routine*/
void main(void)
{
    /*Interrupt configuration
     * INTCON register.
     * GIE = 0b0, disable interrupt while configuring the device
     * PEIE = 0b1, enable peripheral interrupts
     * T0IE = 0b0, mask timer 0 interrupt
     * INTE = 0b0, mask external interrupt
     * RAIE = 0b1, mask port A interrupt-on-change
     * T0IF = 0b0, clear timer 0 interrupt flag
     * INTF = 0b0, clear external interrupt flag
     * RAIF = 0b0, clear GPIO interrupt flag
     */
    INTCON = 0x48;
    
    /*Configure internal oscillator
     * IRCF2:IRCF0 = 0b010, use 250 KHz frequency
     * SCS = = b1, internal clock is used for system clock
     */
    OSCCON = 0x21;
    
    /*Clear peripheral interrupt flags*/
    PIR1 = 0x00;
    
    /*Configure peripheral interrupts
     * EEIE = 0b1, unmask EEPROM interrupt, bit 7
     * TMR1IF = 0b1, unmask timer 1 overflow interrupt, bit 0
     */
    PIE1 = 0x81;
    
    /*Configure option register
     * RAPU = 0b0, Enable port A pull-up resistor to be activated
     * INTEDG = 0b0, interrupt on falling edge, for external interrupt
     * PSA = 0b1, assign pre-scalar to watchdog timer
     * PS2:PS1 = 0b111, 1:128 pre-scalar rate
     */
    OPTION_REG = 0x8F;
    
    /*Configure port A*/
    TRISA = 0x07;   //RA2:RA0 pins are input
    ANSEL = 0x00;   //all port A pins are digital IO
    WPUA = 0x00;    //disable port A weak pull-up resistors
    IOCA = 0x07;    //enable interrupt-on-change on RA3:RA0
    ADCON0 = 0x00;  //disable A/D module not to interfere with port A operation
    CMCON0 = 0x07;  //disable comparator; RA0, RA1, RC0 and RC1 are digital IO
    CMCON1 = 0x00;  //disable RA4 function as timer 1 gate
    VRCON = 0x00;   //disable comparator voltage reference
    
    /*Configure port C*/
    TRISC = 0x00;   //all port C pins are output
    
    
    /*Configure timer 1
     * T1GINV = 0b0, timer 1 gate is active low
     * TMR1GE = 0b0, timer 1 is on regardless of the state of the gate
     * T1CKPS1:T1CKPS0 = 0b11, 1:8 pre-scalar rate
     * T1OSCEN = 0b0, LP oscillator is off
     * T1SYNC = 0b0, ignored as we use the internal clock
     * TMR1CS = 0b0, use internal clock (Fosc/4)
     * TMR1ON = 0b0, stop timer 1 until finishing initialization
     */
    T1CON = 0x30;
    
    /*Initializing all variables*/
    changing = 0b0;
    wrong = 0b0;
    open = 0b0;
    sleeping = 0b0;
    digit_count = eeprom_read(0);       //read the number of password digits
    digits_entered = 0;
    last_pres_time = 0;
    w_led_time = 0;
    ch_pres_cnt = 0;
    LOCK = !LOCK_OPEN;      //lock initial state is being closed 
    OPEN_LED = !OPEN_LED_ON;
    CLOSE_LED = CLOSE_LED_ON;
    WRONG_LED = !WRONG_LED_ON;
    BUZZER = !BUZZER_ON;
    TMR1ON = 0b1;
    ei();       //enable interrupts
    while(1)
    {
        PORTC = (PORTC & 0xF0) | 0x01;
        CLRWDT();
        PORTC = (PORTC & 0xF0) | 0x02;
        CLRWDT();
        PORTC = (PORTC & 0xF0) | 0x04;
        CLRWDT();
        PORTC = (PORTC & 0xF0) | 0x08;
        CLRWDT();
    }
}