// #####################################################################################################################
// ##### OLED MONOCHROME HEADER FILE ###################################################################################
// #####################################################################################################################
/*
	DELOARTS RESEARCH INC. 2015

	This library is written for an 4-pin I2C monochrome display with a resolution of 128x64 pixels.
	It's tested with the following display:

		- 0,96" I2C OLED, Manufacturer: Heltec-Taobao

	It is not tested with other displays, therefor I can't guarantee that it will work with another device.

	Wiring notes (Wire.h on Arduino UNO/Nano):

		- SDA -> A4
		- SCL -> A5
		- VCC -> 3.3-5V (depending on your display)
		- GND -> Ground

	For further information, please visit:
		- http://deloarts.com/de/arduino/oled_display/
		- http://deloarts.com/en/arduino/oled_display/
*/	
// #####################################################################################################################
// ##### DEFINE CODE ###################################################################################################
// #####################################################################################################################
#ifndef OLED_data_H
#define OLED_data_H

#include <Arduino.h>
#include <Wire.h>

#define OLED_Max_X 16
#define OLED_Max_Y 8

#define OLED_ADDRESS 0x3C
#define I2C_400KHZ 1 // 0 to use default 100Khz, 1 for 400Khz
// Registers
#define COMMAND_MODE 0x80
#define OLED_DATA_MODE 0x40
// Commands
#define COMMAND_CHARGE_PUMP_SETTING	0x8d
#define COMMAND_CHARGE_PUMP_ENABLE 0x14

#define COMMAND_DISPLAY_OFF 0xAE
#define COMMAND_DISPLAY_ON 0xAF
#define COMMAND_BLACK_BACKGROUND 0xA6
#define COMMAND_WHITE_BACKGROUND 0xA7
#define COMMAND_SET_BRIGHTNESS 0x81

#define COMMAND_MIRROR_VERTICAL	0xA0 | 0x1
#define COMMAND_MIRROR_HORIZONTAL 0xC8

#define HORIZONTAL_ADDRESSING 0x00
#define PAGE_ADDRESSING	0x02
// #####################################################################################################################
// ##### CLASS OLED ####################################################################################################
// #####################################################################################################################
class Display
{
	public:
		byte addressingMode;
		void initialize();
		void sendCommand(byte Command);
		void sendData(byte Data);
		void printChar(byte Char);
		void printChar(byte Char, byte PosX, byte PosY); 
		void printString(const byte *Data, byte PosX = 255, byte PosY = 255);
		void printNum(signed char Data);
		void printNum(char Data, byte PosX, byte PosY);
		void drawBitmap(const byte *Bitmaparray, byte PosX, byte PosY, byte Width, byte Height);
		void clearDisplay();
		void setCursor(byte PosX, byte PosY);
		void setBlackBackground();
		void setWhiteBackground();
		void setDisplayOff();
		void setDisplayOn();
		void setPageMode();
		void setHorizontalMode();
		void setBrightness(byte Brightness);
		void rotateDisplay180();
		void setFontSize(byte f);
		byte px, py, fontsize;
		boolean scroll = false;
};

extern Display oled;
#endif
// #####################################################################################################################
// ##### END OF CODE ###################################################################################################
// #####################################################################################################################
