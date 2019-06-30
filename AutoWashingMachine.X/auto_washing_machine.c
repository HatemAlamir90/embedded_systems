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
#include "stepper.h"

/*Microcontroller configuration*/

/*Oscillator selection, external oscillator*/
#pragma config FOSC = XT
/*enable watchdog timer, timeout is about 17.5 ms, without pre-scalar
 * at 25 degree Celsius*/
#pragma config WDTE = ON
/*enable power-up timer, fixed 72 ms*/
#pragma config PWRTE = ON
/*enable brown-out reset, when Vdd is at 4.0 volts or lower for 100 us or more*/
#pragma config BOREN = ON
/*disable low voltage in-circuit serial programming*/
#pragma config LVP = OFF
/*protect EEPROM memory from reading from the outside, refer to the device
 * data sheet for more information*/
#pragma config CPD = ON
/*EEPROM write enable, only from 1000h to 1FFF can be written by code*/
#pragma config WRT = HALF
/*disable in-circuit debugger*/
#pragma config DEBUG = OFF
/*protect code program memory from reading from the outside, refer to the device
 * data sheet for more information*/
#pragma config CP = ON

/*the states that the machine washes through, normal clothing washing states
  must begin at zero and precede all other states.
  the following states are defined:
  cur_state: the current state of the machine,
  prev_state: the previous state before the current state
  skip_beg: the last state to be completed before jumping to another state,
            skipping all states in between
  skip_end: the state to be jumped to when we need to alter the normal sequence
            of states
  temp, temp_beg, temp_end: temporary variables*/
enum state{BEGINNING= -2,SUSPENSION,A,B,C,D,F,G,H,J,K,L,M,N,P,Q,NO_SPIN, HALF_LOAD} cur_state, prev_state, skip_beg, skip_end, temp, temp, temp_beg, temp_end;
/*set if spinning is desired*/
volatile bit no_spin;
/*set if half-load washing is desired*/
volatile bit half_load;
/*set to run the drain pump*/
volatile bit dp_on;
/*set when the drain pump is actually on*/
static bit dp_is_on;
/*set to rotate the motor*/
volatile bit mot_rot;
/*set when powering on the motor*/
static bit mot_is_rot;
/*set to select high speed, otherwise low speed*/
volatile bit mot_high;
/*set to fire the heater*/
volatile bit ht_on;
/*set when the heater is actually on*/
static bit ht_is_on;
/*set to open solenoid 1*/
volatile bit sol1_on;
/*set to open solenoid 2*/
volatile bit sol2_on;
/*set to open solenoid 3*/
volatile bit sol3_on;
/*set when the solenoid pump is actually on*/
static bit sol_is_on;
/*how long has been rotating in a specific state, in minutes*/
volatile unsigned char rot_time;
/*used to count seconds*/
volatile unsigned char seconds;
/*used to count minutes*/
volatile unsigned char idle_time;
/*how long the solenoid has been open for, in minutes*/
volatile unsigned char sol_op_time;
/*how long the drain pump has been open for, in minutes*/
volatile unsigned char dp_op_time;

/*initialize the program*/
void init(void);
/*function that implements state A of the program*/
void state_a(void);
/*function that implements state B of the program*/
void state_b(void);
/*function that implements state C of the program*/
void state_c(void);
/*function that implements state D of the program*/
void state_d(void);
/*function that implements state F of the program*/
void state_f(void);
/*function that implements state G of the program*/
void state_g(void);
/*function that implements state H of the program*/
void state_h(void);

/*function that implements state J of the program*/
void state_j(void);
/*function that implements state K of the program*/
void state_k(void);
/*function that implements state L of the program*/
void state_l(void);
/*function that implements state M of the program*/
void state_m(void);
/*function that implements state N of the program*/
void state_n(void);
/*function that implements state P of the program*/
void state_p(void);
/*function that implements state Q of the program*/
void state_q(void);

