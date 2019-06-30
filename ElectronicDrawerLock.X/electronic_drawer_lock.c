/*
 * electronic_drawer_lock.c
 * Created by: Hatem Alamir, Feb.-16-2016.
 * This file implements a program that controls a drawer lock electronic circuit.
 * The lock is password protected, up to 255 numbers; with the capability of
 * password changing.
 * This program is intended for Microchip 'PIC16F84A' microcontroller.
 */

#include <xc.h>      //Basic declarations and definitions
#include "electronic_drawer_lock.h"  //project specific declarations
#define MAS_KEY_SIZE 8u

/*Device configurations*/
#pragma config FOSC = HS  //internal oscillator with io function on
                                //RA4, and RA5
#pragma config WDTE = OFF        //ensable watchdog timer
#pragma config PWRTE = ON       //enable power-up timer, 72 ms
#pragma config CP = ON          //disable program memory code protection

__EEPROM_DATA(0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
volatile bit changing;       //set if the user is changing the password
volatile bit wrong;      //set if the user if case of wrong entry
volatile bit open;   //set when the lock is open
volatile bit sleeping; //set prior going to sleep
volatile bit but_pres;   //set if a button is still pressed
volatile unsigned char digit_count;      //the number of digits of the saved password
volatile unsigned char digits_entered;   //how many digits the user has entered
volatile unsigned char digit;            //holds a number entered
volatile unsigned char read_digit;            //holds a number read from the EEPROM
volatile unsigned char last_pres_time_l;   //the time elapsed since the last button
volatile unsigned char last_pres_time_h;   //pressed, in terms of timer0 overflows
volatile unsigned char w_led_time;       //how long the wrong-LED has been on
volatile unsigned char buz_time; //how long the buzeer has been open
volatile unsigned char lock_time; //how long the lock has been open
volatile unsigned char ch_pres_cnt;      //how many times the user has pressed 'change'
volatile unsigned char master_key[] = {4, 5, 0, 7, 3, 9, 2, 7};

/*Interrupt service routine*/
void interrupt isr(void)
{
    if(T0IF)   //if timer0 overflowed, so about 66 ms elapsed
    {
        T0IF = 0b0; //clear interrupt flag
        if(WRONG_LED == WRONG_LED_ON)
        {
            w_led_time++;
            if(w_led_time >= MAX_W_LED_TIME)    //if the wrong-LED has been
            {                                   //on so long
                WRONG_LED = !WRONG_LED_ON;  //extinguish the LED
                BUZZER = !BUZZER_ON;
                w_led_time = 0;     //ready for the next failed trial
            }
        }
        if(BUZZER == BUZZER_ON)
        {
            if(changing == 0b0)
                buz_time++;
            if(buz_time >= BUZ_LO_TIME)
            {
                BUZZER = !BUZZER_ON;
                buz_time = 0;
            }
        }
        if(LOCK == LOCK_OPEN)
        {
            lock_time++;
            if(lock_time >= LOCK_LO_TIME)
            {
                LOCK = !LOCK_OPEN;
                OPEN_LED = OPEN_LED_ON;
                lock_time = 0;
            }
        }
        if(!but_pres)
            last_pres_time_l++;
        if(last_pres_time_l >= 255)
            last_pres_time_h++;
        if(last_pres_time_h >= MAX_PRES_INT)  //if a long time has elapsed
        {                                   //since the last button pressed
            /*Reset every thing*/
            last_pres_time_h = 0;
            digits_entered = 0;
            wrong = 0b0;
            ch_pres_cnt = 0;
            OPEN_LED = !OPEN_LED_ON;
            WRONG_LED = !WRONG_LED_ON;
            BUZZER = !BUZZER_ON;
            ROW0 = !BUT_PRES;
            ROW1 = !BUT_PRES;
            ROW2 = !BUT_PRES;
            ROW3 = BUT_PRES;
            sleeping = 0b1;
            SLEEP();
        }
    }
    if(RBIF)
    {
        __delay_ms(BOUNCE_DELAY_ms);    //wait until the bouncing finishes
        digit = 10;
        last_pres_time_l = 0;
        last_pres_time_h = 0; 
        if(sleeping)    //if the user has just awakened the device from sleep
        {
            sleeping = 0b0; //be awakened
            return; //continue work in main
        }
        if(COL0 == BUT_PRES)
        {
            but_pres = 0b1;
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
                    {
                        changing = 0b0;     //clear the state
                        CHANGE_LED = !CHANGE_LED_ON;
                    }
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
                            changing = 0b1;     //enter change state
                            digit_count = 0;    //password digits equals zero until
                                            //the user enters digits
                            eeprom_write(0, 0);   //save the new number of digits
                            CHANGE_LED = CHANGE_LED_ON;
                        }
                    }
                    BUZZER = BUZZER_ON;
                    __delay_ms(BUZ_SH_TIME_ms);
                    BUZZER = !BUZZER_ON;
                }
                RBIF = 0b0; //clear interrupt flag
                return;
            }
        }
        else if(COL1 == BUT_PRES)
        {
            but_pres = 0b1;
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
            but_pres = 0b1;
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
                if(changing == 0b0)
                {
                    /*if the lock is open, and the user pressed 'open' again*/
                    if(open)
                    {
                        LOCK = !LOCK_OPEN;  //close the lock immediately
                        open = 0b0;
                        OPEN_LED = !OPEN_LED_ON;
                    }
                    else if(wrong)    //if the user entered a wrong password
                    {
                        w_led_time = 0;
                        WRONG_LED = WRONG_LED_ON;   //light the wrong-LED
                        BUZZER = BUZZER_ON;
                    }  //lock is not open
                    /*if the user entered the right number of the right digits
                     * of the saved password or the master key
                     */
                    else if(digits_entered == digit_count || digits_entered == MAS_KEY_SIZE)
                    {
                        WRONG_LED = !WRONG_LED_ON;  //extinguish wrong-LED, if on
                        LOCK = LOCK_OPEN;   //open the lock
                        open = 0b1;
                        lock_time = 0;
                    }
                    else
                    {
                        w_led_time = 0;
                        WRONG_LED = WRONG_LED_ON;   //light the wrong-LED
                        BUZZER = BUZZER_ON;
                    }
                    wrong = 0b0;
                    digits_entered = 0;
                    ch_pres_cnt = 0;
                }
                RBIF = 0b0; //clear interrupt flag
                return;
            }
        }
        else
        {
            but_pres = 0b0;
            RBIF = 0b0;
            return;
        }
        if(digit < 10)
        {
            /*If the user is changing the password, every number press is
             * considered a new password digit, and is saved immediately to
             * the EEPROM.
             */
            if(changing)
            {
                
                if(digit_count < (_EEPROMSIZE - 2))
                {
                    digit_count++;
                    eeprom_write(digit_count, digit);   //save the new digit
                    eeprom_write(0, digit_count);   //update digit count
                    BUZZER = BUZZER_ON;
                    __delay_ms(BUZ_SH_TIME_ms);
                    BUZZER = !BUZZER_ON;
                }
                
            }
            /*if the user entered any wrong digits, discard the rest until he / she
             * presses #.
             */
            else if(!open)
            {
                if(!wrong)
                {
                    digits_entered++;
                    read_digit = eeprom_read(digits_entered);
                    /*If the user entered a wrong digit, save your time with the
                     *next number presses*/
                    if(read_digit != digit)
                    {
                        /*Check if the user is entering the master key*/
                        if(digits_entered > MAS_KEY_SIZE || digit != master_key[digits_entered - 1])
                            wrong = 0b1;
                    }
                }
                BUZZER = BUZZER_ON;
                __delay_ms(BUZ_SH_TIME_ms);
                BUZZER = !BUZZER_ON;
            }
        }
       RBIF = 0b0; //clear interrupt flag
    }
    if(EEIE)//if the EEPROM has finished writing
    {
        BUZZER = !BUZZER_ON;
        EEIE = 0b0; //just clear the interrupt flag
    }
}

