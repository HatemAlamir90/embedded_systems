/* 
 * File:   Keypad.h
 * Author: Hatem Alamir
 *
 * Created on February 1, 2014, 2:41 PM
 * Disc. : Reads input from keypad and wites in a 'long int' representing the input,
 * or a special character from the keypad
 */

#ifndef KEYPAD_H
#define	KEYPAD_H
#ifndef input_port
#define input_port PORTD     //the port connected to the keypad
#endif
#ifndef buzzer
#define buzzer RE0     //whet the user for a putton push; comment this difinition if you don't need a buzzer,
                       //or replace RE0 with the pin you connect the buzzer to
#endif
char keypad_read(volatile unsigned long int *input);     //read the input as a number
char spec_func1(volatile unsigned long int *input);     //special
char spec_func2(volatile unsigned long int *input);     //functions
char spec_func3(volatile unsigned long int *input);     //associated with
char spec_func4(volatile unsigned long int *input);     //special keys
#endif	/* KEYPAD_H */