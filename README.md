# OLED Library

This is a simple, lightweight library for an 0,96" monochrome organic liquid crystal display (128x64 pixels)
It has been tested with the following device:
	- 0,96" I2C OLED, Manufacturer: Heltec-Taobao
	- Teensy 3.2 

Used for a small [https://www.untergeek.de/2017/09/v2-0-a-teensy-based-midi-controller/](MIDI controller project).

If this library is not for you, you may try the Adafruit_SSD1306 library - does the same things more consistently, 

Forked from a project by deloarts because I built my own character enlargement routines into it - using a small and simple 8x8 pixel font, it can display nice, readable characters in three sizes 1x, 2x, and 3x (i.e. with up to 24x24px). 

TODO: 
- Replace CP437 font with a CP1252 font and...
- ...add println() functionality to interpret the most used utf-8 chars (this is the way the Arduino IDE stores umlauts)
- Clean up and all that

My blog can be found at https://www.untergeek.de

For further information on deloarts' original project, please visit:
- Blog:	https://deloarts.wordpress.com/2015/06/17/oled-i%C2%B2c-128x64-monochrome-library/
- Website, english version: http://deloarts.com/en/arduino/oled_display/
- Website, german version: http://deloarts.com/de/arduino/oled_display/ 

### How to use it

- Download all files as ZIP
- In the Arduino/Teensyduino IDE, select "Sketch/Add library.../Select ZIP". Add the library
- Add <OLED_I2C_128x64_Monochrome.h> to your sketch
- The library creates an object called oled - the first thing to do is to call the method oled.initialize();

#### oled.methods - The commands

		void initialize();
		void sendCommand(byte Command);
		void sendData(byte Data);
		void printChar(byte Char);
		void printChar(byte Char, byte PosX, byte PosY); 
		void printString(const byte *Data, byte PosX = 255, byte PosY = 255);
		void printNum(signed char Data);
		void printNum(char Data, byte PosX, byte PosY);
		void printHex(byte Data);
		void drawBitmap(const byte *Bitmaparray, byte PosX, byte PosY, byte Width, byte Height);
		void clearDisplay();
		void setPos(byte PosX, byte PosY);
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
		void invert(boolean f); 
		byte px, py, fontsize;

#### The additional functions of this library

Compared with deloarts' project, these additional methods exist:

- oled.fontsize = f with f in {1, 2, 3}
- oled.invert(true) to print black characters on white ground
- oled.printNum(byte) - print three-digit values from -99 to 127 -> will rename that to oled.printByte
- oled.printHex(byte) - print hexadecimal values
- oled.px and oled.py for the cursor position, can be set by oled.setCursor()
- oled.setPos is an internal method to set the graphics cursor
