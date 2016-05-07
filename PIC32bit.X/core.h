#ifndef CORE_H_
#define	CORE_H_

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "freq.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

void delay_ms ( uint16_t time_ms );
void delay_us ( uint16_t time_us );
void pll_32MHz_init ( void );

void i2c_init( uint32_t Fscl );
int8_t i2c_write_byte_eeprom( uint8_t slave_addr, uint8_t eeprom_addr, uint8_t data );
int8_t i2c_write_word_eeprom( uint8_t slave_addr, uint8_t eeprom_addr, uint16_t data );
int8_t i2c_write_bits_eeprom( uint8_t slave_addr, uint8_t eeprom_addr, uint8_t bit_start, uint8_t length, uint8_t data );
int8_t i2c_read_bytes_eeprom( uint8_t slave_addr, uint8_t eeprom_addr, uint8_t *data, uint8_t lenght );
uint8_t i2c_read_byte_eeprom( uint8_t slave_addr, uint8_t eeprom_addr );

typedef enum
{
    UART_115200 = 42,
    UART_57600 = 86,
    UART_38400 = 129,
    UART_19200 = 259,
    UART_9600 = 520
}UART_speed_t;

void UART_init( UART_speed_t UART_br );
void UART_write_words( uint16_t *arr, uint8_t count );
void UART_write_string( const char *fstring, ... );
void UART_write_byte( uint8_t elem );
uint8_t UART_get_last_received_command();

void lcd_init( void );
void lcd_write_string ( char * );
void lcd_clear ( void );
void lcd_setLineOne ( void );
void lcd_setLineTwo ( void );

void potnt_init ( void );
uint16_t potnt_get_value ( void );

typedef struct
{
    uint8_t tm_sec,
            tm_min,
            tm_hour,
            tm_mday,
            tm_mon,
            tm_year,
            tm_wday,
            tm_yday,
            tm_isdst;
} time_t;

void rtc_init ( void );
void rtc_receive_time ( void );
time_t *rtc_get_raw_data ( void );

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif	/* CORE_H_ */






