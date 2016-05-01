/*
 * File:   timing.c
 * Author: Alex Devyatkin
 *
 */

#include "core.h"

void pll_32MHz_init ( void )
{
    /*
     * Настройка на 32 MHz с PLLx4 (8*4) MHz 
     */
    OSCCONbits.IRCF = 0b1110;
    OSCCONbits.SCS = 0b00;
    OSCCONbits.SPLLEN = 1;
    while ( PLLR == 0 ) { }
}

void delay_ms ( uint16_t time_ms )
{
    TMR2 = 0;                   // Сброс таймера
    T2CONbits.T2CKPS = 0b11;    // Настройка на делителе 64
    PR2 = FCY/64/1000;          // Расчет на работу таймера до переполенния в течении 1 мс
                                // 125 = 32MHz / 4 / 64 / 1 * 0.001 sec
    T2CONbits.TMR2ON = 1;       // Включаем таймер
    
    uint16_t msCounter = 0;     // Каждые 1 мс таймер увеличивается на 1
    while ( msCounter < time_ms )   // Счет до получения требуемых микросекунд
    {
        while ( !TMR2IF ) {  }  // Ожидаем переполнения
        TMR2IF = 0;             // очищаем флаг
        msCounter++;            // Отсчитали 1 мс - инкрементируем таймер
    }
    
    T2CONbits.TMR2ON = 0;       // Отключаем таймер
}

void delay_us ( uint16_t time_us )
{
    TMR2 = 0;                   // Сброс таймера
    T2CONbits.T2CKPS = 0b00;    // Настройка на делителе 1
    PR2 = FCY/1000000;          // Расчет на работу таймера до переполенния в течении 1 мкс
                                // 8 = 32MHz / 4 / 1 / 1 * 0.000001 sec
    T2CONbits.TMR2ON = 1;       // Включаем таймер
    
    uint16_t usCounter = 0;     // Каждые 1 мкс таймер увеличивается на 1
    while ( usCounter < time_us )   // Счет до получения требуемых микросекунд
    {
        while ( !TMR2IF ) { }   // Ожидаем переполнения
        TMR2IF = 0;             // очищаем флаг
        usCounter++;            // Отсчитали 1 мкс - инкрементируем таймер
    }
    
    T2CONbits.TMR2ON = 0;       // Отключаем таймер
}
