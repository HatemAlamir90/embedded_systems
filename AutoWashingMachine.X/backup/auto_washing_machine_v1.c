/*
 * File:   auto_washing_machine.c
 * Author: Hatem Al-Amir
 *
 * Created on February 19, 2016, 10:06 AM
 * Comments: This file implements a control system for "Ideal Zanussi
 *           Z 650 XS" automatic washing machine. This project uses Microchip
 *           PIC16F877A microcontroller.
 */


#include <xc.h>
#include "auto_washing_machine.h"

/*Microcontroller configuration*/
#pragma config FOSC = XT    //Oscillator selection, external oscillator
#pragma config WDTE = ON    //Enable watchdog timer, timeout is about 17.5 ms,
                            //at 25 degree
#pragma config PWRTE = ON   //Enable power-up timer, 72 ms
#pragma config BOREN = ON   //Enable brown-out reset, at Vdd = 4.0, for 100 us
#pragma config LVP = OFF    //Disable low voltage in-cercit serial programming
#pragma config CPD = ON     //Code-protect data EEPROM memory
#pragma config WRT = HALF   //EEPROM write enable, only from 1000h to 1FFF can
                            //be written by EECON control
#pragma config DEBUG = OFF  //Disable in-cercit debugger
#pragma config CP = ON      //Code-protect flash program memory

/*Enumeration that represents the states of the machine
 *state cur_state, The current state
 *state prev_state, the sate before the current state
 *state back_cur_state, back-up value for the current state
 *state skip_beg, the last state before the sequence of states to be skipped
 *state skip_end, the first state after the sequence of states to be skipped
 */
enum  {BEGINNING= -2,SUSPENSION,A,B,C,D,F1,F2,Gs,Gr,H,J,K,L,M,N,P,Q,NO_SPIN, HALF_LOAD} cur_state, prev_state, back_cur_state, skip_beg, skip_end, temp;

/*List of state rotation timeouts, in minutes*/
unsigned char state_rot_timeout[ACTIVE_STATE_NUM];
/*List of state LED addresses*/
unsigned short state_led[ACTIVE_STATE_NUM];
unsigned char seconds;  //seconds elapsed so far
unsigned char minutes;  //minutes elapsed so far
unsigned char sol_op_time;  //how long has the solenoid been open, in minutes
unsigned char dpump_op_time;    //how long has the drain pump been open, minutes
unsigned char rot_spin_time;    //how long has the current state been rotating/spinning
unsigned char rot_spin_timeout; //how long the current state is supposed to be
                                //rotating / spinning
unsigned char rot_dir_time;     //how long the has been rotating in the same direction

bit door_open;  //set if the door is open
bit spin;   //set if spinning is desired
bit half_load;  //set if half-load washing is desired
/*The following flags represent the required states of the components during
 * the current stage of the state
 */
bit dp_on;  //set if the drain pump is on
bit mot_rot;    //set if the motor is rotating left or right
bit mot_active; //set if the motor is active
bit ht_on;  //set if the heater is on
bit sol1_on;    //set if solenoid 1 is on
bit sol2_on;    //set if solenoid 2 is on
bit sol3_on;    //set if solenoid 3 is on

void terminate(void);   //Initialize the system

/*void main(void)
 * Self-latch the device not to lose power, configure interrupts and peripherals,
 * initializes the system, and keep clearing the watchdog timer.
 */