/*rotate the motor in low speed*/
void mot_low(unsigned char state_time);
/*rotate the motor in high speed*/
void mot_high(unsigned char state_time);

/*its clear, what is the following function*/
void main(void)
{
    di(); //disable interrupts
    SELF_LATCH_DIR = 0b0; //set self-latching pin as output
    SELF_LATCH = SELF_LATCH_SIG; //self-latch the device no to lose power
    RST_DIR = 0b0; //set reset pin as output
    RST = !RST_SIG; //keep the device at active, non-reset, state
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
    TRISE = 0x03;   //all port E pins are input; disable PSP module
    
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
    /*check for a selected state, as long as the device is powered on*/
    while(1)
    {
        CLRWDT();
        if(A == cur_state)
            state_a();
        else if(B == cur_state)
            state_b();
        else if(C == cur_state)
            state_c();
        else if(D == cur_state)
            state_d();
        else if(F == cur_state)
            state_f();
        else if(G == cur_state)
            state_g();
        else if(H == cur_state)
            state_h();
        else if(J == cur_state)
            state_j();
        else if(K == cur_state)
            state_k();
        else if(L == cur_state)
            state_l();
        else if(M == cur_state)
            state_m();
        else if(N == cur_state)
            state_n();
        else if(P == cur_state)
            state_p();
        else if(Q == cur_state)
            state_q();
    }
}

/*
 * void init(void);
 * terminates the sequence, and resets the program to its initial state.
 */
void init(void)
{
    /*Initialize the state of the device*/
    cur_state = BEGINNING; //at the beginning of every thing
    prev_state = BEGINNING; //to begin from A when the user begins the sequence
    skip_beg = BEGINNING; //don't skip anything if, the user
    skip_end = A; //doesn't choose what to skip
    
    seconds = 0x00;
    
    /*initialize the motor state*/
    MOTOR_POWER = !MOTOR_ON;
    MOTOR_SPEED = !MOTOR_HIGH;
    MOTOR_DIRECTION = MOTOR_RIGHT;
    mot_rot = 0b0;
    mot_is_rot = 0b0;
    mot_high = 0b0;
    rot_time = 0x00;
    /*stop the water until we begin*/
    SOLENOID = !SOLENOID_ON;
    sol1_on = 0b0;
    sol2_on = 0b0;
    sol3_on = 0b0;
    sol_is_on = 0b0;
    STEP_PIN0 = !STEP_ACT_SIG;
    STEP_PIN1 = !STEP_ACT_SIG;
    STEP_PIN2 = !STEP_ACT_SIG;
    STEP_PIN3 = !STEP_ACT_SIG;
    sol_op_time = 0x00;
    /*stop the drain pump*/
    DRAIN_PUMP = !DRAIN_PUMP_ON;
    dp_on = 0b0;
    dp_is_on = 0b0;
    dp_op_time = 0x00;
    /*extinguish the heater*/
    HEATER = !HEATER_ON;
    ht_is_on = 0b0;
    ht_on = 0b0;
    /*defaults are, spin, and full-load*/
    no_spin = 0b0; //the default is to spin
    half_load = 0b0;    //the default is to use full-load
    /*unlock the door*/
    LOCK_DOOR = !LOCK_DOOR_SIG;
    /*At the beginning, light state A LED to notify the user that if he doesn't
     * choose another state, we will begin from A
     */
    __LIGHT_LED((unsigned char)A);
    
    TMR1L = 0x00;   //clear timer 1 low significant byte
    TMR1H = 0x00;   //clear timer 1 high significant byte
    TMR1ON = 0b1;   //begin seconds timer
    TMR2 = 0x00;    //clear timer 2
    TMR2ON = 0b0;   //stop control timer, until the user begins a new program
}
/*discharges the water from the vessel*/
void discharge(void);
/*implements state A of the washing program sequence*/
void state_a(void)
{
    __LIGHT_LED((int)A); //light state-A LED
    mot_rot = 0b0;
    /*the water of all other states is not valid for state A, so, whatever the
      previous state, empty the vessel first*/
    discharge();
    /*take water from the first channel of detergent dispenser, with heat*/
    sol_op_time = 0;
    sol1_on = 0b1;
    ht_on = 0b1;
    /*wait until the vessel is half_full, then rotate slow, to blend clothes
      with water*/
    while(VESSEL_IS_HALF != VESSEL_HALF)
        CLRWDT();
    mot_high = 0b0;
    mot_rot = 0b1;
    /*in half-load mode, we get by half of the vessel, to save water and
     * detergent */
    if(0b0 == half_load)
    {
        /*wait until the vessel is full*/
        while(VESSEL_IS_FULL != VESSEL_FULL)
            CLRWDT();
    }
    sol1_on = 0b0;
    /*now give the clothes a pre-wash for ROT_A minutes*/
    rot_time = 0;
    while(rot_time < ROT_A)
        CLRWDT();
    /*now drain the water, and wait until the pump empties the vessel*/
    discharge();
    di(); //disable interrupts not to risk while changing machine state
    prev_state = A; //we has almost finished A, so its the previous state
    if(A == skip_beg) //if the user wants to skip some states after A
        cur_state = skip_end; //jump to the specified jump destination
    else
        cur_state = B; //or, continue the default state sequence
    ei(); //enable interrupts again, and return
}

