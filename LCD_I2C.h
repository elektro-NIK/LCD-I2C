#ifndef LCD_I2C_h
#define LCD_I2C_h

/*@{*/
#define LCD_ADDR            0x3F        // 0x27 for PCF8584T or 0x3F for PCF8584AT (default address)
// commands
#define LCD_CLRSCR          0b00000001  // clear screen & go to [0,0]
#define LCD_GOTO00          0b00000010  // go to [0,0]
#define LCD_ENTRYMODE       0b00000100  // shift cursor & display mode
#define LCD_DISPCTRL        0b00001000  // control display on/off mode, cursor on/off mode
#define LCD_SHIFTMODE       0b00010000  // cursor or display shift
#define LCD_SETUP           0b00100000  // parameters display and bus
#define LCD_CGRAMADDR       0b01000000  // 6 lsb = address CGRAM
#define LCD_DDRAMADDR       0b10000000  // 7 lsb = address DDRAM
// flags names
#define LCDID               1           // increment (1) or decrement (0). Increment is left-to-right mode.
#define LCDS                0           // display shift enable (1) or disable (0). As old typewriter.
#define LCDD                2           // display on (1)
#define LCDC                1           // cursor underscores (1)
#define LCDB                0           // cursor black box (1)
#define LCDSC               3           // display (1) or cursor (0) shift. Once for command.
#define LCDRL               2           // direction for shift. Right (1) or left (0)
#define LCDDL               4           // bus lenght. 8-bit (1) or 4-bit (0).
#define LCDN                3           // number of lines. 2 line (1) or 1 line (0)
#define LCDF                2           // size of symbols. 5x7 (0) or 5x10 (1). If N=1, then F does not matter.
#define DB7                 7           // data bit 7/3 in 4-bit mode
#define DB6                 6           // data bit 6/2 in 4-bit mode
#define DB5                 5           // data bit 5/1 in 4-bit mode
#define DB4                 4           // data bit 4/0 in 4-bit mode
#define LCDBL               3           // backlight bit
#define LCDEN               2           // enable bit
#define LCDRW               1           // read/write bit
#define LCDRS               0           // register select bit
// flags for display entry mode
#define LCD_COUNTERINC      1<<LCDID
#define LCD_COUNTERDEC      0<<LCDID
#define LCD_DISPSHIFTON     1<<LCDS
#define LCD_DISPSHIFTOFF    0<<LCDS
// flags for display on/off control
#define LCD_DISPON          1<<LCDD
#define LCD_DISPOFF         0<<LCDD
#define LCD_CURSUNDERON     1<<LCDC
#define LCD_CURSUNDEROFF    0<<LCDC
#define LCD_CURSBOXON       1<<LCDB
#define LCD_CURSBOXOFF      0<<LCDB
// flags for display/cursor shift
#define LCD_DISPMOVE        1<<LCDSC
#define LCD_CURSMOVE        0<<LCDSC
#define LCD_MOVERIGHT       1<<LCDRL
#define LCD_MOVELEFT        0<<LCDRL
// flags for function set
#define LCD_8BITMODE        1<<LCDDL
#define LCD_4BITMODE        0<<LCDDL
#define LCD_2LINE           1<<LCDN
#define LCD_1LINE           0<<LCDN
#define LCD_5x10DOTS        1<<LCDF
#define LCD_5x8DOTS         0<<LCDF
// flags for I2C convertor
#define LCD_BL              1<<LCDBL
#define LCD_EN              1<<LCDEN
#define LCD_RW              1<<LCDRW
#define LCD_RS              1<<LCDRS
#define LCDCOMMAND          0
#define LCDDATA             1
#define LCDWRITE            0
#define LCDREAD             1
#define MASKDB              (1<<DB7 | 1<<DB6 | 1<<DB5 | 1<<DB4)

#define LCD_SETUP_BL        0
#define LCD_SETUP_UCURS     1
#define LCD_SETUP_BCURS     2
#define LCD_SETUP_INCDEC    3
#define LCD_SETUP_DISPSHIFT 4
#define SETBACKLIGHT        (lcd_settings & 1<<LCD_SETUP_BL)
#define SETUNDERCURS        (lcd_settings & 1<<LCD_SETUP_UCURS)
#define SETBOXCURS          (lcd_settings & 1<<LCD_SETUP_BCURS)
#define SETINCDEC           (lcd_settings & 1<<LCD_SETUP_INCDEC)
#define SETDISPSHIFT        (lcd_settings & 1<<LCD_SETUP_DISPSHIFT)

#define CURS_NONE           0b00
#define CURS_UNDER          0b01
#define CURS_BOX            0b10
#define CURS_BOTH           0b11
/*@}*/

void LCD_Init();                                      // Initialize LCD
void LCD_ClrScr();                                    // Clear screen & go to [0,0] position cursor
void LCD_GotoHome();                                  // Go to [0,0] cursor & screen position
void LCD_On();                                        // Screen on
void LCD_Off();                                       // Screen off
void LCD_Cursor(uint8_t cursor);                      // Set cursor type or cursor off (CURS_NONE, CURS_UNDER (_), CURS_BOX (black block), CURS_BOTH (both cursors))
void LCD_LeftToRight();                               // Left to right writing
void LCD_RightToLeft();                               // Right to left writing
void LCD_ShiftDispOn();                               // Screen shifting on (follows the cursor shifting)
void LCD_ShiftDispOff();                              // Screen shifting off
void LCD_MoveCursorLeft();                            // Move the cursor one position to the left
void LCD_MoveCursorRight();                           // Move the cursor one position to the right
void LCD_MoveScreenLeft();                            // Move screen one position to the left
void LCD_MoveScreenRight();                           // Move screen one position to the right
void LCD_BacklightOn();                               // On backlight
void LCD_BacklightOff();                              // Off backlight
void LCD_GotoXY(uint8_t line, uint8_t symb);          // Go to position (line = 0...1, symb = 0...39)
void LCD_CreateChar(uint8_t pos, uint8_t* symb);      // Create user symbol (pos = 0...7, *symb - array of 8 bytes). Symbol 5x8 dots (3 lsb in each byte are unimportant)
void LCD_putchar(char symb);                          // Write symbol to display
void LCD_putstring(char* str);                        // Write string to display

void    _LCD_Transmit4bit (uint8_t data, uint8_t rs); // Transmit 1 nible (4 bit) to LCD
void    _LCD_WriteCommand(uint8_t command);           // Transmit LCD command
uint8_t _LCD_ReadCommand();                           // Read LCD command (read Busy flag & address counter)
void    _LCD_WriteData(uint8_t data);                 // Transmit LCD data
uint8_t _LCD_ReadData();                              // Read LCD data (CGRAM or DDRAM)
uint8_t _LCD_ReadAddressCounter();                    // Return AC value
uint8_t _LCD_Busy();                                  // Return busy flag

#endif //LCD_I2C_h