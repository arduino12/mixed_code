/*
	Purpos:	Timer for oven.
	Author:	A.E Tec 14/2/15
	
		/-----------\
		| 8 8 : 8 8 |
		\-----------/
		     | |
		[^]  [+]  [-]
		SET  ADD  SUB
*/

//{ Includes
	#include <DS3232RTC.h>
	#include <Time.h>
	#include <Wire.h>
	#include <Buttons.h>
//}

//{ Defines
//#define DEBUG					// debug to uart (serial) if defined
#ifdef DEBUG
	#define DEBUG_INIT			Serial.begin(115200)
	#define DEBUG_WRITE(...)	Serial.write(__VA_ARGS__)
	#define DEBUG_PRINT(...)	Serial.print(__VA_ARGS__)
	#define DEBUG_PRINTLN(...)	Serial.println(__VA_ARGS__)
#else
	#define DEBUG_INIT
	#define DEBUG_WRITE(...)
	#define DEBUG_PRINT(...)
	#define DEBUG_PRINTLN(...)
#endif

#define HEAT_MASK			(0x08) // pin 11
#define LIGHT_MASK			(0x10) // pin 12
#define BIP_MASK			(0x20) // pin 13

#define CHIP_CLK_MASK		(0x01) // pin A0
#define CHIP_DATA_MASK		(0x02) // pin A1
#define CHIP_LOAD_MASK		(0x04) // pin A2

#define DIGIT_COUNT			(0x04)
#define SEGMENT_COUNT		(0x07)
#define BUFF_SIZE			(0x08)

#define KEY_SET				(0x01)
#define KEY_ADD				(0x02)
#define KEY_SUB				(0x04)
#define KEY_LONG			(0x08)
#define KEY_BOTH			(KEY_ADD | KEY_SUB)
#define KEY_LONG_SET		(KEY_SET | KEY_LONG)
#define KEY_LONG_ADD		(KEY_ADD | KEY_LONG)
#define KEY_LONG_SUB		(KEY_SUB | KEY_LONG)
#define KEY_LONG_BOTH		(KEY_BOTH | KEY_LONG)

#define MODE_CLOCK			(0x00)
#define MODE_SET			(0x01)
#define MODE_EFFECT			(0x02)
#define MODE_TIMER			(0x03)
#define MODE_FINISH			(0x04)

#define DOT_U_MASK			(0x01)
#define DOT_D_MASK			(0x02)
#define DOT_BOTH_MASK		(DOT_U_MASK | DOT_D_MASK)

#define SMALL_SET_CHANGE	(1)
#define BIG_SET_CHANGE		(20)
#define SMALL_TIMER_CHANGE	(5)
#define BIG_TIMER_CHANGE	(30)

#define BIP_ON_MS			(150)

#define KEY_SEP_MS			(10)
#define KEY_SHORT_ON_MS		(60)
#define KEY_LONG_ON_MS		(700)
#define KEY_LONG_OFF_MS		(450)

#define MIN_PER_HOUR		(60)
#define MIN_PER_DAY			(SECS_PER_DAY / SECS_PER_MIN)

#define DEFAULTE_TIMER		(10)
#define CLOCK_DOT_MS		(650)

#define MESSAGE_COUNT		(15)
#define MESSAGE_SPEED_MS	(350)

#define pgm_read_ptr(address)	(void *)__LPM_word((uint16_t)(address))
#define PARRAY(name, ...)		const name[] PROGMEM = {__VA_ARGS__}
#define PCHARS(name, ...)		char PARRAY(name, __VA_ARGS__)


const uint8_t FONT[] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,	// 0,1,2,3,4,5,6,7,8,9
	0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76,	0x30, 0x0E,	// a,b,c,d,e,f,g,h,i,j
	0x00, 0x38, 0x00, 0x54, 0x3F, 0x73, 0x67, 0x50, 0x6D, 0x78,	// k,l,m,n,o,p,q,r,s,t
	0x3E, 0x1C, 0x00, 0x00, 0x6E, 0x5C, 0x08, 0x40, 0x01, 0x00,	// u,v,w,x,y,z,_,-,^, 
};

PCHARS(TEXT_00_P, "arad is the best");
PCHARS(TEXT_01_P, "chef dallia presents");
PCHARS(TEXT_02_P, "love you");
PCHARS(TEXT_03_P, "fast-food");
PCHARS(TEXT_04_P, "good-job");
PCHARS(TEXT_05_P, "bon-appetit");
PCHARS(TEXT_06_P, "enjoy eating");
PCHARS(TEXT_07_P, "not cold");
PCHARS(TEXT_08_P, "redy and done");
PCHARS(TEXT_09_P, "be careful -hot-");
PCHARS(TEXT_10_P, "delicious taste");
PCHARS(TEXT_11_P, "be hungry");
PCHARS(TEXT_12_P, "-arduino-");
PCHARS(TEXT_13_P, "^^a-e-tec_2015^^");
PCHARS(TEXT_14_P, "_-^-_-^-_-^-_-^-_-^-_-^-_");

