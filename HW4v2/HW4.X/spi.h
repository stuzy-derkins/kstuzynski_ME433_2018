#ifndef _SPI_H   
#define _SPI_H

//#define CS = LATAbits.LATA1 

void setVoltage(char, int);
    
unsigned char SPI1_IO(unsigned char);
    
void initSPI1(void);

#endif
