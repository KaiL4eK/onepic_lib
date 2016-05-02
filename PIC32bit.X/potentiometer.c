/*
 * File:   potentiometer.c
 * Author: alexey
 *
 * Created on May 1, 2016, 10:48 PM
 */

#include "core.h"

#define POTNT_CH 11

void potnt_init ( void )
{
//ADON: A/D Operating Mode bit (1)
//1 = A/D Converter module is operating
//0 = A/D Converter is off
    AD1CON1bits.ADON = 0;
//ASAM: A/D Sample Auto-Start bit
//1 = Sampling begins immediately after last conversion completes; SAMP bit is auto-set
//0 = Sampling begins when the SAMP bit is set
    AD1CON1bits.ASAM = 1;
//PCFG<15:0>: Analog Input Pin Configuration Control bits
//1 = Analog input pin in Digital mode, port read input enabled, ADC input multiplexer input for this analog
//input connected to AVss
//0 = Analog input pin in Analog mode, digital port read will return as a ‘1’ without regard to the voltage on
//the pin, ADC samples pin voltage
    AD1PCFGbits.PCFG &= ~(1 << POTNT_CH);
//SSRC<2:0>: Conversion Trigger Source Select bits
//111 = Internal counter ends sampling and starts conversion (auto convert)
//110 = Reserved
//101 = Reserved
//100 = Reserved
//011 = Reserved
//010 = Timer3 period match ends sampling and starts conversion
//001 = Active transition on INT0 pin ends sampling and starts conversion
//000 = Clearing SAMP bit ends sampling and starts conversion
    AD1CON1bits.SSRC = 0b111;
    AD1CON2 = 0;
//SAMC<4:0>: Auto-Sample Time bits
//11111 = 31 T AD
//·····
//00001 = 1 T AD
//00000 = 0 T AD (not recommended)
    AD1CON3bits.SAMC = 0b11111;
//11111111 = TPB • 2 • (ADCS<7:0> + 1) = 512 • TPB = TAD
//•
//•
//•
//00000001 = TPB • 2 • (ADCS<7:0> + 1) = 4 • TPB = TAD
//00000000 = TPB • 2 • (ADCS<7:0> + 1) = 2 • TPB = TAD
    AD1CON3bits.ADCS = 0;
//CH0SA<3:0>: Positive Input Select bits for MUX A Multiplexer Setting
//1111 = Channel 0 positive input is AN15
//1110 = Channel 0 positive input is AN14
//1101 = Channel 0 positive input is AN13
//•
//•
//•
//0001 = Channel 0 positive input is AN1
//0000 = Channel 0 positive input is AN0
    AD1CHSbits.CH0SA = POTNT_CH;
    AD1CSSL = 0;
    AD1CON1bits.ADON = 1;
}

uint16_t ADC_res = 0;

uint16_t potnt_get_value ( void )
{
//DONE: A/D Conversion Status bit
//1 = A/D conversion is done
//0 = A/D conversion is NOT done
    if ( AD1CON1bits.DONE )
    {
        AD1CON1bits.DONE = 0;            // reset DONE bit
        ADC_res = ADC1BUF0;
    }
	return( ADC_res );       			// read ADC1 data 
}
