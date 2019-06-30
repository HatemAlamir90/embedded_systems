/*
 * File:   ST7066U.c
 * Author: Hatem Alamir
 * Created on January 30, 2014, 13:38
 * Containing definitions of function targeting ST7066U LCD drivver:
 * init(): initializes the driver
 * disp(): displays a line on the driver
 */
#include <xc.h>     //for delay macros
#include "ST7066U_Controlls.h"
#define _XTAL_FREQ 4000000     //cryatal frequency, take care for projects wirh different frequency crystals
volatile bit N, F, D, C, B, I_D, S;     //configuration bits of the LCD module
volatile unsigned char t0_ovs;     //timer0's overflows counter
unsigned char bf_fails;
char chk_bf()     //check busy flag
{
    char bf;
    EN = 0b0;
    out_port_dir |= 0xF0;
    out_port &= 0x0F;
    RS = 0b0;
    RW = 0b1;
    EN = 0b1;
    __delay_us(4);
    bf = out_port;
    EN = 0b0;
    __delay_us(4);
    EN = 0b1;
    __delay_us(4);
    EN = 0b0;
    RW = 0b0;
    out_port_dir &= 0x0F;
    if((bf & 0x80) == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
//initialization function
char init()
{
    volatile unsigned char temp;
    __delay_ms(50);     //this initialization
    EN=0b0;             //is procedural
    temp = 0x30;        //refer to ST7066U datasheet
    RS = 0b0;           //for detailed information
    RW = 0b0;
    out_port &= 0x0F;
    EN = 0b1;
    out_port |= temp;
    __delay_us(4);
    EN = 0b0;
    __delay_us(40);
    temp = 0x20;
    out_port &= 0x0F;
    EN = 0b1;
    out_port |= temp;
    __delay_us(4);
    EN = 0b0;
    temp = 0x00;
    if(N == 0b1)
        temp |= 0x01;
    temp <<= 1;
    if(F == 0b1)
        temp |= 0x01;
    temp <<= 6;
    out_port &= 0x0F;
    EN = 0b1;
    out_port |= temp;
    __delay_us(4);
    EN = 0b0;
    __delay_us(40);
    temp = 0x20;
    out_port &= 0x0F;
    EN = 0b1;
    out_port |= temp;
    __delay_us(4);
    EN = 0b0;
    temp = 0x00;
    if(N == 0b1)
        temp |= 0x01;
    temp <<= 1;
    if(F == 0b1)
        temp |= 0x01;
    temp <<= 6;
    out_port &= 0x0F;
    EN = 0b1;
    out_port |= temp;
    __delay_us(4);
    EN = 0b0;
    __delay_us(40);
    bf_fails = 0x00;
    while(chk_bf() == 1)
    {
        __delay_us(80);
        bf_fails++;
        if(bf_fails >= 25)
        {
            return 1;
        }
    }
    out_port &= 0x0F;
    EN = 0b1;
    __delay_us(4);
    EN = 0b0;
    temp = 0x01;
    temp <<= 1;
    if(D == 0b1)
        temp |= 0x01;
    temp <<= 1;
    if(C == 0b1)
        temp |= 0x01;
    temp <<= 1;
    if(B == 0b1)
        temp |= 0x01;
    temp <<= 4;
    EN = 0b1;
    out_port |= temp;
    __delay_us(4);
    EN = 0b0;
    __delay_us(40);
    bf_fails = 0x00;
    while(chk_bf() == 1)
    {
        __delay_us(80);
        bf_fails++;
        if(bf_fails >= 25)
        {
            return 1;
        }
    }
    out_port &= 0x0F;
    EN = 0b1;
    __delay_us(4);
    EN = 0b0;
    __delay_us(4);
    EN = 0b1;
    out_port |= 0x10;
    __delay_us(4);
    EN = 0b0;
    __delay_ms(2);
    bf_fails = 0x00;
    while(chk_bf() == 1)
    {
        __delay_us(80);
        bf_fails++;
        if(bf_fails >= 25)
        {
            return 1;
        }
    }
    out_port &= 0x0F;
    EN = 0b1;
    __delay_us(4);
    EN = 0b0;
    temp = 0x01;
    temp <<= 1;
    if(I_D == 0b1)
        temp |= 0x01;
    temp <<= 1;
    if(S == 0b1)
        temp |= 0x01;
    temp <<= 4;
    EN = 0b1;
    out_port |= temp;
    __delay_us(4);
    EN =0b0;
    __delay_us(40);
    return 0;
}
//display a line function
char disp(char line[],unsigned short int cnt,unsigned char linum)
{
    unsigned short int i;
    char ch;
    bf_fails = 0x00;
    while(chk_bf() == 1)
    {
        __delay_us(80);
        ++bf_fails;
        if(bf_fails >= 25)
        {
            return 1;
        }
    }
    RS = 0b0;     //clear the display
    RW = 0b0;
    out_port &= 0x0F;
    EN = 0b1;
    __delay_us(4);
    EN = 0b0;
    __delay_us(4);
    EN = 0b1;
    out_port |= 0x10;
    __delay_us(4);
    EN = 0b0;
    __delay_ms(2);
    while(chk_bf() == 1)
        {
            __delay_us(80);
            bf_fails++;
            if(bf_fails >= 25)
            {
                return 1;
            }
        }
    out_port &= 0x0F;
    EN = 0b1;
    __delay_us(4);
    EN = 0b0;
    __delay_us(4);
    EN = 0b1;
    out_port |= 0xC0;
    __delay_us(4);
    EN = 0b0;
    __delay_us(40);
    if(linum == 0x01)     //if the user selects the second line
    {
        while(chk_bf() == 1)
        {
            __delay_us(80);
            bf_fails++;
            if(bf_fails >= 25)
            {
                return 1;
            }
        }
        RS = 0b0;             //change the display ram address to the beginning of
        RW = 0b0;             //the second line,
        out_port &= 0x0F;
        EN = 0b1;
        out_port |= 0xC0;     //go there
        __delay_us(4);
        EN = 0b0;
        __delay_us(4);
        out_port &= 0x0F;
        EN = 0b1;
        __delay_us(4);
        EN = 0b0;
        __delay_us(40);
    }
    if(linum == 0x00)     //if the user selects beginnig from line0
    {
        while(chk_bf() == 0b1)
        {
            __delay_us(80);
            ++bf_fails;
            if(bf_fails >= 25)
            {
                return 1;
            }
        }
        EN = 0b0;
        RS = 0b0;
        RW = 0b0;
        out_port &= 0x0F;
        EN = 0b1;
        out_port |= 0x80;
        __delay_us(4);
        EN = 0b0;
        __delay_us(4);
        out_port &= 0x0F;
        EN = 0b1;
        __delay_us(4);
        EN = 0b0;
        __delay_us(40);
    }
    for(i = 0; i<cnt; i++)     //for every character of the line to be displayed
    {
            bf_fails = 0x00;
            while(chk_bf() == 1)
            {
                __delay_us(80);
                 ++bf_fails;
                if(bf_fails >= 25)
                {
                    return 1;
                }
            }
            ch = line[i];     //buffer the next character
            ch &= 0xF0;     //remove the low nibble
            RS = 0b1;
            RW = 0b0;
            out_port &= 0x0F;
            EN = 0b1;
            out_port |= ch;     //put the high nibble to be sent on the high nibble of the output 
            __delay_us(2);
            EN = 0b0;
            ch = line[i];
            ch <<= 4;     //prepare the low nibble
            ch &= 0xF0;
            out_port &= 0x0F;
            EN = 0b1;
            out_port |= ch;
            __delay_us(2);
            EN = 0b0;
            __delay_us(250);
    }
    RA0 = 0b1;
    return 0;
}
char disp_off()
{
    while(chk_bf() == 1)
    {
        __delay_us(80);
        ++bf_fails;
        if(bf_fails >= 25)
        {
            return 1;
        }
    }
    EN = 0b0;
    RS = 0b0;
    RW = 0b0;
    out_port &= 0x0F;
    EN = 0b1;
    __delay_us(4);
    EN = 0b0;
    __delay_us(4);
    EN = 0b1;
    out_port |= 0x80;
    __delay_us(4);
    EN = 0b0;
    __delay_us(40);
    return 0;
}