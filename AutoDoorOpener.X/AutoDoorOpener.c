/* 
 * File:    AutoDoorOpener.c
 * Author: Hatem Alamir
 *
 * Created on May 21, 2015, 12:34 PM
 *
 * The main file containig the basic algorithm.
 */

#include <xc.h>     //compiler definitions and declarations
#include "ST7066U_8bit.h"       //LCD functions
//Device configuration
#pragma config USBDIV = 2, CPUDIV = OSC4_PLL6, PLLDIV = 1, IESO = OFF, \
               FCMEN = OFF, FCMEN = OFF, FOSC = INTOSCIO_EC, VREGEN = OFF, \
               BORV = 3, BOR = ON, PWRT = ON, WDTPS = 1, WDT = OFF, \
               MCLRE = ON,LPT1OSC = OFF, PBADEN = OFF, CCP2MX = ON, \
               DEBUG = OFF, XINST = OFF, ICPRT = OFF, LVP = OFF, \
               STVREN = OFF, CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF, \
               CPD = OFF, CPB = OFF, WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, \
               WRT3 = OFF, WRTD = OFF, WRTB = OFF, WRTC = OFF, EBTR0 = OFF, \
               EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF, EBTRB = OFF
/*
 *Settings:
    *USBDIV: USB clock selection (Full speed only), 96 MHz PLL /2
    *CPUDIV: system clock sostscaler selection, 96 MHz PLL / 6
    *PLLDIV: PLL prescaler selection, no PLL prescalar
    *IESO: Internal/external oscillator switchover, disabled
    *FCMEN: Fail-safe clock monitor enable, disabled
    *FOSC: Oscillator Selection, internal oscillator, port function on RA6,
    *      EC used by USB
    *VREGEN: USB voltage regulator enable, disabled
    *BORV: Brown-out reset voltage, minimum setting (2.00 to 2.16 V)
    *BOR: Brown-out reset enable, enable in hardware only
    *PWRT: Power-up timer enable, enabled
    *WDTPS: Watchdog timer postscale select, 1:1
    *WDT: Watchdog timer enable, disabled
    *MCLRE: MCLR pin enable, enabled, RE3 pin disabled
    *LPT1OSC: Low-power timer1 oscillator enable, disabled, higher power operation
    *PBADEN: PORTB A/D enable, disabled, PORTB<4:0> pins are configured as
    *        digital I/O on Reset
    *CCP2MX: CCP2 MUX, CCP2 input/output is multiplexed with RC1
    *DEBUG: Background debugger enable, disabled
    *XINST: Extended instruction set enable, disabled (legacy mode)
    *ICPRT: Dedicated in-circuit debug/programming port enable, disabled
    *LVP: Single-supply ICSP enable, disabled
    *STVREN: Stack full/underflow reset enable, disabled
    *CP0: Block0 (000800-001FFFh) code protection, disabled
    *CP1: Block1 (002000-003FFFh) code protection, disabled
    *CP2: Block2 (004000-005FFFh) code protection, disabled
    *CP3: Block3 (006000-007FFFh) code protection, disabled
    *CPD: Data EEPROM code protection, disabled
    *CPB: Boot block (000000-0007FFh) code protection, disabled
    *WRT0: Block0 (000800-001FFFh) write protection, disabled
    *WRT1: Block1 (002000-003FFFh) write protection, disabled
    *WRT2: Block2 (004000-005FFFh) write protection, disabled
    *WRT3: Block3 (006000-007FFFh) write protection, disabled
    *WRTD: Data EEPROM write protection, disabled
    *WRTB: Boot block write protection, disabled
    *WRTC: Configuration register write protection, disabled
    *EBTR0: Block0 (000800-001FFFh) table read protection, disabled
    *EBTR1: Block1 (002000-003FFFh) table read protection, disabled
    *EBTR2: Block2 (004000-005FFFh) table read protection, disabled
    *EBTR3: Block3 (006000-007FFFh) table read protection, disabled
    *EBTRB: Boot block table read protection, disabled
 */