/*implements state B of the washing program sequence*/
void state_b(void)
{
    __LIGHT_LED((unsigned char)B); //light sate-B LED
    mot_rot = 0b0; //in state B, no rotation until the vessel is full
    /*if the previous state to B is not C, or D, we have to empty the vessel,
      to full the vessel with suitable detergents*/
    if(C != prev_state && D != prev_state)
        discharge();
    /*then, full the vessel with water and detergent from channel 2, of the
      detergent dispenser, and heat the water*/
    sol_op_time = 0;
    sol2_on = 0b1;
    ht_on = 0b1;
    /*in half-load mode, save half of water and detergent*/
    if(0b1 == half_load)
    {
        while(VESSEL_IS_HALF != VESSEL_HALF)
            CLRWDT();
    }
    else
    {
        while(VESSEL_IS_FULL != VESSEL_FULL)
            CLRWDT();
    }
    sol2_on = 0b0;
    /*now give state-B its time to wash*/
    mot_rot = 0b1;
    rot_time = 0;
    while(rot_time < ROT_B)
        CLRWDT();
    /*while changing states, disable interrupts*/
    di();
    prev_state = B;
    if(B == skip_beg)
        cur_state = skip_end;
    else
        cur_state = C;
    ei();
}

/*implements state C of the washing program sequence*/
void state_c(void)
{
    __LIGHT_LED((unsigned char)C); //light sate-C LED
    /*if the previous state to C is not B, or D, we have to empty the vessel,
      to full the vessel with suitable detergents*/
    if(B != prev_state && D != prev_state)
    {
        mot_rot = 0b0;
        discharge();
    }
    /*then, full the vessel with water and detergent from channel 2, of the
      detergent dispenser*/
    sol_op_time = 0;
    sol2_on = 0b1;
    ht_on = 0b1;
    /*in half-load mode, save half of water and detergent*/
    if(0b1 == half_load)
    {
        while(VESSEL_IS_HALF != VESSEL_HALF)
            CLRWDT();
    }
    else
    {
        while(VESSEL_IS_FULL != VESSEL_FULL)
            CLRWDT();
    }
    sol2_on = 0b0;
    /*now give state-B its time to wash*/
    rot_time = 0;
    mot_rot = 0b1;
    while(rot_time < ROT_C)
        CLRWDT();
    /*while changing states, disable interrupts*/
    di();
    prev_state = C;
    if(C == skip_beg)
        cur_state = skip_end;
    else
        cur_state = D;
    ei();
}

