/*
 * File:   ST7066U_8bit.c
 * Author: Hatem Alamir
 * Created on May 26, 2015, 09:22
 * Containing definitions of functions suing ST7066U LCD drivver:
 * init(): initializes the driver
 * disp(): displays a line on the driver
 */
#include "ST7066U_8bit.h"      //function declarations, and important definitions
#include<xc.h>      //important for different compiler definitions
static unsigned long int nl;
/*number of loop iteratoins giving the required delay
assuming every delay loop interations executes two instructions*/
static const unsigned long int ipus = (unsigned long int)INST_FREQ / (unsigned long int)1000000;
//instructions per microsecond
static unsigned long int i;     //dummy variable
static unsigned char data;     //data to be sent to the LCD module

/*
 * Function Name: chkBF
 * Discription: chekcs the busy flag
 * takes: void
 * returns: char, grater than zero if busy flag is set
 */
char chkBF(void)
{
    out_port_dir = 0xFF;        //set data port direction to input
    RS = 0;     //instruction
    RdWt = 0b1;       //read
    EN = 0b1;       //enable transaction
    if(out_port & 0x80)     //if DB7 is high
        return 1;      //device is busy, rerurn status
    else        //if DB7 is low
        return -1;      //device is free, return status
}

/*
 * Function Name: clrDisp
 * Discription: clears the display
 * takes: void
 * returns: char, grater than zero if succedded
 */
char clrDisp(void)
{
    EN = 0b0;
    out_port_dir = 0x00;        //set data port direction to input
    RS = 0b0;       //instruction rergister
    RdWt = 0b0;       //write
    //constructing "Display Clear" instruction
    data = 0x01;
    //Display Clear = 0x01
    if(chkBF())     //check busy flag twice before returning with failure
    {
        nl = 80 * ipus / 2;
        for(i = 0; i < nl; i++);
        if(chkBF())
            return -1;
    }
    EN = 0b1;       //enable transaction
    out_port = data;        //load the instruction to the port
    EN = 0b0;
    nl = 1520 * ipus /2;
    for(i = 0; i < nl; i++);        //1.55 milliseconds delay
    return 1;
}

/*
 * Function Name: retHome
 * Discription: returns to address zero, resets display (if shifted) and cursor
 * takes: void
 * returns: char, grater than zero if succedded
 */
char retHome(void)
{
    EN = 0b0;
    out_port_dir = 0x00;        //set data port direction to input
    RS = 0b0;       //instruction rergister
    RdWt = 0b0;       //write
    //constructing "Return Home" instruction
    data = 0x02;
    //Return Home = 0x02
    if(chkBF())     //check busy flag twice before returning with failure
    {
        nl = 80 * ipus / 2;
        for(i = 0; i < nl; i++);
        if(chkBF())
            return -1;
    }
    EN = 0b1;       //enable transaction
    out_port = data;        //load the instruction to the port
    EN = 0b0;
    nl = 1520 * ipus /2;
    for(i = 0; i < nl; i++);        //1.55 milliseconds delay
    return 1;
}

/*
 * Function Name: entryModeSet
 * Discription: controls shift direction of the cursor/blink, and shift of the
 *              entir display
 * takes: char (ids)
 *          ids<0>: direction of shift of cursor/blink and display, high: right
 *          ids<1>: high:shift the entire display when writing to DDRAM
 * returns: char, grater than zero if succedded
 */
char entryModeSet(char ids)
{
    EN = 0b0;
    out_port_dir = 0x00;        //set data port direction to input
    RS = 0b0;       //instruction rergister
    RdWt = 0b0;       //write
    //constructing "Entry Mode Set" instruction
    data = 0x04;
    data |= (ids & (unsigned char)0x03);
    //Entry Mode Set = 0b 0 0 0 0 0 1 I_D S
    if(chkBF())     //check busy flag twice before returning with failure
    {
        nl = 80 * ipus / 2;
        for(i = 0; i < nl; i++);
        if(chkBF())
            return -1;
    }
    EN = 0b1;       //enable transaction
    out_port = data;        //load the instruction to the port
    EN = 0b0;
    nl = 37 * ipus / 2;
    for(i = 0; i < nl; i++);        //37 microseconds delay
    return 1;
}

