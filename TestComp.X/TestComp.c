//TestComp.c
//Compares two analog inputs and lightens a LEDs on when input on pinD1 > input on pind0
#include <xc.h>     //important definitions and declarations
#pragma config OSC=HS, WDT=OFF     //high speed oscillator, no WatchDog Timer
void main()
{
    CMCON &= 0xCF;     //outputs not inverted
    CMCON &= 0xF8;
    CMCON |= 0x01;     //single independent comparator with output
    TRISD = 0xFF;     //portD input, comparator inputs on pinD0 and pinD1
    CVREN = 0b0;     //disable internal comparator reference module
    GIE = 0b0;     //diable all interrupts
    TRISE1 = 0b0;     //pinE0 output for comparator output
    while(1);
}