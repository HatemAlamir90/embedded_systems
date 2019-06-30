/* 
 * File: auto_washing_machine.h
 * Author: Hatem Alamir
 * Comments: This file includes declarations to be used in AutoWashingMachine
 *           project, which implements a control system for "Ideal Zanussi
 *           Z 650 XS" automatic washing machine. This project uses Microchip
 *           PIC16F877A microcontroller.
 * Revision history: Rev.0
 */

// This is a guard condition so that contents of this file are not included
// more than once.
#ifndef AUTO_WASHING_MACHINE_H
#define	AUTO_WASHING_MACHINE_H

#define SOLENOID1 RA0   //Solenoid 1, delivers water for detergent dispenser
#define SOLENOID1_DIR TRISA0    //Solenoid 1 data direction (0b0, output)
#define SOLENOID2 RA1   //Solenoid 2, delivers water for detergent dispenser
#define SOLENOID2_DIR TRISA1    //Solenoid 1 data direction (0b0, output)
#define SOLENOID3 RA2   //Solenoid 3, delivers water for detergent dispenser
#define SOLENOID3_DIR TRISA2    //Solenoid 1 data direction (0b0, output)
#define SOLENOID_ON 0b1     //The digital value to drive the solenoid on
#define HEATER RD0      //Heater, used to warm the water up to the temperature
                        //set by the user
#define HEATER_DIR TRISD0      //Heater data direction (output)
#define HEATER_ON 0b1       //The digital value that turns the heater on
#define VESSEL_FULL RB1     //Receives the full signal from the pressure switch
#define VESSEL_FULL_DIR TRISB1     //The data direction of VESSEL_FULL (input)
#define VESSEL_IS_FULL 0b0      //The input value when the vessel is full
#define VESSEL_HALF RB2 //Receives the half-full signal from the pressure switch
#define VESSEL_HALF_DIR TRISB2     //The data direction of VESSEL_HALF (input)
#define VESSEL_IS_HALF 0b0      //The input value when the vessel is half-full
#define VESSEL_EMPTY RB3    //Receives the empty signal from the pressure switch
#define VESSEL_EMPTY_DIR TRISB3     //The data direction of VESSEL_EMPTY (input)
#define VESSEL_IS_EMPTY 0b0      //The input value when the vessel is empty
#define MOTOR_POWER RC0        //The motor main power
#define MOTOR_POWER_DIR TRISC0     //Motor main power data direction (output)
#define MOTOR_POWER_ON 0b1      //The output value that powers the motor on
#define MOTOR_SPEED RC1        //The motor speed control (high(spin) / low)
#define MOTOR_SPEED_DIR TRISC1   //Motor main speed control data direction (output)
#define MOTOR_HIGH_SPEED 0b1    //The output value that selects motor high speed
#define MOTOR_DIRECTION RC2 //The motor rotation direction
#define MOTOR_DIRECTION_DIR TRISC2  //Motor rotation direction data direction(output)
#define MOTOR_RIGHT 0b0      //The output value that rotates the motor right
#define THERMOSTAT RE1   //Thermostat signal input
#define THERMOSTAT_DIR TRISE1   //Thermostat data direction (input)
#define THERMOSTAT_OFF 0b0  //The digital value that represent that thermostat
                              //is off (temperature is over)
#define DRAIN_PUMP RD1  //Drain pump control
#define DRAIN_PUMP_DIR TRISD1   //Drain pump data direction (output)
#define DRAIN_PUMP_ON 0b1   //The value that drives the drain pump on
#define DOOR_LOCK_CHECK RE0     //Check if the door lock is closed
#define DOOR_LOCK_CHECK_DIR TRISE0     //Data direction of door lock check pin
#define DOOR_LOCK_CHECK_SIG 0b1   //Input digital signal when the door is locked
#define LOCK_DOOR RA5       //The pin used to lock the door
#define LOCK_DOOR_DIR TRISA5     //Data direction of 'lock door' pin
#define LOCK_DOOR_SIG 0b1     //The output digital signla to lock the door
#define START_STOP_BT RB0       //Start/Stop button input pin
#define START_STOP_BT_DIR TRISB0       //Start/stop button data direction (input)
#define START_STOP_BT_PRES 0b0        //The digital value got when start/stop
                                      //button is pressed
#define HALF_LOAD_BT RB4       //Half-load button input pin
#define HALF_LOAD_BT_DIR TRISB4       //Half-load button data direction (input)
#define HALF_LOAD_BT_PRES 0b0        //The digital value got when half-load
                                     //button is pressed
#define NO_SPIN_BT RB5      //No-spin button input pin
#define NO_SPIN_BT_DIR TRISB5      //No-spin button data direction (input)
#define NO_SPIN_BT_PRES 0b0     //The digital value got when no-spin button
                                //is pressed