/*main routine*/
void main(void)
{
    /*Interrupt configuration
     * INTCON register.
     * GIE = 0b0, disable interrupt while configuring the device
     * EEIE = 0b1, enable EEPROM write completion interrupt
     * T0IE = 0b1, unmask timer 0 interrupt
     * INTE = 0b0, mask external interrupt
     * RBIE = 0b1, unmask port B interrupt-on-change
     * T0IF = 0b0, clear timer 0 interrupt flag
     * INTF = 0b0, clear external interrupt flag
     * RBIF = 0b0, clear GPIO interrupt flag
     */
    INTCON = 0x68;
    
    /*Configure option register
     * RBPU = 0b1, disable port B pull-up resistor to be activated
     * INTEDG = 0b0, external interrupt is on falling edge, irrelevant
     * T0CS = 0b0, timer0 clock select, internal instruction clock cycle
     * T0SE = 0b0, timer0 external source edge selection, irrelevant
     * PSA = 0b0, assign pre-scalar to watchdog timer
     * PS2:PS1 = 0b111, 1:256 pre-scalar rate
     */
    OPTION_REG = 0x87;
    
    /*Configure port A*/
    PORTA = 0x00;   //clear port A upon configuration
    TRISA = 0x00;   //all port A pins are output
    
    /*Configure port B*/
    PORTB = 0x00;   //clear port B upon configuration
    TRISB = 0x70;   //RB3:RB0 are output, RB4:RB7 are input
    
    /*Initializing all variables*/
    changing = 0b0;
    wrong = 0b0;
    open = 0b0;
    sleeping = 0b0;
    but_pres = 0b0;
    digit_count = eeprom_read(0);       //read the number of password digits
    digits_entered = 0;
    last_pres_time_l = 0;
    last_pres_time_h = 0;
    w_led_time = 0;
    buz_time = 0;
    lock_time = 0;
    ch_pres_cnt = 0;
    LOCK = !LOCK_OPEN;      //lock initial state is being closed 
    OPEN_LED = OPEN_LED_ON;
    WRONG_LED = WRONG_LED_ON;
    CHANGE_LED = CHANGE_LED_ON;
    __delay_ms(POW_ON_TIME_ms);
    OPEN_LED = !OPEN_LED_ON;
    WRONG_LED = !WRONG_LED_ON;
    CHANGE_LED = !CHANGE_LED_ON;
    BUZZER = !BUZZER_ON;
    ei();       //enable interrupts
    while(1)
    {
        ROW1 = !BUT_PRES;
        ROW2 = !BUT_PRES;
        ROW3 = !BUT_PRES;
        ROW0 = BUT_PRES;
        NOP();  //allow port B change to be detected to interrupt the processor
        while(but_pres);
        
        ROW0 = !BUT_PRES;
        ROW2 = !BUT_PRES;
        ROW3 = !BUT_PRES;
        ROW1 = BUT_PRES;
        NOP();
        while(but_pres);
        
        ROW0 = !BUT_PRES;
        ROW1 = !BUT_PRES;
        ROW3 = !BUT_PRES;
        ROW2 = BUT_PRES;
        NOP();
        while(but_pres);
        
        ROW0 = !BUT_PRES;
        ROW1 = !BUT_PRES;
        ROW2 = !BUT_PRES;
        ROW3 = BUT_PRES;
        NOP();
        while(but_pres);
    }
}