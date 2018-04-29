#include "i2c_master_noint.h"
#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

// Demonstrate I2C by having the I2C2(PIC32MX250F128B) talk to IO Expander MCP23008 
// Master will use SDA2 (B2) and SCL2 (B3).  Connect these through 10k resistors to
// Vcc (3.3 V)
// Expander will use SDA (pin2) and SCL (pin1)
// SDA2 -> SDA
// SCL2 -> SCL
// Two bytes will be written to the slave and then read back to the slave.

// DEVCFG0
#pragma config DEBUG = 00 // no debugging          QUESTIoN it says '0x' in datasheet not 00?????
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 0b11 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = 0 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = 0b011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 0b10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // disable secondary osc                               QUESTIoN ?????
#pragma config FPBDIV = 00 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = 0b11 // do not enable clock switch            QUESTIoN it says '1x' in datasheet not 11?????
#pragma config WDTPS = 0b10100 // use slowest wdt                         slowest? 10100 = 1:1048576???????????????
#pragma config WINDIS = 1 // wdt no window mode
#pragma config FWDTEN = 0 // wdt disabled
#pragma config FWDTWINSZ = 0b11 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = 001 // divide input clock to be in range 4-5MHz         x2 divider QUESTIoN ?????
#pragma config FPLLMUL = 0b111 // multiply clock after FPLLIDIV                        x24 multiplier QUESTIoN ?????
#pragma config FPLLODIV = 001 // divide clock after FPLLMUL to get 48MHz             x2 divider QUESTIoN ?????
#pragma config UPLLIDIV = 001 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB     ?????
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what    was already 0 ????
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module


//#define ADDR 0b10000000; implemented in the h file

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0;   //makes pin A4 with LED an output
    TRISBbits.TRISB4 = 1;   //makes pin B4 with button an input
    
    LATA = 0;               //clears all outputs on A to low
    LATAbits.LATA4 = 1;     //makes A4 output low
    
    initExpander(); // sets up expander with correct speed and initializes i2c_master_setup
    
    __builtin_enable_interrupts();
 
    while(1) {
        if(getExpander()==0){
            setExpander(0x0A, 0b0000001);
        } else {
            setExpander(0x0A, 0b0000000);
        }
        
        while(_CP0_GET_COUNT() < 2400000) {
            LATAbits.LATA4 = !LATAbits.LATA4;
        }  
    }
}