/*implements state D of the washing program sequence*/
void state_d(void)
{
    __LIGHT_LED((unsigned char)D); //light sate-C LED
    /*if the previous state to D is not B, or C, we have to empty the vessel,
      to full the vessel with suitable detergents*/
    if(B != prev_state && C != prev_state)
    {
        mot_rot = 0b0;
        discharge();
    }
    /*then, full the vessel with water and detergent from channel 2, of the
      detergent dispenser*/
    sol_op_time = 0;
    sol2_on = 0b1;
    ht_on = 0b1;
    /*in half-load mode, save half of water and detergent*/
    if(0b1 == half_load)
    {
        while(VESSEL_IS_HALF != VESSEL_HALF)
            CLRWDT();
    }
    else
    {
        while(VESSEL_IS_FULL != VESSEL_FULL)
            CLRWDT();
    }
    sol2_on = 0b0;
    /*now give state-B its time to wash*/
    mot_rot = 0b1;
    rot_time = 0;
    while(rot_time < ROT_D)
        CLRWDT();
    /*after state F, discharge washing water*/
    mot_rot = 0b0;
    discharge();
    /*while changing states, disable interrupts*/
    di();
    prev_state = D;
    if(D == skip_beg)
        cur_state = skip_end;
    else
        cur_state = F;
    ei();
}

/*implements state F of the washing program sequence*/
void state_f(void)
{
    __LIGHT_LED((unsigned char)F);
    ht_on = 0b0; //use normal water for rinsing
    mot_rot = 0b1;
    /*first empty the vessel because this stage is for rinsing*/
    discharge();
    /*then, full the vessel with water and detergent from channel 2, of the
      detergent dispenser*/
    sol_op_time = 0;
    sol2_on = 0b1;
    /*in half-load mode, save half of water and detergent*/
    if(0b1 == half_load)
    {
        while(VESSEL_IS_HALF != VESSEL_HALF)
            CLRWDT();
    }
    else
    {
        while(VESSEL_IS_FULL != VESSEL_FULL)
            CLRWDT();
    }
    sol2_on = 0b0;
    /*rinse the clothes two short times*/
    rot_time = 0;
    while(rot_time < 1)
        CLRWDT();
    /*replace rinsing water*/
    discharge();
    sol_op_time = 0;
    sol2_on = 0b1;
    if(0b1 == half_load)
    {
        while(VESSEL_IS_HALF != VESSEL_HALF)
            CLRWDT();
    }
    else
    {
        while(VESSEL_IS_FULL != VESSEL_FULL)
            CLRWDT();
    }
    sol2_on = 0b0;
    rot_time = 0;
    while(rot_time < 1)
        CLRWDT();
    /*replace rinsing water again*/
    discharge();
    sol_op_time = 0;
    sol2_on = 0b1;
    /*in half-load mode, save half of water and detergent*/
    if(0b1 == half_load)
    {
        while(VESSEL_IS_HALF != VESSEL_HALF)
            CLRWDT();
    }
    else
    {
        while(VESSEL_IS_FULL != VESSEL_FULL)
            CLRWDT();
    }
    sol2_on = 0b0;
    /*now rinse clothes well, for some minutes*/
    rot_time = 0;
    while(rot_time < ROT_F)
        CLRWDT();
    /*now discharge rinsing water*/
    discharge();
    /*go to the next desired state*/
    di();
    prev_state = F;
    if(F == skip_beg)
        cur_state = skip_end;
    else
        cur_state = G;
    ei();
}

