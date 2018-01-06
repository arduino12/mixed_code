//{ Consts And Defines

	//#define DEBUG

#ifdef DEBUG
	#define SERIAL_BAUDRATE	(115200)
#else
	#define SERIAL_BAUDRATE	(9600)
#endif

	#define TEST_INT	(0)
	#define TEST_PIN	(2)
	#define SIDE_PIN	(A6)
	#define MODE_PIN	(A7)
//	#define CMD_PIN		(8)

	#define LED_1	(0x0001)
	#define LED_2	(0x0002)
	#define LED_3	(0x0004)
	#define LED_4	(0x0008)
	#define LED_5	(0x0010)
	#define LED_6	(0x0020)
	#define LED_7	(0x0040)
	#define LED_8	(0x0080)
	#define LED_9	(0x0100)
	#define LED_10	(0x0200)
	
	#define MATRIX_KEY_NONE				(0xFF)
	#define MATRIX_KEY_SHORT_PRESS_MS	(60)
	#define MATRIX_KEY_PRESSED_MS		(MATRIX_KEY_SHORT_PRESS_MS + 10)
	#define MATRIX_KEY_LONG_PRESS_MS	(1000)
	
	#define KEY_COLORS		(15)
	#define COLORS_COUNT	(9)
	
	#define MSG_LEN			(3)
	#define MSG_LEDS_LSB	(0)
	#define MSG_LEDS_MSB	(1)
	#define MSG_COMMAND		(2)

	#define CMD_LEFT_SIDE	(0x40)
	#define CMD_RIGHT_SIDE	(0x80)
	#define CMD_INDEX_MASK	(0x3F)
	
	const uint16_t LED_MODES[] =  {
		LED_1 | LED_2 | LED_3 | LED_4 | LED_5 | LED_6 | LED_7 | LED_8,
		LED_1 | LED_2 | LED_3 | LED_4,
		LED_1 | LED_2 | LED_5 | LED_6,
		LED_1 | LED_5 |LED_9,
		LED_1 | LED_2 | LED_5 | LED_6 | LED_9,
		LED_9,
		LED_1 | LED_2 | LED_3 | LED_4 | LED_5 | LED_6 | LED_7 | LED_8 | LED_9 | LED_10,
		LED_10,
		LED_3 | LED_4 | LED_7 | LED_8 | LED_10,
		LED_4 | LED_8 | LED_10,
		LED_3 | LED_4 | LED_7 | LED_8,
		LED_5 | LED_6 | LED_7 | LED_8,
	};
	
//}

//{ Veraibles

	volatile uint8_t is_test;
	
//}

//{ Main and Loop

void setup() {
	DDRB  &= 0xC3;  					// (10 - 13)	INPUT
	PORTB |= 0x3C; 						// (10 - 13)	PULLUP
	DDRC  |= 0x0F;  					// (A0 - A3)	OUTPUT
	
	pinMode(CMD_PIN, OUTPUT);			// (8)			OUTPUT
	digitalWrite(CMD_PIN, true);		// (8)			HIGH
	pinMode(TEST_PIN, INPUT_PULLUP);	// (2)			INPUT PULLUP
	attachInterrupt(TEST_INT, intTest, FALLING); // FALLING CHANGE RISING

	is_test = false;
	
	Serial.begin(SERIAL_BAUDRATE);
	
#ifdef DEBUG
	Serial.println(F("Start!"));
#endif
}

