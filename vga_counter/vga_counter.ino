#include <VGA.h>
#include "arad.h"
#include "didi.h"
#include "digit.h"
#include "base.h"
#include "mario.h"

#define MAX_NUMBER_DIGITS	(4)
#define BUTTON_ADD_PIN		(10)
#define BUTTON_RST_PIN		(11)
#define NEXT_PLATE_MS		(2 * 60000)
#define NEXT_ABOUT_MS		(11 * 60000)

uint8_t counter;

void setup() {
	vga_begin();
	pinMode(BUTTON_ADD_PIN ,INPUT_PULLUP);
	pinMode(BUTTON_RST_PIN ,INPUT_PULLUP);
	vga_delay(5000);
	counter = 0;
	drawAbout();
	drawBases();
}

void loop() {
	static uint32_t time_ms = 1;
	if (!digitalRead(BUTTON_ADD_PIN)) {
		if (counter >= 15)	counter = 0;
		else				counter++;
		drawBases();
		time_ms = 0;
		do		vga_delay(500);
		while	(!digitalRead(BUTTON_ADD_PIN));
	}
	else if (!digitalRead(BUTTON_RST_PIN)) {
		counter = 0;
		drawBases();
		time_ms = 0;
		do		vga_delay(500);
		while	(!digitalRead(BUTTON_RST_PIN));
	}
	else if ((time_ms % NEXT_PLATE_MS) == 0) {
		counter = 0;
		drawBases();
	}
	else if ((time_ms % NEXT_ABOUT_MS) == 0) {
		drawAbout();
		drawBases();
	}
	vga_delay(1);
	time_ms++;
}

void drawBases() {
	fillDisplay(COLOR_0);
	nextColorsPlate();
	drawMonocolorBitmap(86,	1,	base_bin,	COLOR_2);
	drawMonocolorBitmap(83,	30,	base_oct,	COLOR_2);
	drawMonocolorBitmap(21,	1,	base_dec,	COLOR_2);
	drawMonocolorBitmap(0,	30,	base_hex,	COLOR_2);
	fillRect(58,	0,	2,	DISPLAY_HEIGHT,	COLOR_3);
	printNumber(60,	11,	counter,	2,	4,	COLOR_1);
	printNumber(60,	41,	counter,	8,	2,	COLOR_1);
	printNumber(1,	11,	counter,	10,	2,	COLOR_1);
	printNumber(1,	41,	counter,	16,	1,	COLOR_1);
}

void drawAbout() {
	fillDisplay(COLOR_0);
	drawBicolorBitmap(0, 0, didi);
	vga_delay(3000);
	for (uint8_t i = 0; i < 60; i++) {
		nextColorsPlate();
		vga_delay(500 - i * 8);
	}
	
	fillDisplay(COLOR_0);
	setColors(COLORS_PLATE_R_G);
	drawBicolorBitmap(0, 0, arad);
	vga_delay(3000);
	uint8_t w = getBitmapWidth(mario_0);
	uint8_t h = getBitmapHeight(mario_0);
	for (uint8_t i = 26; i < 45; i++) {
		drawBicolorBitmap(i * 2, 35, mario(i % 4));
		vga_delay(500);
		fillRect(i * 2, 35, w, h, COLOR_0);
	}
	fillRect(0, 35, DISPLAY_WIDTH, h, COLOR_0);
	drawBicolorBitmap(0, 35, mario_4);
	vga_delay(800);
	drawBicolorBitmap(30, 35, mario_5);
	vga_delay(600);
	drawBicolorBitmap(60, 35, mario_4);
	vga_delay(400);
	drawBicolorBitmap(90, 35, mario_5);
	vga_delay(500);
	for (uint8_t i = 0; i < 60; i++) {
		swapColors();
		vga_delay(500 - i * 8);
	}
}

void printNumber(int8_t x, int8_t y, uint16_t number, uint8_t base, uint8_t zeros, color_t c) {
	uint32_t max_number = 1;
	
	for (uint8_t i = 0; i < MAX_NUMBER_DIGITS; i++)
		max_number *= base;
	
	for (uint8_t i = MAX_NUMBER_DIGITS, n; max_number > 0; i--) {
		n = number / max_number;
		if ((n > 0) || (zeros > i)) {
			const uint8_t * bitmap = numberDigit(n);
			drawMonocolorBitmap(x, y, bitmap, c);
			x += getBitmapWidth(bitmap);
		}
		number %= max_number;
		max_number /= base;
	}
}

const uint8_t * numberDigit(uint8_t digit) {
	switch (digit) {
		case 0x00:	return digit_0;
		case 0x01:	return digit_1;
		case 0x02:	return digit_2;
		case 0x03:	return digit_3;
		case 0x04:	return digit_4;
		case 0x05:	return digit_5;
		case 0x06:	return digit_6;
		case 0x07:	return digit_7;
		case 0x08:	return digit_8;
		case 0x09:	return digit_9;
		case 0x0A:	return digit_A;
		case 0x0B:	return digit_B;
		case 0x0C:	return digit_C;
		case 0x0D:	return digit_D;
		case 0x0E:	return digit_E;
		default:	return digit_F;
	}
}

const uint8_t * mario(uint8_t index) {
	switch (index) {
		case 0x00:	return mario_0;
		case 0x01:	return mario_1;
		case 0x02:	return mario_2;
		case 0x03:	return mario_3;
		case 0x04:	return mario_4;
		default:	return mario_5;
	}
}

uint8_t nextColorsPlate() {
	static uint8_t index = 0;
	setColors(colors(index++));
	if (index >= COLORS_PLATE_COUNT) index = 0;
}

uint8_t colors(uint8_t index) {
	switch (index) {
		case 0x00:	return COLORS_PLATE_R_G;
		case 0x01:	return COLORS_PLATE_R_B;
		case 0x02:	return COLORS_PLATE_R_GB;
		case 0x03:	return COLORS_PLATE_RG_B;
		case 0x04:	return COLORS_PLATE_G_R;
		case 0x05:	return COLORS_PLATE_G_B;
		case 0x06:	return COLORS_PLATE_G_RB;
		case 0x07:	return COLORS_PLATE_GB_R;
		case 0x08:	return COLORS_PLATE_B_R;
		case 0x09:	return COLORS_PLATE_B_G;
		case 0x0A:	return COLORS_PLATE_B_RG;
		default:	return COLORS_PLATE_BR_G;
	}
}
