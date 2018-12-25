mangOH RED i2c LCD service for pcf8574 lcd 16x2, 20x4

![alt text](https://github.com/niladridmgit/legatoApp/blob/master/lcdService/i2clcd.png)

lcdservice_lcdCmd:

CLEAR_DISPLAY	0x01

LCD_TYPE_16X2   0x02

LCD_TYPE_20X4   0x03

in the code I have already enabled i2c lines in RPI header by sending below command to the i2c expander which address is 0x71
 >i2cset -y 0 0x71 0x08

Use below apis to play with i2c LCD

if your pcf8574 i2c address is 0x27 

1. lcdservice_i2cInit(0x27,"/dev/i2c-0");

2. lcdservice_lcdCmd(0x27,0x01); // CLEAR DISPLAY

3. lcdservice_lcdCmd(0x27,0x02); // LCD_TYPE_16X2 

4. lcdservice_lcdWriteString(1, "Hello"); // write string in 1st line

5. lcdservice_lcdWriteString(1, "World"); // write string in 2nd line
