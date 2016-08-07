// #####################################################################################################################
// ##### OLED CPP FILE #################################################################################################
// #####################################################################################################################
#include "OLED_I2C_128x64_Monochrome.h"
#include "OLED_I2C_128x64_Monochrome_Font.h"
#include <Wire.h>
#include <avr/pgmspace.h>

#define USE_REGULATOR
#define OLED_I2C_128x64_Monochrome Display

// #####################################################################################################################
// ##### INITZIALIZE ###################################################################################################
// #####################################################################################################################
void Display::initialize()
{
	Wire.begin();
	// upgrade to 400KHz! (only use when your other i2c device support this speed)
	if (I2C_400KHZ)
	{
		// save I2C bitrate (default 100Khz)
		byte twbrbackup = TWBR;
		TWBR = 12;
	}
	#ifdef USE_REGULATOR
	// Charge pump regulator
    sendCommand(COMMAND_CHARGE_PUMP_SETTING);
	sendCommand(COMMAND_CHARGE_PUMP_ENABLE);
	#endif
    setDisplayOff();
    setBlackBackground();
	setPageMode();
	clearDisplay();
    setDisplayOn();
}
// #####################################################################################################################
// ##### SEND COMMAND ##################################################################################################
// #####################################################################################################################
void Display::sendCommand(byte Command)
{
	Wire.beginTransmission(OLED_ADDRESS); // begin transmitting
	Wire.write(COMMAND_MODE); // data mode
	Wire.write(Command); // send command
	Wire.endTransmission(); // stop transmitting
}
// #####################################################################################################################
// ##### SEND DATA #####################################################################################################
// #####################################################################################################################
void Display::sendData(byte Data)
{
	Wire.beginTransmission(OLED_ADDRESS);
	Wire.write(OLED_DATA_MODE);
	Wire.write(Data);
	Wire.endTransmission();
}
// #####################################################################################################################
// ##### PRINT CHARACTER ###############################################################################################
// #####################################################################################################################
void Display::printChar(char Char, byte PosX, byte PosY)
{
	if (PosX < OLED_Max_X)
	{
		setCursor(PosX, PosY);
	}
	// Ignore unused ASCCII characters
    if (Char < 32 || Char > 127)
    {
		Char = '?'; // ?: characters that can't be displayed
    }
    for(byte i = 0; i < 8; i++)
    {
       sendData(pgm_read_byte(&BasicFont[Char - 32][i])); // font array starts at 0, ASCII starts at 32.
    }
}
// #####################################################################################################################
// ##### PRINT STRING ##################################################################################################
// #####################################################################################################################
void Display::printString(const char *Data, byte PosX, byte PosY)
{
	if (PosX < OLED_Max_X)
	{
		setCursor(PosX, PosY);
	}
	byte i = 0;
    while(Data[i] && i < OLED_Max_X)
    {
		printChar(Data[i++]);
	}
}
// #####################################################################################################################
// ##### PRINT NUMBER (NON FLOAT VALUE) ################################################################################
// #####################################################################################################################
byte Display::printNumber(long Data, byte PosX, byte PosY)
{
	if (PosX < OLED_Max_X)
	{
		setCursor(PosX, PosY);
	}
	byte Buffer[10] = "";
	byte IntPart = 0;
	byte FloatPart = 0;

	if (Data < 0)
	{
		FloatPart++;
		printChar('-');
		Data = -Data;
	}
	else if (Data == 0)
	{
		FloatPart++;
		printChar('0');
		return FloatPart;
	}
	while (Data > 0)
	{
		Buffer[IntPart++] = Data % 10;
		Data /= 10;
	}

	FloatPart += IntPart;
	for(; IntPart > 0; IntPart--)
	{
		printChar('0' + Buffer[IntPart - 1]);
	}
	return FloatPart;
}
// #####################################################################################################################
// ##### PRINT NUMBER (FLOAT VALUE) ####################################################################################
// #####################################################################################################################
byte Display::printNumber(float Data, byte Precision, byte PosX, byte PosY)
{
	if (PosX < OLED_Max_X)
	{
		setCursor(PosX, PosY);
	}
	byte NumInt = 0; // Precision: 6 maximum
	byte NumFrac = 0;
	byte NumExtra = 0;

	long IntPart = Data; // get the integer part
	float FloatPart  = Data - IntPart; // get the fractional part

	if (IntPart == 0 && FloatPart < 0.0)
	{
		printChar('-');
		NumExtra++;
		printChar('0');
		NumExtra++;
		FloatPart *= -1;
	}
	else if (IntPart < 0 && FloatPart < 0.0)
	{
		NumInt = printNumber(IntPart); // count how many digits in integer part
		FloatPart *= -1;
	}
	else
	{
		NumInt = printNumber(IntPart); // count how many digits in integer part
	}

	if (FloatPart > 0.0) // only when fractional part > 0, decimal point is shown
	{
		printChar('.');
		NumExtra++;
		long FloatPartShift = 1;

		if (NumInt + Precision > 8)
		{
			Precision = 8 - NumInt;
		}
		for (byte I = 0; I < Precision; I++)
		{
			FloatPartShift *= 10;
		}
		NumFrac = printNumber((long)(FloatPart * FloatPartShift)); // count how many digits in fractional part
	}
	return NumInt + NumFrac + NumExtra;
}
// #####################################################################################################################
// ##### DRAW BITMAP ###################################################################################################
// #####################################################################################################################
void Display::drawBitmap(const byte *Bitmaparray, byte PosX, byte PosY, byte Width, byte Height)
{
	// max width = 16
	// max height = 8
	setCursor(PosX, PosY);

	byte Column = 0;
	for(int i = 0; i < Width * 8 * Height; i++)
	{
		sendData(pgm_read_byte(&Bitmaparray[i]));
		if (++Column == Width * 8)
		{
			Column = 0;
			setCursor(PosX, ++PosY);
		}
	}
}
// #####################################################################################################################
// ##### SET CURSOR ####################################################################################################
// #####################################################################################################################
void Display::setCursor(byte PosX, byte PosY)
{
	// Y - 1 unit = 1 page (8 pixel rows)
	// X - 1 unit = 8 pixel columns
    sendCommand(0x00 + (8 * PosX & 0x0F)); // set column lower address
    sendCommand(0x10 + ((8 * PosX >> 4) & 0x0F)); // set column higher address
	sendCommand(0xB0 + PosY); // set page address
}
// #####################################################################################################################
// ##### CLEAR DISPLAY #################################################################################################
// #####################################################################################################################
void Display::clearDisplay()
{
	for(byte X = 0; X < 8; X++)
	{
		setCursor(0, X);
		for(byte Y = 0; Y < 128; Y++) // clear all columns
		{
			sendData(0);
		}
	}
	setCursor(0, 0);
}
// #####################################################################################################################
// ##### ROTATE DISPLAY 180 DEGREES ####################################################################################
// #####################################################################################################################
void Display::rotateDisplay180()
{
	sendCommand(COMMAND_MIRROR_VERTICAL);
	sendCommand(COMMAND_MIRROR_HORIZONTAL);
}
// #####################################################################################################################
// ##### WHITE BACKGROUND ##############################################################################################
// #####################################################################################################################
void Display::setWhiteBackground()
{
	sendCommand(COMMAND_WHITE_BACKGROUND);
}
// #####################################################################################################################
// ##### BLACK BACKGROUND ##############################################################################################
// #####################################################################################################################
void Display::setBlackBackground()
{
	sendCommand(COMMAND_BLACK_BACKGROUND);
}
// #####################################################################################################################
// ##### DISPLAY POWER OFF #############################################################################################
// #####################################################################################################################
void Display::setDisplayOff()
{
	sendCommand(COMMAND_DISPLAY_OFF);
}
// #####################################################################################################################
// ##### DISPLAY POWER ON ##############################################################################################
// #####################################################################################################################
void Display::setDisplayOn()
{
	sendCommand(COMMAND_DISPLAY_ON);
}
// #####################################################################################################################
// ##### BRIGHTNESS ####################################################################################################
// #####################################################################################################################
void Display::setBrightness(byte Brightness)
{
	sendCommand(COMMAND_SET_BRIGHTNESS);
	sendCommand(Brightness);
}
// #####################################################################################################################
// ##### PAGE MODE #####################################################################################################
// #####################################################################################################################
void Display::setPageMode()
{
	addressingMode = PAGE_ADDRESSING;
	sendCommand(0x20); //set addressing mode
	sendCommand(PAGE_ADDRESSING); //set page addressing mode
}
// #####################################################################################################################
// ##### HORIZONTAL MODE ###############################################################################################
// #####################################################################################################################
void Display::setHorizontalMode()
{
	addressingMode = HORIZONTAL_ADDRESSING;
	sendCommand(0x20); // set addressing mode
	sendCommand(HORIZONTAL_ADDRESSING); // set page addressing mode
}
// #####################################################################################################################
// ##### PREINSTANTIAE OBJECTS #########################################################################################
// #####################################################################################################################
Display lcd;
// #####################################################################################################################
// ##### END OF CODE ###################################################################################################
// #####################################################################################################################