#define SKIP_SEL_BT RB6     //Skip-select button input pin
#define SKIP_SEL_BT_DIR TRISB6     //No-spin button data direction (input)
#define SKIP_SEL_BT_PRES 0b0        //The digital value got when skip-select
                                    //button is pressed
#define STATE_SEL_BT RB7        //State-select button input pin
#define STATE_SEL_BT_DIR TRISB7        //State-select button data direction (input)
#define STATE_SEL_BT_PRES 0b0        //The digital value got when state-select button
                                //is pressed
#define SELF_LATCH RD7      //The pin that self-latches the microcotroller
#define SELF_LATCH_DIR TRISD7      //Self latching pin data direction (input)
#define SELF_LATCH_SIG 0b1      //Self latching output digital value
#define IDLE_TIMEOUT 20    //The maximum time ,in minutes, the device can
                            //be still powered up without beginning the sequence
#define SOLENOID_TIMEOUT 10u        //The maximum time the solenoid may be
                                    //powered without filling the vessel
#define D_PUMP_TIMEOUT 10u   //The maximum time the drain pump may still powered
                             //without draining all the water from the vessel
#define ROT_DIR_TIMEOUT 10u     //The maximum time the motor may rotate in the
                                //same direction
#define ACTIVE_STATE_NUM 16        //The number of active states
#define ST_A_ROT_TIMEOUT 10        //State A rotation Timeout
#define ST_A_LED_ADDR 0          //State A LED address
#define ST_B_ROT_TIMEOUT 000u        //State B rotation Timeout
#define ST_B_LED_ADDR 1u          //State B LED address
#define ST_C_ROT_TIMEOUT 000        //State C rotation Timeout
#define ST_C_LED_ADDR 2u          //State C LED address
#define ST_D_ROT_TIMEOUT 000        //State D rotation Timeout
#define ST_D_LED_ADDR 3u          //State D LED address
#define ST_F1_ROT_TIMEOUT 000       //State F1 rotation Timeout
#define ST_F1_LED_ADDR 4u          //State F1 LED address
#define ST_F2_ROT_TIMEOUT 000       //State F2 rotation Timeout
#define ST_F2_LED_ADDR 4u          //State F2 LED address
#define ST_Gs_ROT_TIMEOUT 000       //State Gs rotation Timeout
#define ST_Gs_LED_ADDR 5u          //State Gs LED address
#define ST_Gr_ROT_TIMEOUT 000       //State Gr rotation Timeout
#define ST_Gr_LED_ADDR 5u          //State Gr LED address
#define ST_H_ROT_TIMEOUT 000        //State H rotation Timeout
#define ST_H_LED_ADDR 6u          //State H LED address
#define ST_J_ROT_TIMEOUT 000        //State J rotation Timeout
#define ST_J_LED_ADDR 7u          //State J LED address
#define ST_K_ROT_TIMEOUT 000        //State K rotation Timeout
#define ST_K_LED_ADDR 8u          //State K LED address
#define ST_L_ROT_TIMEOUT 000        //State L rotation Timeout
#define ST_L_LED_ADDR 9u          //State L LED address
#define ST_M_ROT_TIMEOUT 000        //State M rotation Timeout
#define ST_M_LED_ADDR 10u         //State M LED address
#define ST_N_ROT_TIMEOUT 000        //State N rotation Timeout
#define ST_N_LED_ADDR 11u          //State N LED address
#define ST_P_ROT_TIMEOUT 000        //State P rotation Timeout
#define ST_P_LED_ADDR 12u          //State P LED address
#define ST_Q_ROT_TIMEOUT 000        //State Q rotation Timeout
#define ST_Q_LED_ADDR 13u          //State Q LED address
#define DOOR_OPEN_LED RD4       //LED that indicates that the door is open
#define DOOR_OPEN_LED_DIR TRISD4      //Data direcion of door-open LED pin (output)
#define DOOR_OPEN_LED_ON 0b0        //Door-open LED activation signal
#define NO_SPIN_LED RD5       //LED that indicates that the door is open
#define NO_SPIN_LED_DIR TRISD5      //Data direcion of door-open LED pin (output)
#define NO_SPIN_LED_ON 0b0        //Door-open LED activation signal
#define HALF_LOAD_LED RD6       //LED that indicates that the door is open
#define HALF_LOAD_LED_DIR TRISD6      //Data direcion of door-open LED pin (output)
#define HALF_LOAD_LED_ON 0b0        //Door-open LED activation signal
#define SECONDS_PER_MINUTE 57       //Number of main timer overfloaws per minute
             //this is because the timer doesn't overflow every one exact second
#define _XTAL_FREQ 2000000      //Frequncy of the clock of the system
/*Macro to light a LED*/
#define __LIGHT_LED(A)  (PORTC = (A << 4) | (PORTC & 0x0F))

#endif	/* AUTO_WASHING_MACHINE_H */