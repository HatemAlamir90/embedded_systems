/* 
 * File: water_pump.h
 * Author: Hatem Al-Amir
 * Comments: defines the pins used to control the pump
 * Revision history: Rev.0
 */

/*This is a guard condition so that contents of this file are not included
  more than once*/
#ifndef WATER_PUMP_H
#define WATER_PUMP_H

#include <xc.h> // include processor files - each processor file is guarded
/*pump control*/
#define PUMP RC0
/*data direction of PUMP*/
#define PUMP_DIR TRISC0
/*the output digital value that turns the pump on*/
#define PUMP_ON 0b1
/*this pin is connected to mode button, that enables the user to select how long
 he/she needs the pump to work for*/
#define MODE RA1
/*data direction control of MODE*/
#define MODE_DIR TRISA1
/*the digital value that is considered as button press*/
#define MODE_SIG 0b0
/*this pin is connected to the start/stop button*/
#define START_STOP RA0
/*data direction control of START_STOP*/
#define START_STOP_DIR TRISA0
/*the digital value that is considered as button press*/
#define START_STOP_SIG 0b0
/*power-on LED, lights when circuit is powered on*/
#define POWER_LED RC1
/*data direction control of POWER_LED*/
#define POWER_LED_DIR TRISC1
/*digital value that lightens POWER_LED*/
#define POWER_LED_ON 0b1
/*start LED, lights when the motor is started*/
#define START_LED RC2
/*data direction control of START_LED*/
#define START_LED_DIR TRISC2
/*digital value that lightens START_LED*/
#define START_LED_ON 0b1
/*number of modes*/
#define MODES 3u
/*mode 1 LED, lights when the user selects mode 1*/
#define MODE1_LED RC3
/*data direction control of MODE1_LED*/
#define MODE1_LED_DIR TRISC3
/*digital value that lightens MODE1_LED*/
#define MODE1_LED_ON 0b1
/*mode 2 LED, lights when the user selects mode 2*/
#define MODE2_LED RC4
/*data direction control of MODE2_LED*/
#define MODE2_LED_DIR TRISC4
/*digital value that lightens MODE2_LED*/
#define MODE2_LED_ON 0b1
/*mode 3 LED, lights when the user selects mode 3*/
#define MODE3_LED RC5
/*data direction control of MODE3_LED*/
#define MODE3_LED_DIR TRISC5
/*digital value that lightens MODE3_LED*/
#define MODE3_LED_ON 0b1
/*how long to run the motor in mode 1*/
#define MODE1_DUR_min 1u
/*how long to run the motor in mode 2*/
#define MODE2_DUR_min 10u
/*how long to run the motor in mode 3*/
#define MODE3_DUR_min 20u
/*software delay time to overcome mechanical button bouncing*/
#define BOUNCE_DELAY_ms 120
/*processor clock frequency*/
#define _XTAL_FREQ 4000000
/*number of timer 1 overflows per second*/
#define OVF_SEC 15u
#endif	/* WATER_PUMP_H */