/*
	RGB 5 LEDs V0.0.0.5 Software!
	==========================
	Copyright (C) 2013 Arad Eizen (A.E Tec) LTD INC.
	This Program Makes The 5 RGB LEDs Blink And Fade In Many Efects!
	Main Board: Arduino Pro Mini.
	Language: cxx
	License For Shimon.H!
*/

#include <MY_PWM.h>					  // For PWM'ing The LEDs
#include <MY_VS1838BA.h>			  // For IR1838B IR Sensor
#include <EEPROM.h>					  // For Saving Settings

//#define USE_5_LEDs				  // Main Define For 5/3 LEDs

const int IRCV_PIN       = 2;		  // IR1838B Output (Data)
const int BIP_PIN        = 18;		  // Bipper (Activ HIGH) [A6]
// RGB LEDs Pins         = [3-17]     // {R0, G0, B0, ..., R4, G4, B4}

const byte KEY_ONOFF     = 10;		  // Key On/Off Index
const byte KEY_NEXT      = 11;		  // Key Next Index
const byte KEY_RESET     = 12;		  // Key Reset Index
const byte KEY_FADEIN    = 13;		  // Key Fade In Index
const byte KEY_FADEOUT   = 14;		  // Key Fade Out Index
const byte KEY_RANDOM    = 15;		  // Key Random Index
const byte KEY_SLOWER    = 16;		  // Key Slower Index
const byte KEY_FASTER    = 17;		  // Key Faster Index
const byte KEY_COLOR     = 18;		  // Key Color Index
const byte KEY_ZERO      = 19;		  // Key Zero Index
const byte KEY_ONE     	 = 20;		  // Key One Index
const byte KEY_TWO       = 21;		  // Key Two Index
const byte KEY_THREE     = 22;		  // Key Three Index
const byte KEY_FOUR      = 23;		  // Key Four Index
const byte KEY_FIVE		 = 24;		  // Key Five Index
const byte KEY_RINC      = 25;		  // Key R+ Index
const byte KEY_GINC      = 26;		  // Key G+ Index
const byte KEY_BINC      = 27;		  // Key B+ Index
const byte KEY_RDEC      = 28;		  // Key R- Index
const byte KEY_GDEC      = 29;		  // Key G- Index
const byte KEY_BDEC		 = 30;		  // Key B- Index
const byte KEY_LONG      = 128;		  // Long Key Press Index 
const byte KEY_NONE      = 255;		  // No Key Is Pressed Index

const byte KEY_BIP       = 10;		  // Key Press Bip Timout
const byte KEY_PRESS     = 10;		  // Key Long Press Timout
const byte KEY_PRESS_F   = 2;		  // Key Fast Long Press Timout
const byte A			 = 0x01;	  // LED A
const byte B			 = 0x02;	  // LED B
const byte C			 = 0x04;	  // LED C
const byte D			 = 0x08;	  // LED D
const byte E			 = 0x10;	  // LED E
const byte Z			 = 0x00;	  // All Off

#if defined(USE_5_LEDs)
	const byte LED_NUM	 = 5;		  // Number Of LEDs Spots
#else
	const byte LED_NUM	 = 3;		  // Number Of LEDs Spots
#endif

const byte LED_COUNT     = LED_NUM*3; // Total Number Of Emetors
const byte LED_DELTA     = 3;		  // Offset From P0.0 (RX)

const byte SET_EFECT     = 1;		  // EEPROM Efect Setting Adress
const byte SET_FADEIN    = 2;		  // EEPROM Fade In Setting Adress
const byte SET_FADEOUT   = 3;		  // EEPROM Fade Out Setting Adress
const byte SET_SPEED     = 4;		  // EEPROM Speed Setting Adress
const byte SET_SPEC      = 5;		  // EEPROM Spec Setting Adress
const byte SET_GLOBAL_C  = 6;		  // EEPROM Color Setting Adress
const byte SET_BLACK     = 7;		  // EEPROM Black Setting Adress
const byte SET_MAX_C     = 8;		  // EEPROM Color Setting Adress
const byte SET_COLORS    = 10;		  // EEPROM Colors Start Setting Adress