// char * PARRAY(TEXTS_ARRAY_PP,
PGM_P PARRAY(TEXTS_ARRAY_PP,
	TEXT_00_P, TEXT_01_P, TEXT_02_P, TEXT_03_P, TEXT_04_P,
	TEXT_05_P, TEXT_09_P, TEXT_07_P, TEXT_08_P, TEXT_09_P,
	TEXT_10_P, TEXT_11_P, TEXT_12_P, TEXT_13_P, TEXT_14_P
);
//}

Buttons buttons;

//{ Main and Loop
void setup() {
	DEBUG_INIT;
	DEBUG_PRINTLN(F("setup!"));
	//delay(2000);
	DDRC  |= 0x07;				// (A0 - A2)	OUTPUT
	PORTC &= 0xF8;				// (A0 - A2)	LOW
	
	DDRB  &= 0xF8;				// (8 - 10)		INPUT
	PORTB |= 0x07;				// (8 - 10)		PULLUP
	
	DDRB  |= 0x38;				// (11 - 13)	OUTPUT
	PORTB &= 0xE7;				// (11 - 12)	LOW
	PORTB |= 0x20;				// (13)			HIGH
	
	max7219_send(9, 0);			// set matrix Decode Mode
	max7219_send(10, 15);		// set max intensity (1-15)
	max7219_send(11, 7);		// set max scan limit
	max7219_send(12, 1);		// cancel shutdown Mode	
	
	//for (uint8_t i = 1; i < 9; i++)
	//	sendMax7219(i,	0);		// clear all segments
	
	// TODO...
	buttons.begin();
	
	setSyncProvider(RTC.get);
	
	DEBUG_PRINTLN(F("loop!"));
}

