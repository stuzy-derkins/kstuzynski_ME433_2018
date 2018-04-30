//#include "NU32.h"       // constants, funcs for startup and UART
//#include <math.h> 	//for sine wave plotting
#include <stdio.h>
#include <xc.h> 
#include <sys/attribs.h> 
#include "spi.h"


#define CS LATAbits.LATA0       // chip select pin

void setVoltage(char channel, int voltage) {
    unsigned short t = 0;
	t= channel << 15; //a is at the very end of the data transfer
	t = t | 0b0111000000000000;
	t = t | ((voltage & 0b1111111111) <<2); //rejecting excessive bits (above 10)
	
	CS = 0;
	SPI1_IO(t>>8);
	SPI1_IO(t&0xFF);
	CS = 1;
}

// send a byte via spi and return the response
unsigned char SPI1_IO(unsigned char write) {
  SPI1BUF = write;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

// initialize spi1 and the MCP module
void initSPI1() {
  // set up the chip select pin as an output
  // the chip select pin is used by the MCP to indicate
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)
  TRISAbits.TRISA0 = 0; //makes ship select pin an output  
  CS = 1;  //makes output low

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