/*implements state G of the washing program sequence*/
void state_g(void)
{
    __LIGHT_LED((unsigned char)G);
    ht_on = 0b0;
    /*make sure that the vessel is empty before spinning*/
    mot_rot = 0b1;
    discharge();
    /*if the user wants to spin the clothes, we are at his proposal*/
    if(0b0 == no_spin)
    {
        mot_high = 0b1;
        rot_time = 0;
        while(rot_time < SPIN_G)
            CLRWDT();
        mot_high = 0b0;
    }
    /*now add special treatments, from channel 3 of detergent dispenser*/
    sol_op_time = 0;
    sol3_on = 0b1;
    if(0b1 == half_load)
    {
        while(VESSEL_IS_HALF != VESSEL_HALF)
            CLRWDT();
    }
    else
    {
        while(VESSEL_IS_FULL != VESSEL_FULL)
            CLRWDT();
    }
    sol3_on = 0b0;
    rot_time = 0;
    while(rot_time < ROT_G)
        CLRWDT();
    /*now, discharge water*/
    discharge();
    /*now, to the next state*/
    di();
    prev_state = G;
    if(G == skip_beg)
        cur_state = skip_end;
    else
        cur_state = H;
    ei();
}

/*implements state H of the washing program sequence*/
void state_h(void)
{
    if(0b0 == no_spin)
    {
        __LIGHT_LED((unsigned char)H);
        ht_on = 0b0;
        discharge();
        mot_high = 0b1;
        mot_rot = 0b1;
        rot_time = 0b0;
        while(rot_time < SPIN_H)
            CLRWDT();
    }
    /*to the next state*/
    di();
    prev_state = H;
    if(H == skip_beg)
        cur_state = skip_end;
    else //if the user doesn't want to alter the normal sequence
        init(); //so, we have finished washing program, so return to idle state
    ei();
}

/*implements state J of the washing program sequence*/
void state_j(void)
{
    
}

/*implements state K of the washing program sequence*/
void state_k(void)
{
    
}

/*implements state L of the washing program sequence*/
void state_l(void)
{
    
}

/*implements state M of the washing program sequence*/
void state_m(void)
{
    
}

/*implements state N of the washing program sequence*/
void state_n(void)
{
    
}

/*implements state P of the washing program sequence*/
void state_p(void)
{
    
}