void main(void)
{
    di();   //disable interrupts
    SELF_LATCH_DIR = 0b0;   //set self-latching pin as output
    SELF_LATCH = SELF_LATCH_SIG;    //self-latch the device no to lose power
    
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
    TRISA = 0x00;   //set port A pins as input
    
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
    state_rot_timeout[A] = ST_A_ROT_TIMEOUT;
    state_led[A] = ST_A_LED_ADDR;
    state_rot_timeout[B] = ST_B_ROT_TIMEOUT;
    state_led[B] = ST_B_LED_ADDR;
    state_rot_timeout[C] = ST_C_ROT_TIMEOUT;
    state_led[C] = ST_C_LED_ADDR;
    state_rot_timeout[D] = ST_D_ROT_TIMEOUT;
    state_led[D] = ST_D_LED_ADDR;
    state_rot_timeout[F1] = ST_F1_ROT_TIMEOUT;
    state_led[F1] = ST_F1_LED_ADDR;
    state_rot_timeout[F2] = ST_F2_ROT_TIMEOUT;
    state_led[F2] = ST_F2_LED_ADDR;
    state_rot_timeout[Gs] = ST_Gs_ROT_TIMEOUT;
    state_led[Gs] = ST_Gs_LED_ADDR;
    state_rot_timeout[Gr] = ST_Gr_ROT_TIMEOUT;
    state_led[Gr] = ST_Gr_LED_ADDR;
    state_rot_timeout[H] = ST_H_ROT_TIMEOUT;
    state_led[H] = ST_H_LED_ADDR;
    state_rot_timeout[J] = ST_J_ROT_TIMEOUT;
    state_led[J] = ST_J_LED_ADDR;
    state_rot_timeout[K] = ST_K_ROT_TIMEOUT;
    state_led[K] = ST_K_LED_ADDR;
    state_rot_timeout[L] = ST_L_ROT_TIMEOUT;
    state_led[L] = ST_L_LED_ADDR;
    state_rot_timeout[M] = ST_M_ROT_TIMEOUT;
    state_led[M] = ST_M_LED_ADDR;
    state_rot_timeout[N] = ST_N_ROT_TIMEOUT;
    state_led[N] = ST_N_LED_ADDR;
    state_rot_timeout[P] = ST_P_ROT_TIMEOUT;
    state_led[P] = ST_P_LED_ADDR;
    state_rot_timeout[Q] = ST_Q_ROT_TIMEOUT;
    state_led[Q] = ST_Q_LED_ADDR;
    terminate();    //initialize the system
    ei();
    while(1)
    {
        CLRWDT();
    }
}

