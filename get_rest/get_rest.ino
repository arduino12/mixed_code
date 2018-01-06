/*
	Get-Rest V0.1 Software!
	=======================
	Copyright (C) 2015 Arad Eizen (A.E Tec) LTD INC.
	This program counts and calculates the key code,
	and shows it on the displays!
	Main board: Arduino-Pro-Mini.
	License for Smart-it Only!
*/
//{ includes:
#include <avr/sleep.h>			// for power saving sleep modes
#include <EEPROM.h>				// for saving the key code
#include <Wire.h>				// for SH1106 OLED display module
#include <SH1106.h>				// for SH1106 OLED display module
#include <Segments.h>			// for seven segments display module
#include <SevenSegments.h>		// for seven segments display module
#include "digit.h"				// for decimal digits
#include "aroma.h"				// for aroma logo
#include "get_rest_1.h"			// for get rest logo
//#include "battery.h"			// for battery logos
//#include "beer_qr_4.h"		// for beer QR logo
//}

//{ defines:
#define TOUCH_SENSOR_PIN		(2)
#define TOUCH_SENSOR_INT		(0)
#define BATTERY_IS_CHARGING_PIN	(3)
#define BATTERY_IS_CHARGED_PIN	(4)
#define SEGMENTS_100_PIN		(5)
#define SEGMENTS_10_PIN			(6)
#define SEGMENTS_1_PIN			(7)
#define SEGMENTS_E_PIN			(8)
#define SEGMENTS_F_PIN			(9)
#define SEGMENTS_G_PIN			(10)
#define SEGMENTS_DP_PIN			(11)
#define SEGMENTS_A_PIN			(A0)
#define SEGMENTS_B_PIN			(A1)
#define SEGMENTS_C_PIN			(A2)
#define SEGMENTS_D_PIN			(A3)
#define SH1106_SDA_PIN			(A4)
#define SH1106_SCL_PIN			(A5)

#define DIGITS_COUNT			(3)
//}

//{ globals
SH1106 oled;
SevenSegments seven_segments;
const uint8_t * digits[] = {
	digit_0, digit_1, digit_2, digit_3, digit_4,
	digit_5, digit_6, digit_7, digit_8, digit_9,
};
//}

void setup() {
	/* init IOs */
	pinMode(TOUCH_SENSOR_PIN, INPUT);	// (02) INPUT
	PORTB &= 0xF0; DDRB |= 0x0F;		// (08 - 13) OUTPUTS (LOW)
	PORTC &= 0xF0; DDRC |= 0x0F;		// (A0 - A3) OUTPUTS (LOW)
	PORTD |= 0xE0; DDRD |= 0xE0;		// (05 - 07) OUTPUTS (HIGH)
	/* init hardware */
	seven_segments.begin(DIGITS_COUNT, setDigit, setSegments);
	oled.begin();
	/* test all pixels and segments */
	setBrightness(255);
	seven_segments.fill();
	oled.fill();
	delay(2000);
	seven_segments.clear();
	oled.clear();
//	setBrightness(0);
//	oled.drawBitmap(35, 3, beer_qr_4, COLOR_WHITE);
//	delay(60000);
//	oled.clear();
}

void loop() {
	static uint8_t i = 0;
	/* sleep until somebody touch */
	goToSleep();
	/* fade-in with the number */
	setBrightness(0);
	seven_segments.printIntegr(i, 0, true);
	printNumber(0, 0, i++, 10, DIGITS_COUNT);
	for (uint8_t i = 1; i <= 16; i++) {
		setBrightness(i * i - 1);
		delay(150);
	}
	setBrightness(255);
	/* show get-rest icon in OLED */
	delay(1000);
	oled.clear();
	oled.drawBitmap(0, 0, get_rest_1, COLOR_WHITE);
	/* show aroma icon in OLED */
	delay(2000);
	oled.clear();
	oled.drawBitmap(0, 0, aroma, COLOR_WHITE);
	delay(2000);
	/* fade-out */
	for (uint8_t i = 15; i > 0; i--) {
		setBrightness(i * i - 1);
		delay(150);
	}
	seven_segments.clear();
	oled.clear();
}

void goToSleep() {
	setPower(false);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);				// SLEEP_MODE_IDLE, SLEEP_MODE_ADC, SLEEP_MODE_PWR_SAVE,
	sleep_enable();										// SLEEP_MODE_STANDBY, SLEEP_MODE_PWR_DOWN
	attachInterrupt(TOUCH_SENSOR_INT, NULL, RISING);	// LOW, CHANGE, RISING, FALLING
	sleep_mode();										// sleep!
	sleep_disable();
	detachInterrupt(TOUCH_SENSOR_INT);
	setPower(true);
}

void printNumber(int16_t x, int16_t y, uint16_t number, uint8_t base, uint8_t zeros) {
	uint32_t max_number = 1;

	for (uint8_t i = 0; i < DIGITS_COUNT; i++)
		max_number *= base;

	for (uint8_t i = DIGITS_COUNT, n; max_number > 0; i--) {
		n = number / max_number;
		if ((n > 0) || (zeros > i)) {
			const uint8_t * bitmap = digits[n];
			oled.drawBitmap(x, y, bitmap, COLOR_WHITE);
			x += pgm_read_byte(bitmap + BITMAP_WIDTH_OFFSET) + 4;
		}
		number %= max_number;
		max_number /= base;
	}
}

void setBrightness(
	uint8_t		brightness
) {
	seven_segments.setBrightness(brightness);
	oled.setBrightness(brightness);
}

void setPower(
	bool is_on
) {
	seven_segments.setPower(is_on);
	oled.setPower(is_on);
}

void setDigit(uint8_t index) {
	PORTD = PORTD & 0x1F | ((0x20 << index) & 0xE0);
}

void setSegments(segment_mask_t segments_mask) {
	PORTB = PORTB & 0xF0 | (~segments_mask & 0x0F);
	PORTC = PORTC & 0xF0 | (~segments_mask >> 4);
}
