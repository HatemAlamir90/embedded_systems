/*
 * File:   Keypad.h
 * Author: Hatem Alamir
 *
 * Created on February 1, 2014, 15:02 PM
 * Disc. : Reads input from keypad and writes in a 'long int' representing the input,
 * 
 */
#include <xc.h>     //for delay macros
#include"Keypad.h"
#define _XTAL_FREQ 4000000     //cryatal frequency, take care for projects wirh different frequency crystals
#ifndef buzzer     //if the buzzer isn't needed
char buzzer;     //define a dummy variable with the identifier 'buzzer'
#endif
char keypad_read(volatile unsigned long int *input)     //intrpret input on the keypad
{
    (*input)=0;     //clear the variable to hold the entered password
    while(1)
    {
        input_port = 0xFE;
        if((input_port & 0x10) == 0)     //if the user pressed key '1'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*input) *= (unsigned long int)10;
            (*input) += 1;     //input-key variable + 1
            buzzer = 0b0;
        }
        else if((input_port & 0x20) == 0)     //if the user pressed key '2'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*input) *= (unsigned long int)10;
            (*input) += 4;     //input-key variable + 2
            buzzer = 0b0;
        }
        else if((input_port & 0x40) == 0)     //if the user pressed key '3'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*input) *= (unsigned long int)10;
            (*input) += 7;     //input-key variable + 3
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
            (*input) *= (unsigned long int)10;
            (*input) += 2;     //input-key variable + 4
            buzzer = 0b0;
        }
        else if((input_port & 0x20) == 0)     //if the user pressed key '5'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*input) *= (unsigned long int)10;
            (*input) += 5;     //input-key variable + 5
            buzzer = 0b0;
        }
        else if((input_port & 0x40) == 0)     //if the user pressed key '6'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*input) *= (unsigned long int)10;
            (*input) += 8;     //input-key variable + 6
            buzzer = 0b0;
        }
        else if((input_port & 0x80) == 0)
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*input) *= (unsigned long int)10;
            buzzer = 0b0;
        }
        input_port = 0xFB;
        if((input_port & 0x10) == 0)     //if the user pressed key '7'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*input) *= (unsigned long int)10;
            (*input) += 3;     //input-key variable + 7
            buzzer = 0b0;
        }
        else if((input_port & 0x20) == 0)     //if the user pressed key '8'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*input) *= (unsigned long int)10;
            (*input) += 6;     //input-key variable + 8
            buzzer = 0b0;
        }
        else if((input_port & 0x40) == 0)     //if the user pressed key '9'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            (*input) *= (unsigned long int)10;
            (*input) += 9;     //input-key variable + 9
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
            return spec_func1(input);     //return special character '*'
        }
        else if((input_port & 0x20) == 0)     //if the user pressed key '0'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            buzzer = 0b0;
            return spec_func2(input);     //input-key variable + 0
        }
        else if((input_port & 0x40) == 0)     //if the user pressed the key '#'
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            buzzer = 0b0;
            return spec_func3(input);
        }
        else if((input_port & 0x80) == 0)     //if the user pressed the fourth special key
        {
            buzzer = 0b1;     //whet the user for a putton push
            __delay_ms(150);     //wait for the button bouncing
            __delay_ms(150);
            buzzer = 0b0;
            return spec_func4(input);     //do the special function associated with the key
        }
    }
    return '\0';
}