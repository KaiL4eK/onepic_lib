#ifndef CORE_H_
#define	CORE_H_

#include <xc.h>
#include <stdint.h>
#include "freq.h"

void delay_ms ( uint16_t time_ms );
void delay_us ( uint16_t time_us );
void pll_32MHz_init ( void );

void i2c_init( uint32_t Fscl );
int8_t i2c_write_byte_eeprom( uint8_t slave_addr, uint8_t eeprom_addr, uint8_t data );
int8_t i2c_write_word_eeprom( uint8_t slave_addr, uint8_t eeprom_addr, uint16_t data );
int8_t i2c_write_bits_eeprom( uint8_t slave_addr, uint8_t eeprom_addr, uint8_t bit_start, uint8_t length, uint8_t data );
int8_t i2c_read_bytes_eeprom( uint8_t slave_addr, uint8_t eeprom_addr, uint8_t *data, uint8_t lenght );
uint8_t i2c_read_byte_eeprom( uint8_t slave_addr, uint8_t eeprom_addr );

void lcd_init( void );
void lcd_write_string ( char * );
void lcd_clear ( void );
void lcd_setLineOne ( void );
void lcd_setLineTwo ( void );

void potnt_init ( void );
int16_t potnt_get_value ( void );

void pwm_init ( uint16_t period_ms );
void pwm_set_dutyCycle_percent ( uint8_t percentage );

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

#endif	/* CORE_H_ */






