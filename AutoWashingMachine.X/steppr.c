/*
 * File:   stepper.c
 * Author: Hatem Al-Amir
 *
 * Created on May 19, 2016, 1:09 PM
 */

#include "stepper.h"

/*keeps track of which step the motor is at now*/
static unsigned char cur_step = 0;

/*assuming that the motor is a two phase bipolar motor, and has 8 equal steps, 
  this function moves the motor to a specific position. Step 0 is the reference
   position, and each step from 1 to 7 adds 45 degrees to the reference position*/
static void step(unsigned char step_num)
{
    switch(step_num)
    {
        case 0:
            STEP_PIN1 = !STEP_ACT_SIG;
            STEP_PIN2 = !STEP_ACT_SIG;
            STEP_PIN0 = STEP_ACT_SIG;
            STEP_PIN3 = !STEP_ACT_SIG;
            break;
        case 1:
            STEP_PIN2 = !STEP_ACT_SIG;
            STEP_PIN3 = !STEP_ACT_SIG;
            STEP_PIN0 = STEP_ACT_SIG;
            STEP_PIN1 = STEP_ACT_SIG;
            break;
        case 2:
            STEP_PIN2 = !STEP_ACT_SIG;
            STEP_PIN3 = !STEP_ACT_SIG;
            STEP_PIN1 = STEP_ACT_SIG;
            STEP_PIN0 = !STEP_ACT_SIG;
            break;
        case 3:
            STEP_PIN0 = !STEP_ACT_SIG;
            STEP_PIN3 = !STEP_ACT_SIG;
            STEP_PIN1 = STEP_ACT_SIG;
            STEP_PIN2 = STEP_ACT_SIG;
            break;
        case 4:
            STEP_PIN0 = !STEP_ACT_SIG;
            STEP_PIN3 = !STEP_ACT_SIG;
            STEP_PIN2 = STEP_ACT_SIG;
            STEP_PIN1 = !STEP_ACT_SIG;
            break;
        case 5:
            STEP_PIN0 = !STEP_ACT_SIG;
            STEP_PIN1 = !STEP_ACT_SIG;
            STEP_PIN2 = STEP_ACT_SIG;
            STEP_PIN3 = STEP_ACT_SIG;
            break;
        case 6:
            STEP_PIN0 = !STEP_ACT_SIG;
            STEP_PIN1 = !STEP_ACT_SIG;
            STEP_PIN3 = STEP_ACT_SIG;
            STEP_PIN2 = !STEP_ACT_SIG;
            break;
        case 7:
            STEP_PIN1 = !STEP_ACT_SIG;
            STEP_PIN2 = !STEP_ACT_SIG;
            STEP_PIN3 = STEP_ACT_SIG;
            STEP_PIN0 = STEP_ACT_SIG;
            break;
        default:
            STEP_PIN0 = !STEP_ACT_SIG;
            STEP_PIN1 = !STEP_ACT_SIG;
            STEP_PIN2 = !STEP_ACT_SIG;
            STEP_PIN3 = !STEP_ACT_SIG;
    }
}

/*rotate 45 degrees clockwise*/
void step_cw(void)
{
    cur_step = (cur_step + 1) % STEP_REV;
    step(cur_step);
}

/*rotates 45 degrees counter-clockwise*/
void step_ccw(void)
{
    if(0 == cur_step)
        cur_step = (STEP_REV - 1);
    else
        cur_step--;
    step(cur_step);
}

/*rotates 360 degrees clockwise*/
void rev_cw(void)
{
    int i;
    for(i = 0; i < STEP_REV; i++)
        step_cw();
}

/*rotates 360 degrees counter-clockwise*/
void rev_ccw(void)
{
    int i;
    for(i = 0; i < STEP_REV; i++)
        step_ccw();
}

/*powers off the motor*/
void mot_off(void)
{
    STEP_PIN0 = !STEP_ACT_SIG;
    STEP_PIN1 = !STEP_ACT_SIG;
    STEP_PIN2 = !STEP_ACT_SIG;
    STEP_PIN3 = !STEP_ACT_SIG;
}