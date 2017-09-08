// #####################################################################################################################
// ##### OLED CPP FILE #################################################################################################
// #####################################################################################################################
#include "OLED_I2C_128x64_Monochrome.h"
#include "8x8_vertikal_LSB_2.h"
#include <Wire.h>
#include <avr/pgmspace.h>

#define USE_REGULATOR
#define OLED_I2C_128x64_Monochrome Display

// #####################################################################################################################
// ##### INITIALIZE ###################################################################################################
// #####################################################################################################################
void Display::initialize()
{
	Wire.begin();
	// upgrade to 400KHz! (only use when your other i2c device support this speed)
	if (I2C_400KHZ)
	{
		// save I2C bitrate (default 100Khz)
//		byte twbrbackup = TWBR;
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

// Enhanced byteacter print routine. Increment cursor (and display line, if necessary) - and use the old Scale2x 
// and Scale3x algorithms to produce nice-looking larger characters. 
// More on the scale2x/scale3x algorithms unter www.scale2x.it - it is so simple and lightweight that I could easily
// adapt it.

void Display::printChar(byte Char, byte PosX, byte PosY) 
{	
	px = PosX % OLED_Max_X ;
	py = PosY % OLED_Max_Y ;
	setCursor(px,py);
	printChar(Char);
}

void Display::printChar(byte Char)
{
// ###################################
//  fontsize=1 ==> Standard 8x8 chars
// ####################################
	if (fontsize == 1)
	{	
		setCursor(px,py);
		for(byte i = 0; i < 8; i++)
    {
	   sendData(pgm_read_byte(&BasicFont[Char][i])); 
			//Simple block copy operation, aided by the display. 
		}
	}
// ###################################
//  fontsize=2 ==> scale2x to 16x16
// ###################################
	if (fontsize == 2)
	{
// Call scale2x algorithm to display readable 16x16 font
// for a single 8x8 bit block (=8 bytes)
// Result is a (2x8 bytes)x2 array of those blocks
		byte scale2x[16][2];
		for (byte x=0;x<8;x++)	// Loop through the columns
		{
		//init the four target bytes in the output array
			scale2x[x*2][0]=0;
			scale2x[x*2][1]=0;
			scale2x[x*2+1][0]=0;
			scale2x[x*2+1][1]=0;
		// Read the source data and the neighbouring bytes
			byte col = pgm_read_byte(&BasicFont[Char][x]);
			byte prev = x>0 ? pgm_read_byte(&BasicFont[Char][x-1]) : 0;
			byte next = x<7 ? pgm_read_byte(&BasicFont[Char][x+1]) : 0; // if out of bounds, take 0
		// Loop through the lines (i.e. the bits of the source byte)
			for(byte y=0;y<8;y++)
			{
				byte h_mask = 0x01 << y; //Bit mask for reading the single pixels from the column byte
				// Store the bits as named in the scale2x algorithm
				boolean e = (col & h_mask) != 0;
				boolean b = ((col << 1) & h_mask) != 0; // Bits next to the masked bit
				boolean h = ((col >> 1) & h_mask) != 0; // shifted just under the mask
				boolean d  = (prev & h_mask) != 0;	// Bits from left and right neighbor
				boolean f = (next & h_mask) != 0;
	
				// Target byte address
				byte y2x_b = y / 4; // lower or higher byte?

				// This is where the scale2x magic happens: 
				if (b != h && d != f)		// scale2x only if surrounding pixels differ
				{
					h_mask = 0x01 << ((y & 0x03) * 2); 	// 1 bit shifted by 0, 2, 4, 6
					if (d == b ? d : e) // Write pixel e0 but only if relevant bit is set
						{ scale2x[x*2][y2x_b] = scale2x[x*2][y2x_b] | h_mask; }
					if (b == f ? f : e) // Write pixel e1 but only if relevant bit is set
						{ scale2x[x*2+1][y2x_b] = scale2x[x*2+1][y2x_b] | h_mask; }
					h_mask = h_mask << 1; // Move on to odd-line pixels
					if (d == h ? d : e) // Write pixel e2 but only if relevant bit is set
						{ scale2x[x*2][y2x_b] = scale2x[x*2][y2x_b] | h_mask; }
					if (h == f ? f : e) // Write pixel e3 but only if relevant bit is set
						{ scale2x[x*2+1][y2x_b] = scale2x[x*2+1][y2x_b] | h_mask; }
				} else {
					if (e)	//if pixel is set, write 2x2px block to output
					{
						h_mask = 0x03 << ((y & 0x03) * 2) ; // 2 bits shifted by 0, 2, 4, 6
						scale2x[x*2][y2x_b] = scale2x[x*2][y2x_b] | h_mask;
						scale2x[x*2+1][y2x_b] = scale2x[x*2+1][y2x_b] | h_mask;
					} /* if (e) */		
				} /* if scale2x else */ 
			} /* for y */ 
		} /* for x */
		//Now copy prepared bytes to display
		setCursor(px, py);
		for (byte x = 0;x<16;x++)
		{ 
			sendData(scale2x[x][0]); 
		}
		setCursor(px, py+1);
		for (byte x = 0;x<16;x++)
		{ 
			sendData(scale2x[x][1]);
		}
	} 
// ###################################
//  fontsize==3 ==> scale3x to 24x24
// ###################################
	if (fontsize == 3)
	{ 
// Call scale3x algorithm to convert 8x8 pixel font to 24x24px
// Source is an 8-byte block, result is a (3x8bit)x3 array
		byte scale3x[24][3];
		for (byte x=0;x<8;x++)	// Loop through the source columns
		{
			//init target bytes in the output array
			byte xxx=3 * x;
			for(byte i =0;i<3;i++)
			{
				scale3x[xxx][i]=0;
				scale3x[xxx+1][i]=0;
				scale3x[xxx+2][i]=0;
			}
			// Load the data byte for this column, and its direct neighbours
			byte col = pgm_read_byte(&BasicFont[Char][x]);
			byte prev = x>0 ? pgm_read_byte(&BasicFont[Char][x-1]) : 0;
			byte next = x<7 ? pgm_read_byte(&BasicFont[Char][x+1]) : 0; // if out of bounds, take 0
			// Step through the lines.
			for(byte y=0;y<8;y++)
			{
				byte h_mask = 0x01 << y; //Bit mask for reading the single pixels 
									// Store the bits as named in the scale3x algorithm
				boolean a = ((prev << 1) & h_mask) != 0; 
				boolean b = ((col << 1) & h_mask) != 0; // Bits next to the masked bit
				boolean c = ((next << 1) & h_mask) != 0;
				boolean d  = (prev & h_mask) != 0;	// Bits from left and right neighbor
				boolean e = (col & h_mask) != 0;
				boolean f = (next & h_mask) != 0;
				boolean g = ((prev >> 1) & h_mask) != 0;
				boolean h = ((col >> 1) & h_mask) != 0; // shifted just under the mask
				boolean i = ((next >> 1) & h_mask) != 0;
			// Target addresses
			// 3 bits make for uncomfortable shifting operations, 
			// prepare addressing mode
				byte t_lo = (y * 3) % 8;
				byte t_hi = (y * 3) / 8; 
	
				if (b != h && d != f)		// scale3x only if surrounding pixels differ
				{
					h_mask = 0x01 << t_lo; 	// 1 bit shifted to posn. */
					if (d == b ? d : e) // Write pixel e0 but only if relevant bit is set
						scale3x[xxx][t_hi] = scale3x[xxx][t_hi] | h_mask;
					if ((d == b && e != c) || (b ==f && e != a) ? b : e) // Pixel e1
						scale3x[xxx+1][t_hi] = scale3x[xxx+1][t_hi] | h_mask;
					if (b == f ? f : e) // Pixel e2
						scale3x[xxx+2][t_hi] = scale3x[xxx+2][t_hi] | h_mask;
					// Next line. 
					if (++t_lo > 7) 	// Increment mask index; adjust for boundary
					{
						t_lo = 0;
						t_hi++;
					}
					h_mask = 0x01 << t_lo;  // Create bit mask
					if ((d == b && e != g) || (d == h && e != a) ? d : e)	// Pixel e3
						scale3x[xxx][t_hi] = scale3x[xxx][t_hi] | h_mask;
					if (e) 	// Pixel e4
						scale3x[xxx+1][t_hi] = scale3x[xxx+1][t_hi] | h_mask;
					if ((b == f && e != i) || (h == f && e != c) ? f : e) // Pixel e5
						scale3x[xxx+2][t_hi] = scale3x[xxx+2][t_hi] | h_mask;
					// Next line.
					if (++t_lo > 7) 	// Increment mask index; adjust for boundary
					{
						t_lo = 0;
						t_hi++;
					}					
					h_mask = 0x01 << t_lo;	// Create bit mask
					if (d == h ? d : e)	// Pixel e6
						scale3x[xxx][t_hi] = scale3x[xxx][t_hi] | h_mask;
					if ((d == h && e != i) || (h == f && e != g) ? h : e) 	// Pixel e7
						scale3x[xxx+1][t_hi] = scale3x[xxx+1][t_hi] | h_mask;
					if (h == f ? f : e) // Pixel e8
						scale3x[xxx+2][t_hi] = scale3x[xxx+2][t_hi] | h_mask;
				} else { // 
					if (e)	//if pixel is set, write 3x3px block to output
					{
						for (byte i=3;i>0;i--)
						{
							h_mask = 0x01 << t_lo ; // 2 bits shifted by 0, 2, 4, 6
							for (byte k=0;k<3;k++)
								scale3x[xxx+k][t_hi] = scale3x[xxx+k][t_hi] | h_mask;
							if (++t_lo > 7) 	// Increment mask index; adjust for boundary
							{
								t_lo = 0;
								t_hi++;
							}							
						} // for i
					} /* if (e) */		
				} /* if scale3x else */ 
			} /* for y */ 
		} /* for x */
		//Now copy prepared bytes to display
		for (byte y = 0;y<3;y++)
		{
			setCursor(px, py+y);
			for (byte x = 0;x<24;x++)
			{ 
				sendData(scale3x[x][y]);
			}
		}
	} 
	px += fontsize;
	if (px >= OLED_Max_X)
	{
		px=0;
		py += fontsize;
		if (py >= OLED_Max_Y)
		{
			if (scroll)
			{
				// IMPLEMENT HARDWARE SCROLL HERE 
				py = OLED_Max_Y-fontsize;
			} else {
				py = 0;
			}
		}		
	} 
}

//
//
//
/* SAVED for Print

*/


// #####################################################################################################################
// ##### PRINT STRING ##################################################################################################
// #####################################################################################################################
void Display::printString(const byte *Data, byte PosX, byte PosY)
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
// ##### FONT SIZE #####################################################################################################
// #####################################################################################################################
void Display::setFontSize(byte f)
{	
	fontsize = f;
}

// #####################################################################################################################
// ##### PRINT NUMBER (NON FLOAT VALUE) ################################################################################
// #####################################################################################################################

void Display::printNum(char Data, byte PosX, byte PosY)
// Special version of this function to display MIDI data - either 0..127, or -64..63 range. Either fits into a Char.

{
  px = PosX;
  py = PosY;
	printNum(Data); 
}

void Display::printNum(char Data)
{
	if (Data < 0) 
	{
		Data = (-Data > 99) ? 99 : -Data; //negative values are clipped at -99
		printChar('-');
	} else {
		printChar(0x20);
	}
  for (byte xx=100;xx>0;xx=xx/10)
  {
    if (xx > Data) printChar(0x30); else printChar(0x30+(Data/xx)); 
    Data = Data % xx; 
  }
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
Display oled;
// #####################################################################################################################
// ##### END OF CODE ###################################################################################################
// #####################################################################################################################
