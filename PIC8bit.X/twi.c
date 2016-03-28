#include "core_.h"

inline void i2c_idle( void )
{
    // Wait until I2C Bus is Inactive
    while ( SSPCON2bits.SEN || SSPCON2bits.RSEN || SSPCON2bits.PEN || SSPCON2bits.RCEN ||
            SSPCON2bits.ACKEN ) { }
}

void i2c_init( long Fscl )
{
   SSPCON1bits.SSPEN = 0;           // Disable I2C Mode
   SSPCON1bits.SSPM3 = 1;           // Set I2C Master
   SSPCON1bits.SSPM2 = SSPCON1bits.SSPM1 = SSPCON1bits.SSPM0 = 0;
   SSPADD = (FCY/Fscl) - 1;
   
   SSPCON1bits.SSPEN = 1;           // Enable I2C Mode
   i2c_idle();
}

inline void i2c_start( void )
{
    i2c_idle();                     //Ensure Module is Idle
    SSPCON2bits.SEN = 1;	//Initiate Start condition
    while ( SSPCON2bits.SEN );   
}

inline void i2c_stop( void )
{
    i2c_idle();                     //Ensure Module is Idle
    //initiate stop bit
    SSPCON2bits.PEN = 1;    //Initiate stop bit
    while ( SSPCON2bits.PEN );         // Wait for stop condition to finish
}

inline void i2c_restart( void )
{
    i2c_idle();                     //Ensure Module is Idle
    SSPCON2bits.RSEN = 1;	//Initiate restart condition
    while ( SSPCON2bits.RSEN );
}

inline void i2c_ack( void )
{
    i2c_idle();                     //Ensure Module is Idle
    SSPCON2bits.ACKDT = 0;          // Acknowledge data bit, 0 = ACK
    SSPCON2bits.ACKEN = 1;          // Ack data enabled
    while ( SSPCON2bits.ACKEN );       // wait for ack data to send on bus
}

inline void i2c_nack( void )
{
    i2c_idle();                     //Ensure Module is Idle
    SSPCON2bits.ACKDT = 1;          // Acknowledge data bit, 1 = NAK
    SSPCON2bits.ACKEN = 1;          // Ack data enabled
    while ( SSPCON2bits.ACKEN );       // wait for ack data to send on bus
}

inline int8_t i2c_send_byte(uint8_t data)
{
    i2c_idle();                     //Ensure Module is Idle
    SSPBUF = data;    
    while ( SSPSTATbits.BF );       // wait till complete data is sent from buffer */
    while ( SSPCON2bits.ACKSTAT ); 
    i2c_idle();
    return( 0 );
}

inline int8_t i2c_receive_byte(uint8_t *data)
{
    i2c_idle();                     //Ensure Module is Idle
    SSPCON2bits.RCEN = 1;        // Enable data reception
    while ( SSPCON2bits.RCEN && !SSPSTATbits.BF );
    i2c_idle();
    *data = SSPBUF;
    return( 0 );
}

inline int8_t i2c_send_bytes(uint8_t *str, uint8_t lenght)
{
    while(lenght--)                    //Transmit Data Until Pagesize
    {
        i2c_idle();                     //Ensure Module is Idle
        if ( i2c_send_byte(*str) != 0 )       //Write 1 byte
        {
            return( -1 );
        }
        i2c_idle();                 //Wait for Idle bus
        str++;
    }
    return( 0 );
}

inline int8_t i2c_receive_bytes(uint8_t *str, uint8_t length)
{
    while(length--)
    {
        i2c_idle();                     //Ensure Module is Idle
        if ( i2c_receive_byte( str++ ) != 0 )		//get a single byte
        {
            return( -1 );
        }
        i2c_idle();
        if(length)
            i2c_ack();				//Acknowledge until all read
        else
            i2c_nack();
        i2c_idle();
    }
    return( 0 );
}

/** External API **/

int8_t i2c_write_byte_eeprom(uint8_t slave_addr, uint8_t eeprom_addr, uint8_t data)
{
    i2c_start();                    //Generate Start COndition
    // Function send already has idle inside
    if ( i2c_send_byte( (slave_addr << 1) | 0x0 ) != 0 )       //Write Control byte
    {
        return( -1 );
    }
    if ( i2c_send_byte( eeprom_addr ) != 0 )            //Write Low Address
    {
        return( -1 );
    }
    if ( i2c_send_byte( data ) != 0 )                 //Write Data
    {
        return( -1 );
    }
    i2c_stop();                     //Initiate Stop Condition
    return( 0 );
}

int8_t i2c_write_word_eeprom(uint8_t slave_addr, uint8_t eeprom_addr, uint16_t data)
{
    i2c_start();                    //Generate Start COndition
    // Function send already has idle inside
    if ( i2c_send_byte( (slave_addr << 1) | 0x0 ) != 0 )       //Write Control byte
    {
        return( -1 );
    }
    if ( i2c_send_byte( eeprom_addr ) != 0 )            //Write Low Address
    {
        return( -1 );
    }
    if ( i2c_send_byte( data >> 8 ) != 0 )                 //Write Data
    {
        return( -1 );
    }
    if ( i2c_send_byte( data & 0xFF ) != 0 )                 //Write Data
    {
        return( -1 );
    }
    i2c_stop();                     //Initiate Stop Condition
    return( 0 );
}

int8_t i2c_read_bytes_eeprom(uint8_t slave_addr, uint8_t eeprom_addr, uint8_t *data, uint8_t lenght)
{
    i2c_start();                    //Generate Start Condition
    if ( i2c_send_byte( (slave_addr << 1) | 0x0 ) != 0 )        //Write Control Byte
    {
        return( -1 );
    }
    if ( i2c_send_byte( eeprom_addr ) != 0 )            //Write start address
    {
        return( -1 );
    }
    i2c_restart();                  //Generate restart condition
    if ( i2c_send_byte( (slave_addr << 1) | 0x1 ) != 0 ) //Write control byte for read
    {
        return( -1 );
    }
    if ( i2c_receive_bytes( data, lenght ) != 0 )             //read Length number of bytes
    {
        return( -1 );
    }
    i2c_stop();                     //Generate Stop
    return( 0 );
}

uint8_t i2c_read_byte_eeprom(uint8_t slave_addr, uint8_t eeprom_addr)
{
    uint8_t data;
    i2c_read_bytes_eeprom( slave_addr, eeprom_addr, &data, 1 );
    return( data );
}

int8_t i2c_write_bits_eeprom(uint8_t slave_addr, uint8_t eeprom_addr, uint8_t bit_start, uint8_t length, uint8_t data)
{
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t data_reg,
            data_write = data;
    i2c_read_bytes_eeprom(slave_addr, eeprom_addr, &data_reg, 1);
    uint8_t mask = ((1 << length) - 1) << (bit_start - length + 1);
    data_write <<= (bit_start - length + 1); // shift data into correct position
    data_write &= mask; // zero all non-important bits in data
    data_reg &= ~(mask); // zero all important bits in existing byte
    data_reg |= data_write; // combine data with existing byte
    i2c_write_byte_eeprom(slave_addr, eeprom_addr, data_reg);
    return( 0 );
}
