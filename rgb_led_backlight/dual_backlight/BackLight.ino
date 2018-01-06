/*
	BackLightCPP V0.0.1.2 Software!
	==========================
	Copyright (C) 2014 Arad Eizen (A.E Tec) LTD INC.
	This Program Makes The 9 RGB LEDs Blink And Fade In Many Efects!
	Also Via UART From EXE Software Running On A PC.
	Main Board: Arduino Pro Mini.
	Language: cxx.
*/

//{ includes

#include <MyRemote.h>								// For IR1838B IR Sensor
#include <EEPROM.h>									// For Saving Settings

//}

//{ Constant and Definitions

#define DATA_PORT				PORTC				// 
#define DATA_DDR				DDRC				// 
#define DATA_PIN				0					// Pin A0
#define RESET_PIN				3					// Pin A3
#define BAUD_RATE				115200				// 

const uint8_t IR_PIN			= 3;				//
const uint8_t KEY_NONE			= 0xFF;				//


const uint8_t RGB_COUNT			= 3;				// RGB = 3 LEDs
const uint8_t LED_COUNT			= 9;				// Smart Pixels Count
const uint8_t HUE_COUNT			= 252;				// Max Hue Value
const uint16_t COLOR_COUNT		= 256;				// Max Channel Color Value
const uint8_t STEP_COUNT		= 50;				// STEP Count
const uint8_t SPEED_COUNT		= 16;				// SPEED Count
const uint8_t PATTERN_COUNT		= 9;				// PATTERN Count
const uint8_t PATTERN_LEN		= 4;				// PATTERN Lenght
const uint8_t EFFECT_COUNT		= 16;				// EFECT Count
const uint8_t EFFECT_LEN		= 10;				// EFECT Lenght
const uint8_t FIX_COLORS_COUNT	= 20;				// Fix Colors Buttons Count
const uint8_t DIY_COUNT			= 6;				// 
const uint8_t DIY_TYPE_COUNT	= 5;				// 
const uint16_t PING_TIMEOUT		= 4000;				// Max Ping Time
const uint8_t MSG_TIMEOUT		= 10;				// Max Serial Time
const uint8_t LED_BUFF_LEN		= LED_COUNT * RGB_COUNT;	// Total LEDs Count

const uint8_t STEP_MIN			= 1;				// 
const uint8_t STEP_MAX			= (STEP_COUNT-1);	// 
const uint8_t SPEED_MAX			= (SPEED_COUNT-1);	// 
const uint8_t COLOR_MAX			= (COLOR_COUNT-1);	// 
const uint8_t DIY_MAX			= (DIY_COUNT-1);	// 
const uint8_t DIY_TYPE_MAX		= (DIY_TYPE_COUNT-1);	// 


const uint8_t EFFECT_MODE_COLOR_CONST	= 0;		// 
const uint8_t EFFECT_MODE_COLOR_HUE		= 1;		// 
const uint8_t EFFECT_MODE_COLOR_RANDOM	= 2;		// 
const uint8_t EFFECT_MODE_COLOR_PATTERN	= 3;		// 

const uint8_t EFFECT_MODE_STEP_DOWN		= 4;		// 
const uint8_t EFFECT_MODE_STEP_UP_DOWN	= 8;		// 

const uint8_t EFFECT_MODE_PAINT_NEW		= 0;		// 
const uint8_t EFFECT_MODE_PAINT_BLACK	= 16;		// 
const uint8_t EFFECT_MODE_PAINT_OLD		= 32;		// 
const uint8_t EFFECT_MODE_PAINT_CYCLIC	= 48;		// 

const uint8_t EFFECT_MODE_COLOR_MASK	= 3;		// 
const uint8_t EFFECT_MODE_STEP_MASK		= 12;		// 
const uint8_t EFFECT_MODE_PAINT_MASK	= 48;		// 

const uint8_t EFFECT_INDEX_PAINT_BLACK	= 128;		// 


const uint8_t MAIN_MODE_POWER			= 1;		// 
const uint8_t MAIN_MODE_RUN_EFFECT		= 2;		// 
const uint8_t MAIN_MODE_AUTO_OFF		= 4;		// 
const uint8_t MAIN_MODE_AUTO_ON_BLINK	= 8;		// 
const uint8_t MAIN_MODE_CONNECTED		= 16;		// Sent Data To PC? (Good Pings)

const uint8_t MAIN_MODE_EFFECT_MASK		= 3;		// 


const uint8_t DIY_MODE_NONE				= 0;		// 
const uint8_t DIY_MODE_COLOR			= 1;		// 
const uint8_t DIY_MODE_IMAGE			= 2;		// 
const uint8_t DIY_MODE_EFFECT			= 3;		// 
const uint8_t DIY_MODE_STREAM			= 4;		// 


const uint8_t MSG_BUFF_LEN		= 36;		// 36 byte total 2.5ms on 115200bps

const uint8_t MSG_ID			= 0;		// 1  byte
const uint8_t MSG_LEDS			= 1;		// 27 bytes
const uint8_t MSG_COLOR			= 28;		// 3  bytes
const uint8_t MSG_MODE			= 31;		// 1  byte
const uint8_t MSG_INDEX			= 32;		// 1  byte
const uint8_t MSG_SPEED			= 33;		// 1  byte
const uint8_t MSG_STEP			= 34;		// 1  byte
const uint8_t MSG_PATTERN		= 35;		// 1  byte


const uint8_t MSG_ID_EFFECT		= 1;		// 
const uint8_t MSG_ID_STREAM		= 2;		// 
const uint8_t MSG_ID_DIY_1		= 3;		// 
const uint8_t MSG_ID_DIY_2		= 4;		// 
const uint8_t MSG_ID_DIY_3		= 5;		// 
const uint8_t MSG_ID_DIY_4		= 6;		// 
const uint8_t MSG_ID_DIY_5		= 7;		// 
const uint8_t MSG_ID_DIY_6		= 8;		// 
const uint8_t MSG_ID_SETTINGS	= 9;		// 

