/*
 * File:   pmain.c
 * Author: Alex Devyatkin
 *
 */

#include <xc.h>
#include <stdint.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = ON         // Data Memory Code Protection (Data memory code protection is enabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

void delay_ms ( uint16_t time_ms )
{
    TMR2 = 0;                   // Сброс таймера
    T2CONbits.T2CKPS = 0b11;    // Настройка на делителе 64
    PR2 = 125;                  // Расчет на работу таймера до переполенния в течении 1 мс
                                // 125 = 32MHz / 4 / 64 * 0.001 sec
    T2CONbits.TMR2ON = 1;       // Включаем таймер
    
    uint16_t msCounter = 0; // Каждые 1 мс таймер увеличивается на 1
    while ( msCounter < time_ms )   // Счет до получения требуемых микросекунд
    {
        while ( !TMR2IF ) { }   // Ожидаем переполнения
        TMR2IF = 0;             // очищаем флаг
        msCounter++;            // Отсчитали 1 мс - инкрементируем таймер
    }
    
    T1CONbits.TMR1ON = 0;       // Отключаем таймер
}

uint16_t freqVal = 0;

void main( void ) 
{  
    /*
     * Настройка на 32 MHz с PLLx4 (8*4) MHz 
     */
    OSCCONbits.IRCF = 0b1110;
    OSCCONbits.SCS = 0b00;
    OSCCONbits.SPLLEN = 1;
    /*******************************************/
    
    TRISB0 = 1;
    ANSB0 = 1;
    CPSCON0bits.CPSON = 1;
    CPSCON0bits.CPSRNG = 0b11;
    CPSCON0bits.CPSRM = 1;
    CPSCON0bits.T0XCS = 1;
    
    // Enable interrupts
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    OPTION_REGbits.nWPUEN = 1;
    OPTION_REGbits.INTEDG = 1;
    OPTION_REGbits.PS = 0b111;
    TMR0IF = 0;
    TMR0IE = 1;
    
    T1CONbits.TMR1CS = 0b11;
    T1CONbits.nT1SYNC = 1;
    T1CONbits.TMR1ON = 1;
    
    T1GCONbits.TMR1GE = 1;
    T1GCONbits.T1GPOL = 1;
    T1GCONbits.T1GTM = 1;
    T1GCONbits.T1GVAL = 1;
    T1GCONbits.T1GSS = 0b01;
    
    TMR1GIF = 0;
    TMR1GIE = 1;
    
    
    
    TRISA0 = 0;
    
    
    while ( 1 )
    {  
        if ( freqVal > 300 )
        {
            LATA0 = 1;
        }
        else
        {
            LATA0 = 0;
        }
//        for ( int i = 0; i < 6; i++ )
//        {
//            LATA |= 1 << i;
//            delay_ms( 1000 );
//        }
//        LATA = 0b000000;
    }
    return;
}

void interrupt tc_int(void)
{
    if ( TMR0IF )
    {
        freqVal = ((int)TMR1H << 8) | TMR1L;
        T1CONbits.TMR1ON = 0;
        TMR1H = TMR1L = 0;
        T1CONbits.TMR1ON = 1;
        TMR0IF = 0;
    }
    else if ( TMR1GIF )
    {
        TMR1GIF = 0;
    }
} 
