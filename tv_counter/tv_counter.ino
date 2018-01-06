#include <TVout.h>
#include "arad.h"
#include "didi.h"
#include "bases.h"
#include "bin_digit.h"
#include "dec_digit.h"

#define MAX_NUMBER_DIGITS		(4)
#define MAX_COUNT				(15)
#define BASKET_TIMEOUT_MS		(100)
#define ON_TIMEOUT_MS			(8*60000)	// 8 minutes
#define ELEVATOR_TIMEOUT_MS		(30*1000)	// 30 seconds
#define GATE_OPEN_MS			(8*1000)	// 8 seconds
#define MAX_COUNT_DELAY_MS		(5*1000)	// 5 seconds

#define BUTTON_START_PIN		(A0)
#define SENSOR_BASKET_PIN		(A1)
#define RELAY_TV_PIN			(2)
#define RELAY_OPEN_GATE_PIN		(3)
#define RELAY_CLOSE_GATE_PIN	(6)
#define RELAY_ELEVATOR_PIN		(4)
#define RELAY_POWER_PIN			(5)

TVout tv;

const uint8_t * bin_digits[] = {
	bin_digit_0,
	bin_digit_1,
};
const uint8_t * dec_digits[] = {
	dec_digit_0,
	dec_digit_1,
	dec_digit_2,
	dec_digit_3,
	dec_digit_4,
	dec_digit_5,
	dec_digit_6,
	dec_digit_7,
	dec_digit_8,
	dec_digit_9,
};

void setup() {
	pinMode(BUTTON_START_PIN			,INPUT_PULLUP);
	pinMode(SENSOR_BASKET_PIN			,INPUT_PULLUP);
	pinMode(RELAY_POWER_PIN				,OUTPUT);
	digitalWrite(RELAY_POWER_PIN		,LOW);
	delay(200);
	pinMode(RELAY_TV_PIN				,OUTPUT);
	pinMode(RELAY_OPEN_GATE_PIN			,OUTPUT);
	pinMode(RELAY_CLOSE_GATE_PIN		,OUTPUT);
	pinMode(RELAY_ELEVATOR_PIN			,OUTPUT);
	digitalWrite(RELAY_OPEN_GATE_PIN	,HIGH);
	digitalWrite(RELAY_CLOSE_GATE_PIN	,LOW);
	digitalWrite(RELAY_TV_PIN			,HIGH);
	digitalWrite(RELAY_ELEVATOR_PIN		,LOW);
	tv.begin(NTSC);
	drawAbout();
	tv.end();
	digitalWrite(RELAY_TV_PIN			,LOW);
	digitalWrite(RELAY_CLOSE_GATE_PIN	,HIGH);
	delay(200);
	digitalWrite(RELAY_POWER_PIN		,HIGH);
}

void loop() {
	static uint8_t counter;
	static uint32_t on_ms = 0;
	static uint32_t basket_ms;
	
	if (on_ms > 0) {
		if (--on_ms == 0) {
			digitalWrite(RELAY_ELEVATOR_PIN		,LOW);
			my_delay(MAX_COUNT_DELAY_MS);
			do		drawAbout();
			while	(!digitalRead(BUTTON_START_PIN));
			tv.end();
			digitalWrite(RELAY_TV_PIN			,LOW);
			digitalWrite(RELAY_OPEN_GATE_PIN	,HIGH);
			digitalWrite(RELAY_CLOSE_GATE_PIN	,HIGH);
			delay(200);
			digitalWrite(RELAY_POWER_PIN		,HIGH);
			return;
		}
		else if (on_ms == (ON_TIMEOUT_MS - GATE_OPEN_MS)) {
			digitalWrite(RELAY_OPEN_GATE_PIN	,HIGH);
			digitalWrite(RELAY_CLOSE_GATE_PIN	,LOW);
		}
		if (basket_ms > 0) {
			if (digitalRead(SENSOR_BASKET_PIN))
				basket_ms--;
		}
		else if (!digitalRead(SENSOR_BASKET_PIN)) {
			basket_ms = BASKET_TIMEOUT_MS;
			counter++;
			drawBases(counter);
			if (counter >= MAX_COUNT)
				on_ms = 1;
		}
	}
	else if (!digitalRead(BUTTON_START_PIN)) {
		on_ms = ON_TIMEOUT_MS;
		basket_ms = BASKET_TIMEOUT_MS;
		counter = 0;
		digitalWrite(RELAY_POWER_PIN		,LOW);
		delay(200);
		digitalWrite(RELAY_TV_PIN			,HIGH);
		tv.begin(NTSC);
		drawBases(counter);
		digitalWrite(RELAY_OPEN_GATE_PIN	,LOW);
		digitalWrite(RELAY_ELEVATOR_PIN		,HIGH);
	}
	my_delay(1);
}

void drawBases(uint8_t number) {
	tv.clear_screen();
	tv.bitmap(0, 0, bases);
	printNumber((number < 10 ? 26 : 34), 5, number, 10, 2, dec_digits);
	printNumber(14, 61, number, 2, 4, bin_digits);
}

void drawAbout() {
	tv.clear_screen();
	tv.bitmap(0, 0, didi);
	tv.delay(5000);
	for (uint8_t i = 0; i < tv.hres(); i += 2) {
		tv.shift(2, LEFT);
		tv.delay(30);
	}
	tv.bitmap(0, 0, arad);
	tv.delay(5000);
	for (uint8_t i = 0; i < 20; i++) {
		tv.fill(INVERT);
		tv.delay(200 - i * 8);
	}
	for (uint8_t i = 0; i < tv.hres(); i += 2) {
		tv.shift(2, RIGHT);
		tv.shift(1, DOWN);
		tv.delay(30);
	}
}

void printNumber(int8_t x, int8_t y, uint16_t number, uint8_t base, uint8_t zeros, const uint8_t * digits[]) {
	uint32_t max_number = 1;
	
	for (uint8_t i = 0; i < MAX_NUMBER_DIGITS; i++)
		max_number *= base;
	
	for (uint8_t i = MAX_NUMBER_DIGITS, n; max_number > 0; i--) {
		n = number / max_number;
		if ((n > 0) || (zeros > i)) {
			const uint8_t * bitmap = digits[n];
			tv.bitmap(x, y, bitmap);
			x += pgm_read_byte(bitmap) + 1;
		}
		number %= max_number;
		max_number /= base;
	}
}

void my_delay(uint32_t millis) {
	millis *= 2;
	while (millis--) {
		asm volatile(
			".rept 50	\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			"	nop		\n\t"
			".endr		\n\t"
		);
	}
}
