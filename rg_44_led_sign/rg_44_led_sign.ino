/*
	44 RG LEDs V0.0.0.1 Software!
	==========================
	Copyright (C) 2013 Arad Eizen (A.E Tec) LTD INC.
	This Program Makes The 44 RG LEDs Blink In Many Efects!
	And Controls The HV Lamps And Windwo Motor Via I2C.
	Main Board: Arduino Pro Mini.
	Language: cxx
*/
#include <Wire.h>

const int KEY1_PIN       = 2;		  // 
const int KEY2_PIN       = 3;		  // 
// RG LEDs Pins          = [4-11]     // {R1, R2, R3, R4, G1, G2, G3, G4}
const int HV1_PIN        = 12;		  // White Lamp (Activ LOW)
const int HV2_PIN        = 13;		  // Blue Lamp (Activ LOW)
//const int HV_EN_PIN      = 14;		  // HV Enable (Activ HIGH)
const int MOT1_PIN       = 14;		  // Motor Left (Activ HIGH)
const int MOT2_PIN       = 15;		  // Motor Right (Activ HIGH)

const byte I2C_ADDRESS 	 = 4;		  // 

const byte KEY_1     	 = 1;		  // 
const byte KEY_2      	 = 2;		  // 
const byte KEY_LONG      = 128;		  // Long Key Press Index
const byte KEY_1_LONG = KEY_1 + KEY_LONG;
const byte KEY_2_LONG = KEY_2 + KEY_LONG;
const byte KEY_NONE      = 255;		  // No Key Is Pressed Index

const byte LED_OFFSET	 = 4;		  // RG LEDs (4 - 11)
const byte LED_COUNT	 = 8;		  // (4-7) Green, (8-11) Red

const byte MODE_OFF      = 0;		  // All LEDs Off
const byte MODE_MANUAL   = 1;		  // Manual Mode Index
const byte MODE_NORMAL   = 2;		  // Automatic Mode Index
const byte MODE_RANDOM   = 3;		  // Random Mode Index

const byte A_LED_MODE    = 1;		  // 
const byte A_LED_SPEED   = 2;		  // 
const byte A_LED_EFECT   = 3;		  // 
const byte A_LED_VALUE   = 4;		  // 
const byte A_HV1_ON      = 5;		  // 
const byte A_HV1_OFF     = 6;		  // 
const byte A_HV2_ON      = 7;		  // 
const byte A_HV2_OFF     = 8;		  // 
const byte A_MOT_UP      = 9;		  // 
const byte A_MOT_DOWN    = 10;		  // 
const byte A_MOT_STOP    = 11;		  // 

const byte MAX_SPEED     = 12;		  // speeds Lenght
const byte MAX_EFECT 	 = 29;		  // efects Lenght

const int KEY_LONG_P    = 4500;		  // Key Long Press Timout
const int KEY_SHORT_P   = 300;		  // Key short Press Timout

const int speeds[MAX_SPEED] = {1, 2, 5, 8, 10, 14, 18,
							   25, 30, 40, 50, 100};
							   
const byte efects[MAX_EFECT][9] =	// Efect = {LEN, STEP1, [STEP2], [STEP3], [STEPn]}
{	
	{4, 0x01, 0x02, 0x04, 0x08}, {4, 0x08, 0x04, 0x02, 0x01},
	{4, 0x10, 0x20, 0x40, 0x80}, {4, 0x80, 0x40, 0x20, 0x10},
	{4, 0x03, 0x06, 0x0C, 0x09}, {4, 0x09, 0x0C, 0x06, 0x03},
	{4, 0x30, 0x60, 0xC0, 0x90}, {4, 0x90, 0xC0, 0x60, 0x30},
	{2, 0x0F, 0x00}, {2, 0xF0, 0x00}, {2, 0xA5, 0x5A},
	{8, 0x01, 0x10, 0x02, 0x20, 0x04, 0x40, 0x08, 0x80},
	{4, 0x07, 0x0E, 0x0D, 0x0B}, {4, 0x0B, 0x0D, 0x0E, 0x07},
	{4, 0x70, 0xE0, 0xD0, 0xB0}, {4, 0xB0, 0xD0, 0xE0, 0x70},
	{4, 0x36, 0x6C, 0xC9, 0x93}, {4, 0x93, 0xC9, 0x6C, 0x36},
	{4, 0x10, 0x22, 0x04, 0x88}, {4, 0x88, 0x04, 0x22, 0x10},
	{4, 0x01, 0x20, 0x04, 0x80}, {4, 0x80, 0x04, 0x20, 0x01},
	{8, 0x18, 0x28, 0x48, 0x88, 0x81, 0x82, 0x84, 0x88},
	{4, 0x1E, 0x2D, 0x4B, 0x87}, {4, 0x87, 0x4B, 0x2D, 0x1E},
	{4, 0xE1, 0xD2, 0xB4, 0x78}, {4, 0x78, 0xB4, 0xD2, 0xE1},
	{4, 0x50, 0xA0, 0x05, 0x0A}, {3, 0x0F, 0xFF, 0xF0}
};

long old_millis;
int key_duration;
int efect_speed;
int speed_pos;

byte key_index = KEY_NONE;
byte efect_pos   = 1;
byte efect_index = 0;
byte speed_index = 4;
byte efect_length;
byte mode = MODE_OFF;
byte key_old;

bolean hv1_state = false;
bolean hv2_state = false;

