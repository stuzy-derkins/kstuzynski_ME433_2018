#include "ST7735_HW7.h"      //to use LCD functions
#include <stdio.h>       //to use sprintf command
#include "i2c_master_noint_HW7.h"

#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

// DEVCFG0
#pragma config DEBUG = 00 // no debugging          QUESTIoN it says '0x' in datasheet not 00?????
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 11 // use PGED1 and PGEC1
#pragma config PWP = 111111111 // no write protect
#pragma config BWP = 0 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = 011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // disable secondary osc                               QUESTIoN ?????
#pragma config FPBDIV = 00 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = 11 // do not enable clock switch            QUESTIoN it says '1x' in datasheet not 11?????
#pragma config WDTPS = 10100 // use slowest wdt                         slowest? 10100 = 1:1048576???????????????
#pragma config WINDIS = 1 // wdt no window mode
#pragma config FWDTEN = 0 // wdt disabled
#pragma config FWDTWINSZ = 11 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = 001 // divide input clock to be in range 4-5MHz         x2 divider QUESTIoN ?????
#pragma config FPLLMUL = 111 // multiply clock after FPLLIDIV                        x24 multiplier QUESTIoN ?????
#pragma config FPLLODIV = 001 // divide clock after FPLLMUL to get 48MHz             x2 divider QUESTIoN ?????
#pragma config UPLLIDIV = 001 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB     ?????
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what    was already 0 ????
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module


int main(void) {

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
    LATAbits.LATA4 = 0;     //makes A4 output low
    
    __builtin_enable_interrupts();

    _CP0_SET_COUNT(0);
    
    LCD_init();
    LCD_clearScreen(WHITE);
    initIMU();
    
    
        
        
    while (1){
        _CP0_SET_COUNT(0);
        char message[30];
        unsigned char reg = 0x20; //starts at OUT_TEMP_L
        unsigned char data[10];
        int length = 14;
        
        i2c_read_multiple(reg, data, length); //length = 12 
        signed short acc_x = data[0] | (data[1]<<8);
        signed short acc_y;
        signed short acc_z;
        signed short ang_vel_x;
        signed short ang_vel_y;
        signed short ang_vel_z;
        
        sprintf(message, "Acceleration X: %d",acc_x);
        LCD_drawString(10, 10, message, BLACK, WHITE);  
        
        
        
        //LCD_drawProgressBar(10, 25, 5, j, YELLOW, GREEN);
        while(_CP0_GET_COUNT() < 1200000) {
            
        } 
        LATAbits.LATA4 = !LATAbits.LATA4;
    }
    

  return 0;
}