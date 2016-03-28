/*
 * File:   pmain.c
 * Author: Alex Devyatkin
 *
 */

#include "LCD.h"
#include "core_.h"

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
                                // 125 = 32MHz / 4 / 64 / 1 * 0.001 sec
    T2CONbits.TMR2ON = 1;       // Включаем таймер
    
    uint16_t msCounter = 0; // Каждые 1 мс таймер увеличивается на 1
    while ( msCounter < time_ms )   // Счет до получения требуемых микросекунд
    {
        while ( !TMR2IF ) { }   // Ожидаем переполнения
        TMR2IF = 0;             // очищаем флаг
        msCounter++;            // Отсчитали 1 мс - инкрементируем таймер
    }
    
    T2CONbits.TMR2ON = 0;       // Отключаем таймер
}

void delay_us ( uint16_t time_us )
{
    TMR2 = 0;                   // Сброс таймера
    T2CONbits.T2CKPS = 0b00;    // Настройка на делителе 1
    PR2 = 8;                    // Расчет на работу таймера до переполенния в течении 1 мкс
                                // 8 = 32MHz / 4 / 1 / 1 * 0.000001 sec
    T2CONbits.TMR2ON = 1;       // Включаем таймер
    
    uint16_t msCounter = 0;     // Каждые 1 мкс таймер увеличивается на 1
    while ( msCounter < time_us )   // Счет до получения требуемых микросекунд
    {
        while ( !TMR2IF ) { }   // Ожидаем переполнения
        TMR2IF = 0;             // очищаем флаг
        msCounter++;            // Отсчитали 1 мкс - инкрементируем таймер
    }
    
    T2CONbits.TMR2ON = 0;       // Отключаем таймер
}

uint16_t readADCVal ( int channel )
{
    ADCON0bits.CHS = channel;
    
    delay_us( 5 );
    
    ADCON0bits.GO = 1;
    while ( ADCON0bits.nDONE ) { }
    
    return( ((int)ADRESH << 8) | ADRESL );
}

typedef enum
{
    MID_B = 8,
    UP_B = 9,
    LEFT_B = 10,
    RIGHT_B = 11,
    DOWN_B = 12
            
}CapButtons_t;

uint16_t    iOffsetUp_b = 0,
            iOffsetDown_b = 0,
            iOffsetMid_b = 0,
            iOffsetLeft_b = 0,
            iOffsetRight_b = 0;

uint16_t scanMTouchButton ( CapButtons_t button  );

uint16_t getMTouchButIOffset ( CapButtons_t button )
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

void initMTouchButtons ( void )
{
    ADCON1bits.ADFM = 1;
    ADCON1bits.ADCS = 0b101;
    ADCON0bits.ADON = 1;
    
    for ( int i = 0; i < MTOUCH_SAMPLES; i++ )
    {
        iOffsetUp_b += scanMTouchButton( UP_B );
        iOffsetDown_b += scanMTouchButton( DOWN_B );
        iOffsetMid_b += scanMTouchButton( MID_B );
        iOffsetLeft_b += scanMTouchButton( LEFT_B );
        iOffsetRight_b += scanMTouchButton( RIGHT_B );
    }
    
    iOffsetUp_b = (uint16_t)(iOffsetUp_b/MTOUCH_SAMPLES * OFF_PERCENT);
    iOffsetDown_b = (uint16_t)(iOffsetDown_b/MTOUCH_SAMPLES * OFF_PERCENT);
    iOffsetMid_b = (uint16_t)(iOffsetMid_b/MTOUCH_SAMPLES * OFF_PERCENT);
    iOffsetLeft_b = (uint16_t)(iOffsetLeft_b/MTOUCH_SAMPLES * OFF_PERCENT);
    iOffsetRight_b = (uint16_t)(iOffsetRight_b/MTOUCH_SAMPLES * OFF_PERCENT);
}

uint16_t scanMTouchButton ( CapButtons_t button  )
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

int isMTouchButPressed ( CapButtons_t button )
{
    return( scanMTouchButton( button ) < getMTouchButIOffset( button ) ? 1 : 0 );
}