const byte MAX_SPEC    	 = 252;		  // Max Hue Value
const byte MAX_FADE      = 8;		  // fades Lenght
const byte MAX_SPEED     = 13;		  // speeds Lenght
const byte MAX_COLOR     = 5;		  // colors Lenght

const int fades[MAX_FADE]   = {0, 10, 40, 100, 300, 700, 1200, 2000};
const int speeds[MAX_SPEED] = {1, 5, 10, 40, 70, 120, 300, 600,
							   1000, 1500, 2500, 3900, 5000};
byte colors[MAX_COLOR][3];

#if defined(USE_5_LEDs)               // Efect = {LEN, STEP1, [STEP2], [STEP3], [STEPn]}
	const byte MAX_EFECT = 25;		  // efects Lenght
	const byte efects[MAX_EFECT][11] =
	{	
		{5, A, B, C, D, E},
		{10,A, Z, B, Z, C, Z, D, Z, E, Z},
		{10,A, B, C, D, E, E, D, C, B, A},
		{5, E, D, C, B, A},
		{10,E, Z, D, Z, C, Z, B, Z, A, Z},
		{3, C, B | D, A | E},
		{6, C, Z, B | D, Z, A | E, Z},
		{6, C, B | D, A | E,  A | E, B | D, C},
		{3, A | E, B | D, C},
		{6, A | E, Z, B | D, Z, C, Z},
		{4, A | B, B | C, C | D, D | E},
		{8, A | B, B | C, C | D, D | E, D | E, C | D, B | C, A | B},
		{4, D | E, C | D, B | C, A | B},
		{1, A | B | C | D | E},
		{2, A | B | C | D | E, Z},
		{1, A},
		{2, A, Z},
		{1, B},
		{2, B, Z},
		{1, C},
		{2, C, Z},
		{1, D},
		{2, D, Z},
		{1, E},
		{2, E, Z}
	};
#else
	const byte MAX_EFECT = 18;		  // efects Lenght
	const byte efects[MAX_EFECT][7] =
	{	
		{3, B, C, D},
		{6, B, Z, C, Z, D, Z},
		{6, B, C, D, D, C, B},
		{3, D, C, B},
		{6, D, Z, C, Z, B, Z},
		{3, B | C, C | D, D | B},
		{6, B | C, Z, C | D, Z, D | B, Z},
		{2, C, B | D},
		{4, C, Z, B | D, Z},
		{6, B, B | C, B | C | D, C | D, D, Z},
		{1, B | C | D},
		{2, B | C | D, Z},
		{1, B},
		{2, B, Z},
		{1, C},
		{2, C, Z},
		{1, D},
		{2, D, Z}
	};
#endif
long old_millis;
					
int bip_duration = 0;
int key_duration = 0;
int speed_duration;

byte key_index = KEY_NONE;
byte color_index = 0;
byte efect_pos   = 0;
byte spec_pos    = 0;
byte key_old;
byte efect_index;
byte efect_speed;
byte efect_length;
byte color_max;
byte fade_in;
byte fade_out;
byte spec_step;

boolean change_color = false;
boolean power_on = true;
boolean global_color;
boolean black_color;

decode_results results;
MY_VS1838BA irRecv(IRCV_PIN);

