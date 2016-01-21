#ifndef F_CPU
#define F_CPU 16000000UL          // Change if use use another frequency
#endif

#include <util/delay.h>
#include "I2C.h"
#include "LCD_I2C.h"

volatile uint8_t lcd_settings = 0;

void LCD_Init() {
  _delay_ms(40);
  _LCD_Transmit4bit((LCD_SETUP | LCD_8BITMODE) >> 4, LCDCOMMAND);
  _delay_us(4100);
  _LCD_Transmit4bit((LCD_SETUP | LCD_8BITMODE) >> 4, LCDCOMMAND);
  _delay_us(100);
  _LCD_Transmit4bit((LCD_SETUP | LCD_8BITMODE) >> 4, LCDCOMMAND);
  while(_LCD_Busy());
  _LCD_Transmit4bit((LCD_SETUP | LCD_4BITMODE) >> 4, LCDCOMMAND);
  _LCD_WriteCommand(LCD_SETUP | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
  _LCD_WriteCommand(LCD_DISPCTRL | LCD_DISPOFF | LCD_CURSUNDEROFF | LCD_CURSBOXOFF);
  _LCD_WriteCommand(LCD_CLRSCR);
  _LCD_WriteCommand(LCD_ENTRYMODE | LCD_COUNTERINC | LCD_DISPSHIFTOFF);
  _LCD_WriteCommand(LCD_GOTO00);
  LCD_BacklightOn();
  _LCD_WriteCommand(LCD_DISPCTRL | LCD_DISPON | LCD_CURSUNDEROFF | LCD_CURSBOXOFF);
}

void LCD_ClrScr() {
  _LCD_WriteCommand(LCD_CLRSCR);
}

void LCD_GotoHome() {
  _LCD_WriteCommand(LCD_GOTO00);
}

void LCD_On() {
  _LCD_WriteCommand( \
    LCD_DISPCTRL | \
    LCD_DISPON | \
    (SETUNDERCURS ? LCD_CURSUNDERON : LCD_CURSUNDEROFF) | \
    (SETBOXCURS ? LCD_CURSBOXON : LCD_CURSBOXOFF));
}

void LCD_Off() {
  _LCD_WriteCommand( \
  LCD_DISPCTRL | \
  LCD_DISPOFF | \
  (SETUNDERCURS ? LCD_CURSUNDERON : LCD_CURSUNDEROFF) | \
  (SETBOXCURS ? LCD_CURSBOXON : LCD_CURSBOXOFF));
}

void LCD_Cursor(uint8_t type) {
  switch(type) {
    case CURS_NONE:
      lcd_settings &= ~(1<<LCD_SETUP_UCURS | 1<<LCD_SETUP_BCURS);
      break;
    case CURS_UNDER:
      lcd_settings |= 1<<LCD_SETUP_UCURS;
      break;
    case CURS_BOX:
      lcd_settings |= 1<<LCD_SETUP_BCURS;
      break;
    case CURS_BOTH:
      lcd_settings |= 1<<LCD_SETUP_UCURS | 1<<LCD_SETUP_BCURS;
      break;
  }
  _LCD_WriteCommand( \
    LCD_DISPCTRL | \
    LCD_DISPON | \
    (SETUNDERCURS ? LCD_CURSUNDERON : LCD_CURSUNDEROFF) | \
    (SETBOXCURS ? LCD_CURSBOXON : LCD_CURSBOXOFF));
}

void LCD_LeftToRight() {
  lcd_settings |= 1<<LCD_SETUP_INCDEC;
  _LCD_WriteCommand(LCD_ENTRYMODE | LCD_COUNTERINC | (SETDISPSHIFT ? LCD_DISPSHIFTON : LCD_DISPSHIFTOFF));
}

void LCD_RightToLeft() {
  lcd_settings &= ~(1<<LCD_SETUP_INCDEC);
  _LCD_WriteCommand(LCD_ENTRYMODE | LCD_COUNTERDEC | (SETDISPSHIFT ? LCD_DISPSHIFTON : LCD_DISPSHIFTOFF));
}

void LCD_ShiftDispOn() {
  lcd_settings |= 1<<LCD_SETUP_DISPSHIFT;
  _LCD_WriteCommand(LCD_ENTRYMODE | (SETINCDEC ? LCD_COUNTERINC : LCD_COUNTERDEC) | LCD_DISPSHIFTON);
}

void LCD_ShiftDispOff() {
  lcd_settings &= ~(1<<LCD_SETUP_DISPSHIFT);
  _LCD_WriteCommand(LCD_ENTRYMODE | (SETINCDEC ? LCD_COUNTERINC : LCD_COUNTERDEC) | LCD_DISPSHIFTOFF);
}

void LCD_MoveCursorLeft() {
  _LCD_WriteCommand(LCD_SHIFTMODE | LCD_CURSMOVE | LCD_MOVELEFT);
}

void LCD_MoveCursorRight() {
  _LCD_WriteCommand(LCD_SHIFTMODE | LCD_CURSMOVE | LCD_MOVERIGHT);
}

void LCD_MoveScreenLeft() {
  _LCD_WriteCommand(LCD_SHIFTMODE | LCD_DISPMOVE | LCD_MOVELEFT);
}

void LCD_MoveScreenRight() {
  _LCD_WriteCommand(LCD_SHIFTMODE | LCD_DISPMOVE | LCD_MOVERIGHT);
}

void LCD_BacklightOn() {
  lcd_settings |= 1<<LCD_SETUP_BL;
  uint8_t msg[2];
  msg[0] = LCD_ADDR<<1 | TWI_WRITE<<TWI_RW_BIT;
  msg[1] = (SETBACKLIGHT ? LCD_BL : 0);
  TWI_Start_Transmition(msg, 2);
}

void LCD_BacklightOff() {
  lcd_settings &= ~(1<<LCD_SETUP_BL);
  uint8_t msg[2];
  msg[0] = LCD_ADDR<<1 | TWI_WRITE<<TWI_RW_BIT;
  msg[1] = (SETBACKLIGHT ? LCD_BL : 0);
  TWI_Start_Transmition(msg, 2);
}

void LCD_GotoXY(uint8_t line, uint8_t symb) {
  uint8_t line_start[] = {0x00, 0x40};
  _LCD_WriteCommand(LCD_DDRAMADDR | (symb + line_start[line]));
}

void LCD_CreateChar(uint8_t pos, uint8_t* symb) {
  uint8_t temp = _LCD_ReadAddressCounter();
  _LCD_WriteCommand(LCD_CGRAMADDR | (pos<<3));
  for(uint8_t i=0; i<8; i++) {
    _LCD_WriteData(*(symb+i));
  }
  _LCD_WriteCommand(LCD_DDRAMADDR | temp);
}

void LCD_putchar(char symb) {
  _LCD_WriteData(symb);
}

void LCD_putstring(char* str) {
  uint8_t i=0;
  while(*(str+i)) {
    LCD_putchar(*(str+i));
    i++;
  }
}

void _LCD_Transmit4bit (uint8_t data, uint8_t rs) {
  uint8_t msg[4];
  msg[0] = LCD_ADDR<<1 | TWI_WRITE<<TWI_RW_BIT;
  msg[1] = data<<4 | (SETBACKLIGHT ? LCD_BL : 0) | (rs ? LCD_RS : 0);
  msg[2] = data<<4 | (SETBACKLIGHT ? LCD_BL : 0) | (rs ? LCD_RS : 0) | LCD_EN;
  msg[3] = data<<4 | (SETBACKLIGHT ? LCD_BL : 0) | (rs ? LCD_RS : 0);
  TWI_Start_Transmition(msg, 4);
}

void _LCD_WriteCommand (uint8_t command) {
  while(_LCD_Busy());
  _LCD_Transmit4bit(command >> 4, LCDCOMMAND);
  _LCD_Transmit4bit(command & 0x0F, LCDCOMMAND);
}

uint8_t _LCD_ReadCommand() {
  uint8_t res;
  uint8_t msg[4];
  msg[0] = LCD_ADDR<<1 | TWI_WRITE<<TWI_RW_BIT;
  msg[1] = (SETBACKLIGHT ? LCD_BL : 0) | LCD_RW;
  msg[2] = (SETBACKLIGHT ? LCD_BL : 0) | LCD_RW | LCD_EN | MASKDB;
  TWI_Start_Transmition(msg, 3);
  msg[0] = LCD_ADDR<<1 | TWI_READ<<TWI_RW_BIT;
  TWI_Start_Transmition(msg, 2);
  TWI_Get_Data(msg, 2);
  res = msg[1] & MASKDB;
  msg[0] = LCD_ADDR<<1 | TWI_WRITE<<TWI_RW_BIT;
  msg[1] = (SETBACKLIGHT ? LCD_BL : 0) | LCD_RW;
  msg[2] = (SETBACKLIGHT ? LCD_BL : 0) | LCD_RW | LCD_EN | MASKDB;
  TWI_Start_Transmition(msg, 3);
  msg[0] = LCD_ADDR<<1 | TWI_READ<<TWI_RW_BIT;
  TWI_Start_Transmition(msg, 2);
  TWI_Get_Data(msg, 2);
  res |= msg[1] >> 4;
  msg[0] = LCD_ADDR<<1 | TWI_WRITE<<TWI_RW_BIT;
  msg[1] = (SETBACKLIGHT ? LCD_BL : 0) | LCD_RW;
  TWI_Start_Transmition(msg, 2);
  return res;
}

void _LCD_WriteData (uint8_t data) {
  while(_LCD_Busy());
  _LCD_Transmit4bit(data >> 4, LCDDATA);
  _LCD_Transmit4bit(data & ~MASKDB, LCDDATA);
}

uint8_t _LCD_ReadData() {
  uint8_t res;
  uint8_t msg[4];
  msg[0] = LCD_ADDR<<1 | TWI_WRITE<<TWI_RW_BIT;
  msg[1] = (SETBACKLIGHT ? LCD_BL : 0) | LCD_RW | LCD_RS;
  msg[2] = (SETBACKLIGHT ? LCD_BL : 0) | LCD_RW | LCD_RS | LCD_EN | MASKDB;
  TWI_Start_Transmition(msg, 3);
  msg[0] = LCD_ADDR<<1 | TWI_READ<<TWI_RW_BIT;
  TWI_Start_Transmition(msg, 2);
  TWI_Get_Data(msg, 2);
  res = msg[1] & MASKDB;
  msg[0] = LCD_ADDR<<1 | TWI_WRITE<<TWI_RW_BIT;
  msg[1] = (SETBACKLIGHT ? LCD_BL : 0) | LCD_RW | LCD_RS;
  msg[2] = (SETBACKLIGHT ? LCD_BL : 0) | LCD_RW | LCD_RS | LCD_EN | MASKDB;
  TWI_Start_Transmition(msg, 3);
  msg[0] = LCD_ADDR<<1 | TWI_READ<<TWI_RW_BIT;
  TWI_Start_Transmition(msg, 2);
  TWI_Get_Data(msg, 2);
  res |= msg[1] >> 4;
  msg[0] = LCD_ADDR<<1 | TWI_WRITE<<TWI_RW_BIT;
  msg[1] = (SETBACKLIGHT ? LCD_BL : 0) | LCD_RW | LCD_RS;
  TWI_Start_Transmition(msg, 2);
  return res;
}

uint8_t _LCD_ReadAddressCounter() {
  return _LCD_ReadCommand() & ~(1<<DB7);
}

uint8_t _LCD_Busy() {
  return _LCD_ReadCommand() & 1<<DB7;
}