void loop() {
	uint8_t buff[MSG_LEN];
	uint8_t clicked_key;
	uint8_t cur_matrix_key;
	uint8_t old_matrix_key;
	uint8_t key_color = 0;
	uint8_t side;
	//uint8_t old_side = 0;
	uint16_t leds;
	uint32_t matrix_key_ms;
	uint32_t cur_ms, delta_ms, old_ms = millis();

	while(true) {
		cur_ms		= millis();
		delta_ms	= cur_ms - old_ms;
		old_ms		= cur_ms;	
		
		cur_matrix_key = get_matrix_key();
		
		if (old_matrix_key != cur_matrix_key) {
			matrix_key_ms = 0;
			old_matrix_key = cur_matrix_key;
		}
		
		if (MATRIX_KEY_NONE != cur_matrix_key) {
			matrix_key_ms += delta_ms;
			if (((MATRIX_KEY_SHORT_PRESS_MS < matrix_key_ms) && (MATRIX_KEY_PRESSED_MS > matrix_key_ms)) || (MATRIX_KEY_LONG_PRESS_MS < matrix_key_ms)) {
				matrix_key_ms = MATRIX_KEY_PRESSED_MS;
				clicked_key = cur_matrix_key;
				if (KEY_COLORS == clicked_key) {
					clicked_key += key_color;
					if (++key_color >= COLORS_COUNT)
						key_color = 0;
				}
			}
		}
		
		if (MATRIX_KEY_NONE != clicked_key) {
#ifdef DEBUG
			Serial.print(F("Key: "));
			Serial.println(clicked_key);
			Serial.print(F("Mode: "));
			Serial.println(get_mode_index());
#else
			leds = LED_MODES[get_mode_index()];
			side = get_side_mask() | clicked_key;
			buff[MSG_LEDS_LSB] = leds;
			buff[MSG_LEDS_MSB] = leds >> 8;
			buff[MSG_COMMAND] = side;
			
			Serial.write(buff, MSG_LEN);
			/*
			if (CMD_LEFT_SIDE & side)
			{
				send_side(CMD_LEFT_SIDE, buff);
			}
			
			if (CMD_RIGHT_SIDE & side)
			{
				send_side(CMD_RIGHT_SIDE, buff);
			}*/
#endif
			clicked_key = MATRIX_KEY_NONE;
		}
			
		if (is_test) {
#ifdef DEBUG
			Serial.println(F("Test!"));
			delay(1000);
#else
			for (leds = LED_1; leds <= LED_10; leds <<= 1) {
				buff[MSG_LEDS_LSB] = leds;
				buff[MSG_LEDS_MSB] = leds >> 8;			
				for (uint8_t i = 8; i < 11; i++) {
					buff[MSG_COMMAND] = get_side_mask() | i;
					Serial.write(buff, MSG_LEN);
					delay(300);
				}
				buff[MSG_COMMAND] = get_side_mask() | 4;
				Serial.write(buff, MSG_LEN);
			}
#endif
			is_test = false;
		}
	}
}

//}

//{ Oters
void send_side(uint8_t side, uint8_t * buff) {
	uint8_t AT_ID[] = "AT+ID=0000";
	#define AT_ID_LEN (10)

	if (CMD_RIGHT_SIDE & side)
		AT_ID[AT_ID_LEN - 1]++; // ID=0001
	digitalWrite(CMD_PIN, false);
	delay(50);
	Serial.write(AT_ID, AT_ID_LEN);
	delay(50);
	if ('Y' != Serial.read())
		return;
	digitalWrite(CMD_PIN, true);
	delay(50);
	Serial.write(buff, MSG_LEN);
	delay(50);
}

uint8_t get_side_mask() {
	uint8_t value = analogRead(SIDE_PIN) / 10;
	if (value < 30)	return CMD_LEFT_SIDE;
	if (value < 70)	return CMD_LEFT_SIDE | CMD_RIGHT_SIDE;
	return CMD_RIGHT_SIDE;
}

uint8_t get_mode_index() {
	uint8_t value = analogRead(MODE_PIN) / 10;
	if (value < 7)	return 0;
	if (value < 18)	return 1;
	if (value < 27)	return 2;
	if (value < 37)	return 3;
	if (value < 46)	return 4;
	if (value < 55)	return 5;
	if (value < 64)	return 6;
	if (value < 72)	return 7;
	if (value < 80)	return 8;
	if (value < 88)	return 9;
	if (value < 98)	return 10;
	return 11;
}

uint8_t get_matrix_line(uint8_t mask) {
	PORTC &= 0xF0;
	PORTC |= mask;
	delay(1);
	return (PINB >> 2) & 0xF;
}

uint8_t get_matrix_key() {
	uint8_t line;
	
	line = get_matrix_line(0x07);
	if (0x7 == line)	return 4;
	if (0xB == line)	return 5;
	if (0xD == line)	return 6;
	if (0xE == line)	return 7;

	line = get_matrix_line(0x0B);
	if (0x7 == line)	return 8;
	if (0xB == line)	return 9;
	if (0xD == line)	return 10;
	if (0xE == line)	return 11;

	line = get_matrix_line(0x0D);
	if (0x7 == line)	return 12;
	if (0xB == line)	return 13;
	if (0xD == line)	return 14;
	if (0xE == line)	return KEY_COLORS;

	line = get_matrix_line(0x0E);
	if (0x7 == line)	return 0;
	if (0xB == line)	return 1;
	if (0xD == line)	return 2;
	if (0xE == line)	return 3;
	
	return MATRIX_KEY_NONE;
}

void intTest() {
	is_test = true;
}

//}