void loop() {
	buttons.update();
	uint8_t i, curMode = MODE_CLOCK, dots = 0, curKey, oldKey = 0, tmpKey, msgIndex = 0, msgPosition = 0;
	uint8_t buffer[8] = {0};
	char textBuff[32] = {0};
	uint16_t curTime, oldTime = 0, timerTime;
	uint32_t curMs, oldMs = 0xFFFF, deltaMs, deltaKeyMs, keyMs, bipOnMs = 0, bipOffMs;
	
	sprint_message(textBuff, msgIndex);
	for (;;) {
		curMs = millis();
		deltaMs = curMs - oldMs;
		tmpKey = ~PINB & (KEY_SET | KEY_BOTH);
		
		curKey = 0;
		if (oldKey != tmpKey) {
			if (tmpKey < oldKey) {
				deltaKeyMs = curMs - keyMs;
				if ((KEY_SHORT_ON_MS < deltaKeyMs) && ((KEY_LONG_ON_MS - KEY_LONG_OFF_MS) > deltaKeyMs)) {
					curKey = oldKey;
				}
				max7219_send(9, 0);			// set matrix Decode Mode
				max7219_send(10, 15);		// set max intensity (1-15)
				max7219_send(11, 7);		// set max scan limit
				max7219_send(12, 1);		// cancel shutdown Mode					
			}
			keyMs = curMs;
			oldKey = tmpKey;
		}		
		
		if (tmpKey) {
			deltaKeyMs = curMs - keyMs;
			if ((KEY_LONG_ON_MS < deltaKeyMs) && ((KEY_LONG_OFF_MS + KEY_LONG_ON_MS) > deltaKeyMs)) {
				curKey = tmpKey | KEY_LONG;
				if ((KEY_ADD == tmpKey) || (KEY_SUB == tmpKey))
					keyMs += KEY_LONG_OFF_MS;
				else
					keyMs -= KEY_LONG_OFF_MS;
			}
		}

		switch (curMode) {
			case MODE_CLOCK:
				if ((KEY_ADD == curKey) || (KEY_SUB == curKey)) {
					sprint_dots(buffer, 0);
					sprint_message(textBuff, msgIndex);
					curMode = MODE_EFFECT;
					break;
				}
				
				if (KEY_LONG_SET == curKey) {
					curTime = get_current_time();
					oldTime = curTime;
					timerTime = DEFAULTE_TIMER;					
					sprint_time(buffer, timerTime);
					PORTB |= (LIGHT_MASK | HEAT_MASK);
					curMode = MODE_TIMER;
					break;
				}
				
				if (KEY_LONG_BOTH == curKey) {
					dots = DOT_U_MASK;
					curMode = MODE_SET;
					break;
				}

				if (CLOCK_DOT_MS < deltaMs) {
					oldMs = curMs;
					curTime = get_current_time();
					if (oldTime != curTime) {
						sprint_time(buffer, curTime);
						oldTime = curTime;
					}
					sprint_dots(buffer, dots);
					dots = ~dots;
				}
			break;
			case MODE_SET:
				if ((KEY_LONG_BOTH == curKey) || (KEY_SET == curKey)) {
					time_t t = RTC.get();
					t = t - (t % SECS_PER_DAY) + (t % SECS_PER_MIN) + curTime * SECS_PER_MIN;
					RTC.set(t);
					setTime(t);
					dots = 0;
					oldTime--;
					curMode = MODE_CLOCK;
					break;
				}

				if (CLOCK_DOT_MS < deltaMs) {
					oldMs = curMs;
					sprint_dots(buffer, dots);
					dots = ~dots;
				}
				
				if (0 == (KEY_BOTH & curKey)) break;
				
				if (BIG_SET_CHANGE > curTime)		curTime += MIN_PER_DAY;
				if		(KEY_ADD == curKey)			curTime += SMALL_SET_CHANGE;
				else if	(KEY_SUB == curKey)			curTime -= SMALL_SET_CHANGE;
				else if	(KEY_LONG_ADD == curKey)	curTime += BIG_SET_CHANGE;
				else if	(KEY_LONG_SUB == curKey)	curTime -= BIG_SET_CHANGE;
				if (MIN_PER_DAY <= curTime)			curTime -= MIN_PER_DAY;
				sprint_time(buffer, curTime);
			break;
			case MODE_EFFECT:
				if		(KEY_ADD == curKey)
				{
					if (++msgIndex >= MESSAGE_COUNT) msgIndex = 0;
					sprint_message(textBuff, msgIndex);
					deltaMs = MESSAGE_SPEED_MS;
					msgPosition = 0;
				}
				else if (KEY_SUB == curKey)
				{
					if (--msgIndex >= MESSAGE_COUNT) msgIndex = (MESSAGE_COUNT - 1);
					sprint_message(textBuff, msgIndex);
					deltaMs = MESSAGE_SPEED_MS;
					msgPosition = 0;
				}
				else if (curKey)
				{
					oldTime--;
					curMode = MODE_CLOCK;
					break;
				}
				
				if (MESSAGE_SPEED_MS < deltaMs) {
					oldMs = curMs;
					if (++msgPosition >= (strlen(textBuff) + 4)) msgPosition = 1;
					sprint_text(buffer, msgPosition, textBuff);
				}
			break;
			case MODE_TIMER:
				if ((KEY_LONG_BOTH == curKey) || (KEY_SET == curKey)) {
					PORTB &= ~(LIGHT_MASK | HEAT_MASK);
					oldTime--;
					curMode = MODE_CLOCK;
					break;
				}
				
				if (CLOCK_DOT_MS < deltaMs) {
					oldMs = curMs;
					sprint_dots(buffer, dots);
					dots = ~dots;
					
					curTime = get_current_time();
					if (oldTime != curTime) {
						if (0 == --timerTime) {
							PORTB &= ~HEAT_MASK;
							PORTB &= ~BIP_MASK;
							sprint_dots(buffer, 0);
							sprint_message(textBuff, msgIndex);
							curMode = MODE_FINISH;
							break;
						}
						oldTime = curTime;
						sprint_time(buffer, timerTime);
					}
				}
				
				if (0 == (KEY_BOTH & curKey)) break;
				
				if (BIG_TIMER_CHANGE > timerTime)	timerTime += MIN_PER_DAY;
				if		(KEY_ADD == curKey)			timerTime += SMALL_TIMER_CHANGE;
				else if	(KEY_SUB == curKey)			timerTime -= SMALL_TIMER_CHANGE;
				else if	(KEY_LONG_ADD == curKey)	timerTime += BIG_TIMER_CHANGE;
				else if	(KEY_LONG_SUB == curKey)	timerTime -= BIG_TIMER_CHANGE;
				if (MIN_PER_DAY <= timerTime)		timerTime -= MIN_PER_DAY;
				if (0 == timerTime)					timerTime = SMALL_TIMER_CHANGE;
				sprint_time(buffer, timerTime);
			break;
			case MODE_FINISH:
				if (curKey) {
					PORTB &= ~LIGHT_MASK;
					PORTB |= BIP_MASK;
					oldTime--;
					curMode = MODE_CLOCK;
					break;
				}
				
				if (MESSAGE_SPEED_MS < deltaMs) {
					oldMs = curMs;
					if (++msgPosition >= (strlen(textBuff) + 4)) {
						if (++msgIndex >= MESSAGE_COUNT) msgIndex = 0;
						sprint_message(textBuff, msgIndex);
						deltaMs = MESSAGE_SPEED_MS;
						msgPosition = 1;
					}
					sprint_text(buffer, msgPosition, textBuff);
				}
			break;
		}
		
		if (curKey) {
			bipOnMs = BIP_ON_MS;
			PORTB &= ~BIP_MASK;
			DEBUG_PRINTLN(curKey, HEX);
		}
		
		if (bipOnMs) {
			if (0 == --bipOnMs)
				PORTB |= BIP_MASK;
		}
		delay(1);
	}
}
//}

