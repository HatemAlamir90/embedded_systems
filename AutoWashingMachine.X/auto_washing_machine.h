/* 
 * File: auto_washing_machine.h
 * Author: Hatem Al-Amir
 * Comments: This file includes declarations to be used in AutoWashingMachine
 *           project, which implements a control system for "Ideal Zanussi
 *           Z 650 XS" automatic washing machine. This project uses Microchip
 *           PIC16F877A microcontroller.
 * Revision history: Rev.1
 */

/*This is a guard condition so that contents of this file are not included
more than once*/
#ifndef AUTO_WASHING_MACHINE_H
#define	AUTO_WASHING_MACHINE_H

/*the main water solenoid*/
#define SOLENOID RD3
/*data direction of the solenoid control pin*/
#define SOLENOID_DIR TRISD3
/*the digital value that turns the solenoid on*/
#define SOLENOID_ON 0b1
/*the heater is used to warm the water up to the temperature set by the user*/
#define HEATER RD0
/*data direction of the heater control pin*/
#define HEATER_DIR TRISD0
/*the digital value that turns the heater on*/
#define HEATER_ON 0b1
/*input pin that receives signal from the pressure switch when the vessel is
  full*/
#define VESSEL_FULL RB1
/*data direction of VESSEL_FULL*/
#define VESSEL_FULL_DIR TRISB1
/*the input digital value expected when the vessel is full*/
#define VESSEL_IS_FULL 0b0
/*input pin that receives signal from the pressure switch when the vessel is
  half-full*/
#define VESSEL_HALF RB2
/*data direction of VESSEL_HALF*/
#define VESSEL_HALF_DIR TRISB2
/*the input digital value expected when the vessel is half-full*/
#define VESSEL_IS_HALF 0b0
/*input pin that receives signal from the pressure switch when the vessel is
  empty*/
#define VESSEL_EMPTY RB3
/*data direction of VESSEL_EMPTY*/
#define VESSEL_EMPTY_DIR TRISB3
/*the input digital value expected when the vessel is empty*/
#define VESSEL_IS_EMPTY 0b0
/*output pin that controls the motor main power supply*/
#define MOTOR_POWER RC0
/*data direction of MOTOR_POWER*/
#define MOTOR_POWER_DIR TRISC0
/*the digital value that powers the motor up*/
#define MOTOR_ON 0b1
/*output pin that selects motor speed (high / low)*/
#define MOTOR_SPEED RC1
/*data direction of MOTOR_SPEED*/
#define MOTOR_SPEED_DIR TRISC1
/*the digital value that selects motor high speed*/
#define MOTOR_HIGH 0b1
/*output pin that selects motor rotation direction*/
#define MOTOR_DIRECTION RC2
/*data direction MOTOR_DIRECTION*/
#define MOTOR_DIRECTION_DIR TRISC2
/*the digital value that selects right rotation direction*/
#define MOTOR_RIGHT 0b1
/*input pin that reads the state of the thermostat*/
#define THERMOSTAT RE1
/*data direction THERMOSTAT*/
#define THERMOSTAT_DIR TRISE1
/*the digital value expected when the thermostat is off*/
#define THERMOSTAT_OFF 0b0
/*output pin that controls the drain pump*/
#define DRAIN_PUMP RD1
/*data direction of DRAIN_PUMP*/
#define DRAIN_PUMP_DIR TRISD1
/*the digital value that starts the drain pump*/
#define DRAIN_PUMP_ON 0b1
/*input pin that checks if the door is closed or not*/
#define DOOR_CHECK RE0
/*data direction of DOOR_CHECK*/
#define DOOR_CHECK_DIR TRISE0
/*the digital value expected when the door is locked*/
#define DOOR_CHECK_SIG 0b1
/*output pin that locks the door*/
#define LOCK_DOOR RD2
/*data direction of LOCK_DOOR*/
#define LOCK_DOOR_DIR TRISD2
/*the digital value that locks the door*/
#define LOCK_DOOR_SIG 0b1
/*input pin that reads the start / stop button*/
#define START_STOP_BT RB0
/*data direction of START_STOP_BT*/
#define START_STOP_BT_DIR TRISB0
/*the digital value expected when the start / stop is pressed*/
#define START_STOP_BT_SIG 0b0
/*input pin that reads the half-load button*/
#define HALF_LOAD_BT RB4
/*data direction of HALF_LOAD_BT*/
#define HALF_LOAD_BT_DIR TRISB4
/*the digital value expected when the half-load is pressed*/
#define HALF_LOAD_BT_SIG 0b0
/*input pin that reads the no-spin button*/
#define NO_SPIN_BT RB5
/*data direction of NO_SPIN_BT*/
#define NO_SPIN_BT_DIR TRISB5
/*the digital value expected when the no-spin button is pressed*/
#define NO_SPIN_BT_SIG 0b0
/*input pin that reads the button that selects a state sequence to be skipped*/
#define SKIP_SEL_BT RB6
/*data direction SKIP_SEL_BT*/
#define SKIP_SEL_BT_DIR TRISB6
/*the digital value expected when the skip-select button is pressed*/
#define SKIP_SEL_BT_SIG 0b0
/*input pin that reads the button that selects a state to begin with*/
#define STATE_SEL_BT RB7
/*data direction of STATE_SEL_BT*/
#define STATE_SEL_BT_DIR TRISB7
/*the digital value expected when the state-select button is pressed*/
#define STATE_SEL_BT_SIG 0b0
/*output pin that self-latches the microcontroller not to lose power*/
#define SELF_LATCH RD7
/*data direction of SELF_LATCH*/
#define SELF_LATCH_DIR TRISD7
/*the digital value to self-latch the microcontroller*/
#define SELF_LATCH_SIG 0b1
/*output pin that resets the microcontroller*/
#define RST RA0
/*data direction of RESET*/
#define RST_DIR TRISA0
/*the digital value to reset the microcontroller*/
#define RST_SIG 0b1
/*the maximum time ,in minutes, the device is idle form, before powering-off
  the microcontroller*/
