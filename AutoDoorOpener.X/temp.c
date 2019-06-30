
#include "Keypad.h"     //Keypad function declarations
#include "ST7066U_4bit.h"     //LCD module driver function declarations

#define _XTAL_FREQ 4000000     //crystal frequency, important for delay macros
#ifndef buzzer     //if the buzzer isn't needed
char buzzer;     //define a dummy variable with the identifier 'buzzer'
#endif
volatile bit paswd_chng, wrn;     //marks the change of the password, warning state
volatile unsigned long int paswd;     //holds input password, saved password
volatile unsigned char paswd0, paswd1, paswd2, paswd3, wrn_num;     //for manipulation of 4-byte passwords
char chk[]="Enter Password:";     //password entry prompt
char suc[]="Happy Drive !";     //succssful authentication prompt
char rng[]="WRONG PASSWORD!";     //failed authentication prompt
char old[]="OLD Password :";     //old password entry prompt
char new[]="NEW Password :";     //new password entry prompt
char pschng[]="Password Changed";     //password change success
//Keypad special function keys
char spec_func1(volatile unsigned long int *input)     //Enter key
{
    return '\0';
}
char read(volatile unsigned long int *inp)     //replacement for keypad_read() to avoid recursion
{
    (*inp)=0;     //clear the variable to hold the entered password
    while(1)
    {
        input_port = 0xFE;
        if((input_port & 0x10) == 0)     //if the user pressed key '1'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*inp) *= (unsigned long int)10;
            (*inp) += 1;     //input-key variable + 1
            buzzer = 0b0;
        }
        else if((input_port & 0x20) == 0)     //if the user pressed key '2'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*inp) *= (unsigned long int)10;
            (*inp) += 4;     //input-key variable + 2
            buzzer = 0b0;
        }
        else if((input_port & 0x40) == 0)     //if the user pressed key '3'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*inp) *= (unsigned long int)10;
            (*inp) += 7;     //input-key variable + 3
            buzzer = 0b0;
        }
        else if((input_port & 0x80) == 0)     //if the user pressed first special key
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            buzzer = 0b0;
            return '*';     //do the special function associated with the key
        }
        input_port = 0xFD;
        if((input_port & 0x10) == 0)     //if the user pressed key '4'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*inp) *= (unsigned long int)10;
            (*inp) += 2;     //input-key variable + 4
            buzzer = 0b0;
        }
        else if((input_port & 0x20) == 0)     //if the user pressed key '5'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*inp) *= (unsigned long int)10;
            (*inp) += 5;     //input-key variable + 5
            buzzer = 0b0;
        }
        else if((input_port & 0x40) == 0)     //if the user pressed key '6'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*inp) *= (unsigned long int)10;
            (*inp) += 8;     //input-key variable + 6
            buzzer = 0b0;
        }
        else if((input_port & 0x80) == 0)
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*inp) *= (unsigned long int)10;
            buzzer = 0b0;
        }
        input_port = 0xFB;
        if((input_port & 0x10) == 0)     //if the user pressed key '7'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*inp) *= (unsigned long int)10;
            (*inp) += 3;     //input-key variable + 7
            buzzer = 0b0;
        }
        else if((input_port & 0x20) == 0)     //if the user pressed key '8'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*inp) *= (unsigned long int)10;
            (*inp) += 6;     //input-key variable + 8
            buzzer = 0b0;
        }
        else if((input_port & 0x40) == 0)     //if the user pressed key '9'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*inp) *= (unsigned long int)10;
            (*inp) += 9;     //input-key variable + 9
            buzzer = 0b0;
        }
        else if((input_port & 0x80) == 0)     //if the user pressed the third special key
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            buzzer = 0b0;
            return '#';     //do the special function associated with the key
        }
        input_port = 0xF7;
        if((input_port & 0x10) == 0)     //if the user pressed the key '*'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            buzzer = 0b0;
            return spec_func1(inp);     //return special character '*'
        }
        else if((input_port & 0x40) == 0)     //if the user pressed the key '#'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            buzzer = 0b0;
            RA2 = 0b0;     //power off the LCD module
            __delay_ms(197);     //wait for a while
            __delay_ms(197);
            __delay_ms(197);
            __delay_ms(197);
            __delay_ms(197);
            RA2 = 0b1;     //power it up again
            if(init() == 0)     //try to initialize the LCD module
                RA5 = 0b0;     //if succeeded, extinguish the indiacation LED
            else
                RA5 = 0b1;     //if failed, light the indication LED
            return '\0';
        }
        else if((input_port & 0x80) == 0)     //if the user pressed the fourth special key
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            buzzer = 0b0;
            return spec_func4(inp);     //do the special function associated with the key
        }
    }
    return '\0';
}
char spec_func2(volatile unsigned long int *inp)     //change password key
{
    volatile unsigned long int temp;     //temporary hold of input password
    if(disp(old,14,0) == 0b0)     //prompt the user to enter the old password
        RA5 = 0b1;
    else
        RA5 = 0b0;
    read(inp);     //read the old password
    if((*inp)== paswd)     //if user is successfuly authenticated
    {
        if(disp(new,14,0) == 0b0)     //prompt the user to input the new password
            RA5 = 0b1;
        else
            RA5 = 0b0;
        read(inp);     //read it
        temp = (*inp);     //not to alter the entered password
        paswd0 = 0x00;     //divide the input password to four separate bytes
        paswd0 = (volatile unsigned char)temp;
        temp = (*inp);
        temp >>= 8;
        paswd1 = 0x00;
        paswd1 = (volatile unsigned char)temp;
        temp = (*inp);
        temp >>= 16;
        paswd2 = 0x00;
        paswd2 |= (volatile unsigned char)temp;
        temp = (*inp);
        temp >>= 24;
        paswd3 = 0x00;
        paswd3 |= (volatile unsigned char)temp;
        eeprom_write(0x00,(paswd0));     //save the 4-byte password in the EEPROM
        eeprom_write(0x01,(paswd1));
        eeprom_write(0x02,(paswd2));
        eeprom_write(0x03,(paswd3));
        while(WR);
        if(disp(pschng,16,0) == 0b0)     //tell the user the password has been changed
            RA5 = 0b1;
        else
            RA5 = 0b0;
        __delay_ms(197);
        __delay_ms(197);
        __delay_ms(197);
        __delay_ms(197);
        __delay_ms(197);
        __delay_ms(197);
        __delay_ms(197);
        __delay_ms(197);
        paswd_chng = 0b1;
        return '\0';
    }
    else     //if the user failed to be authenticated right
    {
        return '\0';     //return immediately
    }
}
char spec_func3(volatile unsigned long int *input)     //the same as Enter key
{
    RA2 = 0b0;     //power off the LCD module
    __delay_ms(197);     //wait for a while
    __delay_ms(197);
    __delay_ms(197);
    __delay_ms(197);
    __delay_ms(197);
    RA2 = 0b1;     //power it up again
    if(init() == 0)     //try to initialize the LCD module
        RA5 = 0b0;     //if succeeded, extinguish the indiacation LED
    else
        RA5 = 0b1;     //if failed, light the indication LED
    return '\0';
}
char spec_func4(volatile unsigned long int *input)     //the same as Enter key
{
    return '\0';
}
void interrupt isr()     //interrupt service routine
{
    volatile unsigned char temp;     //holds portB value
    volatile unsigned long int input;
    if(RBIF)     //if somebody is trying to do something
    {
        temp = PORTB;     //read portB to end the mismatch condition
        if(RB4 == 0b0)     //if the user is trying to ignit the car
        {
            RC0 = 0b1;     //light the LCD backlight
            paswd0 = eeprom_read(0x00);     //restore the saved password
            paswd1 = eeprom_read(0x01);
            paswd2 = eeprom_read(0x02);
            paswd3 = eeprom_read(0x03);
            paswd = 0x00;     //reconstruct the password
            paswd |= (volatile unsigned long int)paswd3;
            paswd <<=8;
            paswd |= (volatile unsigned long int)paswd2;
            paswd <<=8;
            paswd |= (volatile unsigned long int)paswd1;
            paswd <<=8;
            paswd |= (volatile unsigned long int)paswd0;
            if(disp(chk,15,0x00) == 0)     //prompt the user to enter the password
                RA5 = 0b0;
            else
                RA5 = 0b1;
            keypad_read(&input);     //read the password
            while((input != paswd) && (paswd_chng == 0b0))     //while the user doesn't remeber
            {
                if(disp(rng,15,0) == 0b0)     //warn the user for a wrong password
                    RA5 = 0b0;
                else
                    RA5 = 0b1;
                __delay_ms(190);     //wait for about 2 seconds
                __delay_ms(190);
                __delay_ms(190);
                __delay_ms(190);
                __delay_ms(190);
                __delay_ms(190);
                __delay_ms(190);
                __delay_ms(190);
                __delay_ms(190);
                __delay_ms(190);
                if(disp(chk,15,0) == 0b0)     //prompt him again
                    RA5 = 0b0;
                else
                    RA5 = 0b1;
                keypad_read(&input);     //read the password again
            }
            paswd_chng = 0b0;     //clear password change flag
            RA0 = 0b1;     //close car ignition circuit
            RA1 = 0b1;     //connect the fuel pump
            RE1 = 0b1;     //connect the car key connections
            RE2 = 0b0;     //switch off the horn
            wrn_num = 0;     //reset warnings counter
            if(disp(suc,13,0) == 0b0)     //prompt the user to happily drive
                RA5 = 0b0;
            else
                RA5 = 0b1;
            __delay_ms(197);
            __delay_ms(197);
            __delay_ms(197);
            __delay_ms(197);
            __delay_ms(197);
            __delay_ms(197);
            __delay_ms(197);
            __delay_ms(197);
            __delay_ms(197);
            __delay_ms(197);
            RC0 = 0b0;     //distinguish the LCD backlight
        }
        if(RB4 == 0b1)     //if the user turned off the car
        {
            RA0 = 0b0;     //open the car ignition circuit
            RA1 = 0b0;     //disconnect the fuel pump
            RE1 = 0b0;     //disconnect the car key connctions
            disp_off();     //turn off the display
            RA5 = 0b0;     //extinguish the LCD error indication LED

        }
        if(RB5 == 0b0)     //somebody is trying to open the door
        {
            if(wrn == 0b1)      //without switching the car
                RE2 =0b1;     //release the horn
        }
        if(RB6 == 0b0)     //somebody is trying to open the engine hood
        {
            if(wrn == 0b1)      //without switching the car
                RE2 = 0b1;     //release the horn
        }
        if(RB7 == 0b0)     //somebody approaches the door
        {
            RE2 = 0b1;     //release the horn
            wrn_num++;
        }
        else     //no body near the door
            RE2 = 0b0;     //switch the horn off
        RBIF = 0b0;     //clear the interrupt flag
    }
    if(INTF)     //if the user gives the sign
    {
        wrn ^= 0b1;     //toggle the state of the door opening warning
        RA3 ^= 0b1;     //toggle the indication LED
        RE2 = 0b0;     //extinguish the horn
        INTF = 0b0;     //clear the interrupt flag
    }
}
void main()
{
    T1CON = 0x00;     //diable timer1 not to interfere with portC
    TRISC = 0x00;     //RC4:7 output initially for the LCD module data
    TRISD = 0xF0;
    CMCON |= 0x07;     //comparator off
    CVREN = 0b0;     //disable comparator reference circuit
    ADCON1 = 0x07;     //no analog pins on portA
    ADON = 0b0;     //turn off the A/D module
    T0CS = 0b0;     //use internal clock for timer0
    TRISA = 0x00;     //portA output to control the LCD module
    TRISB = 0xFF;     //portB input, for interrupt on change and enable
    nRBPU = 0b0;     //portB pull-up resistors
    RBIF = 0b0;     //clear portB on-change interrupt flag
    RBIE = 0b1;     //enable portB on-change interrupt
    INTF = 0b0;     //clear external interrupt flag
    INTEDG = 0b0;     //select the falling edge to generate the interrupt
    INTE = 0b1;     //enable external interrupt
    paswd_chng = 0b0;     //password is not changed initially
    N=0b1;     //LCD setting values, refer to ST7066U_4bit.h
    F=0b0;
    D=0b1;
    C=0b0;
    B=0b0;
    I_D=0b1;
    S=0b0;
    wrn = 0b0;     //door lock alarm inactive
    wrn_num = 0;     //no warnings yet
    RA3 = 0b0;     //door alarm indication off
    TRISE0 = 0b0;
    TRISE1 = 0b0;
    TRISE2 = 0b0;
    RE0 = 0b1;     //when the PIC is burned out, this pin goes low to allow the car to be powered normally
    RA0 = 0b0;     //the car ignition circuit is initially open
    RA1 = 0b0;     //fuel pump is initially disconnected
    RE1 = 0b0;     //car key connections is intially disconnected
    RE2 = 0b0;     //the horn is initially off
    RA2 = 0b1;     //power on the LCD module (if off)
    if(init() == 0)     //initialize the LCD module
        RA5 = 0b0;
    else
        RA5 = 0b1;
    PORTB;
    GIE = 0b1;     //enable interrupts globally
    while(1)     //if no action needed
        SLEEP();     //sleep to sava power
}