byte hv1_pos;
byte hv2_pos;

byte hv1_on;
byte hv2_on;
byte hv1_off;
byte hv2_off;

void setup() // Main
{
/*
	DDRD  |= 0xFC;  					// (2 - 7)  OUTPUT
	PORTD &= 0x03; 						// (2 - 7)  LOW
	DDRB  |= 0x0F;  					// (8 - 11) OUTPUT
	PORTB &= 0xF0; 						// (8 - 11) LOW
	Serial.begin(9600);
*/
	for (byte i = LED_OFFSET; i < LED_COUNT + LED_OFFSET; i++)
	{
		pinMode(i, OUTPUT);				// OUTPUT
		digitalWrite(i, LOW);			// Set LED Off
	}
	pinMode(HV1_PIN,  OUTPUT);			// OUTPUT
	pinMode(HV2_PIN,  OUTPUT);			// OUTPUT
	pinMode(MOT1_PIN, OUTPUT);			// OUTPUT
	pinMode(MOT2_PIN, OUTPUT);			// OUTPUT	
	pinMode(KEY1_PIN, INPUT_PULLUP);	// INPUT
	pinMode(KEY2_PIN, INPUT_PULLUP);	// INPUT
	
	digitalWrite(HV1_PIN, LOW);			// Set Lamp Off
	digitalWrite(HV2_PIN, LOW);			// Set Lamp Off
	digitalWrite(MOT1_PIN, LOW);		// Set Lamp Off
	digitalWrite(MOT2_PIN, LOW);		// Set Lamp Off
	
	efect_speed  = speeds[speed_index];
	efect_length = efects[efect_index][0];
	
	delay(2000); 						// Time For Master SR To Init
	Wire.begin(I2C_ADDRESS);			// join i2c bus with address #4
	Wire.onReceive(receiveEvent);		// register event
}

void loop() // Infinity Loop
{
	long tmp_millis = millis() / 10;
	byte delta_millis = abs(tmp_millis - old_millis);
	byte key_cur = (~(PIND >> 2)) & 0x03;
	
	if (delta_millis)
	{
		old_millis = tmp_millis;
		switch (mode)
		{
			case MODE_NORMAL:
				if (speed_pos) speed_pos--;
				else
				{
					speed_pos = efect_speed;
					SetLEDs(efects[efect_index][efect_pos]);
					if (++efect_pos > efect_length) efect_pos = 1;
				}
			break;
			case MODE_RANDOM:
				if (random(400) == 0)
				{
					efect_index = random(MAX_EFECT);
					RefEfect();
				}
			break;
		}
	}

	if(key_cur == key_old)
	{
		if(key_cur) if (key_duration) key_duration--;
	}
	else
	{
		if((key_duration < (KEY_LONG_P - KEY_SHORT_P)) && (key_cur == 0))
		{
			key_index = key_old;
			if(key_duration == 0) key_index |= KEY_LONG;
		}
		key_duration = KEY_LONG_P;
		key_old = key_cur;
	}

	if (mode)
	{
		switch (key_index)
		{
			case KEY_1:
				efect_change = false;
				if (++efect_index == MAX_EFECT) efect_index = 0;
				RefEfect();
			break;
			case KEY_2:
				if (++speed_index == MAX_SPEED) speed_index = 0;
				speed_pos = efect_speed = speeds[speed_index];			
			break;
			case KEY_1_LONG:
				if (mode == MODE_NORMAL)
				{
					mode = MODE_RANDOM;
					efect_index = random(MAX_EFECT);
					RefEfect();
				}
				else mode = MODE_NORMAL;
			break;
			case KEY_2_LONG:
				mode = MODE_OFF;
				SetLEDs(0);
			break;			
		}
	}
	else
	{
		if ((key_index == KEY_2) || (key_index == KEY_2_LONG)) mode = MODE_NORMAL;
	}

	if (key_index < KEY_NONE)
	{
		key_index = KEY_NONE;
		//Bip(KEY_BIP);
	}
}


void receiveEvent(int howMany)
{
	if (Wire.available() != 2) return;
	byte addres = Wire.read();
	byte value  = Wire.read();

	switch (addres)
	{
		case A_LED_MODE:
			if (value > MODE_RANDOM) return;
			mode = value;
		break;
		case A_LED_SPEED:
			speed_pos = efect_speed = value;
		break;
		case A_LED_EFECT:
			if (value >= MAX_EFECT) return;
			efect_index = value;
			RefEfect();
		break;
		case A_LED_VALUE:
			SetLEDs(value);
		break;
		case A_HV1_ON:
			hv1_on  = value;
			if (value == 0) 
			{
				hv1_state = false;
				digitalWrite(HV1_PIN, LOW);
			}
		break;
		case A_HV1_OFF:
			hv1_off = value;
		break;
		case A_HV2_ON:
			hv2_on  = value;
			if (value == 0) 
			{
				hv2_state = false;
				digitalWrite(HV2_PIN, LOW);
			}
		break;
		case A_HV2_OFF:
			hv2_off = value;
		break;
		case A_MOT_UP:

		break;
		case A_MOT_DOWN:
	
		break;
		case A_MOT_STOP:

		break;
	}
}

void RefEfect()
{
	efect_length = efects[efect_index][0];
	efect_pos = 1;
}

void SetLEDs(byte data)
{
	PORTD = (PORTD & 0x0F) | (data << 4);
	PORTB = (PORTB & 0xF0) | (data >> 4);
}