void begin_current_state(void);
void state_jump(void);
void suspend(void);
void resume(void);
/*Interrupt service routine*/
void interrupt isr(void)
{
    if(TMR1IF == 0b1)  //if the main timer has overflowed
    {
        TMR1IF = 0b0;   //first of all, clear the interrupt flag
        /*Redundant check, to make sure not to activate device, or count anything
         * while the machine is suspended.
         */
        if(cur_state == SUSPENSION)
            return;
        seconds++;  //increment seconds
          /*If the motor is active in this stage, but in the low speed, not to
            reverse the direction of the high speed*/
        if(mot_active && (MOTOR_SPEED != MOTOR_HIGH_SPEED))
        {
            rot_dir_time++; //increment directional rotation time (in seconds)
            if(rot_dir_time >= ROT_DIR_TIMEOUT) //every specified period
            {
                rot_dir_time = 0;
                if(mot_rot) //if rotating
                    MOTOR_POWER = !MOTOR_POWER_ON;  //stop
                else    //if stopped
                {
                    MOTOR_DIRECTION != MOTOR_DIRECTION; //in the other direction
                    MOTOR_POWER = MOTOR_POWER_ON;   //rotate
                }
                mot_rot != mot_rot; //save the state
            }
        }
        if(seconds >= SECONDS_PER_MINUTE)   //if a minute elapsed
        {
            seconds = 0;    //begin a new one
            minutes++;  //add a new minute
            
            /*If no interaction with the washer has done, for the specified
             * timeout, kill the power.
             */
            if(cur_state == BEGINNING && minutes >= IDLE_TIMEOUT)
                SELF_LATCH = !SELF_LATCH_SIG;
            
            /*If any solenoid is on while the vessel hasn't full, for the
             *the specified timeout, suspend, assuming that something went wrong
             */
            if(sol1_on == 0b1 || sol2_on == 0b1 || sol3_on == 0b1)
            {
                sol_op_time++;
                if(sol_op_time >= SOLENOID_TIMEOUT)
                {
                    sol_op_time = 0;
                    suspend();
                }
            }
            /*If the drain pump is on while the water hasn't discharged, for the
             *the specified timeout, suspend, assuming that something went wrong
             */
            if(dp_on == 0b1)
            {
                dpump_op_time++;
                if(dpump_op_time >= D_PUMP_TIMEOUT)
                {
                    dpump_op_time = 0;
                    suspend();
                }
            }
            if(mot_active == 0b1)  //if the motor is active
            {
                    rot_spin_time++;    //count a new minute of rotation
                    if(rot_spin_time >= rot_spin_timeout) //if the current stage
                    {                             //rotation period has finished
                        /*Drain the water for the following states*/
                        if(cur_state == A || cur_state == D
                           || cur_state == F1 || cur_state == F2
                           || cur_state == Gr)
                        {
                           DRAIN_PUMP = DRAIN_PUMP_ON;
                           dp_on = 0b1;
                           if(cur_state == D) //state D doesn't require
                           {                        //the motor while draining
                               MOTOR_POWER = !MOTOR_POWER_ON;
                               mot_active = 0b0;
                               mot_rot = 0b0;
                               rot_dir_time = 0;
                           }
                           /*Reset rotation time, not to enter the same condition
                            if the drain pump takes more than one minute*/
                           rot_spin_time = 0;
                        }
                        else if(cur_state == H) //after state H, terminate
                            terminate();
                        else if(cur_state == skip_beg) //if the next state is
                        {                //the beginning of the skipped states
                            /*Jump to the first state after the skip sequence of
                             states.*/
                            prev_state = skip_end;
                            state_jump();
                            return;
                        }
                        else    //for states, B, C, and Gs,
                        {       //just go to the next state
                            cur_state = cur_state + 1;
                            back_cur_state = cur_state;
                            if(cur_state == Gr)
                            {
                                MOTOR_POWER = !MOTOR_POWER_ON;
                                mot_active = 0b0;
                                mot_rot = 0b0;
                                rot_spin_time = 0;
                                rot_dir_time = 0;
                                MOTOR_SPEED = !MOTOR_HIGH_SPEED;
                                MOTOR_DIRECTION = MOTOR_RIGHT;
                                SOLENOID3 = SOLENOID_ON;
                                sol3_on = 0b1;
                            }
                            else
                                begin_current_state();
                        }
                    }
            }
        } 
    }
    if(TMR2IF == 0b1)  //if check-timer has overflowed
    {
        TMR2IF = 0b0;
        /*If we detect that the door is open, suspend immediately*/
        if(door_open == 0b0 && DOOR_LOCK_CHECK != DOOR_LOCK_CHECK_SIG)
        {
            door_open = 0b1;
            DOOR_OPEN_LED = DOOR_OPEN_LED_ON;
            suspend();
            return;
        }
        /*When the user closes an open door, save the state of the closed door,
        * but don't resume until the user pushes resume-button.
        */
        else if(door_open == 0b1 && DOOR_LOCK_CHECK == DOOR_LOCK_CHECK_SIG)
        {
            door_open = 0b0;
            DOOR_OPEN_LED = !DOOR_OPEN_LED_ON;
            return;
        }
        /*Make sure that no other thing is changed when the process is suspended*/
        else if(cur_state == SUSPENSION)
            return;
        if(HEATER == HEATER_ON && THERMOSTAT == THERMOSTAT_OFF)
        {
            HEATER = !HEATER_ON;
            ht_on = 0b0;
        }
        /*When the vessel is full / half-full, stop the solenoids immediately,
         * to make sure that the vessel doesn't overflow, and to begin state
         * rotation.
         */
        if(SOLENOID1 == SOLENOID_ON || SOLENOID2 == SOLENOID_ON || SOLENOID3 == SOLENOID_ON)
        {
            if(VESSEL_FULL == VESSEL_IS_FULL || ((VESSEL_HALF == VESSEL_IS_HALF )&& (half_load == 0b1)))
            {
                SOLENOID1 = !SOLENOID_ON;
                sol1_on = 0b0;
                SOLENOID2 = !SOLENOID_ON;
                sol2_on = 0b0;
                SOLENOID3 = !SOLENOID_ON;
                sol3_on = 0b0;
                begin_current_state();
                return;
            }
            else if((VESSEL_HALF == VESSEL_IS_HALF) && (half_load = 0b0) && (cur_state == A || cur_state == Gr))
            {
                if(mot_active == 0b0)
                {
                    MOTOR_SPEED = !MOTOR_HIGH_SPEED;
                    MOTOR_DIRECTION = MOTOR_RIGHT;
                    rot_spin_time = 0;
                    rot_dir_time = 0;
                    MOTOR_POWER = MOTOR_POWER_ON;
                    mot_active = 0b1;
                    mot_rot = 0b1;
                }
            }
        }
        /*When all the water is drained, begin the next state*/
        if(DRAIN_PUMP == DRAIN_PUMP_ON && VESSEL_EMPTY == VESSEL_IS_EMPTY && cur_state != H && cur_state != Gs)
        {
            DRAIN_PUMP = !DRAIN_PUMP_ON;
            dp_on = 0b0;
            /*If the state that has just finished is the last state before
             * a skip sequence, skip to the first state after the skip sequence.
             */
            if(cur_state == skip_beg)
            {
                prev_state = skip_end;
                state_jump();
                return;
            }
            /*If the user is following the normal sequence (until this state),
             * begin the next state.
             */
            else if(cur_state == back_cur_state)
                cur_state = cur_state + 1;
            back_cur_state = cur_state;
            if(cur_state == A)
            {
                SOLENOID1 = SOLENOID_ON;
                sol1_on = 0b1;
            }
            else if(cur_state == Gr)
            {
                SOLENOID3 = SOLENOID_ON;
                sol3_on = 0b1;
            }
            else if(cur_state == B || cur_state == C || cur_state == D
                    || cur_state == F1 || cur_state == F2)
            {
                SOLENOID2 = SOLENOID_ON;
                sol2_on = 0b1;
            }
            /*States B, C, and D don't rotate while filling the vessel*/
            if(cur_state != B && cur_state != C && cur_state != D)//
                begin_current_state();//
        }
    }
    if(INTF)    //if the user presses start / stop
    {
        INTF = 0b0;
        /*The user wants to start / resume*/
        if(cur_state == BEGINNING || cur_state == SUSPENSION)
        {
            if(door_open == 0b1) //Don't take any action while the door is open
            {
                DOOR_OPEN_LED = DOOR_OPEN_LED_ON;
                return;
            }
            else
            {
                if(skip_beg > skip_end) //make sure that the skip sequence
                {                      //specified is in the right order
                    temp = skip_beg;
                    skip_beg = skip_end;
                    skip_end = temp;
                }
                if(cur_state == BEGINNING)  //if the user begins the process
                {
                    back_cur_state = prev_state;
                    if(prev_state == Gs || prev_state == H) //if the user begins
                    {                                      //at a spinning stage
                        MOTOR_DIRECTION = MOTOR_RIGHT;//spinning is always right
                        MOTOR_SPEED = MOTOR_HIGH_SPEED;
                        mot_active = 0b1;
                        mot_rot = 0b1;
                    }
                    /*All other states begin with filling the vessel, from
                     * a specific chanel in the detergent dispenser. Also, the
                     * the heater is always on when we begin these states
                     */
                    else
                    {
                        ht_on = 0b1;
                        if(prev_state == A)
                            sol1_on = 0b1;
                        else if(prev_state == Gr)
                            sol3_on = 0b1;
                        else
                            sol2_on = 0b1;
                    }
                }
                state_jump();
            }
        }
        else
            suspend();
    }
    if(RBIF)    //if the user pushed half-load, no-spin, state-select, or skip-
    {           //select button
        RBIF = 0b0;
        if(NO_SPIN_BT == NO_SPIN_BT_PRES)   //if the user presses no-spin button
        {
            spin != spin;   //invert the state of no-spinning
            NO_SPIN_LED != NO_SPIN_LED; //change the state of the indication LED
        }
        if(HALF_LOAD_BT == HALF_LOAD_BT_PRES)   //if the user presses half-load
        {
            half_load != half_load; //invert the state of half-loading
            HALF_LOAD_LED != HALF_LOAD_LED; //adjust the indication LED
        }
        else if(cur_state == BEGINNING || cur_state == SUSPENSION)
        {
            /*Only if the state is suspended or at the beginning, allow her to
             * start / resume from a specific state, that is saved as prev_state
             */
            if(STATE_SEL_BT == STATE_SEL_BT_PRES)
            {
                prev_state = (prev_state + 1) % ACTIVE_STATE_NUM;
                /*F2 and Gr are sub-states and cannot be chosen by the user explicitly*/
                if(prev_state == F2 || prev_state == Gr)
                    prev_state++;
                __LIGHT_LED(state_led[prev_state]);
            }
            /*Only the process is suspended or at the beginning, allow the user 
             * to specify a sequence of states to skip.
             */
            if(SKIP_SEL_BT == SKIP_SEL_BT_PRES)
            {
                skip_beg = skip_end;
                skip_end = prev_state;
            }
        }
    }
}
/*
 * void terminate(void);
 * Terminates the sequence, and resets the system to its initial state.
 */
