/*
 * File:   pmain.c
 * Author: Alex Devyatkin
 *
 */

#include "core.h"
#include "pragmas.h"
#include "gy85.h"
//
int16_t  gx,gy,gz;
int32_t gx_rate, gy_rate, gz_rate;
//int ix, iy, iz;
float anglegx=0.0, anglegy=0.0, anglegz=0.0;
int16_t ax,ay,az;  
int16_t rawX, rawY, rawZ;
int32_t X, Y, Z;
float rolldeg, pitchdeg;
float aoffsetX, aoffsetY, aoffsetZ;
uint32_t time_ms = 0;
uint8_t time_ms_step = 100;
ADXL345 acc;
    
void enable_int_timer( void )
{
    TMR2 = time_ms = 0;
    T2CONbits.TON = 1;
}

void disable_int_timer( void )
{
    T2CONbits.TON = 0;
}

void system_timer_init( void )
{
    disable_int_timer();
    T2CONbits.TCKPS = 0b111;
    IEC0bits.T2IE = 1;
    IFS0CLR = _IFS0_T2IF_MASK;
    PR2 = SYS_CLK/1000/256*time_ms_step;
}

int main ( void )
{
//    lcd_init();
//    potnt_init();
    UART_init( UART_115200 );
    i2c_init( 400000 );
//    INTCONbits.MVEC = 1;
//    __asm__("EI");
    acc.powerOn();
    for (int i = 0; i <= 200; i++) 
    {
        acc.readAccel(&ax, &ay, &az);
        if (i == 0) {
            aoffsetX = ax;
            aoffsetY = ay;
            aoffsetZ = az;
        }
        if (i > 1) {
            aoffsetX = (ax + aoffsetX) / 2;
            aoffsetY = (ay + aoffsetY) / 2;
            aoffsetZ = (az + aoffsetZ) / 2;
        }
    }
    system_timer_init();
//    UART_write_string( "Hello, it`s me, Mario!\n" );
    
//    _TRISD7 = 0;
//    uint16_t ADC_Val = 0;
//    char    buf[16];
    while ( 1 ) // repeat continuously
    {
        switch( UART_get_last_received_command() )
        {
            case 'a':
//                UART_write_string(">>>\n");
                enable_int_timer();
                break;
            case 's':
//                UART_write_string("<<<\n");
                disable_int_timer();
                break;
        }
        
        if ( IFS0bits.T2IF )
        {
            acc.readAccel(&ax, &ay, &az); //read the accelerometer values and store them in variables  x,y,z
            rawX = ax - aoffsetX;
            rawY = ay - aoffsetY;
            rawZ = az  - (255 - aoffsetZ);
            // increase type length for convertion
            X = rawX;
            Y = rawY;
            Z = rawZ;
            rolldeg = 180*(atan(Y/sqrtf(X*X+Z*Z)))/M_PI; // calculated angle in degrees
            pitchdeg = 180*(atan(X/sqrtf(Y*Y+Z*Z)))/M_PI; // calculated angle in degrees
            int16_t intRoll = rolldeg*1000;
            int16_t intPitch = pitchdeg*1000;

            UART_write_string( "%d,%d,%ld\n\r", intRoll, intPitch, time_ms );
            time_ms += time_ms_step;
            IFS0CLR = _IFS0_T2IF_MASK;
        }
//        ADC_Val = potnt_get_value();
//        sprintf( buf, "%d", ADC_Val );
//        lcd_clear();
//        lcd_write_string( buf );
        
//        _LATD7 = 1;
//        delay_ms( 1000 );
//        _LATD7 = 0;
//        delay_ms( 1000 );
    }
    
    return( 0 );
}

