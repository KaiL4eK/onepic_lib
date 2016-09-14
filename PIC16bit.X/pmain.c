/*
 * File:   pmain.c
 * Author: Alex Devyatkin
 *
 */

#include "core.h"
#include <stdio.h>
#include "ad7705.h"

#include "pragmas.h"

#define SWITCH_OFF_ANALOGS AD1PCFGL = 0x1FFF;
//RCDIV<2:0>: FRC Postscaler Select bits
//111 = 31.25 kHz (divide by 256)
//110 = 125 kHz (divide by 64)
//101 = 250 kHz (divide by 32)
//100 = 500 kHz (divide by 16)
//011 = 1 MHz (divide by 8)
//010 = 2 MHz (divide by 4)
//001 = 4 MHz (divide by 2)
//000 = 8 MHz (divide by 1)
#define SET_POSTSCALER_ONE CLKDIVbits.RCDIV = 0b000;

/*
 * Connection:
 *      <AD7705>        <Direct connect>    <PIC24>
 *      SCLK        -   SCK(12)         -   RB5
 *      MCLK IN     -   QUARTZ
 *      MCLK OUT    -   QUARTZ
 *      CS          -   GND(Always chosen)
 *      nRESET      -   PTP17(17)       -   RE4
 *      AIN1(+)     -   YELLOW_TENZO
 *      AIN1(-)     -   ORANGE_TENZO
 *      GND         -   GND
 *      VDD         -   VDD_IN(3.3V)
 *      DIN         -   SDO(8)          -   RD2
 *      DOUT        -   SDI(10)         -   RD1
 *      nDRDY       -   PTP12(18)       -   RD6
 *      REF IN(-)   -   GND
 *      REF IN(+)   -   POT_OUT
 * 
 *      <Potentiometer> <Connect>
 *      POT_SUP(+)  -   VDD(3.3V)
 *      POT_SUP(-)  -   GND
 * 
 *      <Tenzo>         <Connect>
 *      RED_TENZO   -   USB(+)(red)(+5V)
 *      BROWN_TENZO -   GND
 * 
 *      <USB>           <Connect>           <PIC24>
 *      Green       -   PTP6            -   RB15
 *      White       -   PTP5            -   RB14
 *      Black       -   GND
 */

int main ( void ) 
{
    SWITCH_OFF_ANALOGS
    SET_POSTSCALER_ONE
    
//    lcd_init();
//    potnt_init();
//    pwm_init( 50 );
//    i2c_init( 400000 );
//    rtc_init();щ
//    _TRISF6 = 0;
//    _LATF6 = 1;
    spi_init();
    init_UART1( UART_460800 );
    UART_write_string("UART initialized\r\n");

    int res = 0;
    if ( ( res = ad7705_init() ) < 0 )
    {
        UART_write_string( "AD7705 initialization failed, %d\n", res );
        while ( 1 );
    }
    spi_set_speed( SPI_PRIM_1, SPI_SEC_2 );
    UART_write_string( "AD7705 initialized and calibrate\n" );
            
    while ( 1 )
    {
//        delay_ms( 1000 );
//        _LATF6 = 1;
//        lcd_clear();
//        lcd_write_string( "Hello!" );
//        delay_ms( 1000 );
//        _LATF6 = 0;
//        lcd_clear();
//        lcd_write_string( "Bye!" );
        
        
//        time_t *time_data = rtc_get_raw_data();
//        uint16_t pVal = potnt_get_value();
//        char    buf[16];
//        uint16_t pVal_prc = 100L*pVal/1023;
//        sprintf( buf, "%02d", pVal_prc );
//        lcd_clear();
//        lcd_write_string( buf );
//        pwm_set_dutyCycle_percent( pVal_prc );
        if ( !nDRDY_PIN )
        {
            uint32_t data_res = ad7705_read_register( DATA );
            UART_write_string( "Data: %ld\n", data_res );
        }
        
        delay_ms( 100 );
    }
    return( 0 );
}
