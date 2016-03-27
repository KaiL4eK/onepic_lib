#ifndef LCD_H_
#define	LCD_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <xc.h>

// D7-D4 pins
#define D7_bit      LATD3
#define D7_ddr      TRISD3

#define D6_bit      LATD2
#define D6_ddr      TRISD2

#define D5_bit      LATD1
#define D5_ddr      TRISD1

#define D4_bit      LATD0
#define D4_ddr      TRISD0

//Enable pin
#define E_bit       LATA6
#define E_ddr       TRISA6

// Register select pin
#define RS_bit      LATC0
#define RS_ddr      TRISC0

// LCD module information
#define cmd_LineOne     0x00                    // start of line 1
#define cmd_LineTwo     0x40                    // start of line 2

// LCD instructions
#define cmd_Clear           0b00000001          // replace all characters with ASCII 'space'
#define cmd_Home            0b00000010          // return cursor to first position on first line
#define cmd_EntryMode       0b00000110          // shift cursor from left to right on read/write
#define cmd_DisplayOff      0b00001000          // turn display off
#define cmd_DisplayOn       0b00001100          // display on, cursor off, don't blink character
#define cmd_FunctionReset   0b00110000          // reset the LCD
#define cmd_FunctionSet4bit 0b00101000          // 4-bit data, 2-line display, 5 x 7 font
#define cmd_SetCursor       0b10000000          // set cursor position


// Function Prototypes
void lcd_init(void);
void lcd_write_4(uint8_t);
void lcd_write_instruction_4d(uint8_t);
void lcd_write_character_4d(uint8_t);
void lcd_write_string_4d(uint8_t *);
void lcd_init_4d(void);

#endif	/* LCD_H_ */