/*implements state Q of the washing program sequence*/
void state_q(void)
{
    
}
/*discharges water frome the vessel*/
void discharge(void)
{
    if(VESSEL_IS_EMPTY != VESSEL_EMPTY)
    {
        dp_op_time = 0;
        dp_on = 0b1;
        while(VESSEL_IS_EMPTY != VESSEL_EMPTY)
            CLRWDT();
        dp_on = 0b0;
    }
}
void suspend(void);
void resume(void);
/*Interrupt service routine*/
void interrupt isr(void)
{
    /*general purpose variable*/
    static unsigned int i;
    /*used to count ticks*/
    static unsigned long ticks;
    /*set when rotating the motor to the left*/
    static bit mot_is_left;
    /*set when rotating the motor at the high speed*/
    static bit mot_is_high;
    /*if timer 1 has overflowed, so about a half of a second have elapsed*/
    if(TMR1IF == 0b1)
    {
        /*while in suspensions state, time stops*/
        if(SUSPENSION != cur_state)
        {
            seconds++;
            /*if the motor is rotating on low speed, every ROT_DIR_TIMEOUT
              half-seconds, stop/change direction of the motor*/
            if(0b1 == mot_rot)
            {
                if(!mot_high && (seconds % ROT_DIR_TIMEOUT == 0))
                {
                    if(mot_is_rot)
                    {
                        MOTOR_POWER = !MOTOR_ON;
                        mot_is_rot = 0b0;
                    }
                    else
                    {
                        mot_is_left ^= 0b1;
                        if(mot_is_left)
                            MOTOR_DIRECTION = !MOTOR_RIGHT;
                        else
                            MOTOR_DIRECTION = MOTOR_RIGHT;
                        MOTOR_POWER = MOTOR_ON;
                        mot_is_rot = 0b1;
                    }
                }
                /*if the user want to activate hight motor speed, activate the
                  high speed, but if the motor is rotating left, take mechanical torque
                  load into account*/
                else if(mot_high && !mot_is_high)
                {
                    if(mot_is_left)
                    {
                        MOTOR_POWER = !MOTOR_ON;
                        __delay_ms(LOW_HIGH_DELAY_ms);
                    }
                    MOTOR_DIRECTION = MOTOR_RIGHT;
                    mot_is_left = 0b0;
                    MOTOR_SPEED = MOTOR_HIGH;
                    mot_is_high = 0b1;
                    MOTOR_POWER = MOTOR_ON;
                    mot_is_rot = 0b1;
                }
                else if(!mot_high && mot_is_high)
                {
                    MOTOR_SPEED = !MOTOR_HIGH;
                    mot_is_high = 0b0;
                }
            }
            else if(!mot_rot && mot_is_rot)
            {
                MOTOR_POWER = !MOTOR_ON;
                mot_is_rot = 0b0;
            }
            /*a minute has passed*/
            if(seconds >= OVF_MIN)
            {
                seconds = 0;
                /*if we still idle for IDLE_TIMEOUT minutes, then power of the
                  microcontroller to save power*/
                if(BEGINNING == cur_state)
                {
                    idle_time++;
                    if(idle_time >= IDLE_TIMEOUT)
                        SELF_LATCH = !SELF_LATCH_SIG;
                }
                /*if any solenoid is left open for SOLENOID_TIMEOUT minutes
                  without filling the vessel, so something is wrong, reset the
                  microcontroller*/
                if(sol1_on || sol2_on || sol3_on)
                {
                    sol_op_time++;
                    if(sol_op_time >= SOLENOID_TIMEOUT)
                        RST = RST_SIG;
                }
                /*if the drain pump is left on for DRAIN_PUMP_TIMEOUT minutes,
                  without emptying the vessel, so something is wrong, reset the
                  microcontroller*/
                if(0b1 == dp_on)
                {
                    dp_op_time++;
                    if(dp_op_time >= DRAIN_PUMP_TIMEOUT)
                        RST = RST_SIG;
                }
                /*count how many minutes has been rotating for*/
                if(0b1 == mot_rot)
                    rot_time++;
            }
        }
        TMR1IF = 0b0;
    }
    if(TMR2IF)  //if control-timer overflowed
    {
        /*if the door is opened, under any circumstances, suspend the machine*/
        if(DOOR_CHECK_SIG != DOOR_CHECK)
        {
            suspend();
            return;
        }
        /*if the user sets drain pump control flag, activate the pump, and
          vice versa*/
        if(dp_on && !dp_is_on)
        {
            DRAIN_PUMP = DRAIN_PUMP_ON;
            dp_is_on = 0b1;
        }
        else if(!dp_on && dp_is_on)
        {
            DRAIN_PUMP = !DRAIN_PUMP_ON;
            dp_is_on = 0b0;
        }
        /*once the thermostat is off, power off the heater*/
        if(THERMOSTAT_OFF == THERMOSTAT)
        {
            HEATER = !HEATER_ON;
            ht_on = 0b0;
            ht_is_on = 0b0;
        }
        /*if the user wants to fire up the heater, check the thermostat first,
          if the thermostat is on, activate the heater, and vice versa*/
        if(ht_on && !ht_is_on)
        {
            if(THERMOSTAT_OFF == THERMOSTAT)
            {
                HEATER = !HEATER_ON;
                ht_on = 0b0;
                ht_is_on = 0b0;
            }
            else
            {
                HEATER = HEATER_ON;
                ht_is_on = 0b1;
            }
        }
        else if(!ht_on && ht_is_on)
        {
            HEATER = !HEATER_ON;
            ht_is_on = 0b0;
        }
        /*if the user wants to open only one of the solenoids, open it,
          otherwise, close all the solenoids*/
        if(sol1_on && !sol2_on && !sol3_on && !sol_is_on)
        {
            SOLENOID = SOLENOID_ON;
            for(i = 0; i < CHA_REV; i++)
            {
                rev_cw();
                CLRWDT();
                __delay_ms(STEP_DELAY_ms);
            }
            sol_is_on = 0b1;
        }
        else if(!sol1_on && sol2_on && !sol3_on && !sol_is_on)
        {
            SOLENOID = SOLENOID_ON;
            for(i = 0; i < CHB_REV; i++)
            {
                rev_cw();
                CLRWDT();
                __delay_ms(STEP_DELAY_ms);
            }
            sol_is_on = 0b1;
        }
        else if(!sol1_on && !sol2_on && sol3_on && !sol_is_on)
        {
            SOLENOID = SOLENOID_ON;
            for(i = 0; i < CHC_REV; i++)
            {
                rev_cw();
                CLRWDT();
                __delay_ms(STEP_DELAY_ms);
            }
            sol_is_on = 0b1;
        }
        else if(!sol1_on && !sol2_on && !sol3_on && sol_is_on)
        {
            SOLENOID = !SOLENOID_ON;
            mot_off();
            sol_is_on = 0b0;
        }
        TMR2IF = 0b0;
    }
    /*handler of start/stop button*/
    if(INTF)
    {
        __delay_ms(BOUNCING_DELAY_ms);
        /*first, suspend / resume the machine*/
        if(BEGINNING == cur_state)
        {
            LOCK_DOOR = LOCK_DOOR_SIG;
            __delay_ms(RELAY_OP_DELAY_ms);
            /*don not change the state of the machine if the is open*/
            if(DOOR_CHECK_SIG == DOOR_CHECK)
            {
                cur_state = skip_end;
                TMR1ON = 0b1;
                TMR2ON = 0b1;
                DOOR_OPEN_LED = !DOOR_OPEN_LED_ON;
            }
            else
            {
                DOOR_OPEN_LED = DOOR_OPEN_LED_ON;
            }
        }
        else if(SUSPENSION == cur_state)
            resume();
        else
            suspend();
        /*then, if the user keep pushing the button for SECONDS_TO_RESET seconds
          reset the machine*/
        ticks = 0;
        while(EXT_INT_EDGE == RB0)
        {
            CLRWDT();
            ticks++;
            if(ticks >= (TICKS_TO_RESET / 4ul))
            {
                RST = RST_SIG;
            }
        }
        INTF = 0b0;
    }
    /*handler of half-load, no-spin, state-select, and skip-select button*/
    if(RBIF)
    {
        __delay_ms(BOUNCING_DELAY_ms);
        /*handler of no-spin button*/
        if(NO_SPIN_BT_SIG == NO_SPIN_BT)
        {
            /*wait until the user releases the button*/
            while(NO_SPIN_BT_SIG == NO_SPIN_BT)
                CLRWDT();
            /*invert the state of no-spinning, and change the state of the
              indication LED*/
            if(0b1 == no_spin)
            {
                no_spin = 0b0;
                NO_SPIN_LED = !NO_SPIN_LED_ON;
            }
            else
            {
                no_spin = 0b1;
                NO_SPIN_LED = NO_SPIN_LED_ON;
            }
        }
        /*handler of half-load button*/
        if(HALF_LOAD_BT_SIG == HALF_LOAD_BT)
        {
            /*wait until the user releases the button*/
            while(HALF_LOAD_BT_SIG == HALF_LOAD_BT)
                CLRWDT();
            half_load = !half_load; //invert the state of half-loading
            /*adjust the indication LED*/
            if(0b1 == half_load)
                HALF_LOAD_LED = HALF_LOAD_LED_ON;
            else
                HALF_LOAD_LED = !HALF_LOAD_LED_ON;
        }
        /*handler of state-marker button*/
        if(STATE_SEL_BT_SIG == STATE_SEL_BT)
        {
            /*wait until the user releases the button*/
            while(STATE_SEL_BT_SIG == STATE_SEL_BT)
                CLRWDT();
            /*if the user hasn't begun the wash yet, proceed through all active
              states, in a circular manner*/
            if(BEGINNING == cur_state)
            {
                skip_end = ((unsigned char)skip_end + 1u) % ACTIVE_STATE_NUM;
                __LIGHT_LED((unsigned char)skip_end);
            }
            /*if the machine is suspended, proceed through the clothing-type
              group of states, that the user is already in*/
            else if(SUSPENSION == cur_state)
            {
                if(prev_state < NORMAL_STATE_NUM)
                    temp = ((unsigned char)temp + 1u) % NORMAL_STATE_NUM;
                else
                    temp = (((unsigned char)temp + 1u) % (ACTIVE_STATE_NUM - NORMAL_STATE_NUM)) + NORMAL_STATE_NUM;
                __LIGHT_LED((unsigned short)temp);
            }
        }
        /*handler of skip-sequence-select button*/
        if(SKIP_SEL_BT_SIG == SKIP_SEL_BT)
        {
            /*wait until the user releases the button*/
            while(SKIP_SEL_BT_SIG == SKIP_SEL_BT)
                CLRWDT();
            /*only if the machine is at beginning/suspension state, allow the user
              to specify a sequence of states to skip*/
            if(BEGINNING == cur_state || SUSPENSION == cur_state)
            {
                temp_beg = temp_end;
                temp_end = temp;
            }
        }
        RBIF = 0b0;
    }
}