/*
 * Function Name: dispOnOff
 * Discription: Control display/cursor/blink ON/OFF
 * takes: char (dcb)
 *          dcb<0>: display ON/OFF control, high: diplay on
 *          dcb<1>: cursor ON/OFF control, high: cursor on
 *          dcb<2>: cursor blink ON/OFF control, high: cursor blink on
 * returns: char, grater than zero if succedded
 */
char dispOnOff(char dcb)
{
    EN = 0b0;
    out_port_dir = 0x00;        //set data port direction to input
    RS = 0b0;       //instruction rergister
    RdWt = 0b0;       //write
    //constructing "Display on/Off Control" instruction
    data = 0x08;
    data |= ((dcb) & (unsigned char)0x07);
    //Display On/Off Control = 0b 0 0 0 0 1 D C B
    if(chkBF())     //check busy flag twice before returning with failure
    {
        nl = 80 * ipus / 2;
        for(i = 0; i < nl; i++);
        if(chkBF())
            return -1;
    }
    EN = 0b1;       //enable transaction
    out_port = data;        //load the instruction to the port
    EN = 0b0;
    nl = 37 * ipus / 2;
    for(i = 0; i < nl; i++);        //37 microseconds delay
    return 1;
}

/*
 * Function Name: funcSet
 * Discription: Control interface data length, number of data lines, and font
 * takes: char (dlnf)
 *          dlnf<0>: display font, high/low = (5*8)/(5*11) respectively
 *          dlnf<1>: number of display lines, high/low = 2/1 respectively
 *          dlnf<2>: interface data length, high/low = 8bit/4bit respectively
 * returns: void
 */
void funcSet(char dlnf)
{
    EN = 0b0;
    out_port_dir = 0x00;        //set data port direction to input
    RS = 0b0;       //instruction rergister
    RdWt = 0b0;       //write
    //constructing "Function Set" instruction
    data = 0x20;
    data |= ((dlnf<<2) & (unsigned char)0x1F);
    //Functoin Set = 0b 0 0 1 1 N F 0 0
    EN = 0b1;       //enable transaction
    out_port = data;        //load the instruction to the port
    EN = 0b0;
    nl = 37 * ipus / 2;
    for(i = 0; i < nl; i++);        //37 microseconds delay
}

/*
 * Function Name: setCGRAMAddr
 * Discription: Set CGRAM address to AC
 *              This instruction makes CGRAM data available from MPU
 * takes: char (addr), CGRAM address , addr<5>:addr<0>
 * returns: char, grater than zero if succedded
 */
char setCGRAMAddr(unsigned char addr)
{
    EN = 0b0;
    out_port_dir = 0x00;        //set data port direction to input
    RS = 0b0;       //instruction rergister
    RdWt = 0b0;       //write
    //constructing "Set CGRAM Address" instruction
    data = 0x40;
    data |= (addr & 0x3F);
    //Set CGRAM Address = 0b 0 1 a5 a4 a3 a2 a1 a0
    if(chkBF())     //check busy flag twice before returning with failure
    {
        nl = 80 * ipus / 2;
        for(i = 0; i < nl; i++);
        if(chkBF())
            return -1;
    }
    EN = 0b1;       //enable transaction
    out_port = data;        //load the instruction to the port
    EN = 0b0;
    nl = 37 * ipus /2;
    for(i = 0; i < nl; i++);        //37 microseconds delay
    return 1;
}

/*
 * Function Name: setDDRAMAddr
 * Discription: Set DDRAM address to AC
 *   This instruction makes DDRAM data available from MPU. When 1-line display
 *   mode (N = 0), DDRAM address is from "00H" to "4FH. In 2-line display mode
 *   (N = 1), DDRAM address in the 1st line is from "00H" to "27H", and DDRAM
 *   address in the 2nd line is from "40H" to "67H".
 * takes: char (addr), DDRAM address , addr<5>:addr<0>
 * returns: char, grater than zero if succedded
 */
char setDDRAMAddr(char addr)
{
    EN = 0b0;
    out_port_dir = 0x00;        //set data port direction to input
    RS = 0b0;       //instruction rergister
    RdWt = 0b0;       //write
    //constructing "Set DDRAM Address" instruction
    data = 0x80;
    data |= (addr & 0x7F);
    //Set DDRAM Address = 0b 1 a6 a5 a4 a3 a2 a1 a0
    if(chkBF())     //check busy flag twice before returning with failure
    {
        nl = 80 * ipus / 2;
        for(i = 0; i < nl; i++);
        if(chkBF())
            return -1;
    }
    EN = 0b1;       //enable transaction
    out_port = data;        //load the instruction to the port
    EN = 0b0;
    nl = 37 * ipus /2;
    for(i = 0; i < nl; i++);        //37 microseconds delay
    return 1;
}

