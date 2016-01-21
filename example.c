#include <avr/interrupt.h>
#include "I2C.h"
#include "LCD_I2C.h"


int main() {
  TWI_Init();
  sei();
  LCD_Init();
  uint8_t A[] = {
    0b10101,
    0b10101,
    0b10101,
    0b10101,
    0b11011,
    0b10101,
    0b11111,
    0b00100
  };
  LCD_CreateChar(0, A);
  LCD_GotoXY(0,0);
  LCD_putchar(0);
  LCD_GotoXY(1,0);
  LCD_putstring("Hello, world!");
  while (1) {}
}