const uint8_t SCREEN_PRIMARY	= 1;		// 
const uint8_t SCREEN_SECONDARY	= 2;		// 
const uint8_t MSG_INDEX_STREAM	= 0;		// 
const uint8_t MSG_INDEX_STREAM2	= 1;		// 
const uint8_t MSG_INDEX_STREAM3	= 2;		// 

const uint8_t MSG_EVENT			= 255;		// 


const uint8_t EEPROM_FIRST_TIME		= 254;		// First Time Address

const uint16_t EEPROM_EFFECT_START		= 0;		// Start Address
const uint16_t EEPROM_EFFECT_LEN		= 8;		// Total Effect Data Length
const uint16_t EEPROM_EFFECT_COUNT		= 4;		// Saved Effect Count
const uint16_t EEPROM_EFFECT_END		= 			// End Address
					EEPROM_EFFECT_START + EEPROM_EFFECT_LEN * (EEPROM_EFFECT_COUNT + 1);

const uint16_t EFFECT_EEPROM_INDEX		= 0;		// Effect Index Base Address
const uint16_t EFFECT_EEPROM_SPEED		= 1;		// Effect Speed Base Address
const uint16_t EFFECT_EEPROM_MODE		= 2;		// Effect Mode Base Address
const uint16_t EFFECT_EEPROM_PATTERN	= 3;		// Effect Pattern Base Address
const uint16_t EFFECT_EEPROM_STEP		= 4;		// Effect Step Base Address
const uint16_t EFFECT_EEPROM_COLOR		= 5;		// Effect Color Base Address (3 Bytes)


const uint16_t A			 			= 0x0001;	// LED A
const uint16_t B			 			= 0x0002;	// LED B
const uint16_t C			 			= 0x0004;	// LED C
const uint16_t D			 			= 0x0008;	// LED D
const uint16_t E			 			= 0x0010;	// LED E
const uint16_t F			 			= 0x0020;	// LED F
const uint16_t G			 			= 0x0040;	// LED G
const uint16_t H			 			= 0x0080;	// LED H
const uint16_t I			 			= 0x0100;	// LED I


const uint8_t EEPROM_RESTORE[EEPROM_EFFECT_END - EEPROM_EFFECT_START - EEPROM_EFFECT_LEN] =
{	
	0x00, 0x00, 0x01, 0x00, 0x07, 0x00, 0x00, 0x00,
	0x84, 0x03, 0x13, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x08, 0x03, 0x32, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x05, 0x03, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00,
};

