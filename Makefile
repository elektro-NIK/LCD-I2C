# Project name. Binary file will be name (TARG).hex
TARG=main

CC = avr-gcc
OBJCOPY = avr-objcopy

# Files in the project
SRCS = example.c I2C.c LCD_I2C.c

OBJS = $(SRCS:.c=.o)

# MCU type
MCU=atmega328p

# Flags for compiler
CFLAGS = -mmcu=$(MCU) -Wall -g -Os -lm  -mcall-prologues -std=c99
LDFLAGS = -mmcu=$(MCU)  -Wall -g -Os

all: $(TARG)

$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@.elf  $(OBJS) -lm
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram  $@.elf $@.hex

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(SRCS:.c=.elf) $(OBJS)

prog:
	avrdude -c arduino -P /dev/ttyUSB0 -p $(MCU) -b 115200 -D -U flash:w:$(TARG).hex:i