//{ Others
void sprint_text(uint8_t * buffer, uint8_t position, const char * text) {
	uint8_t length = strlen(text);
	
	memset(buffer, 0, BUFF_SIZE);
	
	if ((0 == position) || (0 == length)) return;

	for (uint8_t i = 0, index = position - DIGIT_COUNT; i < DIGIT_COUNT; i++, index++)
		if (index < length) sprint_char(buffer, i, text[index]);
}

void sprint_char(uint8_t * buffer, uint8_t position, uint8_t character) {
	position = 0x80 >> position;
	if		(('0' <= character) && ('9' >= character))	character -= '0';
	else if	(('a' <= character) && ('z' >= character))	character += 10 - 'a';
	else if	('_' == character)							character = 36;
	else if	('-' == character)							character = 37;
	else if	('^' == character)							character = 38;
	else												character = 39;
	
	for (uint8_t i = 0, m = 1; i < SEGMENT_COUNT; i++, m <<= 1) {
		if (FONT[character] & m)	buffer[i] |= position;
		else						buffer[i] &= ~position;
	}
		
	max7219_send_buffer(buffer);
}

void sprint_dots(uint8_t * buffer, uint8_t dots) {
	buffer += SEGMENT_COUNT;
	*buffer &= 0x9F;
	*buffer |= 0x60 & (dots << 5);
	max7219_send(BUFF_SIZE, *buffer);
}

void sprint_time(uint8_t * buffer, uint16_t minutes) {
	uint8_t hours = minutes / MIN_PER_HOUR;
	minutes %= MIN_PER_HOUR;
	
	for (uint8_t i = 0, m = 1; i < SEGMENT_COUNT; i++, m <<= 1) {
		buffer[i] = 0x00;
		if (FONT[minutes % 10] & m)						buffer[i] |= 0x10;
		if (FONT[minutes / 10 % 10] & m)				buffer[i] |= 0x20;
		if (FONT[hours % 10] & m)						buffer[i] |= 0x40;
		if (9 < hours) if (FONT[hours / 10 % 10] & m)	buffer[i] |= 0x80;
	}
		
	max7219_send_buffer(buffer);
}

uint16_t get_current_time() {
	return (now() % SECS_PER_DAY) / SECS_PER_MIN;
}

void sprint_message(char * buffer, uint8_t index) {
	strcpy_P(buffer, (char*)pgm_read_ptr(&TEXTS_ARRAY_PP[index]));
}

void max7219_send_buffer(uint8_t * buffer) {
	for (uint8_t i = 0; i < SEGMENT_COUNT; i++)
		max7219_send(i + 1, buffer[i]);
}

void max7219_send(uint8_t addres, uint8_t value) {
	uint8_t i;

	for (i = 0x00; i < 0x04; i++)
		max7219_change(false, CHIP_DATA_MASK);		// Dont care

	for (i = 0x08; i > 0x00; i >>= 1)
		max7219_change(addres & i, CHIP_DATA_MASK);	// Adresse

	for (i = 0x80; i > 0x00; i >>= 1)
		max7219_change(value & i, CHIP_DATA_MASK);	// Data

	max7219_pulse(CHIP_LOAD_MASK);
}

void max7219_pulse(uint8_t mask) {
	max7219_change(true, mask);
	max7219_change(false, mask);
}

void max7219_change(bool is_high, uint8_t mask) {
	if (is_high)	PORTC |= mask;
	else			PORTC &= ~mask;
	if (CHIP_DATA_MASK == mask) max7219_pulse(CHIP_CLK_MASK);    // clook pulse after data sent
}
//}
