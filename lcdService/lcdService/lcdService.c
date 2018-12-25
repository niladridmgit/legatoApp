/*
 * lcd.c
 *
 *  Created on: Dec 2, 2018
 *      Author: niladri
 */

#include "lcdservice.h"

void remoteSystemCommand(const char* textPtr)
{

	FILE *fp;
	char path[100];

	fp = popen(textPtr, "r");

	if (fp == NULL)
	{
		LE_INFO("Failed to run command\n");
	}
	else
	{
		/* Read the output a line at a time - output it. */
		while (fgets(path, sizeof(path) - 1, fp) != NULL)
		{
			LE_INFO("%s", path);
		}

		LE_INFO("Command Execution Complete");
		/* close */
		pclose(fp);
	}
}

void lcd_clearDisplay()
{
	//D7 D6 D5 D4 BL EN RW RS//
	//Cursor Home//
	i2c_send_byte(0b00001100); //
	i2c_send_byte(0b00001000); // D7-D4=0
	i2c_send_byte(0b00011100); //
	i2c_send_byte(0b00011000); // D0=display_clear
	//usleep(40); // wait 40usec

}

void lcd_writeString(const char *ptrChr, unsigned int str_size, unsigned char line)
{
	unsigned int char_cnt=0;

	//Line Selection//
	/*
	 * 	LCD_LINE_1 = 0x80 # line 1
		LCD_LINE_2 = 0xC0 # line 2
		LCD_LINE_3 = 0x94 # line 3 // for 16x4
		LCD_LINE_4 = 0xD4 # line 4 // for 16x4
	 */

	//D7 D6 D5 D4 BL EN RW RS//
	if(line==1)
	{
		i2c_send_byte(0b10001100); // 0x8
		i2c_send_byte(0b10001000); //
		i2c_send_byte(0b00001100); // 0x0
		i2c_send_byte(0b00001000); //
	}
	else if(line==2)
	{
		i2c_send_byte(0b11001100); // 0xC
		i2c_send_byte(0b11001000); //
		i2c_send_byte(0b00001100); // 0x0
		i2c_send_byte(0b00001000); //
	}
	else if(line==3)
	{
		i2c_send_byte(0b10011100); // 0x9
		i2c_send_byte(0b10011000); //
		i2c_send_byte(0b01001100); // 0x4
		i2c_send_byte(0b01001000); //
	}
	else if(line==4)
	{
		i2c_send_byte(0b11011100); // 0xD
		i2c_send_byte(0b11011000); //
		i2c_send_byte(0b01001100); // 0x4
		i2c_send_byte(0b01001000); //
	}
	else
		return;

	usleep(40); // wait 40usec

	for(char_cnt=0; char_cnt<str_size; char_cnt++ )
	{
	   unsigned char a = *(ptrChr+char_cnt);
	   unsigned char n0 = a & 0xf0;
	   unsigned char n1 = n0;
	   n0 = n0 | 0x0D;
	   n1 = n1 | 0x09;

	   unsigned char n2 = (a << 4) & 0xf0;
	   unsigned char n3 = n2;
	   n2= n2 | 0x0D;
	   n3= n3 | 0x09;

	   i2c_send_byte(n0);// Higher Nibble
	   i2c_send_byte(n1);
	   i2c_send_byte(n2);// Lower Nibble
	   i2c_send_byte(n3);
	}


}

void i2c_start(uint8_t i2c_address) {
   if((i2cFile = open(i2cBus, O_RDWR)) < 0) {
      LE_INFO("Error failed to open I2C bus [%s].\n", i2cBus);
      exit(-1);
   }
   // set the I2C slave address for all subsequent I2C device transfers
   if (ioctl(i2cFile, I2C_SLAVE, i2c_address) < 0) {
      LE_INFO("Error failed to set I2C address [%d].\n", i2c_address);
      exit(-1);
   }
}

void i2c_stop() { close(i2cFile); }

void i2c_send_byte(unsigned char data) {
   unsigned char byte[1];
   byte[0] = data;
   //LE_INFO(BINARY_FORMAT, BYTE_TO_BINARY(byte[0]));
   write(i2cFile, byte, sizeof(byte));
   /* -------------------------------------------------------------------- *
    * Below wait creates 1msec delay, needed by display to catch commands  *
    * -------------------------------------------------------------------- */
   usleep(1000);
}

