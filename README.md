# OLED Library

This is a simple, lightweight library for an 0,96" monochrome organic liquid crystal display (128x64 pixels)
It has been tested with the following device:
	- 0,96" I2C OLED, Manufacturer: Heltec-Taobao
	- Teensy 3.2 

If this library is not for you, you may try the Adafruit_SSD1306 library - does the same things more consistently, 

Forked from a project by deloarts because I built my own character enlargement routines into it - using a small and simple 8x8 pixel font, it can display nice, readable characters in three sizes 1x, 2x, and 3x (i.e. with up to 24x24px). 

TODO: 
- Fix byte/char inconsistency to use with cp437 character set
- Replace CP437 font with a CP1252 font and...
- ...add println() functionality to interpret the most used utf-8 chars (this is the way the Arduino IDE stores umlauts)
- Clean up and all that

My blog can be found at https://www.untergeek.de

For further information on deloarts' original project, please visit:
- Blog:	https://deloarts.wordpress.com/2015/06/17/oled-i%C2%B2c-128x64-monochrome-library/
- Website, english version: http://deloarts.com/en/arduino/oled_display/
- Website, german version: http://deloarts.com/de/arduino/oled_display/ 

##### Commands
The following commands are implemented:

- initialize
- sendCommand       
- sendData          
- printChar         
- printString       
- printNumber       
- drawBitmap        
- setCursor         
- clearDisplay      
- setBlackBackground
- setWhiteBackground
- setDisplayOff     
- setDisplayOn      
- setPageMode       
- setHorizontalMode 
- setBrightness     
- rotateDisplay180  
