/*
 * File:   ST7066U.h
 * Author: Hatem Alamir
 *
 * Created on January 30, 2014, 11:57 AM
 * Disc.: functions that manipulate the LCD module with ST7066U driver
 */
#ifndef ST7066U_H
#define	ST7066U_H
extern volatile bit N, F, D, C, B, I_D, S;     //configuration bits of the LCD module
extern volatile unsigned char t0_ovs;     //timer0's overflows counter
/*
 * N:  high=2 lines / low=1 line
 * F:  high=5*8 font / low=5*11 font
 * D:  high=display on / low=display off
 * C:  high=cursor on / low=cursor off
 * B:  high=blink on / low=blink off
 * I_D: high=address increment / low=address decrement
 * S:  high=display/cursor shift on / low=display/cursor shift off
 */
char init();     //function that initializes the LCD module
char disp(char line[],unsigned short int cnt, unsigned char linum);     //function that
//displays a line of 'cnt' characters on the LCD line number 'linum'
char disp_off();
#endif	/* ST7066U_H */