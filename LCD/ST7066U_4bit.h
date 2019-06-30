/*
 * File:   ST7066U.h
 * Author: Hatem Alamir
 *
 * Created on January 30, 2014, 11:57 AM
 * Disc.: functions that manipulate the LCD module with ST7066U driver
 */
#ifndef ST7066U_H
#define	ST7066U_H
#define EN RC3
#define RS RC2
#define RW RA0
/*
 * RS: register select, high=data / low=instruction
 * RW: read/wright operation, high=read / low=wright
 * EN: enable/disable the module, high=enable / low=disable
 */
volatile unsigned char *out_port;     //variable holds the data to be sent/received from the LCD module
volatile unsigned char *out_port_dir;     //variable controls data direction of the port connected to the module
volatile bit N, F, D, C, B, I_D, S;     //configuration bits of the LCD module
/*
 * N:  high=2 lines / low=1 line
 * F:  high=5*8 font / low=5*11 font
 * D:  high=display on / low=display off
 * C:  high=cursor on / low=cursor off
 * B:  high=blink on / low=blink off
 * I_D: high=address increment / low=address decrement
 * S:  high=display/cursor shift on / low=display/cursor shift off
 */
void init();     //function that initializes the LCD module
void disp(char const *line,short int cnt, unsigned char linum);     //function that
//displays a line of 'cnt' characters on the LCD line number 'linum'
#endif	/* ST7066U_H */