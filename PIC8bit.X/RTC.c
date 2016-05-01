/*
 * File:   RTC.c
 * Author: Alex Devyatkin
 *
 */

#include "core.h"

const char *weekDay[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };

void rtc_init ( void )
{
    i2c_write_bits_eeprom( 0b1101111, 0x00, 7, 1, 1 );
    
}

void rtc_receive_time ( void )
{
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

//    memset( bufferOne, 0, sizeof( BUF_SIZE ) );
//    sprintf( bufferOne, "Time: %02d:%02d:%02d", hours, minutes, seconds );
//
//
//    memset( bufferTwo, 0, sizeof( BUF_SIZE ) );
//    sprintf( bufferTwo, "%s %02d.%02d.%02d", weekDay[wDay-1], year, month, date );
}
