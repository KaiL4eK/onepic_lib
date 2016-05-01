/*
 * File:   pmain.c
 * Author: Alex Devyatkin
 *
 */

#include "LCD.h"
#include "core.h"
#include "pragmas.h"

void main( void ) 
{  
    pll_32MHz_init();    
    lcd_init();
//    mTouch_init();
    potnt_init();
//    i2c_init( 400000L );
    
    TRISA &= 0b11000000;
    
#define BUF_SIZE 16
    
    while ( 1 )
    {
        uint16_t potencVal = potnt_get_value(),
                 ledStep = 1023/6;
        
        LATA &= 0b11000000;
        
        for ( int i = 0; i <= 5; i++ )
            if ( potencVal > ledStep*i )
                LATA |= 1 << i;
        
        delay_ms( 50 );
    }
    return;
}
