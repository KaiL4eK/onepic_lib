#include "core.h"

/********************************/
/*              UART            */
/********************************/

#define BUFFER_MAX_SIZE 512

void UART_write_byte( uint8_t elem );

char UART_buffer[BUFFER_MAX_SIZE];

void UART_init( UART_speed_t UART_br )
{
	U1BMODEbits.ON = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
	U1BMODEbits.UARTEN = 0;		// Bits8,9 TX,RX enabled, CTS,RTS not
	U1BBRG = UART_br;
//    IEC2bits.U1BRXIE = 1;
//    IFS2bits.U1BRXIF = 0;
    
	U1BMODEbits.ON = 1;	// And turn the peripheral on
	U1BSTAbits.UTXEN = 1;
}

uint8_t input_command = 0;

void __attribute__( (__interrupt__) ) _U1BRXInterrupt()
{
    input_command = U1BRXREG;
    IFS2bits.U1BRXIF = 0;
}

uint8_t UART_get_last_received_command()
{
    if ( input_command == 0 )
        return( 0 );
    uint8_t last_rcv = input_command;
    input_command = 0;
    return( last_rcv );
}

void UART_write_byte( uint8_t elem )
{
    while( U1BSTAbits.UTXBF ) 
        Nop();
    U1BTXREG = elem;
}

#define HIGH( x ) (((x) >> 8) & 0xff)
#define LOW( x ) ((x) & 0xff)

void UART_write_words( uint16_t *arr, uint8_t count )
{
    uint8_t iter = 0;
    for ( iter = 0; iter < count; iter++ )
    {
        UART_write_byte( HIGH( arr[iter] ) );
        UART_write_byte( LOW( arr[iter] ) );
    }
}

void UART_write_string( const char *fstring, ... )
{
    int iter = 0;
    va_list str_args;
    
    va_start( str_args, fstring );
    vsprintf( UART_buffer, fstring, str_args );
    va_end( str_args );
    
    while( UART_buffer[iter] != '\0' )
    {
        UART_write_byte( UART_buffer[iter++] );
    }
}