/*
 * Function Name: readAC
 * Discription: reads address counter
 * takes: void
 * returns: char, AC value
 */
static unsigned char readAC(void)
{
    unsigned char addr;     //holds address read
    EN = 0b0;
    out_port_dir = 0xFF;        //set data port direction to input
    RS = 0;     //instruction
    RdWt = 0b1;       //read
    EN = 0b1;       //enable transaction
    addr = out_port;
    return (addr & 0x7F);
}

/*
 * Function Name: lcdWrite
 * Discription: write binary 8-bit data to DDRAM/CGRAM
 *  The selection of RAM from DDRAM, CGRAM, is set by the address set function
 *  setCGRAMADDR(), or setDDRAMAddr().RAM set instruction can also determine
 *  the AC direction to RAM. After write operation, the address is automatically
 *  increased/decreased by 1, according to the entry mode.
 * takes: unsigned char (data)
 * returns: char, grater than zero if succedded
 */
char lcdWrite(unsigned char dat)
{
    EN = 0b0;
    out_port_dir = 0x00;        //set data port direction to input
    RS = 0b1;       //data rergister
    RdWt = 0b0;       //write
    if(chkBF())     //check busy flag twice before returning with failure
    {
        nl = 80 * ipus / 2;
        for(i = 0; i < nl; i++);
        if(chkBF())
            return -1;
    }
    EN = 0b1;       //enable transaction
    out_port = dat;        //load the instruction to the port
    EN = 0b0;
    nl = 37 * ipus /2;
    for(i = 0; i < nl; i++);        //37 microseconds delay
    return 1;
}

/*
 * Function Name: lcdRead
 * Discription: Read binary 8-bit data from DDRAM/CGRAM.
 *  The selection of RAM is set by the previous address set instruction.
 *  If address set instruction of RAM is not performed before this instruction,
 *  the data that read first is invalid, because the direction of AC is not
 *  determined. If you read RAM data several times without RAM address set
 *  instruction before read operation, you can get correct RAM data from the
 *  second, but the first data would be incorrect, because there is no time
 *  margin to transfer RAM data. In case of DDRAM read operation, cursor shift
 *  instruction plays the same role as DDRAM address set instruction : it also
 *  transfer RAM data to output data register. After read operation address
 *  counter is automatically increased/decreased by 1 according to the entry
 *  mode. After CGRAM read operation, display shift may not be executed correctly.
 * takes: void
 * returns: char, data read from CGRAM/DDRAM
 */
unsigned char lcdRead(void)
{
    unsigned char dat;      //holds data read
    EN = 0b0;
    out_port_dir = 0xFF;        //set data port direction to input
    RS = 0b1;     //data
    RdWt = 0b1;       //read
    if(chkBF())     //check busy flag twice before returning with failure
    {
        nl = 80 * ipus / 2;
        for(i = 0; i < nl; i++);
        if(chkBF())
            return -1;
    }
    EN = 0b1;       //enable transaction
    dat = out_port;
    EN = 0b0;
    nl = 37 * ipus /2;
    for(i = 0; i < nl; i++);        //37 microseconds delay
    return dat;
}

/*
 * Function Name: init
 * Discription: initializes the LCD module
 * takes: char (conf)
 *  conf<0>(F):  high=5*8 font / low=5*11 font
 *  conf<1>(N):  high=2 lines / low=1 line
 *  conf<2>(B):  high=blink on / low=blink off
 *  conf<3>(C):  high=cursor on / low=cursor off
 *  conf<4>(D):  high=display on / low=display off
 *  conf<5>(S):  high=display/cursor shift on / low=display/cursor shift off
 *  conf<6>(I_D): high=address increment / low=address decrement
 * returns: char, grater than zero if succedded
 */
char init(char conf)
{
    
    nl = (unsigned long int)40000 * ipus / 2;
    for(i = 0; i < nl; i++);        //40 ms delay
    funcSet(conf);      //Function Set
    funcSet(conf);      //Function Set again
    if(!dispOnOff(conf>>2))     //Diplay On/Off Control
        return -1;      //if failed return with failure
    if(!clrDisp())      //Dlear Display
        return -1;
    if(!entryModeSet(conf>>5))
        return -1;
    return 1;
}