void terminate(void)
{
    /*Initialize the state of the device*/
    cur_state = BEGINNING;    //at the beginning of every thing
    prev_state = A;   //to begin from A when the user begins the sequence
    back_cur_state = BEGINNING;   //for the semantics to be right
    skip_beg = A; //don't skip anything if, the user
    skip_end = B; //doesn't choose what to skip
    
    seconds = 0x00;
    minutes = 0x00;
    
    /*Nothing has been opened yet*/
    MOTOR_POWER = !MOTOR_POWER_ON;
    MOTOR_SPEED = !MOTOR_HIGH_SPEED;    //normally, begin with the low speed
    MOTOR_DIRECTION = MOTOR_RIGHT;
    rot_spin_time = 0x00;
    rot_spin_timeout = 0x00;
    rot_dir_time = 0x00;
    mot_rot = 0b0;
    mot_active = 0b0;
    SOLENOID1 = !SOLENOID_ON;
    SOLENOID2 = !SOLENOID_ON;
    SOLENOID3 = !SOLENOID_ON;
    sol_op_time = 0x00;
    sol1_on = 0b0;
    sol2_on = 0b0;
    sol3_on = 0b0;
    DRAIN_PUMP = !DRAIN_PUMP_ON;
    dp_on = 0b0;
    dpump_op_time = 0x00;
    HEATER = !HEATER_ON;
    ht_on = 0b0;
    spin = 0b1; //the default is to spin
    half_load = 0b0;    //the default is to use full-load
    
    /*At the beginning, light state A LED to notify the user that if he doesn't
     * choose another state, we will begin from A
     */
    __LIGHT_LED(state_led[A]);
    
    TMR1L = 0x00;   //clear timer 1 low significant byte
    TMR1H = 0x00;   //clear timer 1 high significant byte
    TMR1ON = 0b1;   //begin the main timer
    TMR2 = 0x00;    //clear timer 2
    TMR2ON = 0b1;   //begin checking timer
}