//-----------------------------------------------------------------------------
// setup
//-----------------------------------------------------------------------------
// Main.
void setup()
{
	MY_PWMBegin();								// Start The RGB LEDs
	irRecv.enableIRIn(); 						// Start The IR Receiver

	efect_index  = EEPROM.read(SET_EFECT);		// Efect Index Value
	efect_speed  = EEPROM.read(SET_SPEED);		// Efect Speed Value
	color_max 	 = EEPROM.read(SET_MAX_C);		// Color Max Value
	fade_in 	 = EEPROM.read(SET_FADEIN);		// Fade In Value
	fade_out 	 = EEPROM.read(SET_FADEOUT);	// Fade Out Value
	spec_step    = EEPROM.read(SET_SPEC);		// Spec Step Value	
	global_color = EEPROM.read(SET_GLOBAL_C);	// Global Color Value
	black_color  = EEPROM.read(SET_BLACK);		// Black Color Value

	SetOffLEDs();

	for (int i = 0, j = SET_COLORS; i < MAX_COLOR; i++, j += 3)
	{
		colors[i][0] = EEPROM.read(j + 0);
		colors[i][1] = EEPROM.read(j + 1);
		colors[i][2] = EEPROM.read(j + 2);
	}

	speed_duration = speeds[efect_speed];
	efect_length = efects[efect_index][0];
	
	pinMode(BIP_PIN, OUTPUT);					// OUTPUT
	digitalWrite(BIP_PIN, LOW);					// Set Bip Off
}
//-----------------------------------------------------------------------------
// loop
//-----------------------------------------------------------------------------
// Infinity Loop.
void loop()
{
	long tmp_millis = millis() / 10;
	byte delta_millis = abs(tmp_millis - old_millis);

	if (delta_millis)
	{
		old_millis = tmp_millis;
		if (bip_duration) if (--bip_duration == 0) digitalWrite(BIP_PIN, LOW);
		
		if (power_on && !change_color)
		{
			if (speed_duration) speed_duration--;
			else
			{
				speed_duration = speeds[efect_speed];
				EfectLED(efects[efect_index][efect_pos]);
				if (++efect_pos > efect_length) efect_pos = 1;
			}
		}
	}
	
	ScanIR();
	
	if (power_on)
	{
		switch (key_index)
		{
			case KEY_ONOFF:
				power_on = false;
				SetOffLEDs();
			break;
			case KEY_RESET:
				color_max   = 1;
				color_index = 0;
				efect_index = 0;
				efect_speed = 3;
				efect_pos   = 1;
				fade_in     = 0;
				fade_out    = 0;
				spec_pos    = 0;
				spec_step   = 10;
				efect_length   = efects[efect_index][0];
				speed_duration = speeds[efect_speed];
				global_color = false;
				black_color  = true;
				change_color = false;
			break;
			case KEY_RESET | KEY_LONG:
				colors[0][0] = 0xFF; colors[0][1] = 0x00; colors[0][2] = 0x00;
				colors[1][0] = 0x00; colors[1][1] = 0xFF; colors[1][2] = 0x00;
				colors[2][0] = 0x00; colors[2][1] = 0x00; colors[2][2] = 0xFF;
				colors[3][0] = 0xFF; colors[3][1] = 0xFF; colors[3][2] = 0x00;
				colors[4][0] = 0xFF; colors[4][1] = 0xFF; colors[4][2] = 0xFF;
			break;
			default:
				if (change_color)
				{
					switch (key_index)
					{
						case KEY_RINC: case KEY_RINC | KEY_LONG:
							IncColor(&colors[color_index][0]);
						break;
						case KEY_GINC: case KEY_GINC | KEY_LONG:
							IncColor(&colors[color_index][1]);
						break;
						case KEY_BINC: case KEY_BINC | KEY_LONG:
							IncColor(&colors[color_index][2]);
						break;
						case KEY_RDEC: case KEY_RDEC | KEY_LONG:
							DecColor(&colors[color_index][0]);
						break;
						case KEY_GDEC: case KEY_GDEC | KEY_LONG:
							DecColor(&colors[color_index][1]);
						break;
						case KEY_BDEC: case KEY_BDEC | KEY_LONG:
							DecColor(&colors[color_index][2]);
						break;
						default:
							if (key_index < KEY_LONG)
							{
								SetBlackColor(2);							
								change_color = false;
								MY_PWMSetFadeTime(ALL, fades[fade_in], fades[fade_out]);
							}
						break;
					}	
				}
				else
				{
					switch (key_index)
					{
						case KEY_NEXT:
							if (++efect_index == MAX_EFECT) efect_index = 0;
							efect_length = efects[efect_index][0];
							efect_pos = 1;
							SetOffLEDs();
						break;
						case KEY_FADEIN:
							MY_PWMSetFadeTime(ALL, fades[NextFade(&fade_in)], fades[fade_out]);
						break;
						case KEY_FADEOUT:
							MY_PWMSetFadeTime(ALL, fades[fade_in], fades[NextFade(&fade_out)]);
						break;
						case KEY_FADEIN | KEY_LONG:
							fade_in = 0;
							MY_PWMSetFadeTime(ALL, 0, fades[fade_out]);
						break;
						case KEY_FADEOUT | KEY_LONG:
							fade_out = 0;
							MY_PWMSetFadeTime(ALL, fades[fade_in], 0);
						break;
						case KEY_SLOWER:
							if (efect_speed < MAX_SPEED - 1) efect_speed++;
							speed_duration = speeds[efect_speed];
						break;
						case KEY_FASTER:
							if (efect_speed) efect_speed--;
							speed_duration = speeds[efect_speed];
						break;
						case KEY_COLOR:
							if (global_color) black_color = !black_color;
							global_color = !global_color; // all 4 pos...
						break;
						case KEY_RANDOM:
							color_max = 6;
						break;
						case KEY_ZERO: case KEY_ONE: case KEY_TWO:
						case KEY_THREE: case KEY_FOUR: case KEY_FIVE:
							color_max = key_index - KEY_ZERO;
						break;
						case KEY_ONE | KEY_LONG: case KEY_TWO | KEY_LONG: 
						case KEY_THREE | KEY_LONG: case KEY_FOUR | KEY_LONG: 
						case KEY_FIVE | KEY_LONG: 
							color_index = key_index - KEY_ONE - KEY_LONG;
							MY_PWMSetFadeTime(ALL, 0, 0);
							change_color = true;
							SetOffLEDs();
							SetCurColor(2);
						break;
						default:
							if (color_max == 0)
							{
								switch (key_index)
								{
									case KEY_RINC: case KEY_RINC | KEY_LONG:
									case KEY_GINC: case KEY_GINC | KEY_LONG:
									case KEY_BINC: case KEY_BINC | KEY_LONG:
										if (spec_step < MAX_SPEC) spec_step++;
									break;
									case KEY_RDEC: case KEY_RDEC | KEY_LONG:
									case KEY_GDEC: case KEY_GDEC | KEY_LONG:
									case KEY_BDEC: case KEY_BDEC | KEY_LONG:
										if (spec_step > 1) spec_step--;
									break;
									default:
									break;
								}	
							}
						break;
					}
				}
			break;
		}
	}
	else	//if(key_index == KEY_ONOFF) power_on = true;
	{
		switch (key_index)
		{
			case KEY_ONOFF:
				power_on = true;
			break;
			case KEY_ONOFF | KEY_LONG:
				Save();
			break;
		}
	}

	if (key_index < KEY_NONE)
	{
		key_old      = key_index;
		key_index    = KEY_NONE;
		key_duration = (key_index < KEY_LONG ? KEY_PRESS_F : KEY_PRESS);
		Bip(KEY_BIP);
	}
}
//-----------------------------------------------------------------------------
// IncColor
//-----------------------------------------------------------------------------
// Inc the given color until 255.
void IncColor(byte *color)
{
	if (*color < 246) *color += 10;
	else *color = 255;
	SetCurColor(2);
}
//-----------------------------------------------------------------------------
// DecColor
//-----------------------------------------------------------------------------
// Dec the given color until 0.
void DecColor(byte *color)
{
	if (*color > 9) *color -= 10;
	else *color = 0;
	SetCurColor(2);
}
//-----------------------------------------------------------------------------
// NextFade
//-----------------------------------------------------------------------------
// Step the given fade up.
byte NextFade(byte *fade)
{
	if (++*fade == MAX_FADE) *fade = 0;
	return *fade;
}
//-----------------------------------------------------------------------------
// EfectLED
//-----------------------------------------------------------------------------
// Change the leds color by the efect.
void EfectLED(byte led)
{
	switch (color_max)
	{
		case 0:
#if defined(USE_5_LEDs)
			SetCurLEDHue(led & A, 0);
#endif
			SetCurLEDHue(led & B, 1);
			SetCurLEDHue(led & C, 2);
			SetCurLEDHue(led & D, 3);			
#if defined(USE_5_LEDs)
			SetCurLEDHue(led & E, 4);
#endif
			if (global_color) NextHue();
		break;	
		case 1: case 2: case 3: case 4: case 5:
#if defined(USE_5_LEDs)
			SetCurLEDColor(led & A, 0);
#endif
			SetCurLEDColor(led & B, 1);
			SetCurLEDColor(led & C, 2);
			SetCurLEDColor(led & D, 3);
#if defined(USE_5_LEDs)
			SetCurLEDColor(led & E, 4);
#endif
			if (global_color) NextColor();
		break;	
		case 6:
#if defined(USE_5_LEDs)
			SetCurLEDRnd(led & A, 0);
#endif
			SetCurLEDRnd(led & B, 1);
			SetCurLEDRnd(led & C, 2);
			SetCurLEDRnd(led & D, 3);
#if defined(USE_5_LEDs)
			SetCurLEDRnd(led & E, 4);
#endif
			if (global_color) NextRnd();
		break;
	}
}
//-----------------------------------------------------------------------------
// SetCurLEDColor
//-----------------------------------------------------------------------------
// Change the led color if needed.
void SetCurLEDColor(boolean on, byte i)
{
	if (on)
	{
		SetCurColor(i);
		if (!global_color) NextColor();
	}
	else SetOffLED(i);
}
//-----------------------------------------------------------------------------
// SetCurLEDHue
//-----------------------------------------------------------------------------
// Change the led color if needed.
void SetCurLEDHue(boolean on, byte i)
{
	if (on)
	{
		SetColorHue(i, spec_pos);
		if (!global_color) NextHue();
	}
	else SetOffLED(i);
}
//-----------------------------------------------------------------------------
// SetCurLEDRnd
//-----------------------------------------------------------------------------
// Change the led color if needed.
void SetCurLEDRnd(boolean on, byte i)
{
	if (on)
	{
		SetColorHue(i, spec_pos);
		if (!global_color) NextRnd();
	}
	else SetOffLED(i);
}
//-----------------------------------------------------------------------------
// NextColor
//-----------------------------------------------------------------------------
// Change to the next color.
void NextColor()
{
	if (++color_index >= color_max) color_index = 0;
}
//-----------------------------------------------------------------------------
// NextHue
//-----------------------------------------------------------------------------
// Change to the next hue.
void NextHue()
{
	int tmp = spec_pos + spec_step;
	if (tmp < MAX_SPEC) spec_pos = tmp;
	else spec_pos = 0;
}
//-----------------------------------------------------------------------------
// NextRnd
//-----------------------------------------------------------------------------
// Create new random hue.
void NextRnd()
{
	spec_pos = random(MAX_SPEC);
}
//-----------------------------------------------------------------------------
// SetCurColor
//-----------------------------------------------------------------------------
// Set the led color to the color index.
void SetCurColor(byte i)
{
	SetColorRGB(i, colors[color_index][0],
				   colors[color_index][1],
				   colors[color_index][2]);
}
//-----------------------------------------------------------------------------
// Bip
//-----------------------------------------------------------------------------
// Turn the buzzer on for the duration time.
void Bip(int duration)
{
	if (duration)
	{
		if (bip_duration)
		{
			if(duration > bip_duration) bip_duration = duration;
		}
		else
		{
			bip_duration = duration;
			digitalWrite(BIP_PIN, HIGH);
		}
	}
	else
	{
		bip_duration = 0;
		digitalWrite(BIP_PIN, LOW);
	}
}
//-----------------------------------------------------------------------------
// save
//-----------------------------------------------------------------------------
// Saves the modes and colors.
void Save()
{
	EEPROM.write(SET_EFECT,    efect_index);
	EEPROM.write(SET_SPEED,    efect_speed);
	EEPROM.write(SET_MAX_C,    color_max);
	EEPROM.write(SET_FADEIN,   fade_in);
	EEPROM.write(SET_FADEOUT,  fade_out);
	EEPROM.write(SET_SPEC,     spec_step);
	EEPROM.write(SET_GLOBAL_C, global_color);
	EEPROM.write(SET_BLACK,    black_color);
	
	for (int i = 0, j = SET_COLORS; i < MAX_COLOR; i++, j += 3)
	{
		EEPROM.write(j + 0, colors[i][0]);
		EEPROM.write(j + 1, colors[i][1]);
		EEPROM.write(j + 2, colors[i][2]);
	}
}
//-----------------------------------------------------------------------------
// ScanIR
//-----------------------------------------------------------------------------
// Update the key index to the right one.
void ScanIR()
{
	if (!(irRecv.decode(&results))) return;
	long tmp = results.value;
	irRecv.resume();
	if (tmp == 0xFFFFFFFF)
	{
		if (key_duration) if (--key_duration == 0) key_index = key_old | KEY_LONG;
	}
	else key_index = IRindex(tmp);
}
//-----------------------------------------------------------------------------
// IRindex
//-----------------------------------------------------------------------------
// Returns the key index of the button.
byte IRindex(long value)
{
	switch(value & 0xFFFF)
	{
		case 0xA25D: return KEY_ONOFF;		// CH-
		case 0x629D: return KEY_NEXT;		// CH
		case 0xE21D: return KEY_RESET;		// CH+
		case 0x22DD: return KEY_FADEIN;		// |<<
		case 0x02FD: return KEY_FADEOUT;	// >>|
		case 0xC23D: return KEY_RANDOM;		//  >|
		case 0xE01F: return KEY_SLOWER;		// -
		case 0xA857: return KEY_FASTER;		// +
		case 0x906F: return KEY_COLOR;		// EQ
		case 0x6897: return KEY_ZERO;		// 0
		case 0x9867: return KEY_ONE;		// 100+
		case 0xB04F: return KEY_TWO;		// 200+
		case 0x30CF: return KEY_THREE;		// 1
		case 0x18E7: return KEY_FOUR;		// 2
		case 0x7A85: return KEY_FIVE;		// 3
		case 0x10EF: return KEY_RINC;		// 4
		case 0x38C7: return KEY_GINC;		// 5
		case 0x5AA5: return KEY_BINC;		// 6
		case 0x42BD: return KEY_RDEC;		// 7
		case 0x4AB5: return KEY_GDEC;		// 8
		case 0x52AD: return KEY_BDEC;		// 9
		default:	 return KEY_NONE;
	}
}
//-----------------------------------------------------------------------------
// SetOffLEDs
//-----------------------------------------------------------------------------
// Teurn off all leds.
void SetOffLEDs()
{
#if defined(USE_5_LEDs)
	for (byte i = 0; i < 5; i++)
		SetBlackColor(i);
#else
	for (byte i = 1; i < 4; i++)
		SetBlackColor(i);
#endif
}
//-----------------------------------------------------------------------------
// SetOffLED
//-----------------------------------------------------------------------------
// Teurn off the led if needed.
void SetOffLED(byte i)
{
	if (black_color) SetBlackColor(i);
}
//-----------------------------------------------------------------------------
// SetBlackColor
//-----------------------------------------------------------------------------
// Turn off the led by changing the color to tripple 'O'.
void SetBlackColor(byte i)
{
	SetColorRGB(i, 0, 0, 0);
}
//-----------------------------------------------------------------------------
// SetColorRGB
//-----------------------------------------------------------------------------
// Set the led color to the RGB values.
void SetColorRGB(byte i, byte r, byte g, byte b)
{
	i = i * 3 + LED_DELTA;
	MY_PWMSet(i + 0, r);
	MY_PWMSet(i + 1, g);
	MY_PWMSet(i + 2, b);
}
//-----------------------------------------------------------------------------
// SetColorHue
//-----------------------------------------------------------------------------
// Set the led color to hue [0-251] space. If hue [252-255] then black to white.
void SetColorHue(byte i, byte hue)
{
	const byte MAX_H = 246; // must be divided by 41
	byte x;
	if (hue < MAX_SPEC)
	{
		x = (hue % 42) * (MAX_H / 41.0);

		switch (hue / 42)
		{
			case 0: SetColorRGB(i, MAX_H,   x,       0);       break;
			case 1: SetColorRGB(i, MAX_H-x, MAX_H,   0);       break;
			case 2: SetColorRGB(i, 0,       MAX_H,   x);       break;
			case 3: SetColorRGB(i, 0,       MAX_H-x, MAX_H);   break;
			case 4: SetColorRGB(i, x,       0,       MAX_H);   break;
			case 5: SetColorRGB(i, MAX_H,   0,       MAX_H-x); break;
		}
	}
	else
	{
		x = (hue - MAX_SPEC) * 85; // 255 ==> White
		SetColorRGB(i, x, x, x);
	}
}