void enableI2cBus(uint8_t i2c_address) {
   //mangOH red Enable i2c expander address 0x71//
   remoteSystemCommand("/usr/sbin/i2cset -y 0 0x71 0x08");
   sleep(3);

   i2c_start(i2c_address);

   //Initialize the display, using the 4-bit mode initialization sequence //

   LE_INFO("Init Start:\n");
   LE_INFO("D7 D6 D5 D4 BL EN RW RS\n");

   usleep(15000);             // wait 15msec
   i2c_send_byte(0b00110100); // D7=0, D6=0, D5=1, D4=1, RS,RW=0 EN=1
   i2c_send_byte(0b00110000); // D7=0, D6=0, D5=1, D4=1, RS,RW=0 EN=0
   usleep(4100);              // wait 4.1msec
   i2c_send_byte(0b00110100); //
   i2c_send_byte(0b00110000); // same
   usleep(100);               // wait 100usec
   i2c_send_byte(0b00110100); //
   i2c_send_byte(0b00110000); // 8-bit mode init complete
   usleep(4100);              // wait 4.1msec
   i2c_send_byte(0b00100100); //
   i2c_send_byte(0b00100000); // switched now to 4-bit mode

   //4-bit mode initialization complete. Now configuring the function set//

   usleep(40);                // wait 40usec
   i2c_send_byte(0b00100100); //
   i2c_send_byte(0b00100000); // keep 4-bit mode
   i2c_send_byte(0b10000100); //
   i2c_send_byte(0b10000000); // D3=2lines, D2=char5x8


    //turn display off//                                                *

   usleep(40);                // wait 40usec
   i2c_send_byte(0b00000100); //
   i2c_send_byte(0b00000000); // D7-D4=0
   i2c_send_byte(0b10000100); //
   i2c_send_byte(0b10000000); // D3=1 D2=display_off, D1=cursor_off, D0=cursor_blink

   //Display clear, cursor home//
   usleep(40);                // wait 40usec
   i2c_send_byte(0b00000100); //
   i2c_send_byte(0b00000000); // D7-D4=0
   i2c_send_byte(0b00010100); //
   i2c_send_byte(0b00010000); // D0=display_clear


   //Set cursor direction //
   usleep(40);                // wait 40usec
   i2c_send_byte(0b00000100); //
   i2c_send_byte(0b00000000); // D7-D4=0
   i2c_send_byte(0b01100100); //
   i2c_send_byte(0b01100000); // print left to right

   //Turn on the display //

   usleep(40);                // wait 40usec
   i2c_send_byte(0b00000100); //
   i2c_send_byte(0b00000000); // D7-D4=0
   i2c_send_byte(0b11000100); //
   i2c_send_byte(0b11000000); // D3=1 D2=display_on, D1=cursor_on, D0=cursor_blink

   LE_INFO("Init End.\n");
   sleep(1);
}

le_result_t lcdservice_i2cInit(uint8_t i2cSlave, const char* i2cDevice)
{

	LE_INFO("I2C-Slave: %d, device: %s",i2cSlave,i2cDevice);
	
	if(strlen(i2cDevice)>0)
	{
		strncpy(i2cBus,i2cDevice,strlen(i2cDevice));
	}
	else
	{
		strncpy(i2cBus,I2C_BUS,strlen(I2C_BUS));
	}
	

	enableI2cBus(i2cSlave);

	return LE_OK;

}

le_result_t lcdservice_lcdCmd(uint8_t i2cSlave, uint8_t cmd)
{
	LE_INFO("I2C-Slave: %d, CMD: %.2X",i2cSlave,cmd);

	if(cmd==CLEAR_DISPLAY)
    	lcd_clearDisplay();
	else if(cmd==LCD_TYPE_16X2)
		LCD_MAXCHAR_LINE=16;
	else if(cmd==LCD_TYPE_20X4)
		LCD_MAXCHAR_LINE=20;

	return LE_OK;

}

le_result_t lcdservice_lcdWriteString(uint8_t line, const char* writeBytes)
{
	//LE_INFO("LCD line: %d, String: %s",line,writeBytes);
    unsigned int str_len =strlen(writeBytes);
    if(str_len>LCD_MAXCHAR_LINE)
    	str_len=LCD_MAXCHAR_LINE;
	lcd_writeString(writeBytes,str_len,line);

	return LE_OK;

}


/*static char text[31]="NILADRI DAS MAHAPATRA, TESTING ";

void scrollTimerHandler(le_timer_Ref_t scrollTimer_Timer)
{

	lcdservice_lcdWriteString(2, text);
	uint8_t i=0;
	char tmp=text[0];
	for(i=0;i<sizeof(text)-1;i++)
	{
		text[i]=text[i+1];
	}
	text[i]=tmp;
}*/

//static le_timer_Ref_t scrollTimer = NULL;

COMPONENT_INIT
{
	LE_INFO("i2c LCD Service!");

	/*lcdservice_i2cInit(0x27,"modbus");
	lcdservice_lcdCmd(0x27,0x01);
	

	scrollTimer = le_timer_Create("Scrolling_Timer");
	LE_ASSERT_OK(le_timer_SetHandler(scrollTimer, &scrollTimerHandler));
	LE_ASSERT_OK(le_timer_SetMsInterval(scrollTimer, 1000));
	LE_ASSERT_OK(le_timer_SetRepeat(scrollTimer, 0));
	LE_ASSERT_OK(le_timer_Start(scrollTimer));*/
}
