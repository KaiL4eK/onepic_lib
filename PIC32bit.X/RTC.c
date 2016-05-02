/*
 * File:   RTC.c
 * Author: Alex Devyatkin
 *
 */

#include "core.h"

#define RTC_ADDRESS 0b1101111

#define ENABLE_REG      0x00
#define ENABLE_START    7
#define ENABLE_LNGTH    1

#define SEC_REG         0x00
#define MIN_REG         0x01
#define HOUR_REG        0x02
#define WDAY_REG        0x03
#define DATE_REG        0x04
#define MNTH_REG        0x05
#define YEAR_REG        0x06

const char  *weekDay[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
time_t      currentTime;

void rtc_init ( void )
{
    i2c_write_bits_eeprom( RTC_ADDRESS, ENABLE_REG, ENABLE_START, ENABLE_LNGTH, 1 );  //Enable clock
    
}

void rtc_receive_time ( void )
{
    uint8_t seconds = i2c_read_byte_eeprom( RTC_ADDRESS, SEC_REG ),
            minutes = i2c_read_byte_eeprom( RTC_ADDRESS, MIN_REG ),
            hours = i2c_read_byte_eeprom(   RTC_ADDRESS, HOUR_REG ),
            wDay = i2c_read_byte_eeprom(    RTC_ADDRESS, WDAY_REG ),
            date = i2c_read_byte_eeprom(    RTC_ADDRESS, DATE_REG ),
            month = i2c_read_byte_eeprom(   RTC_ADDRESS, MNTH_REG ),
            year = i2c_read_byte_eeprom(    RTC_ADDRESS, YEAR_REG );
        
    currentTime.tm_sec = (seconds & 0xF) + ((seconds >> 4) & 0x7)*10;
    currentTime.tm_min = (minutes & 0xF) + ((minutes >> 4) & 0x7)*10;
    currentTime.tm_hour = (hours & 0xF) + ((hours >> 4) & 0x3)*10;

    currentTime.tm_wday = wDay & 0x7;
    currentTime.tm_mday = (date & 0xF) + ((date >> 4) & 0x3)*10;
    currentTime.tm_mon = (month & 0xF) + ((month >> 4) & 0x1)*10;
    currentTime.tm_year = (year & 0xF) + ((year >> 4) & 0xF)*10;
}

time_t *rtc_get_raw_data ( void )
{
    rtc_receive_time();
    return( &currentTime );
}