#define _XTAL_FREQ 8000000
#define OPEN RC6        //pin used to drive the motor to open the door
#define CLOSE RC7       //pin used to drive the motor to close the door
#define MOT_ENA RC5     //pin used to enable the H-Bridge chip that drives the motor
#define SOL RA4     //pin used to ignite the solenoid
static volatile unsigned int time = 0;     //used to track time elapsed
static volatile bit open;       //set if the user wants to open the door
static volatile bit close;      //set if the user wants to close the door
static volatile bit sleepy;     //set if the processor has to sleep
static volatile bit warn;       //state of the warninig
void interrupt isr(void)     //interrupt service routine
{
  if(INT0IF)
  {
      INT0IF = 0b0;     //clear interrupt flag
      if(warn)
      {

      }
  }
  if(INT1IF)        //the user wants to open the door
  {
      INT1IF = 0b0;     //clear interrupt flag
      time = 0;     //reset the time
      open = 0b1;       //set the open flag
      close = 0b0;      //unset the close flag
      SOL = 0b1;        //power the solenoids
      TMR0H = 0;        //reset timer0
      TMR0L = 0;
  }
  if(INT2IF)     //the user wants to close the door
  {
      INT2IF = 0b0;     //clear interrupt flag
      time = 0;     //reset the time
      CLOSE = 0b1;      //ignite the motor to close the door
      TMR3 = 0;     //reset timer3
  }
  if(ADIF)
  {
      ADIF = 0b0;     //clear interrupt flag
  }
  if(RBIF)
  {
      RBIF = 0b0;     //clear interrupt flag
  }
  if(TMR0IF)        //timer 0 overflow, every 4.194304 seconds
  {
      TMR0IF = 0b0;     //clear interrupt flag
      if(open == 0b1)       //if the user wanted to open the door
      {
          open = 0b0;       //clear the open flag
          OPEN = 0b1;       //ignite the motor to open the door
          TMR3 = 0;     //reset timer3
          SOL = 0b0;        //unpower the solenoids
      }
      else if(close = 0b1)      //if the user wanted to close the door
      {
          close = 0b0;      //clear the close flag
          SOL = 0b0;        //unpower the solenoids
      }
      else
      {
          time++;
          if(time >= (unsigned int)143)     //if about 10 minuets has elapsed since
          {                                 //the last interaction
              time = 0;     //reset the time
              sleepy = 0b1;     //set the sleep flag
              INT2IF = 0b1;     //close the door
          }
      }
  }
  if(TMR3IF)
  {
      
  }
}

