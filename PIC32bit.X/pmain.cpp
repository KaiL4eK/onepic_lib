/*
 * File:   pmain.c
 * Author: Alex Devyatkin
 *
 */

#include "core.h"
#include "pragmas.h"
#include "gy85.h"

float  gx,gy,gz;
float  gx_rate, gy_rate, gz_rate;
int ix, iy, iz;
float anglegx=0.0, anglegy=0.0, anglegz=0.0;
int ax,ay,az;  
int rawX, rawY, rawZ;
float X, Y, Z;
float rollrad, pitchrad;
float rolldeg, pitchdeg;
int error = 0; 
float aoffsetX, aoffsetY, aoffsetZ;
float goffsetX, goffsetY, goffsetZ;
unsigned long time, looptime;

int main ( void )
{
    lcd_init();
    potnt_init();
    UART_init( UART_115200 );
//    i2c_init( 400000 );
//    
//    ADXL345 acc;
//    acc.powerOn();
//    
//    for (int i = 0; i <= 200; i++) 
//    {
//        acc.readAccel(&ax, &ay, &az);
//        if (i == 0) {
//            aoffsetX = ax;
//            aoffsetY = ay;
//            aoffsetZ = az;
//        }
//        if (i > 1) {
//            aoffsetX = (ax + aoffsetX) / 2;
//            aoffsetY = (ay + aoffsetY) / 2;
//            aoffsetZ = (az + aoffsetZ) / 2;
//        }
//    }
//    for (int i = 0; i <= 200; i++) {
//        gyro.readGyro(&gx,&gy,&gz); 
//        if (i == 0) {
//            goffsetX = gx;
//            goffsetY = gy;
//            goffsetZ = gz;
//        }
//        if (i > 1) {
//            goffsetX = (gx + goffsetX) / 2;
//            goffsetY = (gy + goffsetY) / 2;
//            goffsetZ = (gz + goffsetZ) / 2;
//        }
//    }

    UART_write_string( "Hello, it`s me, Mario!\n" );
//    _TRISD7 = 0;
    uint16_t ADC_Val = 0;
    char    buf[8];
    while ( 1 ) // repeat continuously
    {
//        acc.readAccel(&ax, &ay, &az); //read the accelerometer values and store them in variables  x,y,z
//        rawX = ax - aoffsetX;
//        rawY = ay - aoffsetY;
//        rawZ = az  - (255 - aoffsetZ);
//        X = rawX/256.00; // used for angle calculations
//        Y = rawY/256.00; // used for angle calculations
//        Z = rawZ/256.00; // used for angle calculations
//        rollrad = atan(Y/sqrt(X*X+Z*Z));  // calculated angle in radians
//        pitchrad = atan(X/sqrt(Y*Y+Z*Z)); // calculated angle in radians
//        rolldeg = 180*(atan(Y/sqrt(X*X+Z*Z)))/M_PI; // calculated angle in degrees
//        pitchdeg = 180*(atan(X/sqrt(Y*Y+Z*Z)))/M_PI; // calculated angle in degrees

        UART_write_string( "%d, %d, %d\n", rawX, rawY, rawZ );
//        UART_write_string( "%.2f, %.2f\n", rolldeg, pitchdeg );
        
        ADC_Val = potnt_get_value();
        sprintf( buf, "%d", ADC_Val );
        lcd_clear();
        lcd_write_string( buf );
        
//        _LATD7 = 1;
//        delay_ms( 1000 );
//        _LATD7 = 0;
//        delay_ms( 1000 );

        delay_ms( 500 );
    }
    
    return( 0 );
}