void initPLL32MHz ( void )
{
    /*
     * Настройка на 32 MHz с PLLx4 (8*4) MHz 
     */
    OSCCONbits.IRCF = 0b1110;
    OSCCONbits.SCS = 0b00;
    OSCCONbits.SPLLEN = 1;
    while ( PLLR == 0 ) { }
}

#define POTENC_CH 13

uint16_t potencReadVal ( void )
{    
    return( readADCVal( POTENC_CH ) );
}

char *weekDay[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };

void main( void ) 
{  
    initPLL32MHz();    
    lcd_init();
    initMTouchButtons();
    i2c_init( 400000L );
    i2c_write_bits_eeprom( 0b1101111, 0x00, 7, 1, 1 );
    TRISD &= 0b00001111; 
    TRISA &= 0b11000000;
    
#define BUF_SIZE 16
    
    uint8_t bufferOne[BUF_SIZE],
            bufferTwo[BUF_SIZE];
    
    while ( 1 )
    {  
        LATD4 = isMTouchButPressed( UP_B ) | isMTouchButPressed( MID_B );
        LATD5 = isMTouchButPressed( LEFT_B ) | isMTouchButPressed( MID_B );
        LATD6 = isMTouchButPressed( RIGHT_B ) | isMTouchButPressed( MID_B );
        LATD7 = isMTouchButPressed( DOWN_B ) | isMTouchButPressed( MID_B );
        
        uint16_t potencVal = potencReadVal(),
                ledStep = 1023/6;
        
        LATA &= 0b11000000;
        
        for ( int i = 0; i <= 5; i++ )
        {
            if ( potencVal > ledStep*i )
            {
                LATA |= 1 << i;
            }
        }
        
        uint8_t seconds = i2c_read_byte_eeprom( 0b1101111, 0x00 ),
                minutes = i2c_read_byte_eeprom( 0b1101111, 0x01 ),
                hours = i2c_read_byte_eeprom( 0b1101111, 0x02 ),
                wDay = i2c_read_byte_eeprom( 0b1101111, 0x03 ),
                date = i2c_read_byte_eeprom( 0b1101111, 0x04 ),
                month = i2c_read_byte_eeprom( 0b1101111, 0x05 ),
                year = i2c_read_byte_eeprom( 0b1101111, 0x06 );
        
        seconds = (seconds & 0xF) + ((seconds >> 4) & 0x7)*10;
        minutes = (minutes & 0xF) + ((minutes >> 4) & 0x7)*10;
        hours = (hours & 0xF) + ((hours >> 4) & 0x3)*10;
        
        wDay = wDay & 0x7;
        date = (date & 0xF) + ((date >> 4) & 0x3)*10;
        month = (month & 0xF) + ((month >> 4) & 0x1)*10;
        year = (year & 0xF) + ((year >> 4) & 0xF)*10;
        
        memset( bufferOne, 0, sizeof( BUF_SIZE ) );
        sprintf( bufferOne, "Time: %02d:%02d:%02d", hours, minutes, seconds );
        
//        sprintf( bufferOne, "M:%dU:%dD:%dR:%dL:%d", 
//                            isMTouchButPressed( MID_B ), 
//                            isMTouchButPressed( UP_B ), 
//                            isMTouchButPressed( DOWN_B ),
//                            isMTouchButPressed( RIGHT_B ), 
//                            isMTouchButPressed( LEFT_B ) );
        
        memset( bufferTwo, 0, sizeof( BUF_SIZE ) );
        sprintf( bufferTwo, "%s %02d.%02d.%02d", weekDay[wDay-1], year, month, date );
        
//        sprintf( bufferTwo, "POT:%d", 
//                            potencReadVal() );
        
        lcd_clear();
        lcd_setLineOne();
        lcd_write_string( bufferOne );
        
        lcd_setLineTwo();
        lcd_write_string( bufferTwo );
        
        delay_ms( 50 );
    }
    return;
}

void interrupt tc_int(void)
{
    
} 