const uint8_t FIX_COLORS[FIX_COLORS_COUNT][RGB_COUNT] =
{	
	{0xFF, 0x00, 0x00}, {0xFF, 0x1E, 0x00}, {0xFF, 0x32, 0x00}, {0xFF, 0x64, 0x00}, {0xFF, 0xA0, 0x00},	// Red   - Yellow
	{0x00, 0xFF, 0x00}, {0x50, 0xFF, 0x00}, {0x14, 0xFF, 0x3C}, {0x00, 0xFF, 0x84}, {0x00, 0x96, 0xC8}, // Green - Cyan
	{0x00, 0x00, 0xFF}, {0x00, 0x50, 0xFF}, {0x5A, 0x00, 0xFF}, {0xB4, 0x00, 0xFF}, {0xFF, 0x28, 0xFF}, // Blue  - Magenta
	{0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, // White - Black
};

const uint8_t PATTERN[PATTERN_COUNT][PATTERN_LEN][RGB_COUNT] = // Triple 0x00 = END
{	
	{{0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00}, {0x00, 0x00, 0xFF}, {0x00, 0x00, 0x00}},	// RGB
	{{0x00, 0xFF, 0xFF}, {0xFF, 0x00, 0xFF}, {0xFF, 0xFF, 0x00}, {0x00, 0x00, 0x00}},	// CMY
	{{0x20, 0x20, 0x20}, {0x60, 0x60, 0x60}, {0xA0, 0xA0, 0xA0}, {0xFF, 0xFF, 0xFF}},	// B&W
	{{0xFF, 0x00, 0x00}, {0xFF, 0x50, 0x00}, {0xFF, 0xA0, 0x00}, {0xFF, 0xFF, 0x00}},	// Fire
	{{0x00, 0x00, 0xFF}, {0x00, 0x50, 0xFF}, {0x00, 0xA0, 0xFF}, {0x00, 0xFF, 0xFF}},	// Water
	{{0xFF, 0x00, 0x00}, {0x00, 0x00, 0xFF}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},	// RB
	{{0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},	// BW
	{{0xFF, 0x00, 0x00}, {0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},	// RW
	{{0xFF, 0xFF, 0x00}, {0x00, 0x00, 0xFF}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},	// YB
};

const uint16_t SPEED[SPEED_COUNT]    = {3, 5, 7, 14, 25, 40, 70, 130, 200, 310, 500, 800, 1500, 2800, 4100, 6000};

const uint16_t EFFECT[EFFECT_COUNT][EFFECT_LEN] =
{	
	{1, A|B|C|D|E|F|G|H|I},
	{2, A|D|G, C|F|I},
	{2, A|B|C, G|H|I},
	{2, A|C|G|I, B|D|F|H},
	{3, A|D|G, B|E|H, C|F|I},
	{3, A|B|C, D|E|F, G|H|I},
	{5, A, B|D, C|E|G, F|H, I},
	{5, C, B|F, A|E|I, D|H, G},
	{8, A, B, C, F, I, H, G, D},
	{8, A|B, B|C, C|F, F|I, I|H, H|G, G|D, D|A},
	{4, A|B, C|F, H|I, D|G},	
	{4, A|I, B|H, C|G, F|D},	
	{4, A|B|H|I, B|C|G|H, C|D|F|G, A|D|F|I},
	{4, A|B|C, C|F|I, G|H|I, A|D|G},
	{1, A|C|D|F|G|I},

	{9, A, B, C, D, E, F, G, H, I}, /////////////////
};

//}

//{ Veraibles

uint8_t oldIndexIR, longPressCount, longPressStart;
uint8_t mainMode, mainScreen;
uint8_t effectIndex;
uint8_t effectSpeedIndex;
uint8_t effectPosition;
uint8_t effectMode;
uint8_t effectColor[RGB_COUNT];
uint8_t effectColorPosition;
uint8_t effectColorPatternIndex;
uint8_t effectColorStep;
uint8_t effectColorStepCount;
uint8_t ledBuff3[LED_BUFF_LEN * 2];
uint8_t msgBuff[MSG_BUFF_LEN];
uint8_t retBuff[MSG_BUFF_LEN];
uint32_t oldMs, effectSpeedMs, mainPingMs;

//}

//{ Main and Loop

void setup()
{
	srand(analogRead(0));			// Init Randomizer
	DATA_DDR |= _BV(DATA_PIN);		// Init LEDs
	irBegin(IR_PIN);				// Init Remote
	Serial.begin(BAUD_RATE);		// Init Serial
	Serial.setTimeout(MSG_TIMEOUT);	// Using Serial.readBytes()
	
	mainMode = 0; // EEPROM...
	mainScreen = SCREEN_PRIMARY | SCREEN_SECONDARY;
/*
	if (mainMode & MAIN_MODE_AUTO_ON_BLINK)
	{
		mainMode |= MAIN_MODE_POWER;
		blinkLeds();
	}

	mainMode &= ~MAIN_MODE_POWER;
*/	

	oldIndexIR				= KEY_NONE;
	effectIndex 			= EFFECT_COUNT - 1;
	effectSpeedIndex 		= 3;
	effectMode 				= 0;	
	effectColorPatternIndex = 0;
	effectColorStep 		= 1;
	effectColorRGB(255, 80, 0);
	effectInit();					// Init Effect
	
	if (EEPROM.read(EEPROM_FIRST_TIME) != EEPROM_FIRST_TIME)
	{
		eepromRestore();
		EEPROM.write(EEPROM_FIRST_TIME, EEPROM_FIRST_TIME);
	}
}

void loop()
{
	delay(3);	/////////////////////
	uint32_t curMs = millis();
	uint8_t deltaMs = curMs - oldMs;

	oldMs = curMs;

	if ((mainMode & MAIN_MODE_EFFECT_MASK) == (MAIN_MODE_POWER | MAIN_MODE_RUN_EFFECT))
	{
		if (effectSpeedMs > deltaMs)
			effectSpeedMs -= deltaMs;
		else
		{
			effectUpdateSpeed();
			effectStep();
		}
	}
	
	if (irAvailable())
	{
		uint32_t key = irRead();

		if (key < REPEAT)
		{
			oldIndexIR = remoteIndex44(key);
			if (oldIndexIR < KEY_NONE) shortPress();
		}
		else if (oldIndexIR < KEY_NONE) longPress();
	}
	
	if (Serial.available())
	{
		if (MSG_BUFF_LEN == Serial.readBytes(msgBuff, MSG_BUFF_LEN))
		{
			msgReceived();
		}
	}
/*
	if (mainMode & MAIN_MODE_CONNECTED)
	{
		if (mainPingMs > deltaMs)
			mainPingMs -= deltaMs;
		else
		{
			mainMode &= ~MAIN_MODE_CONNECTED;

			if (mainMode & MAIN_MODE_AUTO_OFF)
			{
				fillLedsBlack();
				updateLEDs();
				mainMode &= ~MAIN_MODE_POWER;
			}
		}
	}
*/
}

//}

//{ Message

uint8_t msgReceived()
{
	mainMode |= MAIN_MODE_CONNECTED;
	mainPingMs = PING_TIMEOUT;
	
	uint8_t id = msgBuff[MSG_ID];
	
	if (id == MSG_ID_EFFECT)
	{
		if (msgBuff[MSG_MODE] == MSG_EVENT)
			msgSendEffect();
		else
			msgUpdateEffect(); // mainMode = ...
	}
	else if (id == MSG_ID_STREAM)
	{
		if (msgBuff[MSG_MODE] == MSG_EVENT)
			msgSendStream();
		else
		{
			if (msgBuff[MSG_INDEX] == MSG_INDEX_STREAM)			msgUpdateLeds();
			else if (msgBuff[MSG_INDEX] == MSG_INDEX_STREAM2)	msgUpdateLeds2();
			else if (msgBuff[MSG_INDEX] == MSG_INDEX_STREAM3)	msgUpdateLeds3();
		}
	}
	else if (id < MSG_ID_SETTINGS) // MSG_ID_DIY_1 to MSG_ID_DIY_6
	{
		uint8_t index = id - MSG_ID_DIY_1;
		if (msgBuff[MSG_MODE] == MSG_EVENT)
			msgSendDiy(index);
		else
		{
			if (msgBuff[MSG_MODE] == DIY_MODE_COLOR)
			{
				msgUpdateColor(); // EERPROM(index);
			}
			else if (msgBuff[MSG_MODE] == DIY_MODE_IMAGE)
			{
				msgUpdateLeds(); // EERPROM(index);
			}
			else if (msgBuff[MSG_MODE] == DIY_MODE_EFFECT)
			{
				msgUpdateEffect(); // EERPROM(index);
			}
			// mainMode = ...
		}
	}
	else if (id == MSG_ID_SETTINGS)
	{
		if (msgBuff[MSG_MODE] == MSG_EVENT)
		{
			mainScreen	= msgBuff[MSG_INDEX];
			msgSendSettings();
		}
		else
		{
			if ((mainMode & MAIN_MODE_POWER) ^ (msgBuff[MSG_MODE] & MAIN_MODE_POWER))
			{
				if (mainMode & MAIN_MODE_POWER)	powerOff(false);
				else							powerOn(false);
			}
			mainMode	= msgBuff[MSG_MODE];
			mainScreen	= msgBuff[MSG_INDEX];
		}
		if (mainScreen == SCREEN_PRIMARY)			memset(ledBuff3 + LED_BUFF_LEN, 0, LED_BUFF_LEN);
		else if (mainScreen == SCREEN_SECONDARY)	memset(ledBuff3, 0, LED_BUFF_LEN);
		updateLEDs();
	}
}

void msgSendColor()
{
	mainMode |= MAIN_MODE_RUN_EFFECT;
	memcpy(retBuff + MSG_COLOR, effectColor, RGB_COUNT);
	msgSend();
}
void msgSendEffect()
{
	retBuff[MSG_ID]			= MSG_ID_EFFECT;
	retBuff[MSG_MODE] 		= effectMode;	
	retBuff[MSG_INDEX] 		= effectIndex & ~EFFECT_INDEX_PAINT_BLACK;
	retBuff[MSG_SPEED] 		= effectSpeedIndex;
	retBuff[MSG_STEP]		= effectColorStep;
	retBuff[MSG_PATTERN]	= effectColorPatternIndex;
	msgSendColor();
}
void msgSendStream()
{
	mainMode &= ~MAIN_MODE_RUN_EFFECT;
	retBuff[MSG_ID]			= MSG_ID_STREAM;
	memcpy(retBuff + MSG_LEDS, ledBuff3, LED_BUFF_LEN);
	msgSend();
}
void msgSendDiy(uint8_t index)
{
	retBuff[MSG_ID]			= MSG_ID_DIY_1 + index;
	//retBuff
	msgSend();
}
void msgSendSettings()
{
	retBuff[MSG_ID]			= MSG_ID_SETTINGS;
	retBuff[MSG_MODE]		= mainMode;
	retBuff[MSG_INDEX]		= mainScreen;
	msgSend();
}

void msgUpdateColor()
{
	mainMode |= MAIN_MODE_RUN_EFFECT;
	memcpy(effectColor, msgBuff + MSG_COLOR, RGB_COUNT);
	effectUpdateSpeed();
	effectInit();
}
void msgUpdateEffect()
{
	effectMode 				= msgBuff[MSG_MODE];	
	effectIndex 			= msgBuff[MSG_INDEX];
	effectSpeedIndex 		= msgBuff[MSG_SPEED];
	effectColorStep 		= msgBuff[MSG_STEP];
	effectColorPatternIndex = msgBuff[MSG_PATTERN];
	msgUpdateColor();
}
void msgUpdateLeds()
{
	mainMode &= ~MAIN_MODE_RUN_EFFECT;
	memcpy(ledBuff3, msgBuff + MSG_LEDS, LED_BUFF_LEN);
	updateLEDs();
}
void msgUpdateLeds2()
{
	mainMode &= ~MAIN_MODE_RUN_EFFECT;
	memcpy(ledBuff3 + LED_BUFF_LEN, msgBuff + MSG_LEDS, LED_BUFF_LEN);
	updateLEDs();
}
void msgUpdateLeds3()
{
	mainMode &= ~MAIN_MODE_RUN_EFFECT;
	memcpy(ledBuff3, msgBuff + MSG_LEDS, LED_BUFF_LEN);
	memcpy(ledBuff3 + LED_BUFF_LEN, msgBuff + MSG_LEDS, LED_BUFF_LEN);
	updateLEDs();
}

void msgSend()
{
	Serial.write(retBuff, MSG_BUFF_LEN);
}


//}

//{ Effects

void effectInit()
{
	uint8_t colorMode = effectMode & EFFECT_MODE_COLOR_MASK;
	
	effectPosition = 1;
	effectColorPosition = 0;
	effectUpdateColorStep();
	
	if (colorMode == EFFECT_MODE_COLOR_HUE)
	{
		effectColorHue();
	}
	else if (colorMode == EFFECT_MODE_COLOR_RANDOM)
	{
		effectColorRandom();
	}
	else if (colorMode == EFFECT_MODE_COLOR_PATTERN)
	{
		effectColorPattern();
	}
	
	fillLedsBlack();
	
	effectStep();
}

void effectStep()
{
	if (effectIndex & EFFECT_INDEX_PAINT_BLACK)
	{
		effectIndex &= ~EFFECT_INDEX_PAINT_BLACK;
		fillLedsBlack();
		updateLEDs();
		return;
	}
	
	uint8_t colorMode = effectMode & EFFECT_MODE_COLOR_MASK;
	
	effectFillLeds();
	updateLEDs();
	
	if (effectMode & EFFECT_MODE_STEP_DOWN)
	{
		if (--effectPosition < 1)
		{
			if (effectMode & EFFECT_MODE_STEP_UP_DOWN)
			{
				effectMode &= ~EFFECT_MODE_STEP_DOWN;
				effectPosition = 1;
			}
			else effectPosition = EFFECT[effectIndex][0];
		}
	}
	else
	{
		if (++effectPosition > EFFECT[effectIndex][0])
		{
			if (effectMode & EFFECT_MODE_STEP_UP_DOWN)
			{
				effectMode |= EFFECT_MODE_STEP_DOWN;
				effectPosition = EFFECT[effectIndex][0];
			}
			else effectPosition = 1;
		}
	}
	
	if ((effectMode & EFFECT_MODE_PAINT_MASK) == EFFECT_MODE_PAINT_BLACK)
		effectIndex |= EFFECT_INDEX_PAINT_BLACK;
	
	if (colorMode == EFFECT_MODE_COLOR_CONST)
	{
		// Do Nothing... // return;
	}
	else if (colorMode == EFFECT_MODE_COLOR_HUE)
	{
		effectColorPosition = ((uint16_t)effectColorPosition + effectColorStep) % HUE_COUNT;
		effectColorHue();
	}
	else if (--effectColorStepCount == 0)
	{
		effectUpdateColorStep();
		if (colorMode == EFFECT_MODE_COLOR_RANDOM)
		{
			effectColorRandom();
		}
		else if (colorMode == EFFECT_MODE_COLOR_PATTERN)
		{
			if (++effectColorPosition >= PATTERN_LEN) effectColorPosition = 0;
			effectColorPattern();
			if (effectColorIsBlack()) {effectColorPosition = 0; effectColorPattern();}
		}
	}
}

void effectFillLeds()
{
	if ((effectMode & EFFECT_MODE_PAINT_MASK) == EFFECT_MODE_PAINT_CYCLIC)
	{
		uint8_t tmpColor[RGB_COUNT], i;
		memcpy(tmpColor, effectColor, RGB_COUNT);
		if (effectMode & EFFECT_MODE_STEP_DOWN)
			for (i = 1; i < EFFECT[effectIndex][0]; i++)
			{
				effectColorFromLeds(EFFECT[effectIndex][i+1]);
				effectColorToLeds(EFFECT[effectIndex][i]);
			}
		else
			for (i = EFFECT[effectIndex][0]; i > 1; i--)
			{
				effectColorFromLeds(EFFECT[effectIndex][i-1]);
				effectColorToLeds(EFFECT[effectIndex][i]);
			}
		memcpy(effectColor, tmpColor, RGB_COUNT);
		effectColorToLeds(EFFECT[effectIndex][i]);
	}
	else
	{
		effectColorToLeds(EFFECT[effectIndex][effectPosition]);
	}
}

void effectColorFromLeds(uint16_t leds)
{
	uint16_t mask = 1;
	for (uint8_t i = 0; i < LED_COUNT; i++, mask <<= 1)
		if (leds & mask)
		{
			if (mainScreen & SCREEN_PRIMARY)	memcpy(effectColor, ledBuff3 + i * RGB_COUNT, RGB_COUNT);
			if (mainScreen & SCREEN_SECONDARY)	memcpy(effectColor, ledBuff3 + LED_BUFF_LEN + i * RGB_COUNT, RGB_COUNT);
			return;
		}
}

void effectColorToLeds(uint16_t leds)
{
	uint16_t mask = 1;
	for (uint8_t i = 0; i < LED_COUNT; i++, mask <<= 1)
	{
		if (leds & mask)
		{
			effectColorFillLed(i);
		}
		else
		{
			if (((effectMode & EFFECT_MODE_PAINT_MASK) == EFFECT_MODE_PAINT_NEW)
			|| ((effectMode & EFFECT_MODE_COLOR_MASK) == EFFECT_MODE_COLOR_CONST))
				fillLedBlack(i);
		}
	}
}

void effectColorFillLed(uint8_t index)
{
	if (mainScreen & SCREEN_PRIMARY)	memcpy(ledBuff3 + index * RGB_COUNT, effectColor, RGB_COUNT);
	if (mainScreen & SCREEN_SECONDARY)	memcpy(ledBuff3 + LED_BUFF_LEN + index * RGB_COUNT, effectColor, RGB_COUNT);
}

void effectColorBlack()
{
	memset(effectColor, 0, RGB_COUNT);
}

bool effectColorIsBlack()
{
	return (effectColor[0] == 0) && (effectColor[1] == 0) && (effectColor[2] == 0);
}

void effectColorRGB(uint8_t r, uint8_t g, uint8_t b)
{
	effectColor[0] = r;
	effectColor[1] = g;
	effectColor[2] = b;
}

void effectColorRandom()
{
	effectColorRGB(random(COLOR_COUNT), random(COLOR_COUNT), random(COLOR_COUNT));
}

void effectColorHue()
{
	const uint8_t MAX_H = 246; // must be divided by 41
	uint8_t x;
	if (effectColorPosition < HUE_COUNT)
	{
		x = (effectColorPosition % 42) * (MAX_H / 41.0);

		switch (effectColorPosition / 42)
		{
			case 0: effectColorRGB(MAX_H,   x,       0);       break;
			case 1: effectColorRGB(MAX_H-x, MAX_H,   0);       break;
			case 2: effectColorRGB(0,       MAX_H,   x);       break;
			case 3: effectColorRGB(0,       MAX_H-x, MAX_H);   break;
			case 4: effectColorRGB(x,       0,       MAX_H);   break;
			case 5: effectColorRGB(MAX_H,   0,       MAX_H-x); break;
		}
	}
	else
	{
		x = (effectColorPosition - HUE_COUNT) * 85; // 255 ==> White
		effectColorRGB(x, x, x);
	}
}

void effectColorPattern()
{
	memcpy(effectColor, PATTERN[effectColorPatternIndex][effectColorPosition], RGB_COUNT);
}

void effectUpdateSpeed()
{
	effectSpeedMs = SPEED[effectSpeedIndex] * 10;
}

void effectUpdateLEDs()
{
	effectColorToLeds(EFFECT[effectIndex][effectPosition]);
	updateLEDs();
}

void effectUpdateColorStep()
{
	effectColorStepCount = effectColorStep;
}
//}

//{ Remote

void shortPress()
{
	longPressCount = 1;
	longPressStart = 3;
	
	if (oldIndexIR == 0)
	{
		longPressStart = 50;
		if (mainMode & MAIN_MODE_POWER)
		{
			powerOff(true);
			return;
		}
		powerOn(true);
		return;
	}
	powerOn(true);
	
	if (oldIndexIR == 1)
	{
		effectIndex &= ~EFFECT_INDEX_PAINT_BLACK;
		if (++effectIndex >= EFFECT_COUNT) effectIndex = 0;
		effectInit();
		msgSendEffect();
	}
	else if (oldIndexIR < 4)
	{
		if (oldIndexIR == 2)
		{
			if (effectColorStep < STEP_MAX) effectColorStep++;
		}
		else
		{
			if (effectColorStep > STEP_MIN) effectColorStep--;
		}
		effectUpdateColorStep();
		msgSendEffect();
	}
	else if (oldIndexIR < 6)
	{
		if (oldIndexIR == 4)
		{
			if (effectSpeedIndex > 0) effectSpeedIndex--;
		}
		else
		{
			if (effectSpeedIndex < SPEED_MAX) effectSpeedIndex++;
		}
		effectUpdateSpeed();
		msgSendEffect();
	}
	else if (oldIndexIR == 6)
	{
		if ((effectMode & EFFECT_MODE_COLOR_MASK) != EFFECT_MODE_COLOR_PATTERN)
		{
			effectMode = (effectMode & ~EFFECT_MODE_COLOR_MASK) | EFFECT_MODE_COLOR_PATTERN;
			effectColorStep = 1;
		}
		else if (++effectColorPatternIndex >= PATTERN_COUNT) effectColorPatternIndex = 0;
		effectInit();
		msgSendEffect();
	}
	else if (oldIndexIR == 7)
	{
		if ((effectMode & EFFECT_MODE_COLOR_MASK) == EFFECT_MODE_COLOR_CONST)
		{
			effectMode &= ~EFFECT_MODE_PAINT_OLD;
			effectMode ^= EFFECT_MODE_PAINT_BLACK;
		}
		else
		{
			uint8_t tmp = effectMode & EFFECT_MODE_PAINT_MASK;
			tmp += EFFECT_MODE_PAINT_BLACK; // 16
			if (tmp > EFFECT_MODE_PAINT_MASK) tmp = 0;
			effectMode = (effectMode & ~EFFECT_MODE_PAINT_MASK) | tmp;
		}
		effectInit();
		msgSendEffect();
	}
	else if (oldIndexIR < 12)
	{
		eepromSaveEffect(EEPROM_EFFECT_COUNT); // Tmp
		eepromOpenEffect(oldIndexIR - 8);
		longPressStart = 14;
		msgSendEffect();
	}
	else if (oldIndexIR < 18)
	{
		effectMode = (effectMode & ~(EFFECT_MODE_COLOR_MASK | EFFECT_MODE_PAINT_OLD)) | EFFECT_MODE_COLOR_CONST;
		uint8_t *ptr = &effectColor[(oldIndexIR - 12) % RGB_COUNT];
		if (oldIndexIR < 15) {if (*ptr < COLOR_MAX) (*ptr)++;}
		else                 {if (*ptr) (*ptr)--;}
		effectUpdateLEDs();
		msgSendEffect();
	}
	else if (oldIndexIR < 24)
	{
		//eepromPrintEffect(); /////////////////
		//msgSendEffect();
		//msgSendEffect();
		msgSendDiy(oldIndexIR - 18);
	}
	else if (oldIndexIR < 41)
	{
		effectMode = (effectMode & ~(EFFECT_MODE_COLOR_MASK | EFFECT_MODE_PAINT_OLD)) | EFFECT_MODE_COLOR_CONST;
		memcpy(effectColor, FIX_COLORS[oldIndexIR - 24], RGB_COUNT);
		effectUpdateLEDs();
		msgSendEffect();
	}
	else if (oldIndexIR == 41)
	{
		if ((effectMode & EFFECT_MODE_COLOR_MASK) != EFFECT_MODE_COLOR_RANDOM)
		{
			effectMode = (effectMode & ~EFFECT_MODE_COLOR_MASK) | EFFECT_MODE_COLOR_RANDOM;
			effectColorStep = 1;
		}
		effectInit();
		msgSendEffect();
	}
	else if (oldIndexIR == 42)
	{
		if ((effectMode & EFFECT_MODE_COLOR_MASK) != EFFECT_MODE_COLOR_HUE)
		{
			effectMode = (effectMode & ~EFFECT_MODE_COLOR_MASK) | EFFECT_MODE_COLOR_HUE;
			effectColorStep = 8;
		}
		effectInit();
		msgSendEffect();
	}
	else if (oldIndexIR == 43)
	{
		uint8_t tmp = effectMode & EFFECT_MODE_STEP_MASK;
		tmp += EFFECT_MODE_STEP_DOWN; // 4
		if (tmp > EFFECT_MODE_STEP_UP_DOWN) tmp = 0;
		effectMode = (effectMode & ~EFFECT_MODE_STEP_MASK) | tmp;
		msgSendEffect();
	}
}
void longPress()
{
	if (longPressStart)
	{
		longPressStart--;
		delay(1);
	}
	else
	{
		if (oldIndexIR == 0)
		{
			cli(); // Disable global interrupts
			DATA_DDR |= _BV(RESET_PIN);
			DATA_PORT = 0;
			for(;;);
		}
		else if (oldIndexIR == 1)
		{

		}
		else if (oldIndexIR < 4)
		{
			longPressStart = 2;
			if (oldIndexIR == 2)
			{
				if (effectColorStep < STEP_MAX) effectColorStep++;
			}
			else
			{
				if (effectColorStep > STEP_MIN) effectColorStep--;
			}
			effectUpdateColorStep();
			msgSendEffect();
		}
		else if (oldIndexIR < 6)
		{
			longPressStart = 2;
			if (oldIndexIR == 4)
			{
				if (effectSpeedIndex > 0) effectSpeedIndex--;
			}
			else
			{
				if (effectSpeedIndex < SPEED_MAX) effectSpeedIndex++;
			}
			effectUpdateSpeed();
			msgSendEffect();
		}
		else if (oldIndexIR == 6)
		{
			longPressStart = 255;
			effectColorPatternIndex = 0;
			effectInit();
			msgSendEffect();
		}
		else if (oldIndexIR == 7)
		{
			longPressStart = 255;
			effectMode &= ~EFFECT_MODE_PAINT_CYCLIC;
			effectInit();
			msgSendEffect();
		}
		else if (oldIndexIR < 12)
		{
			if (longPressCount == COLOR_MAX)
			{
				longPressStart = 255;
				eepromRestore();
				eepromOpenEffect(0);
			}
			else
			{
				longPressCount = COLOR_MAX;
				longPressStart = 40; // Restore
				blinkLeds();
				eepromOpenEffect(EEPROM_EFFECT_COUNT); // Tmp
				eepromSaveEffect(oldIndexIR - 8);
				msgSendEffect();
			}
		}
		else if (oldIndexIR < 18)
		{
			uint8_t *ptr = &effectColor[(oldIndexIR - 12) % RGB_COUNT];
			if (oldIndexIR < 15) {if (*ptr < COLOR_MAX - longPressCount) (*ptr) += longPressCount; 	else (*ptr) = COLOR_MAX;}
			else                 {if (*ptr > longPressCount) (*ptr) -= longPressCount;				else (*ptr) = 0;}
			if (longPressCount < 30) longPressCount++;
			effectUpdateLEDs();
			msgSendEffect();
		}
	}
}

uint8_t remoteIndex44(uint32_t key)
{
	if ((key >> 16) != 0xFF) return KEY_NONE;
	key &= 0xFFFF;
	if (key == 0x3AC5) return 2;	// Brightness +
	if (key == 0xBA45) return 3;	// Brightness -
	if (key == 0x827D) return 1;	// Play
	if (key == 0x02FD) return 0;	// On / Off
	if (key == 0x1AE5) return 24;	// Red
	if (key == 0x2AD5) return 25;	// Color 1
	if (key == 0x0AF5) return 26;	// Color 5
	if (key == 0x38C7) return 27;	// Color 9
	if (key == 0x18E7) return 28;	// Color 13
	if (key == 0x9A65) return 29;	// Green
	if (key == 0xAA55) return 30;	// Color 2
	if (key == 0x8A75) return 31;	// Color 6
	if (key == 0xB847) return 32;	// Color 10
	if (key == 0x9867) return 33;	// Color 14
	if (key == 0xA25D) return 34;	// Blue
	if (key == 0x926D) return 35;	// Color 3
	if (key == 0xB24D) return 36;	// Color 7
	if (key == 0x7887) return 37;	// Color 11
	if (key == 0x58A7) return 38;	// Color 15
	if (key == 0x22DD) return 39;	// White
	if (key == 0x12ED) return 40;	// Color 4
	if (key == 0x32CD) return 41;	// Color 8
	if (key == 0xF807) return 42;	// Color 12
	if (key == 0xD827) return 43;	// Color 16
	if (key == 0x28D7) return 12;	// R+
	if (key == 0xA857) return 13;	// G+
	if (key == 0x6897) return 14;	// B+
	if (key == 0xE817) return 4;	// QUICK
	if (key == 0x08F7) return 15;	// R-
	if (key == 0x8877) return 16;	// G-
	if (key == 0x48B7) return 17;	// B-
	if (key == 0xC837) return 5;	// SLOW
	if (key == 0x30CF) return 18;	// DIY1
	if (key == 0xB04F) return 19;	// DIY2
	if (key == 0x708F) return 20;	// DIY3
	if (key == 0xF00F) return 6;	// AUTO
	if (key == 0x10EF) return 21;	// DIY4
	if (key == 0x906F) return 22;	// DIY5
	if (key == 0x50AF) return 23;	// DIY6
	if (key == 0xD02F) return 7;	// FLASH
	if (key == 0x20DF) return 8;	// JUMP3
	if (key == 0xA05F) return 9;	// JUMP7
	if (key == 0x609F) return 10;	// FADE3
	if (key == 0xE01F) return 11;	// FADE7
	return KEY_NONE;
}

//}

//{ EEPROM

void eepromRestore()
{
	blinkLeds();
	for (uint16_t i = EEPROM_EFFECT_START; i < EEPROM_EFFECT_END - EEPROM_EFFECT_LEN; i++)
		EEPROM.write(i, EEPROM_RESTORE[i - EEPROM_EFFECT_START]);
}

void eepromSaveEffect(uint8_t index)
{
	uint16_t i = EEPROM_EFFECT_START + index * EEPROM_EFFECT_LEN;
	
	EEPROM.write(i + EFFECT_EEPROM_INDEX	, effectIndex);
	EEPROM.write(i + EFFECT_EEPROM_SPEED	, effectSpeedIndex);
	EEPROM.write(i + EFFECT_EEPROM_MODE		, effectMode);
	EEPROM.write(i + EFFECT_EEPROM_PATTERN	, effectColorPatternIndex);
	EEPROM.write(i + EFFECT_EEPROM_STEP		, effectColorStep);
	EEPROM.write(i + EFFECT_EEPROM_COLOR + 0, effectColor[0]);
	EEPROM.write(i + EFFECT_EEPROM_COLOR + 1, effectColor[1]);
	EEPROM.write(i + EFFECT_EEPROM_COLOR + 2, effectColor[2]);
}

void eepromOpenEffect(uint8_t index)
{
	uint16_t i = EEPROM_EFFECT_START + index * EEPROM_EFFECT_LEN;
	
	effectIndex 			= EEPROM.read(i + EFFECT_EEPROM_INDEX);
	effectSpeedIndex 		= EEPROM.read(i + EFFECT_EEPROM_SPEED);
	effectMode 				= EEPROM.read(i + EFFECT_EEPROM_MODE);	
	effectColorPatternIndex = EEPROM.read(i + EFFECT_EEPROM_PATTERN);
	effectColorStep 		= EEPROM.read(i + EFFECT_EEPROM_STEP);
	
	if ((effectMode & EFFECT_MODE_COLOR_MASK) == EFFECT_MODE_COLOR_CONST)
		effectColorRGB(	EEPROM.read(i + EFFECT_EEPROM_COLOR + 0),
						EEPROM.read(i + EFFECT_EEPROM_COLOR + 1),
						EEPROM.read(i + EFFECT_EEPROM_COLOR + 2));
	effectInit();
}
/*
void eepromPrintEffect()
{
	uint8_t tmp;
	Serial.println();
	for (uint16_t i = EEPROM_EFFECT_START; i < EEPROM_EFFECT_END - EEPROM_EFFECT_LEN; i++)
	{
		tmp = EEPROM.read(i);
		Serial.print("0x");
		if (tmp < 10) Serial.print("0");
		Serial.print(tmp, HEX);
		Serial.print(", ");
	}
	Serial.println();
}
*/
/*

	for (uint16_t i = EEPROM_EFFECT_START; i < EEPROM_EFFECT_END - EEPROM_EFFECT_LEN; i+= EEPROM_EFFECT_LEN)
	{
		EEPROM.write(i + EFFECT_EEPROM_INDEX	, 0);
		EEPROM.write(i + EFFECT_EEPROM_SPEED	, 2);
		EEPROM.write(i + EFFECT_EEPROM_MODE		, 0);
		EEPROM.write(i + EFFECT_EEPROM_PATTERN	, 0);
		EEPROM.write(i + EFFECT_EEPROM_STEP		, 1);
		EEPROM.write(i + EFFECT_EEPROM_COLOR + 0, 255);
		EEPROM.write(i + EFFECT_EEPROM_COLOR + 1, 0);
		EEPROM.write(i + EFFECT_EEPROM_COLOR + 2, 0);
	}


void eepromSaveEffectSpeed()
{
	EEPROM.write(EEPROM_EFFECT_START + effectIndex * EEPROM_EFFECT_LEN + EFFECT_EEPROM_SPEED, effectSpeedIndex);
}

void eepromSaveEffectMode()
{
	EEPROM.write(EEPROM_EFFECT_START + effectIndex * EEPROM_EFFECT_LEN + EFFECT_EEPROM_MODE, effectMode);
}

void eepromSaveEffectPattern()
{
	EEPROM.write(EEPROM_EFFECT_START + effectIndex * EEPROM_EFFECT_LEN + EFFECT_EEPROM_PATTERN, effectColorPatternIndex);
}

void eepromSaveEffectStep()
{
	EEPROM.write(EEPROM_EFFECT_START + effectIndex * EEPROM_EFFECT_LEN + EFFECT_EEPROM_STEP, effectColorStep);
}

void eepromSaveEffectColor()
{
	uint16_t i = EEPROM_EFFECT_START + effectIndex * EEPROM_EFFECT_LEN + EFFECT_EEPROM_COLOR;
	EEPROM.write(i + 0, effectColor[0]);
	EEPROM.write(i + 1, effectColor[1]);
	EEPROM.write(i + 2, effectColor[2]);
}
*/

//}

//{ Oters

void powerOn(bool msg)
{
	if (mainMode & MAIN_MODE_POWER) return;
	mainMode |= MAIN_MODE_POWER;
	if (mainMode & MAIN_MODE_RUN_EFFECT)
		effectUpdateLEDs();
	if (msg) msgSendSettings();
}
void powerOff(bool msg)
{
	if (!(mainMode & MAIN_MODE_POWER)) return;
	fillLedsBlack();
	updateLEDs();
	mainMode &= ~MAIN_MODE_POWER;
	if (msg) msgSendSettings();
}

void fillLedsBlack()
{
	memset(ledBuff3, 0, LED_BUFF_LEN * 2);
}

void fillLedsWhite()
{
	memset(ledBuff3, COLOR_MAX, LED_BUFF_LEN * 2);
}

void fillLeds(const uint8_t* rgbBuff)
{
	for (uint8_t i = 0; i < LED_BUFF_LEN; i += RGB_COUNT)
	{
		if (mainScreen & SCREEN_PRIMARY)	memcpy(ledBuff3 + i, rgbBuff, RGB_COUNT);
		if (mainScreen & SCREEN_SECONDARY)	memcpy(ledBuff3 + LED_BUFF_LEN + i, rgbBuff, RGB_COUNT);
	}
}

void fillLedBlack(uint8_t index)
{
	if (mainScreen & SCREEN_PRIMARY)	memset(ledBuff3 + index * RGB_COUNT, 0, RGB_COUNT);
	if (mainScreen & SCREEN_SECONDARY)	memset(ledBuff3 + LED_BUFF_LEN + index * RGB_COUNT, 0, RGB_COUNT);
}

void blinkLeds()
{
	fillLedsWhite();
	updateLEDs();
	fillLedsBlack();
	delay(6);
	updateLEDs();
	delay(2);
}

//}

//{ Refreshing

void updateLEDs()
{
	if ((mainMode & MAIN_MODE_POWER) == 0) return;
	sendArrToLEDs(ledBuff3);
}

void sendArrToLEDs(uint8_t *data)
{
	cli(); // Disable global interrupts
	uint8_t curByte, ctr, maskLo, maskHi = _BV(DATA_PIN), curLed = LED_COUNT * 2;
	maskLo =~ maskHi & DATA_PORT;
	maskHi |= DATA_PORT;
	
	#define SEND_LED_BYTE \
		asm volatile \
		( \
			"		ldi %0,8	\n\t" \
			"loop%=:out %2, %3	\n\t" \
			"lsl	%1			\n\t" \
			"dec	%0			\n\t" \
			"		rjmp .+0	\n\t" \
			"		brcs .+2	\n\t" \
			"		out %2,%4	\n\t" \
			"		rjmp .+0	\n\t" \
			"		nop			\n\t" \
			"		out %2,%4	\n\t" \
			"		breq end%=	\n\t" \
			"		rjmp .+0	\n\t" \
			"		rjmp .+0	\n\t" \
			"		rjmp .+0	\n\t" \
			"		rjmp loop%=	\n\t" \
			"end%=:				\n\t" \
			:	"=&d" (ctr)			  \
			:	"r" (curByte), "I" (_SFR_IO_ADDR(DATA_PORT)), "r" (maskHi), "r" (maskLo) \
		);	

	while (curLed--) 
	{
		curByte = *(data + 1); // G
		SEND_LED_BYTE
		curByte = *data++;     // R
		SEND_LED_BYTE
		curByte = *(++data)++; // B
		SEND_LED_BYTE
	}
	sei(); // Enable global interrupts
}

void sendOffToLEDs()
{
	cli(); // Disable global interrupts
	uint8_t curByte = 0, ctr, maskLo, maskHi = _BV(DATA_PIN), curLed = LED_COUNT * RGB_COUNT;
	maskLo =~ maskHi & DATA_PORT;
	maskHi |= DATA_PORT;
	
	#define SEND_LED_BYTE \
		asm volatile \
		( \
			"		ldi %0,8	\n\t" \
			"loop%=:out %2, %3	\n\t" \
			"lsl	%1			\n\t" \
			"dec	%0			\n\t" \
			"		rjmp .+0	\n\t" \
			"		brcs .+2	\n\t" \
			"		out %2,%4	\n\t" \
			"		rjmp .+0	\n\t" \
			"		nop			\n\t" \
			"		out %2,%4	\n\t" \
			"		breq end%=	\n\t" \
			"		rjmp .+0	\n\t" \
			"		rjmp .+0	\n\t" \
			"		rjmp .+0	\n\t" \
			"		rjmp loop%=	\n\t" \
			"end%=:				\n\t" \
			:	"=&d" (ctr)			  \
			:	"r" (curByte), "I" (_SFR_IO_ADDR(DATA_PORT)), "r" (maskHi), "r" (maskLo) \
		);	

	while (curLed--) 
	{
		SEND_LED_BYTE
	}
	sei(); // Enable global interrupts
}

//}