/*void resume(void);
  Resume the sequence.*/
void resume(void)
{
    /*Redundant check to make sure that nothing will be done if the door is open.
      In case that anything went wrong.*/
    if(DOOR_LOCK_CHECK != DOOR_LOCK_CHECK_SIG)
    {
        DOOR_OPEN_LED = DOOR_OPEN_LED_ON;
        return;
    }
    else
        DOOR_OPEN_LED = !DOOR_OPEN_LED_ON;  //extinguish door-open LED
    TMR1ON = 0b1;   //resume the main timer
    /*IF any device was open, resume with it open*/
    if(mot_active && mot_rot)
        MOTOR_POWER = MOTOR_POWER_ON;
    if(ht_on)
        HEATER = HEATER_ON;
    /*Make sure that only one solenoid is open, only if drain pump is off*/
    if(dp_on)
        DRAIN_PUMP = DRAIN_PUMP_ON;
    else if(sol1_on)
        SOLENOID1 = SOLENOID_ON;
    else if(sol2_on)
        SOLENOID2 = SOLENOID_ON;
    else if(sol3_on)
        SOLENOID3 = SOLENOID_ON;
}

/* void state_jump(void)
 * Jump to a next state, and terminate the current state prematurely
 */
void state_jump(void)
{
    if(prev_state != back_cur_state)  //if we are not resuming in the same state
    {
        if(cur_state != BEGINNING)   //if we are not begging in a specific
        {                            //selected state. Redundant check.
            if((prev_state != B && prev_state != C && prev_state != D)
                || (back_cur_state != B && back_cur_state != C && back_cur_state != D))
            {   //every state needs water with a specific type and amount of detergent
                dp_on = 0b1;    //so drain the old water
                sol1_on = 0b0;  //stop any solenoids
                sol2_on = 0b0;
                sol3_on = 0b0;
            }
            else    //B, C, D do the same action with the same type of detergent
                back_cur_state = prev_state; //so just set rotation time
            /*With any action of the above, rotate the motor on low speed*/
            MOTOR_SPEED = !MOTOR_HIGH_SPEED;
            mot_active = 0b1;
            mot_rot = 0b1;
        }
        /*Begin a new rotation period*/
        rot_spin_time = 0;
        rot_dir_time = 0;
        rot_spin_timeout = state_rot_timeout[prev_state];
    }
    /*Alternate previous and current states*/
    temp = prev_state;
    prev_state = cur_state;
    cur_state = temp;
    resume();   //resume the sequence
}