void main(void)     //main function
{
    di();       //disable interrupt to avoid accedential interrupts

    warn = 0b0;     //disable the warning to avoid undesired warnings

    //Input/output ports settings
    //Port A
    TRISA = 0xDF;
    /*
     * RA1:RA0 = 0b11, analog channels
     * RA3 = 0b1, positive voltage reference for the A/D module
     * RA4 = 0b0, solnoids ignition pin
     */
    USBEN = 0b0;        //disable USB module, not to affect RA4, RB2, RB3
    CVREN = 0b0;        //disable comparator module, not to affect RA2
    CVROE = 0b0;        //disable comparator module output, not to affect RA4,RA5
    CMCON = 0x07;       //disable analog comparator, not to affect RA0:RA3
    //Port B
    TRISB = 0xFF;       /*All port B is input, to make use of the internal pull-up
                         *resistors, and port B interrupt-on-change functionality*/
    nRBPU = 0b1;     //PORTB pull-ups are enabled
    SSPEN = 0b0;        /*Disable master synchronous serial port, not to affect
                          RB0:RB1*/
    SPPEN = 0b0;        /*disable streaming parallel port, not to affect RB4
                          RD0:RD7*/
    CSEN = 0b0;         //disable SPP chip select pin, not to affect RB4
    
    //Port C
    TRISC = 0x06;
    /*
     * RC0 = N/C
     * RC1 = N/C
     * RC2 = ECCP1 capture input from encoder
     * RC3 = NOT IMPLEMENTED in this device
     * RC4 = N/C
     * RC5 = motor H-Bridge enable
     * RC6 = motor direction 1 (IN1 on the h-bridge), open the door
     * RC7 = motor direction 2 (IN2 on the h-brigde), close the door
     */
    SPEN = 0b0;     //disable EUSART module, not to affect RC6:RC7
    TXEN = 0b0;     //disable transmit function in EUSART
    //Port D
    TRISD = 0x00;       //All port D is output, LCD data bits
    
    RDPU = 0b0;     //disable port D internal pull up resistors
    //Port E
    TRISE = 0xF8;
    /*
     * RE0 = LCD register select (RS)
     * RE1 = LCD read/write (R/W)
     * RE2 = LCD enable (E)
     * RE3 = MCLR
     */
    CLK1EN = 0b0;       //disable SPP clolck1, not to affect RE0
    
    //Configure interrupts and modules
    IPEN = 0b0;     //disable interrupt priority feature
    PEIE = 0b1;     //enable peripheral interrupts
    //External interrupt 0(INT0)
    INT0IF = 0b0;     //clear interrupt flag
    INTEDG0 = 0b0;       //on is on falling edge
    INT0IE = 0b1;     //enable interrupt
    //External interrupt 1(INT1)
    INT1IF = 0b0;     //clear interrupt flag
    INTEDG1 = 0b0;       //on is on falling edge
    INT1IP = 0b0;       //low priority
    INT1IE = 0b1;     //enable interrupt
    //External interrupt 2(INT2)
    INT2IF = 0b0;     //clear interrupt flag
    INTEDG2 = 0b0;       //on is on falling edge
    INT2IP = 0b0;       //low priority
    INT2IE = 0b1;     //enable interrupt
    //RB port change interrupt
    RBIF = 0b0;     //clear interrupt flag
    RBIP = 0b0;     //low priority
    RBIE = 0b1;     //enable interrupt
    //Timer0 module
    TMR0IF = 0b0;       //clear timer 0 interrupt flag
    TMR0IP = 0b0;       //low priority
    T0CON = 0x85;
    /*
     * T0ON (MSB) = 0b1, timer 0 on
     * T08BIT = 0b0, 16 bit mode
     * T0CS = 0b0, internal clock
     * T0SE = 0b0, not used in internal clock mode
     * PSA = 0b1, prescalar is assigned
     * T0PS2:T0PS0 = 0b101, 1:64 prescalar
     */
    TMR0IE = 0b1;       //enable timer 0 interrupt
    //Timer3 module
    T3CON = 0xC9;
    /*
     * RD16 = 0b1, register read/write in one 16 bit operation
     * T3CCP2:T3CCP1 = 0b11, timer3 is the clock source for both CCP modules
     * T3CKPS1:T3CKPS0 = 0b00, 1:1 prescalar value
     * nT3SYNC = 0b0, not used when using internal clock for timer3
     * T3CS = 0b0, using internal clock
     * TMR3ON = 0b1, timer3 is on
     */
    TMR3IF = 0b0;       //clear timer3 interrupt flag
    TMR3IP = 0b0;       //low priority
    TMR3IE = 0b1;        //enable timer3 interrupt
    //A/D module
    ADIF = 0b0;     //clear A/D interrupt flag
    ADIP = 0b0;     //low priority
    ADCON0 = 0x00;
    /*
     * CH3:CH0 = 0b0000, channel zero
     * GODONE = 0b0, idle
     * ADON = 0b0, disabled to save current
     */
    ADCON1 = 0x1D;
    /*
     * VCFG0 = 0b01, Vref- = Vss, Vref+ = Voltage on AN3
     * PCFG3:PCFG0 = 0b1101, only RA0 and RA1 are analog channels
     */
    ADCON2 = 0x97;
    /*
     * ADFM = 0b0, result is right justified
     * ACQT2:ACQT0 = 0b010, 4Tad aquisition time
     * ADCS2:ADCS0 = 0b111, clock derived from A/D RC oscillator
     */
    ADIE = 0b1;     //enable A/D interrupt
    //ECCP module
    CCP1CON = 0x05;
    /*
     * CCP1M3:CCP1M0 = 0b0101, capture mode every rising edge
     */
    //CCP module
    CCP2CON = 0x00;     //disabled
    ei();       //enable interrupts
    if(RB0 == 0b0)      //if at least one of the solenoids is open
    {
        SOL = 0b1;

    } 
    while(1)
    {}
}