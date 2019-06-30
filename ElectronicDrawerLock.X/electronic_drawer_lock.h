/*
 * electronic_drawer_lock.h
 * Defines macros for electronic drawer lock project.
 * Created by: Hatem Alamir, Feb.-16-2016
 */
#ifndef ELECTRONIC_DRAWER_LOCK_H
#define ELECTRONIC_DRAWER_LOCK_H

#define ROW0 RB0   //raw 0 in the keypad
#define ROW1 RB1   //raw 1 in the keypad
#define ROW2 RB2   //raw 2 in the keypad
#define ROW3 RB3   //raw 3 in the keypad
#define COL0 RB6    //column 0 in the keypad
#define COL1 RB5    //column 1 in the keypad
#define COL2 RB4    //column 2 in the keypad
#define BUT_PRES 0b1    //the digital value on the button when pressed
#define _XTAL_FREQ 4000000  //Crystal frequency
#define BOUNCE_DELAY_ms 150     //Time delay in ms to overcome bouncing
#define MAX_W_LED_TIME 23  //wrong entry indication LED time, in terms of timer0 overflows
#define LOCK_LO_TIME 23   //The buzzer long sound time, in terms of timer0 overflows
#define BUZ_LO_TIME 23   //The buzzer long sound time, in terms of timer0 overflows
#define BUZ_SH_TIME_ms 100  //buzzer short time
#define POW_ON_TIME_ms 1000 //how long the leds is on once the circuit is powered on
#define CH_PRES 3   //How many presses on 'change-password' button needed to enter the change-password state
#define MAX_PRES_INT 4    //The no-interaction time, in terms of 256 overfolows of timer0
#define LOCK RA0            //The pin that controls the lock
#define LOCK_OPEN 0b1       //tha digital value to open the lock
#define OPEN_LED RA2        //Pin that controls the open-LED
#define OPEN_LED_ON 0b1    //tha digital value that lights the open_LED
#define WRONG_LED RA3           //The pin that controls the wron-LED
#define WRONG_LED_ON 0b1       //tha digital value that lights the wrong_LED
#define BUZZER RA1      //the pin that controlls the buzzer
#define BUZZER_ON 0b1       //the digital value that turns the buzzer on
#define CHANGE_LED RB7  //indicates that the user is changing the password
#define CHANGE_LED_ON 0b1   //tha digital value that lightens the CHANGE_LED
#endif