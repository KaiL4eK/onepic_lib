/*
 * File:   pmain.c
 * Author: Alex Devyatkin
 *
 */

#include "core.h"
#include "pragmas.h"
#include <stdio.h>

int main ( void ) 
{
    SWITCH_OFF_ANALOGS
    lcd_init();
    potnt_init();
    i2c_init( 400000 );
    rtc_init();
//    _TRISF6 = 0;
//    _LATF6 = 1;
    
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
        time_str_t *time_data = rtc_get_raw_data();
//        int16_t pVal = potnt_get_value();
        char    buf[16];
        
        sprintf( buf, "%02d", time_data->tm_sec );
        lcd_clear();
        lcd_write_string( buf );
        delay_ms( 100 );
    }
    return( 0 );
}
