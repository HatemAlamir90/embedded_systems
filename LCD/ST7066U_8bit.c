/*
 * File:   ST7066U.c
 * Author: Hatem Alamir
 * Created on February 13, 2014, 19:07
 * Containing definitions of function targeting ST7066U LCD drivver:
 * init(): initializes the driver
 * disp(): displays a line on the driver
 */
#include <xc.h>     //for delay macros
#include "ST7066U_8bit.h"
#define _XTAL_FREQ 4000000     //cryatal frequency, take care for projects wirh different frequency crystals
bit chk_bf()     //check busy flag
{
    (*out_port_dir) = 0xFF;
    RS = 0b0;
    RW = 0b1;
    EN = 0b1;
    if((*out_port) & 0x80)
    {
     EN = 0b0;
     (*out_port_dir) = 0x00;
     return 0b1;
    }
    else
    {
        EN = 0b0;
        (*out_port_dir) = 0x00;
        return 0b0;
    }
}
//initialization function
void init()
{
    unsigned char temp=0x00;
    __delay_ms(40);     //this initialization
    EN=0b0;             //is procedural
    temp |= N;          //and is described
    temp <<=1;          //in detail
    temp |= F;          //in ST7066U datasheet
    temp <<= 2;         //pleade refer
    temp |= 0x30;      //to this datasheet
    RS=0b0;             //for detailed
    RW=0b0;             //information and
    EN = 0b1;           //description of various instruction
    (*out_port) = temp;
    EN=0b0;
    __delay_us(40);
    EN=0b1;
    (*out_port) = temp;
    EN=0b0;
    __delay_us(40);
    temp=0x00;
    temp |= D;
    temp <<= 1;
    temp |= C;
    temp <<= 1;
    temp |= B;
    temp |= 0x08;
    while(chk_bf() == 0b1)
        __delay_us(80);
    EN = 0b1;
    (*out_port) = temp;
    EN = 0b0;
    __delay_us(40);
    temp = 0x01;
    while(chk_bf() == 0b1)
        __delay_us(80);
    EN = 0b1;
    (*out_port) = temp;
    EN = 0b0;
    __delay_ms(2);
    temp <<= 1;
    temp |= I_D;
    temp <<= 1;
    temp |= S;
    while(chk_bf() == 0b1)
        __delay_us(80);
    EN = 0b1;
    (*out_port) = temp;
    EN = 0b0;
    __delay_us(40);
}
//display a line function
void disp(char const *line,short int cnt,unsigned char linum)
{
    int i=0;
    if(linum == 1)     //if the user selects the second line
    {
        while(chk_bf() == 0b1)     //chick if the module is busy
            __delay_us(80);     //delay before chicking again
        RS = 0b0;     //change the display ram address to the beginning of
        RW = 0b0;     //the second line
        EN = 0b1;     //refer to to ST6066U data sheet for a detailed discription
        (*out_port) = 0xC0;
        EN = 0b0;
        __delay_us(40);
    }
    else     //if the user selects beginnig from line0
    {
        while(chk_bf() == 0b1)     //chick if the module is busy
            __delay_us(80);     //delay before chicking again
        RS = 0b0;     //clear the display
        RW = 0b0;
        EN = 0b1;
        (*out_port) = 0x01;
        EN = 0b0;
        __delay_ms(2);
        for(i;i<cnt;i++)     //for every character of the line to be displayed
        {
            while(chk_bf() == 0b1)     //chick if the module is busy
                __delay_us(80);     //delay before chicking again
            RS = 0b1;     //display the next character of the line
            RW = 0b0;
            EN = 0b1;
            (*out_port) = line[i];
            EN = 0b0;
            __delay_us(40);
        }
    }
}
