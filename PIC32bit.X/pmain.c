/*
 * File:   pmain.c
 * Author: Alex Devyatkin
 *
 */

#include "core.h"
#include "pragmas.h"

int main ( void )
{
    lcd_init();
    potnt_init();
    UART_init( UART_115200 );
    _TRISD7 = 0;
    
    UART_write_string( "Hello, it`s me, Mario!\n" );
    
    uint16_t ADC_Val = 0;
    char    buf[8];
    while ( 1 ) // repeat continuously
    {
        ADC_Val = potnt_get_value();
        sprintf( buf, "%d", ADC_Val );
        lcd_clear();
        lcd_write_string( buf );
        
//        _LATD7 = 1;
//        delay_ms( 1000 );
//        _LATD7 = 0;
//        delay_ms( 1000 );

        delay_ms( 1000 );
    }
    
    return( 0 );
}
