//{ Consts And Defines

	//#define IS_LEFT_STAND
	//#define DEBUG
	
#ifdef DEBUG
	#define SERIAL_BAUDRATE	(115200)
#else
	#define SERIAL_BAUDRATE	(9600)
#endif
	#define SERIAL_TIMEOUT	(200)
	
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
	
	#define MSG_LEN			(3)
	#define MSG_LEDS_LSB	(0)
	#define MSG_LEDS_MSB	(1)
	#define MSG_COMMAND		(2)

	#define CMD_LEFT_SIDE	(0x40)
	#define CMD_RIGHT_SIDE	(0x80)
	#define CMD_INDEX_MASK	(0x3F)
	
	#define IR_CODE_10W_MASK (0x00FF0000)
	#define IR_CODE_40W_MASK (0x00F70000)
	
	#define NEC_HDR_MARK	(9000)
	#define NEC_HDR_SPACE	(4500)
	#define NEC_BIT_MARK	(560)
	#define NEC_ONE_SPACE	(1600)
	#define NEC_ZERO_SPACE	(560)
	#define NEC_RPT_SPACE	(2250)
	#define NEC_BITS		(32)
	#define TOPBIT			(0x80000000)
	
	const uint16_t IR_CODES_B[] = {
		0xD02F, 0xF00F, 0xC837, 0xE817,	// Flash Strobe Fade Smooth
		0x40BF, 0x807F, 0x00FF, 0xC03F,	// OFF B+ B- ON
		0x20DF, 0xA05F, 0x609F, 0xE01F,	// R G B W
		0xA857,	0x6897,	0x28D7,			// C M Y
		0x10EF, 0x30CF, 0x08F7,			// Red - Yellow
		0x906F, 0xB04F, 0x8877,			// Green - Cyan
		0x50AF, 0x708F, 0x48B7,			// Blue - Magenta
	};

#ifdef IS_LEFT_STAND
	const uint16_t IR_CODES_A[] = {
		0xB24D, 0x00FF, 0x58A7, 0x30CF,	// Flash Strobe Fade Smooth
		0xF807, 0xB847, 0x906F, 0xB04F,	// OFF B+ B- ON
		0x9867, 0xD827, 0x8877, 0xA857,	// R G B W
		0x28D7, 0xF00F, 0x38C7, 		// C M Y
		0xE817, 0x02FD, 0x50AF,			// Red - Yellow
		0x48B7, 0x32CD, 0x7887,			// Green - Cyan
		0x6897, 0x20DF, 0x708F,			// Blue - Magenta		
	};
#endif	
	
//}

//{ Veraibles

	uint8_t maskC, maskD;
	
//}

//{ Main and Loop

void setup() {
	DDRC	|= 0x0F;	// (A0 - A3)	OUTPUT
	PORTC	|= 0x0F;	// (A0 - A3)	HIGH	
	DDRD	|= 0xFC;	// (2 - 7)		OUTPUT
	PORTD	|= 0xFC;	// (2 - 7)		HIGH
	
	Serial.begin(SERIAL_BAUDRATE);
	Serial.setTimeout(SERIAL_TIMEOUT); // Using Serial.readBytesUntil()
}

void loop() {
	uint8_t buff[MSG_LEN];

	while(true) {
		if (Serial.available()) {
			if (MSG_LEN == Serial.readBytes(buff, MSG_LEN)) {
#ifdef IS_LEFT_STAND
				if (buff[MSG_COMMAND] & CMD_LEFT_SIDE) {
					irClick((uint16_t)buff[MSG_LEDS_MSB] << 8 | buff[MSG_LEDS_LSB], buff[MSG_COMMAND] & CMD_INDEX_MASK);
				}
#else
				if (buff[MSG_COMMAND] & CMD_RIGHT_SIDE) {
					irClick((uint16_t)buff[MSG_LEDS_MSB] << 8 | buff[MSG_LEDS_LSB], buff[MSG_COMMAND] & CMD_INDEX_MASK);
				}
#endif
			}
		}
	}
}

//}

//{ IR Funcions

void irClick(uint16_t leds, uint8_t code) {
	maskC = leds & 0x0F;
	maskD = leds & 0xF0;
#ifdef IS_LEFT_STAND
	irSend(IR_CODES_A[code] | IR_CODE_10W_MASK);
#else
	irSend(IR_CODES_B[code] | IR_CODE_10W_MASK);
#endif
	maskC = 0;
	maskD = (leds >> 6) & 0x0C;
	irSend(IR_CODES_B[code] | IR_CODE_40W_MASK);
}

void irSend(uint32_t data) {
	irBlink(NEC_HDR_MARK, NEC_HDR_SPACE);
	for (uint8_t i = 0; i < NEC_BITS; i++) {
		irBlink(NEC_BIT_MARK, (data & TOPBIT ? NEC_ONE_SPACE : NEC_ZERO_SPACE));
		data <<= 1;
	}
	irBlink(NEC_BIT_MARK, 0);
}

void irBlink(uint16_t mark, uint16_t space) {
	PORTC	&= ~maskC;
	PORTD	&= ~maskD;
	delayMicroseconds(mark);
	PORTC	|= maskC;
	PORTD	|= maskD;
	delayMicroseconds(space);
}

//}
