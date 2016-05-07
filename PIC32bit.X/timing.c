/*
 * File:   timing.c
 * Author: Alex Devyatkin
 *
 */

#include "core.h"

void pll_32MHz_init ( void )
{

}

void delay_ms ( uint16_t time_ms )
{
    TMR1 = 0;                   // Сброс таймера
//TCKPS<1:0>: Timer Input Clock Prescale Select bits
//11 = 1:256 prescale value
//10 = 1:64 prescale value
//01 = 1:8 prescale value
//00 = 1:1 prescale value
    T1CONbits.TCKPS = 0b01;    // Настройка на делителе 8
    PR1 = SYS_CLK/8/1000;      // Расчет на работу таймера до переполенния в течении 1 мс
                               // FCY / Div * 0.001 sec
    IEC0bits.T1IE = 1;
    T1CONbits.TON = 1;       // Включаем таймер

    uint32_t msCounter = 0;     // Каждые 1 мс таймер увеличивается на 1
    while ( msCounter < time_ms )   // Счет до получения требуемых микросекунд
    {
        while ( !IFS0bits.T1IF ) {  }  // Ожидаем переполнения
        IFS0bits.T1IF = 0;             // очищаем флаг
        msCounter++;            // Отсчитали 1 мс - инкрементируем таймер
    }
    
    T1CONbits.TON = 0;       // Отключаем таймер
}

void delay_us ( uint16_t time_us )
{
    TMR1 = 0;                   // Сброс таймера
    T1CONbits.TCKPS = 0b00;    // Настройка на делителе 1
    PR1 = SYS_CLK/1000000;         // Расчет на работу таймера до переполенния в течении 1 мкс
                                // FCY / Div * 0.000001 sec
    IEC0bits.T1IE = 1;
    T1CONbits.TON = 1;       // Включаем таймер
    
    uint32_t usCounter = 0;     // Каждые 1 мкс таймер увеличивается на 1
    while ( usCounter < time_us )   // Счет до получения требуемых микросекунд
    {
        while ( !IFS0bits.T1IF ) { }   // Ожидаем переполнения
        IFS0bits.T1IF = 0;             // очищаем флаг
        usCounter++;            // Отсчитали 1 мкс - инкрементируем таймер
    }
    
    T1CONbits.TON = 0;       // Отключаем таймер
}
