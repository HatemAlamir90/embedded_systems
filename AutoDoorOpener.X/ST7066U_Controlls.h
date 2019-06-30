/* 
 * File:   ST7066U_Controlls.h
 * Author: root
 *
 * Created on February 16, 2014, 3:10 PM
 */

#ifndef ST7066U_CONTROLLS_H
#define	ST7066U_CONTROLLS_H
#define EN RC3
#define RS RC1
#define RW RC2
/*
 * RS: register select, high=data / low=instruction
 * RW: read/wright operation, high=read / low=wright
 * EN: enable/disable the module, high=enable / low=disable
 */
#define out_port PORTC     //define the port used by the LCD
#define out_port_dir TRISC     //define the data direction of the port used by the LCD
#endif	/* ST7066U_CONTROLLS_H */