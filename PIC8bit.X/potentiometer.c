/*
 * File:   potentiometer.c
 * Author: alexey
 *
 * Created on May 1, 2016, 10:48 PM
 */

#include "core.h"

#define POTNT_CH 13

void potnt_init ( void )
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
}

int16_t potnt_get_value ( void )
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
    
//    delay_us( 5 );
//GO/DONE: A/D Conversion Status bit
//1 = A/D conversion cycle in progress. Setting this bit starts an A/D conversion cycle.
//This bit is automatically cleared by hardware when the A/D conversion has completed.
//0 = A/D conversion completed/not in progress
    ADCON0bits.GO = 1;
    while ( ADCON0bits.nDONE ) { }
    
    return( ((int)ADRESH << 8) | ADRESL );
}
