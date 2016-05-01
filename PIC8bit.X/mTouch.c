/*
 * File:   mTouch.c
 * Author: alexey
 *
 * Created on May 1, 2016, 10:48 PM
 */

#include "core.h"

#define POTNT_CH 13

static uint16_t readADCVal ( uint8_t channel )
{
//CHS<4:0>: Analog Channel Select bits
//11111 = FVR (Fixed Voltage Reference) Buffer 1 Output
//11110 = DAC output
//11101 = Temperature Indicator
//11100 = Reserved. No channel connected.
//•
//•
//•
//01110 = Reserved. No channel connected.
//01101 = AN13
//01100 = AN12
//01011 = AN11
//01010 = AN10
//01001 = AN9
//01000 = AN8
//00111 = AN7
//00110 = AN6
//00101 = AN5
//00100 = AN4
//00011 = AN3
//00010 = AN2
//00001 = AN1
//00000 = AN0
    ADCON0bits.CHS = POTNT_CH;
    
    delay_us( 5 );
//GO/DONE: A/D Conversion Status bit
//1 = A/D conversion cycle in progress. Setting this bit starts an A/D conversion cycle.
//This bit is automatically cleared by hardware when the A/D conversion has completed.
//0 = A/D conversion completed/not in progress
    ADCON0bits.GO = 1;
    while ( ADCON0bits.nDONE ) { }
    
    return( ((int)ADRESH << 8) | ADRESL );
}

uint16_t    iOffsetUp_b = 0,
            iOffsetDown_b = 0,
            iOffsetMid_b = 0,
            iOffsetLeft_b = 0,
            iOffsetRight_b = 0;

static uint16_t mTouch_scanButton ( CapButtons_t button  )
{
    // Установка отедьного пина на высокий потенциал
    TRISE1 = ANSE1 = 0;
    LATE1 = 1;

    ADCON0bits.CHS = 0b00110;

    int capDelay = 5;
    
    switch ( button )
    {
        case MID_B:
            TRISB2 = ANSB2 = LATB2 = 0;
            delay_ms( capDelay );  // Задержка для заряда емкости модуля АЦП
            TRISB2 = ANSB2 = 1;
            break;
            
        case UP_B:
            TRISB3 = ANSB3 = LATB3 = 0;
            delay_ms( capDelay );  // Задержка для заряда емкости модуля АЦП
            TRISB3 = ANSB3 = 1;
            break;
            
        case LEFT_B:
            TRISB1 = ANSB1 = LATB1 = 0;
            delay_ms( capDelay );  // Задержка для заряда емкости модуля АЦП
            TRISB1 = ANSB1 = 1;
            break;
            
        case RIGHT_B:
            TRISB4 = ANSB4 = LATB4 = 0;
            delay_ms( capDelay );  // Задержка для заряда емкости модуля АЦП
            TRISB4 = ANSB4 = 1;
            break;
            
        case DOWN_B:
            TRISB0 = ANSB0 = LATB0 = 0;
            delay_ms( capDelay );  // Задержка для заряда емкости модуля АЦП
            TRISB0 = ANSB0 = 1;
            break;
    }
    
    return( readADCVal( button ) );
}

static uint16_t mTouch_getIOffset ( CapButtons_t button )
{
    switch ( button )
    {
        case MID_B:
            return( iOffsetMid_b );
            
        case UP_B:
            return( iOffsetUp_b );
            
        case LEFT_B:
            return( iOffsetLeft_b );
            
        case RIGHT_B:
            return( iOffsetRight_b );
            
        case DOWN_B:
            return( iOffsetDown_b );
    }
    return( 0 );
}

#define MTOUCH_SAMPLES 10   // Число должно быть не более 65535/1024 = 63
#define OFF_PERCENT 0.9f    // Процент возможного разброса значения с АЦП
static uint8_t ledsSwitchOn = 0;

void mTouch_init ( uint8_t ledMode )
{
//ADFM: A/D Result Format Select bit
//1 = Right justified. Six Most Significant bits of ADRESH are set to ‘0’ when the conversion result is
//loaded.
//0 = Left justified. Six Least Significant bits of ADRESL are set to ‘0’ when the conversion result is
//loaded.
    ADCON1bits.ADFM = 1;
//ADCS<2:0>: A/D Conversion Clock Select bits
//111 = F RC (clock supplied from a dedicated RC oscillator)
//110 = F OSC /64
//101 = F OSC /16
//100 = F OSC /4
//011 = F RC (clock supplied from a dedicated RC oscillator)
//010 = F OSC /32
//001 = F OSC /8
//000 = F OSC /2
    ADCON1bits.ADCS = 0b101;
//ADON: ADC Enable bit
//1 = ADC is enabled
//0 = ADC is disabled and consumes no operating current
    ADCON0bits.ADON = 1;
    
    for ( int i = 0; i < MTOUCH_SAMPLES; i++ )
    {
        iOffsetUp_b += mTouch_scanButton( UP_B );
        iOffsetDown_b += mTouch_scanButton( DOWN_B );
        iOffsetMid_b += mTouch_scanButton( MID_B );
        iOffsetLeft_b += mTouch_scanButton( LEFT_B );
        iOffsetRight_b += mTouch_scanButton( RIGHT_B );
    }
    
    iOffsetUp_b = (uint16_t)(iOffsetUp_b/MTOUCH_SAMPLES * OFF_PERCENT);
    iOffsetDown_b = (uint16_t)(iOffsetDown_b/MTOUCH_SAMPLES * OFF_PERCENT);
    iOffsetMid_b = (uint16_t)(iOffsetMid_b/MTOUCH_SAMPLES * OFF_PERCENT);
    iOffsetLeft_b = (uint16_t)(iOffsetLeft_b/MTOUCH_SAMPLES * OFF_PERCENT);
    iOffsetRight_b = (uint16_t)(iOffsetRight_b/MTOUCH_SAMPLES * OFF_PERCENT);
    
    if ( ledMode )
    {
        TRISD &= 0b00001111;
        ledsSwitchOn = ledMode;
    }
}

int mTouch_isButtonPressed ( CapButtons_t button )
{
    if ( mTouch_scanButton( button ) < mTouch_getIOffset( button ) )
    {
        if ( ledsSwitchOn )
        {
            LATD4 = button == UP_B    || button == MID_B ? 1 : 0;
            LATD5 = button == LEFT_B  || button == MID_B ? 1 : 0;
            LATD6 = button == RIGHT_B || button == MID_B ? 1 : 0;
            LATD7 = button == DOWN_B  || button == MID_B ? 1 : 0;
        }
        return( 1 );
    }
    return( 0 );
}