#define IDLE_TIMEOUT 20u
/*The maximum time, in time, the solenoid is on for, exceeding this time means
  that something went wrong*/
#define SOLENOID_TIMEOUT 10u
/*number of stepper motor revolutions needed to open channel A of the detergent
  dispenser*/
#define CHA_REV 3u
/*number of stepper motor revolutions needed to open channel B of the detergent
  dispenser*/
#define CHB_REV 6u
/*number of stepper motor revolutions needed to open channel C of the detergent
  dispenser*/
#define CHC_REV 9u
/*The maximum time, in time, the drain pump is on for, exceeding this time means
  that something went wrong*/
#define DRAIN_PUMP_TIMEOUT 10u
/*how long, in seconds, the motor rotates in one direction*/
#define ROT_DIR_TIMEOUT 3u
/*number of real washing states*/
#define ACTIVE_STATE_NUM 14u
/*number of normal clothing washing states*/
#define NORMAL_STATE_NUM 7u
/*state A rotation time*/
#define ROT_A 1u
/*state B rotation time, in minutes*/
#define ROT_B 1u
/*state C rotation time, in minutes*/
#define ROT_C 1u
/*state D rotation time, in minutes*/
#define ROT_D 1u
/*state F rotation time, in minutes*/
#define ROT_F 1u
/*state G rotation time, in minutes*/
#define ROT_G 1u
/*state G spin time, in minutes*/
#define SPIN_G 1u
/*state H spin time, in minutes*/
#define SPIN_H 1u
/*state J rotation time, in minutes*/
#define ROT_J 0u
/*state K rotation time, in minutes*/
#define ROT_K 0u
/*state L rotation time, in minutes*/
#define ROT_L 0u
/*state M rotation time, in minutes*/
#define ROT_M 0u
/*state N rotation time, in minutes*/
#define ROT_N 0u
/*state P rotation time, in minutes*/
#define ROT_P 0u
/*state Q rotation time, in minutes*/
#define ROT_Q 0u
/*output pin that lighten a warning LED when the door is open*/
#define DOOR_OPEN_LED RD4
/*data direction of DOOR_OPEN_LED*/
#define DOOR_OPEN_LED_DIR TRISD4
/*the digital value that lightens DOOR_OPEN_LED*/
#define DOOR_OPEN_LED_ON 0b1
/*output pin that lighten an indication LED when no-spin*/
#define NO_SPIN_LED RD5
/*data direction of NO_SPIN_LED*/
#define NO_SPIN_LED_DIR TRISD5
/*the digital value to lighten DOOR_OPEN_LED*/
#define NO_SPIN_LED_ON 0b1
/*output pin that lighten an indication LED when half-load*/
#define HALF_LOAD_LED RD6
/*data direction of HALF_LOAD_LED*/
#define HALF_LOAD_LED_DIR TRISD6
/*the digital value to lighten HALF_LOAD_LED*/
#define HALF_LOAD_LED_ON 0b1
/*the edge at which the external interrupt on RB0 happens*/
#define EXT_INT_EDGE 0b0
/*number of main timer overflows per minute*/
#define OVF_MIN 114
/*main crystal oscillator frequency, required for delay functions*/
#define _XTAL_FREQ 4000000
/*if the user keep pressing on suspend/resume button for SECONDS_TO_RESET
  seconds, reset the device*/
#define SECONDS_TO_RESET 1
/*if the user keep pressing on suspend/resume button for ticks,
  reset the device*/
#define TICKS_TO_RESET (((unsigned long)SECONDS_TO_RESET * (unsigned long)_XTAL_FREQ) / 4ul)
/*delay time inserted when changing motor speed while the motor is rotating in
  the reverse direction*/
#define LOW_HIGH_DELAY_ms 2000
/*delay time to overcome mechanical bouncing of buttons*/
#define BOUNCING_DELAY_ms 150
/*the time that the relay takes to close the circuit*/
#define RELAY_OP_DELAY_ms 12
/*the time that the relay takes to open the circuit*/
#define RELAY_RE_DELAY_ms 6
/*Macro to lighten a LED*/
#define __LIGHT_LED(A)  (PORTC = ((unsigned char)A << 4) | (PORTC & 0x0F))

#endif	/* AUTO_WASHING_MACHINE_H */