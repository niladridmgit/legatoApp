/*
 * lcd.h
 *
 *  Created on: Dec 2, 2018
 *      Author: niladri
 */

#ifndef LCDSERVICE_H_
#define LCDSERVICE_H_

#include "legato.h"
#include "interfaces.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

#define CLEAR_DISPLAY	0x01
#define LCD_TYPE_16X2   0x02
#define LCD_TYPE_20X4   0x03


#define BINARY_FORMAT  " %c  %c  %c  %c  %c  %c  %c  %c\n"
#define BYTE_TO_BINARY(byte) \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

static int i2cFile;
static uint8_t LCD_MAXCHAR_LINE = 20;

static const char I2C_BUS[12]= "/dev/i2c-0" // I2C bus device
static char i2cBus[100]="\0";

void lcd_writeString(const char *ptrChr, unsigned int str_size, unsigned char line);
void i2c_send_byte(unsigned char data);
void i2c_start(uint8_t i2c_address);
void i2c_stop();
void enableI2cBus(uint8_t i2c_address);
void remoteSystemCommand(const char* textPtr);
void lcd_clearDisplay();




#endif /* LCD_H_ */
