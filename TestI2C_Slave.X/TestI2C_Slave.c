//TestI2C_Slave.c
//Exchanges portB data between PIC1F877A and PIC18F458 using I2C, slave side
#include <xc.h>     //important definitions
#pragma config FOSC=HS,WDTE=OFF     //configure the device: high speed osc., no WDT
const unsigned char Add_High = 0xF0 , Add_Low = 0x80;     //high and low byte of device address, respectively
void interrupt isr()     //interrupt service routine
{
    volatile unsigned char rc;     //received byte
    if(!(SSPSTAT & 0x20))     //if an address byte received
    {
        rc = SSPBUF;     //read the address byte received
        if(UA)     //if update udress nedded
        {
            if(rc = Add_High)     //if address high byte last received
                SSPBUF = Add_Low;     //update with the low address byte
            if(rc = Add_Low)     //vice versa
                SSPBUF = Add_High;
        }
        if(SSPSTAT & 0x04)     //if the master wants to read
        {
            SSPBUF = PORTB;     //prepare portB vlaue to be sent
            CKP = 0b1;     //release the clock
        }
    }
    if(SSPSTAT & 0x20)     //if a data byte received
    {
        PORTD = SSPBUF;     //dispaly it on portD
    }
}
void main()
{
    GIE = 0b1;     //enable interrupts globally
    PEIE = 0b1;     //enable peripheral interrupts
    SSPIF = 0b0;     //clear MSSP interrupt flag
    SSPIE = 0b1;     //enable MSSP interrupt
    nRBPU = 0b0;     //enable portB pullup resistors
    TRISB = 0xFF;     //make portB pins as input for pullups to work
    TRISD = 0x00;     //make portE pins as putput to control display LEDs
    PSPMODE = 0b0;     //don't use portD in PSP mode
    PORTD = 0xFF;     //extinguish the LEDs initially
    SMP = 0b1;     //I2C standard mode
    CKE = 0b0;     //disable SMBus specific inputs
    SSPCON = 0x37;     //configure MSSP: enable in slave mode without start and stop bit interrupts
    SSPCON2 = 0x60;     //configure MSSP: disable general call, strch clock in transmit only
    TRISC = 0xFF;     //configure SDA/RC4 and SCL/RC3 as inputs for I2C slave to work
    while(1);     //let interrupts do the job
}