/*void resume(void);
  Resume the sequence.*/
void resume(void)
{
    LOCK_DOOR = LOCK_DOOR_SIG;
    __delay_ms(RELAY_OP_DELAY_ms);
    /*nothing will be done if the door is open*/
    if(DOOR_CHECK_SIG != DOOR_CHECK)
    {
        DOOR_OPEN_LED = DOOR_OPEN_LED_ON;
        return;
    }
    else
        DOOR_OPEN_LED = !DOOR_OPEN_LED_ON;  //extinguish door-open LED
    cur_state = prev_state;
    /*if any device was open before suspension, resume with it open*/
    if(mot_is_rot)
        MOTOR_POWER = MOTOR_ON;
    if(ht_is_on)
        HEATER = HEATER_ON;
    /*make sure that only one solenoid is open, only if drain pump is off*/
    if(dp_is_on)
        DRAIN_PUMP = DRAIN_PUMP_ON;
    if(sol_is_on)
        SOLENOID = SOLENOID_ON;
    /*if the user pressed selected at least two states, in the same 
      clothing-type group of states, take the last two as the beginning and the
      end of the sequence of states that needs to be skipped*/
    if(temp_beg != BEGINNING && ((temp_beg < NORMAL_STATE_NUM && temp_end < NORMAL_STATE_NUM) || (temp_beg >= NORMAL_STATE_NUM && temp_end >= NORMAL_STATE_NUM)))
    {
        skip_beg = temp_beg;
        skip_end = temp_end;
    }
    TMR1ON = 0b1; //resume the main timer
    TMR2ON = 0b1; //resume the control timer
    __LIGHT_LED((unsigned char)cur_state);
}

/*void suspend(void)
 * Suspend the process, but keep the device states, to be able to resume later 
 * with the exact states we suspended the machine with.
 */
void suspend(void)
{
    TMR1ON = 0b0;
    TMR2ON = 0b0;
    MOTOR_POWER = !MOTOR_ON;
    DRAIN_PUMP = !DRAIN_PUMP_ON;
    HEATER = !HEATER_ON;
    LOCK_DOOR = !LOCK_DOOR_SIG;
    SOLENOID = !SOLENOID_ON;
    temp = cur_state;
    prev_state = cur_state;
    cur_state = SUSPENSION;
    temp_beg = BEGINNING;
    temp_end = BEGINNING;
}