/* void begin_current_state(void);
 * Sets rotation timeout, and begins state rotation period.
 */
void begin_current_state(void)
{
    if(!mot_active) //the motor begins rotation right
    {
        rot_dir_time = 0;
        MOTOR_DIRECTION = MOTOR_RIGHT;
    }
    /*If we at a state of spinning*/
    if(cur_state == Gs || cur_state == H)
    {
        if(spin)    //if spinning is allowed
        {
            MOTOR_DIRECTION = MOTOR_RIGHT;
            __delay_ms(500);
            MOTOR_SPEED = MOTOR_HIGH_SPEED;
            rot_dir_time = 0;
            DRAIN_PUMP = DRAIN_PUMP_ON;
            dp_on = 0b1;
        }
        else    //if no spinning, skip to the next state
        {
            rot_spin_time = state_rot_timeout[cur_state];
            return;
        }
    }
    rot_spin_timeout = state_rot_timeout[cur_state];  //new rotation period
    rot_spin_time = 0;
    MOTOR_POWER = MOTOR_POWER_ON;
    mot_active = 0b1;
    mot_rot = 0b1;
    __LIGHT_LED(state_led[cur_state]); //light the state LED
    DOOR_OPEN_LED = !DOOR_OPEN_LED_ON;  //extinguish door-open LED
}

/*void suspend(void)
 * Suspend the process, but keep the device states, to be able to resume later 
 * with the exact states we suspended the machine with.
 */
void suspend(void)
{
    TMR1ON = 0b0;
    SOLENOID1 = !SOLENOID_ON;
    SOLENOID2 = !SOLENOID_ON;
    SOLENOID3 = !SOLENOID_ON;
    MOTOR_POWER = !MOTOR_POWER_ON;
    DRAIN_PUMP = !DRAIN_PUMP_ON;
    HEATER = !HEATER_ON;
    prev_state = cur_state;
    cur_state = SUSPENSION;
}
