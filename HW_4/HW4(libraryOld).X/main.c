//#include "NU32.h"       // constants, funcs for startup and UART
#include <math.h> 	//for sine wave plotting
#include <stdio.h>
#include <xc.h> 
#include <sys/attribs.h> 
// Demonstrates spi by accessing MCP4912 chip
// PIC is the master, MCP is the slave
// Uses microchip MCP4912 chip (see the data sheet for protocol details)
// SDO1 -> SI (pin 2 -> pin 55)
// SDI1 -> SO (pin  -> pin ) NO SDO on this chip!
// SCK1 -> SCK (pin 25 -> pin 4)
// SS1 -> CS (pin 3 -> pin 3)
//
#define CS LATAbits.LATA1       // chip select pin

void setVoltage(char channel, int voltage) {
    unsigned short t = 0;
	t= a << 15; //a is at the very end of the data transfer
	t = t | 0b01110000000000000;
	t = t | ((v&0b1111111111) <<2); //rejecting excessive bits (above 10)
	
	CS = 0;
	SPI1_IO(t>>8);
	SPI1_IO(t&0xFF);
}

void initSPI1() {
// set up the chip select pin as an output
  // the chip select pin is used by the MCP to indicate
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)
  TRISAbits.TRISA1 = 0;
  CS = 1;

  // Master - SPI4, pins are: SDI4(F4), SDO4(F5), SCK4(F13).  
  // we manually control SS4 as a digital output (F12)
  // since the pic is just starting, we know that spi is off. We rely on defaults here
 
  // setup spi1 , all bits must be changed for SPI1 from SPI4
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0x3;            // baud rate to 10 MHz [SPI4BRG = (80000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1

                            // send a MCP set status command.
  CS = 0;                   // enable the MCP
  SPI1_IO(0x01);             // MCP write status
  SPI1_IO(0x41);             // sequential mode (mode = 0b01), hold disabled (hold = 0)
  CS = 1;                   // finish the command
}

char SPI1_IO(char write){
  SPI1BUF = 0;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF; 
}


