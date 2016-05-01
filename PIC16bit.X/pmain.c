/*
 * File:   pmain.c
 * Author: Alex Devyatkin
 *
 * Created on March 26, 2016, 11:43 PM
 */

#include "core.h"
#include "pragmas.h"
#include <stdio.h>

int main(void) {
    AD1PCFGL = 0x1FFF;
    lcd_init();
    potnt_init();
    _TRISF6 = 0;
    _LATF6 = 1;
    
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
        int16_t pVal = potnt_get_value();
        char    buf[8];
        
        sprintf( buf, "%d", pVal );
        lcd_clear();
        lcd_write_string( buf );
        delay_ms( 500 );
    }
    
    return 0;
}
