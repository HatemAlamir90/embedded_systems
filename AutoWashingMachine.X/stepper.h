/* 
 * File: stepper.h
 * Author: Hatem Al-Amir
 * Comments: This file implements basic functionality of a stepper motor
 * Revision history: Rev.0
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef STEPPER_H
#define	STEPPER_H

#include <xc.h> // include processor files - each processor file is guarded.

/*the following definitions are for the pins that outputs the digital values
  required to control the stepper motor, and intended to be used with ULN2003A
  interfacing IC*/
/*first pin controls the motor*/
#define STEP_PIN0 RA1
/*data direction of PIN0*/
#define STEP_PIN0_DIR TRISA1
/*second pin controls the motor*/
#define STEP_PIN1 RA2
/*data direction of PIN1*/
#define STEP_PIN1_DIR TRISA2
/*third pin controls the motor*/
#define STEP_PIN2 RA3
/*data direction of PIN2*/
#define STEP_PIN2_DIR TRISA3
/*fourth pin controls the motor*/
#define STEP_PIN3 RA5
/*data direction of PIN3*/
#define STEP_PIN3_DIR TRISA5
/*the active output signal*/
#define STEP_ACT_SIG 0b1
/*how many steps in a full revolution*/
#define STEP_REV 8u
/*time between each step and the next*/
#define STEP_DELAY_ms 50
/**
    <p><b>Function prototype: void step_cw(void)</b></p>

    <p><b>Summary: rotate 45 degrees clockwise</b></p>

    <p><b>Description: this function rotates the motor one step in the clockwise
          direction</b></p>

    <p><b>Precondition: None</b></p>

    <p><b>Parameters: None</b></p>

    <p><b>Returns: None</b></p>

    <p><b>Example:</b></p>
    <code>
        step_cw();
    </code>

    <p><b>Remarks: None</b></p>
 */
void step_cw(void);

/**
    <p><b>Function prototype: void step_ccw(void)</b></p>
  
    <p><b>Summary: rotate 45 degrees counter clockwise</b></p>

    <p><b>Description: this function rotates the motor one step in the counter
          clockwise direction</b></p>

    <p><b>Precondition: None</b></p>

    <p><b>Parameters: None</b></p>

    <p><b>Returns: None</b></p>

    <p><b>Example:</b></p>
    <code>
        step_ccw();
    </code>

    <p><b>Remarks: None</b></p>
 */
void step_ccw(void);

/**
    <p><b>Function prototype: void rev_cw(void)</b></p>
  
    <p><b>Summary: rotate 360 degrees counter clockwise</b></p>

    <p><b>Description: this function rotates the motor one revolution in the
          clockwise direction</b></p>

    <p><b>Precondition: None</b></p>

    <p><b>Parameters: None</b></p>

    <p><b>Returns: None</b></p>

    <p><b>Example:</b></p>
    <code>
        rev_cw();
    </code>

    <p><b>Remarks: None</b></p>
 */
void rev_cw(void);

/**
    <p><b>Function prototype: void rev_cw(void)</b></p>
  
    <p><b>Summary: rotate 360 degrees counter clockwise</b></p>

    <p><b>Description: this function rotates the motor one revolution in the
          counter clockwise direction</b></p>

    <p><b>Precondition: None</b></p>

    <p><b>Parameters: None</b></p>

    <p><b>Returns: None</b></p>

    <p><b>Example:</b></p>
    <code>
        rev_ccw();
    </code>

    <p><b>Remarks: None</b></p>
 */
void rev_ccw(void);

/**
    <p><b>Function prototype: void mot_off(void)</b></p>
  
    <p><b>Summary: power-off the motor</b></p>

    <p><b>Description: this function powers off the motor</b></p>

    <p><b>Precondition: None</b></p>

    <p><b>Parameters: None</b></p>

    <p><b>Returns: None</b></p>

    <p><b>Example:</b></p>
    <code>
        mot_off();
    </code>

    <p><b>Remarks: None</b></p>
 */
void mot_off(void);
#endif	/* XC_HEADER_TEMPLATE_H */

