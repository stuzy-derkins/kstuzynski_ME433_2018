#include<xc.h>
#include "i2c_master_noint_HW7.h"

// I2C Master utilities, 400 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

void i2c_master_setup(void) {
    I2C2BRG = 0x037;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2
                                    // PGD for this PIC32 = (104ns) at 400kHz
    I2C2CONbits.ON = 1;               // turn on the I2C2 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {
    I2C2CONbits.RSEN = 1;           // send a restart
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
    while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
    if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
    }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
    I2C2CONbits.PEN = 1;              // comm is complete and master relinquishes bus
    while(I2C2CONbits.PEN) { ; }      // wait for STOP to complete
}

void initIMU(void) {
    i2c_master_setup();
    ANSELBbits.ANSB2 = 0;             // I2C2 pins are analog input by default. Turn off analog 
    ANSELBbits.ANSB3 = 0;             // I2C2 pins are analog input by default. Turn off analog
    i2c_write(0x10,0b10000010);     // CTRL1_XL:Set sample rate to 1.66 kHz, 2g sensitivity, and 100 Hz filter.
    i2c_write(0x11,0b10001000);     // CTRL2_G: Set sample rate to 1.66 kHz, with 1000 dps sensitivity.
    i2c_write(0x12,0b00000100);     // CTRL3_C: Make IF_INC bit 1 so automatically reads the next register 
}

void i2c_write(char reg, char val) {
    i2c_master_start();               // make the start bit
    i2c_master_send(addr<<1|0);       // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(reg);             // the register to write to
    i2c_master_send(val);             // the value to put in the register
    i2c_master_stop();                // make the stop bit
}

char i2c_read(char addr_send) {
    i2c_master_start();               // make the start bit
    i2c_master_send(addr<<1|0);       // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(addr_send);            // the register to read from (who am I))
    i2c_master_restart();             // make the restart bit
    i2c_master_send(addr<<1|1);       // write the address, shifted left by 1, or'ed with a 1 to indicate reading
    unsigned char recv = i2c_master_recv(); // save the value returned
    i2c_master_ack(1);                // make the ack so the slave knows we got it
    i2c_master_stop();                // make the stop bit
    return recv;
}

void i2c_read_multiple(unsigned char reg, unsigned char* data, int length) {
    i2c_master_start();               // make the start bit
    i2c_master_send(addr<<1|0);       // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(reg);            // the register to read from (who am I))
    i2c_master_restart();             // make the restart bit
    i2c_master_send(addr<<1|1);       // write the address, shifted left by 1, or'ed with a 1 to indicate reading
    int i;
    for (i=0; i<length; i++){
        data[i] = i2c_master_recv(); // save the value returned
        if(i<(length-1)){
            i2c_master_ack(0);                
        } else {
            i2c_master_ack(1); 
        }
    }
    i2c_master_stop();                // make the stop bit
}