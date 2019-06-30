/*
 * File:   ST7066U.c
 * Author: Hatem Alamir
 * Created on January 30, 2014, 13:38
 * Containing definitions of function targeting ST7066U LCD drivver:
 * init(): initializes the driver
 * disp(): displays a line on the driver
 */
#include <xc.h>     //for delay macros
#include "ST7066U_4bit.h"
#define _XTAL_FREQ 4000000     //cryatal frequency, take care for projects wirh different frequency crystals
bit chk_bf()     //check busy flag
{
    bit bf;
    (*out_port_dir) |= 0xF0;
    RS = 0b0;
    RW = 0b1;
    EN = 0b1;
    if((*out_port) & 0x80)
    {
        bf = 0b1;
    }
    else
    {
        bf = 0b0;
    }
    EN = 0b0;
    NOP();
    EN = 0b1;
    NOP();
    EN = 0b0;
    RW = 0b0;
    (*out_port_dir) &= 0x0F;
    return bf;
}
//initialization function
void init()
{
    unsigned char temp=0x00;
    __delay_ms(40);     //this initialization
    EN=0b0;             //is procedural
    temp = 0x30;
    RS = 0b0;
    RW = 0b0;
    EN = 0b1;
    (*out_port) &= 0x0F;
    (*out_port) |= temp;
    EN = 0b0;
    __delay_us(40);
    temp = 0x20;
    EN = 0b1;
    (*out_port) &= 0x0F;
    (*out_port) |= temp;
    EN = 0b0;
    temp = 0x00;
    temp |= N;
    temp <<= 1;
    temp |= F;
    temp <<= 6;
    EN = 0b1;
    (*out_port) &= 0x0F;
    (*out_port) |= temp;
    EN = 0b0;
    __delay_us(40);
    temp = 0x20;
    EN = 0b1;
    (*out_port) &= 0x0F;
    (*out_port) |= temp;
    EN = 0b0;
    temp = 0x00;
    temp |= N;
    temp <<= 1;
    temp |= F;
    temp <<= 6;
    EN = 0b1;
    (*out_port) &= 0x0F;
    (*out_port) |= temp;
    EN = 0b0;
    __delay_us(40);
    while(chk_bf() == 0b1)
        __delay_us(80);
    EN = 0b1;
    (*out_port) &= 0x0F;
    EN = 0b0;
    temp = 0x01;
    temp <<= 1;
    temp |= D;
    temp <<= 1;
    temp |= C;
    temp <<= 1;
    temp |= B;
    temp <<= 4;
    EN = 0b1;
    (*out_port) |= temp;
    EN = 0b0;
    __delay_us(40);
    while(chk_bf() == 0b1)
        __delay_us(80);
    EN = 0b1;
    (*out_port) &= 0x0F;
    EN = 0b0;
    NOP();
    EN = 0b1;
    (*out_port) |= 0x10;
    EN = 0b0;
    __delay_us(1520);
    while(chk_bf() == 0b1)
        __delay_us(80);
    EN = 0b1;
    (*out_port) &= 0x0F;
    EN = 0b0;
    temp = 0x01;
    temp <<= 1;
    temp |= I_D;
    temp <<= 1;
    temp |= S;
    temp <<= 4;
    EN = 0b1;
    (*out_port) |= temp;
    __delay_us(40);
}
//display a line function
void disp(char const *line,short int cnt,unsigned char linum)
{
    int i=0;
    char ch;
    if(linum == 1)     //if the user selects the second line
    {
        while(chk_bf() == 0b1)     //chick if the module is busy
            __delay_us(80);     //delay before chicking again
        RS = 0b0;             //change the display ram address to the beginning of
        RW = 0b0;             //the second line,
        (*out_port) &= 0x0F;
        EN = 0b1;
        (*out_port) |= 0xC0;     //go there
        EN = 0b0;
        NOP();
        EN = 0b1;
        (*out_port) &= 0x0F;
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
        (*out_port) &= 0x0F;
        EN = 0b0;
        NOP();
        EN = 0b1;
        (*out_port) |= 0x10;
        EN = 0b0;
        __delay_us(1520);
        for(i;i<cnt;i++)     //for every character of the line to be displayed
        {
            while(chk_bf() == 0b1)     //chick if the module is busy
                __delay_us(80);     //delay before chicking again
            ch = line[i];     //buffer the next character
            ch &= 0xF0;     //remove the low nibble
            RS = 0b1;
            RW = 0b0;
            (*out_port) &= 0x0F;
            EN = 0b1;
            (*out_port) |= ch;     //put the high nibble to be sent on the high nibble of the output 
            EN = 0b0;
            ch = line[i];
            ch <<= 4;     //prepare the low nibble
            (*out_port) &= 0x0F;
            EN = 0b1;
            (*out_port) |= ch;
            EN = 0b0;
            __delay_us(37);
        }
    }
}