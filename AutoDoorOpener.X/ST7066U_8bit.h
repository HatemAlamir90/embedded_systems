/*
 * File:   ST7066U_8bit.h
 * Author: Hatem Alamir
 *
 * Created on May 25, 2015, 18:35
 * Discription: functions that manipulate the LCD module with ST7066U driver in
 *              8-bit mode.
 */
#ifndef ST7066U_H
#define	ST7066U_H
//Controls and data ports definitions
#define EN RE2      //enable/disable the module, high=enable / low=disable
#define RS RE0      //register select, high=data / low=instruction
#define RdWt RE1      //read/wright operation, high=read / low=wright
#define out_port PORTD     //the i/o port used by the LCD
#define out_port_dir TRISD     //the data direction of the port used by the LCD
#define INST_FREQ 1000000       //instructions per second, used for delays

/*
 * Function Name: chkBF
 * Discription: chekcs the busy flag
 * takes: void
 * returns: char, grater than zero if busy flag is set
 */
char chkBF(void);

/*
 * Function Name: clrDisp
 * Discription: clears the display
 * takes: void
 * returns: char, grater than zero if succedded
 */
char clrDisp(void);

/*
 * Function Name: retHome
 * Discription: returns to address zero, resets display (if shifted) and cursor
 * takes: void
 * returns: char, grater than zero if succedded
 */
char retHome(void);

/*
 * Function Name: entryModeSet
 * Discription: controls shift direction of the cursor/blink, and shift of the
 *              entir display
 * takes: char (ids)
 *          ids<0>: direction of shift of cursor/blink and display, high: right
 *          ids<1>: high:shift the entire display when writing to DDRAM
 * returns: char, grater than zero if succedded
 */
char entryModeSet(char ids);

/*
 * Function Name: dispOnOff
 * Discription: Control display/cursor/blink ON/OFF
 * takes: char (dcb)
 *          dcb<0>: display ON/OFF control, high: diplay on
 *          dcb<1>: cursor ON/OFF control, high: cursor on
 *          dcb<2>: cursor blink ON/OFF control, high: cursor blink on
 * returns: char, grater than zero if succedded
 */
char dispOnOff(char dcb);

/*
 * Function Name: dispOnOff
 * Discription: Control display/cursor/blink ON/OFF
 * takes: char (dcb)
 *          dcb<0>: display ON/OFF control, high: diplay on
 *          dcb<1>: cursor ON/OFF control, high: cursor on
 *          dcb<2>: cursor blink ON/OFF control, high: cursor blink on
 * returns: char, grater than zero if succedded
 */
char dispOnOff(char dcb);

/*
 * Function Name: funcSet
 * Discription: Control interface data length, number of data lines, and font
 * takes: char (dlnf)
 *          dlnf<0>: display font, high/low = (5*8)/(5*11) respectively
 *          dlnf<1>: number of display lines, high/low = 2/1 respectively
 *          dlnf<2>: interface data length, high/low = 8bit/4bit respectively
 * returns: void
 */
void funcSet(char dlnf);

/*
 * Function Name: setCGRAMAddr
 * Discription: Set CGRAM address to AC
 *              This instruction makes CGRAM data available from MPU
 * takes: char (addr), CGRAM address , addr<5>:addr<0>
 * returns: char, grater than zero if succedded
 */
char setCGRAMAddr(unsigned char addr);

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
char setDDRAMAddr(char addr);

/*
 * Function Name: readAC
 * Discription: reads address counter
 * takes: void
 * returns: char, AC value
 */
static unsigned char readAC(void);

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
char lcdWrite(unsigned char dat);

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
unsigned char lcdRead(void);

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
char init(char conf);
#endif	/* ST7066U_H */