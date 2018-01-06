// http://youtu.be/NLx9pd7iyhM
#include "niceTimer.h"
#include <avr/pgmspace.h>

//{ Consts

const byte CUBE_SIZE = 4;

const byte LED_COUNT = 192;
const byte LED_MAX = LED_COUNT - 1;
const byte CHANEL_R = 0;
const byte CHANEL_G = 64;
const byte CHANEL_B = 128;

const byte PWM_COUNT = 4;
const byte PWM_MAX = PWM_COUNT - 1;

const byte BITS = 0x03;
const byte AXIS_COUNT = 3;

const byte HUE_COUNT  = 24;
const byte HUE_MAX    = HUE_COUNT - 1;
const byte HUE_BLACK  = 24;
const byte HUE_DARK   = 25;
const byte HUE_BRIGHT = 26;
const byte HUE_WHITE  = 27;

const byte BOX_WALLS   = 0;
const byte BOX_LINES   = 1;
const byte BOX_CORNERS = 2;

const byte ROTATE_MIDDLE_COUNT = 6;
const byte ROTATE_SIDE_COUNT = 24;
const byte ROTATE_MIDDLE_MAX = ROTATE_MIDDLE_COUNT - 1;
const byte ROTATE_SIDE_MAX= ROTATE_SIDE_COUNT - 1;

/*
#define BLACK   0
#define RED     1
#define GREEN   2
#define BLUE    4
#define YELLO   (RED   | GREEN)
#define MAGENTA (RED   | BLUE)
#define CHIAN   (GREEN | BLUE)
*/

static const uint8_t c_dir[192] PROGMEM = {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x01,0x08,0x02,0x04,0x02,0x04,0x01,0x08,0x04,0x02,0x08,0x01,0x08,0x01,0x04,0x02,
        0x01,0x08,0x02,0x04,0x02,0x04,0x01,0x08,0x04,0x02,0x08,0x01,0x08,0x01,0x04,0x02,
        0x01,0x08,0x02,0x04,0x02,0x04,0x01,0x08,0x04,0x02,0x08,0x01,0x08,0x01,0x04,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x01,0x08,0x02,0x04,0x02,0x04,0x01,0x08,0x04,0x02,0x08,0x01,0x08,0x01,0x04,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x01,0x08,0x02,0x04,0x02,0x04,0x01,0x08,0x04,0x02,0x08,0x01,0x08,0x01,0x04,0x02,
        0x01,0x08,0x02,0x04,0x02,0x04,0x01,0x08,0x04,0x02,0x08,0x01,0x08,0x01,0x04,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
static const uint8_t c_out[192] PROGMEM = {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x01,0x08,0x02,0x04,0x02,0x04,0x01,0x08,0x04,0x02,0x08,0x01,0x08,0x01,0x04,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x01,0x08,0x02,0x04,0x02,0x04,0x01,0x08,0x04,0x02,0x08,0x01,0x08,0x01,0x04,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x01,0x08,0x02,0x04,0x02,0x04,0x01,0x08,0x04,0x02,0x08,0x01,0x08,0x01,0x04,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
static const uint8_t d_dir[192] PROGMEM = {
        0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,
        0x44,0x84,0x04,0x04,0x48,0x88,0x08,0x08,0x50,0x90,0x10,0x10,0x60,0xA0,0x20,0x20,
        0x04,0x04,0x04,0x04,0x08,0x08,0x08,0x08,0x10,0x10,0x10,0x10,0x20,0x20,0x20,0x20,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,
        0x04,0x04,0x04,0x04,0x08,0x08,0x08,0x08,0x10,0x10,0x10,0x10,0x20,0x20,0x20,0x20,
        0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,
        0x04,0x04,0x04,0x04,0x08,0x08,0x08,0x08,0x10,0x10,0x10,0x10,0x20,0x20,0x20,0x20,
        0x44,0x84,0x04,0x04,0x48,0x88,0x08,0x08,0x50,0x90,0x10,0x10,0x60,0xA0,0x20,0x20,
        0x04,0x04,0x04,0x04,0x08,0x08,0x08,0x08,0x10,0x10,0x10,0x10,0x20,0x20,0x20,0x20,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,
};
static const uint8_t d_out[192] PROGMEM = {
        0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,
        0x04,0x04,0x04,0x04,0x08,0x08,0x08,0x08,0x10,0x10,0x10,0x10,0x20,0x20,0x20,0x20,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,
        0x04,0x04,0x04,0x04,0x08,0x08,0x08,0x08,0x10,0x10,0x10,0x10,0x20,0x20,0x20,0x20,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,0x40,0x80,0x00,0x00,
        0x04,0x04,0x04,0x04,0x08,0x08,0x08,0x08,0x10,0x10,0x10,0x10,0x20,0x20,0x20,0x20,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
static const uint8_t b_out[192] PROGMEM = {
        0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x04,0x08,0x10,0x20,0x08,0x04,0x20,0x10,0x10,0x20,0x04,0x08,0x20,0x10,0x08,0x04,
        0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x04,0x08,0x10,0x20,0x08,0x04,0x20,0x10,0x10,0x20,0x04,0x08,0x20,0x10,0x08,0x04,
        0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x04,0x08,0x10,0x20,0x08,0x04,0x20,0x10,0x10,0x20,0x04,0x08,0x20,0x10,0x08,0x04,
};
static const uint8_t b_dir[192] PROGMEM = {
        0x04,0x08,0x11,0x22,0x08,0x04,0x21,0x12,0x10,0x20,0x05,0x0A,0x20,0x10,0x09,0x06,
        0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x04,0x08,0x10,0x20,0x08,0x04,0x20,0x10,0x10,0x20,0x04,0x08,0x20,0x10,0x08,0x04,
        0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,
        0x04,0x08,0x10,0x20,0x08,0x04,0x20,0x10,0x10,0x20,0x04,0x08,0x20,0x10,0x08,0x04,
        0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,
        0x04,0x08,0x10,0x20,0x08,0x04,0x20,0x10,0x10,0x20,0x04,0x08,0x20,0x10,0x08,0x04,
        0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x01,0x02,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x04,0x08,0x10,0x20,0x08,0x04,0x20,0x10,0x10,0x20,0x04,0x08,0x20,0x10,0x08,0x04,
        0x04,0x08,0x11,0x22,0x08,0x04,0x21,0x12,0x10,0x20,0x05,0x0A,0x20,0x10,0x09,0x06,
};

//}

//{ Veraibles

byte ledBrightness[LED_COUNT];
byte ledBuffer[LED_COUNT];
byte ledIndex[LED_COUNT];

byte curIndex = 0, maxIndex = 0, curPWM = 0;
byte maxAnimation = 0, curAnimation = 0;
byte red, green, blue, hue;

bool isNotOver = false;

void (*drawPixel)(byte x, byte y, byte z);
void (*rotatePos)(byte &l, byte p, byte &x1, byte &y1, byte &x2, byte &y2, byte r);
void (*fillRotatedRect)(byte a, byte l, byte r);

//}

//{ Colors

void setDraw() {drawPixel = &setPixel;}
void addDraw() {drawPixel = &addPixel;}

void setPixel(byte x, byte y, byte z) {
	byte tmp = x + y * CUBE_SIZE * CUBE_SIZE + z * CUBE_SIZE;
	if (tmp > 63) return;
	ledBrightness[      tmp] = red;
	ledBrightness[64  + tmp] = green;
	ledBrightness[128 + tmp] = blue;
}
void addPixel(byte x, byte y, byte z) {
	byte tmp = x + y * CUBE_SIZE * CUBE_SIZE + z * CUBE_SIZE;
	if (tmp > 63) return;
	ledBrightness[      tmp] = min(red   + ledBrightness[      tmp], PWM_MAX);
	ledBrightness[64  + tmp] = min(green + ledBrightness[64  + tmp], PWM_MAX);
	ledBrightness[128 + tmp] = min(blue  + ledBrightness[128 + tmp], PWM_MAX);
}
byte getPixel(byte x, byte y, byte z) {
	if (x > 3 || y > 3 || z > 3) return 255;
	byte tmp = x + y * CUBE_SIZE * CUBE_SIZE + z * CUBE_SIZE;
	return ledBrightness[tmp] + ledBrightness[64 + tmp] << 2 + ledBrightness[128 + tmp] << 4;
}
void setHue(byte h) {
	byte x;
	if (h < HUE_COUNT)
	{
		x = h % 4;
		switch (h / 4)
		{
			case 0: setRGB(BITS,     x,        0);        break;
			case 1: setRGB(BITS - x, BITS,     0);        break;
			case 2: setRGB(0,        BITS,     x);        break;
			case 3: setRGB(0,        BITS - x, BITS);     break;
			case 4: setRGB(x,        0,        BITS);     break;
			case 5: setRGB(BITS,     0,        BITS - x); break;
		}
	}
	else
	{
		x = (h - HUE_COUNT) % BITS;
		setRGB(x, x, x);
	}
}
void setColor(byte c) {
	red   = c & BITS;
	c >>= 2;
	green = c & BITS;
	c >>= 2;
	blue  = c & BITS;
}
void setRGB(byte r, byte g, byte b) {
	red = r; green = g; blue = b;
}

void swapRGB() {
	byte tmp = red;
	red   = green;
	green = blue;
	blue  = tmp;
}

//}

//{ Lines

void drawLineX(byte x, byte y, byte z, byte l) {
	fixLen(l, x);
	while(l--) drawPixel(x + l, y, z);
}
void drawLineY(byte x, byte y, byte z, byte l) {
	fixLen(l, y);
	while(l--) drawPixel(x, y + l, z);
}
void drawLineZ(byte x, byte y, byte z, byte l) {
	fixLen(l, z);
	while(l--) drawPixel(x, y, z + l);
}
void drawLine(byte x1, byte y1, byte z1, byte x2, byte y2, byte z2) {
	bool reverseX = false, reverseY = false, reverseZ = false;
	if (x1 > x2) {swap(x1,x2); reverseX=true;}
	if (y1 > y2) {swap(y1,y2); reverseY=true;}
	if (z1 > z2) {swap(z1,z2); reverseZ=true;}

	byte delx = x2 - x1;
	byte dely = y2 - y1;
	byte delz = z2 - z1;

	byte longest = max(delx, max(dely, delz));
	for (byte x, y, z, i = 0; i < longest; i++)
	{
		x = (reverseX ? longest - i : i);
		y = (reverseY ? longest - i : i);
		z = (reverseZ ? longest - i : i);
		drawPixel(roundClostest(x * delx, longest) + x1, roundClostest(y * dely, longest) + y1, roundClostest(z * delz, longest) + z1);
	}

	if (reverseX) swap(x1,x2);
	if (reverseY) swap(y1,y2);
	if (reverseZ) swap(z1,z2);
	drawPixel(x2, y2, z2);
}

//}

//{ Rects

void drawRectX(byte x, byte y, byte z, byte yl, byte zl) {
	if ((yl-- == 0) || (zl-- == 0)) return;
	drawLineY(x, y , z, yl);
	drawLineY(x, y + 1, z + zl, yl);
	drawLineZ(x, y , z + 1, zl);
	drawLineZ(x, y + yl, z, zl);
}
void drawRectY(byte x, byte y, byte z, byte xl, byte zl) {
	if ((xl-- == 0) || (zl-- == 0)) return;
	drawLineX(x, y , z, xl);
	drawLineX(x + 1, y, z + zl, xl);
	drawLineZ(x, y , z + 1, zl);
	drawLineZ(x + xl, y, z, zl);
}
void drawRectZ(byte x, byte y, byte z, byte xl, byte yl) {
	if ((xl-- == 0) || (yl-- == 0)) return;
	drawLineX(x, y , z, xl);
	drawLineX(x + 1, y + yl, z, xl);
	drawLineY(x, y + 1, z, yl);
	drawLineY(x + xl, y, z, yl);
}

void fillRectX(byte x, byte y, byte z, byte yl, byte zl) {
	fixLen(zl, z);
	while(zl--) drawLineY(x, y , z + zl, yl);
}
void fillRectY(byte x, byte y, byte z, byte xl, byte zl) {
	fixLen(zl, z);
	while(zl--) drawLineX(x, y , z + zl, xl);
}
void fillRectZ(byte x, byte y, byte z, byte xl, byte yl) {
	fixLen(yl, y);
	while(yl--) drawLineX(x, y + yl, z, xl);
}

void fillRotatedRectX(byte x, byte l, byte r) {
	byte y1, z1, y2, z2;
	rotatePos(l, x, y1, z1, y2, z2, r);
	while(l--) drawLine(x + l, y1, z1, x + l, y2, z2);
}
void fillRotatedRectY(byte y, byte l, byte r) {
	byte x1, z1, x2, z2;
	rotatePos(l, y, x1, z1, x2, z2, r);
	while(l--) drawLine(x1, y + l, z1, x2, y + l, z2);
}
void fillRotatedRectZ(byte z, byte l, byte r) {
	byte x1, y1, x2, y2;
	rotatePos(l, z, x1, y1, x2, y2, r);
	while(l--) drawLine(x1, y1, z + l, x2, y2, z + l);
}

void rotatedRectAxis(byte a) {
	if      (a == 0) fillRotatedRect = &fillRotatedRectX;
	else if (a == 1) fillRotatedRect = &fillRotatedRectY;
	else             fillRotatedRect = &fillRotatedRectZ;
}
//}

//{ Box

void fillBox(byte x, byte y, byte z, byte xl, byte yl, byte zl) {
	byte ly, lx;
	fixLen(xl, x); fixLen(yl, y); fixLen(zl, z);
	while(zl--)
	{
		ly = yl;
		while(ly--)
		{
			lx = xl;
			while(lx--) drawPixel(x + lx, y + ly, z + zl);
		}
	}
}
void drawBox(byte x, byte y, byte z, byte xl, byte yl, byte zl, byte s) { // BOX_WALLS, BOX_LINES, BOX_CORNERS
	fixLen(xl, x); fixLen(yl, y); fixLen(zl, z);
	if ((xl-- == 0) || (yl-- == 0) || (zl-- == 0)) return;
	byte ex = x + xl, ey = y + yl, ez = z + zl;
	
	for (byte i = x; i <= ex; i++)
		for (byte j = y; j <= ey; j++)
			for (byte k = z; k <= ez; k++)
				if ((i == x) + (i == ex) + (j == y) + (j == ey) + (k == z) + (k == ez) > s)
					drawPixel(i, j, k);
}

//}

//{ Cube

void invertCube() {
	for (byte i = 0; i < LED_COUNT; i++)
		ledBrightness[i] = BITS - ledBrightness[i];
}
void setColorCube() {
	for (byte i = 0; i < CHANEL_G; i++)
	{
		if (ledBrightness[i + CHANEL_R] || ledBrightness[i + CHANEL_G] || ledBrightness[i + CHANEL_B])
		{
			ledBrightness[i + CHANEL_R] = red;
			ledBrightness[i + CHANEL_G] = green;
			ledBrightness[i + CHANEL_B] = blue;
		}
	}
}

void fillCube() {
	memset(ledBrightness + CHANEL_R, red,   CHANEL_G);
	memset(ledBrightness + CHANEL_G, green, CHANEL_G);
	memset(ledBrightness + CHANEL_B, blue,  CHANEL_G);
}

//}

//{ Oters

void initCube() {
	clearBuffer();
	setDraw();
	rotateMiddle();
	setRGB(PWM_MAX, 0, 0);
	setTimer2Prescaler(1);
	setTimer2Mode(TIMER2_NORMAL);
	enableTimer2OverflowInterrupt();
	
	setTimer1Prescaler(256);
	setTimer1Mode(TIMER1_NORMAL);
	enableTimer1OverflowInterrupt();
}

void fixLen(byte &l, byte p) {
	l = min(l, CUBE_SIZE - p);
}

void rotateMiddle() {
	rotatePos = &rotateMiddle;
}
void rotateSide() {
	rotatePos = &rotateSide;
}
void rotateMiddle(byte &l, byte p, byte &x1, byte &y1, byte &x2, byte &y2, byte r) {
	fixLen(l, p);
	if (r < 3) { x1 = 0;     y1 = 3 - r; x2 = 3;     y2 = r; }
	else       { x1 = r - 3; y1 = 0;     x2 = 6 - r; y2 = 3; }
}
void rotateSide(byte &l, byte p, byte &x1, byte &y1, byte &x2, byte &y2, byte r) {
	fixLen(l, p);
	if (r < 3)       { x1 = 0;      y1 = 0;      x2 = r;      y2 = 3; }
	else if (r < 6)  { x1 = 0;      y1 = 0;      x2 = 3;      y2 = 6 - r; }
	else if (r < 9)  { x1 = 0;      y1 = r - 6;  x2 = 3;      y2 = 0; }
	else if (r < 12) { x1 = r - 9;  y1 = 3;      x2 = 3;      y2 = 0; }
	else if (r < 15) { x1 = 3;      y1 = 3;      x2 = 15 - r; y2 = 0; }
	else if (r < 18) { x1 = 3;      y1 = 3;      x2 = 0;      y2 = r - 15; }
	else if (r < 21) { x1 = 3;      y1 = 21 - r; x2 = 0;      y2 = 3; }
	else             { x1 = 24 - r; y1 = 0;      x2 = 0;      y2 = 3; }
}


void setHue() {
	setHue(hue);
}
void negHue() {
	setHue((hue + HUE_COUNT / 2) % HUE_COUNT);
}
void nextHue() {
	incHue(hue);
	setHue(hue);
}
void incHue(byte &c) {
	if (++c >= HUE_COUNT) c = 0;
}
void decHue(byte &c) {
	if (--c >= HUE_COUNT) c = HUE_MAX;
}
void incSide(byte &r) {
	if (++r >= ROTATE_SIDE_COUNT) r = 0;
}
void decSide(byte &r) {
	if (--r >= ROTATE_SIDE_COUNT) r = ROTATE_SIDE_MAX;
}
void incMiddle(byte &r) {
	if (++r >= ROTATE_MIDDLE_COUNT) r = 0;
}
void decMiddle(byte &r) {
	if (--r >= ROTATE_MIDDLE_COUNT) r = ROTATE_MIDDLE_MAX;
}

void allOff() {
	DDRB  &= 0xC0;
	DDRC  &= 0xF0;
	DDRD  &= 0x03;	
	PORTB &= 0xC0;
	PORTC &= 0xF0;
	PORTD &= 0x03;
}

void clearBuffer() {
	memset(ledBrightness, 0, LED_COUNT);
}
void flushBuffer() {
	disableTimer2OverflowInterrupt();
	allOff();
	//memcpy (ledBuffer, ledBrightness, LED_COUNT);
	
	maxIndex = curIndex = 0;
	for (byte i = 0; i < LED_COUNT; i++)
		if (ledBrightness[i])
		{
			ledBuffer[i] = ledBrightness[i];
			ledIndex[maxIndex++] = i;
		}
	if (maxIndex)
	{
		maxIndex--;
		enableTimer2OverflowInterrupt();
	}
	//else allOff();
}

byte roundClostest(byte numerator, byte denominator) {
	numerator = (numerator << 1) / denominator;
	return (numerator >> 1) + (numerator % 2);
}

void swap(byte &one, byte &two) {
	one ^= two; two = one ^ two; one ^= two;
}

ISR(TIMER2_OVF_vect) {
	byte curLED = ledIndex[curIndex];
	allOff();
	if (ledBuffer[curLED] > curPWM)
	{
		DDRB  |= pgm_read_byte(b_dir + curLED);
		DDRC  |= pgm_read_byte(c_dir + curLED);
		DDRD  |= pgm_read_byte(d_dir + curLED);	
		PORTB |= pgm_read_byte(b_out + curLED);
		PORTC |= pgm_read_byte(c_out + curLED);
		PORTD |= pgm_read_byte(d_out + curLED);
	}
	if (curIndex < maxIndex) curIndex++;
	else
	{
		curIndex = 0;
		curPWM = (curPWM < PWM_MAX - 1 ? curPWM + 1 : 0);
	}
}

ISR(TIMER1_OVF_vect) {
	if (++curAnimation >= maxAnimation)
	{
		isNotOver = false;
		curAnimation = 0;
	}
}

//}


int animationSpeed, gg = 0;

void setup() {
	srand(analogRead(0));
	maxAnimation = 15;
	initCube();
	int d = 400;
	int x = 0;
	int y = 0;
	int z = 0;
	
	
	// setRGB(PWM_MAX, 0, 0);
	// drawPixel(0, 0, 0);
	// flushClearDelay();
	/*
	while(1) {
		setRGB(PWM_MAX, 0, 0);
		drawPixel(x, y, z);
		flushClearDelay();
		delay(d);
		setRGB(0, PWM_MAX, 0);
		drawPixel(x, y, z);
		flushClearDelay();
		delay(d);
		setRGB(0, 0, PWM_MAX);
		drawPixel(x, y, z);
		flushClearDelay();
		delay(d);
		setRGB(0, 0, 0);
		drawPixel(x, y, z);
		flushClearDelay();
		delay(1000);
	}*/
}

void loop() {
	switch (gg++ %10) {
		case 0: movingCubes();	break;
		case 1: chaseTheDot();	break;
		case 2: hueCube();		break;
		case 3: fountian();		break;
		case 4: linesRot();		break;
		case 5: colorProp();	break;
		case 6: fourProps();	break;
		case 7: sideRotats();	break;
		case 8: lineRotats();	break;
		case 9: rain();			break;
	}
}

//{ Efects

void rain() {
	setSpeed(130);
	setDraw();

	byte pX, pY, pZ = 3;
	bool dir = false;

	while (isNotOver) {	
		//setRGB(0, 0, 0);
		//drawPixel(pX, pY, pZ);
		if (++pZ > 3) {
			pX = random(4); 
			pY = random(4); 
			dir = randBool();
			pZ = 0;
		}
		nextHue();
		if (dir) drawPixel(pX, pY, pZ);
		else drawPixel(pX, pY, 3 - pZ);
		flushClearDelay();
	}
}

void chaseTheDot() {
	setSpeed(80);
	setDraw();

	byte pX = 0, pY = 0, pZ = 0;
	byte *mover;
	while (isNotOver) {
		nextHue();
		switch (random(3)) {
			case 0: mover = &pX; break;
			case 1: mover = &pY; break;
			case 2: mover = &pZ; break;
		}
		if (randBool()) {if (++*mover > 3) *mover = 2;}
		else            {if (--*mover > 3) *mover = 1;}

		drawPixel(pX, pY, pZ);
		flushClearDelay();
	}
}

void movingCubes() {
	const byte C_SIZE = 2;
	setSpeed(200);
	addDraw();
	char
		rX = 0, rY = 2, rZ = 0,
		gX = 0, gY = 0, gZ = 2,
		bX = 2, bY = 0, bZ = 0;

	char *mover;
	bool moved = true;
	while(isNotOver) {
		if (moved)
			switch (random(9)) {
				case 0: mover = &bX; break;
				case 1: mover = &bY; break;
				case 2: mover = &bZ; break;
				case 3: mover = &rX; break;
				case 4: mover = &rY; break;
				case 5: mover = &rZ; break;
				case 6: mover = &gX; break;
				case 7: mover = &gY; break;
				case 8: mover = &gZ; break;
			}
		moved = !moved;
		*mover = (((*mover) + 2) % 4) - 1;
		setRGB(PWM_MAX, 0, 0);
		fillBox(abs(gX), abs(gY), abs(gZ), C_SIZE, C_SIZE, C_SIZE);
		setRGB(0, PWM_MAX, 0);
		fillBox(abs(rX), abs(rY), abs(rZ), C_SIZE, C_SIZE, C_SIZE);
		setRGB(0, 0, PWM_MAX);
		fillBox(abs(bX), abs(bY), abs(bZ), C_SIZE, C_SIZE, C_SIZE);
		flushClearDelay();
	}
}

void hueCube() {
	setSpeed(random(4) * 50 + 5);
	setDraw();
	// while (isNotOver) {
		// nextHue();
		// fillCube();
		// flushClearDelay();
	// }
}

void fountian() {
	setSpeed(200);
	setDraw();
	while (isNotOver) {
		nextHue();
		for (byte z = 0; z < 4; z++) {
			fillRectZ(1, 1, z, 2, 2);
			flushClearDelay();
		}
		for (byte z = 4; z > 0; z--) {
			drawRectZ(0, 0, z - 1, 4, 4);
			flushClearDelay();
		}
	}
}

void linesRot() {
	setSpeed(100);
	rotateSide();
	setDraw();
	rotatedRectAxis(random(AXIS_COUNT));
	while (isNotOver) {
		nextHue();
		for (byte i = 0; i < 7; i++) {
			setHue();
			fillRotatedRect(0, 1, i);
			fillRotatedRect(2, 1, i);
			negHue();
			fillRotatedRect(1, 1, 12 - i);
			fillRotatedRect(3, 1, 12 - i);
			flushClearDelay();
		}
		for (byte i = 7; i > 0; i--) {
			setHue();
			fillRotatedRect(0, 1, i - 1);
			fillRotatedRect(2, 1, i - 1);
			negHue();
			fillRotatedRect(1, 1, 13 - i);
			fillRotatedRect(3, 1, 13 - i);
			flushClearDelay();
		}
	}
}

void colorProp() {
	setSpeed(140);
	rotateMiddle();
	setDraw();
	byte i = 0;
	byte color = 0;
	rotatedRectAxis(random(AXIS_COUNT));
	while (isNotOver) {
		setHue(color);
		fillRotatedRect(0, 1, i);
		setHue((color + 1) % HUE_COUNT);
		fillRotatedRect(1, 1, (i + 1) % ROTATE_MIDDLE_COUNT);
		setHue((color + 2) % HUE_COUNT);
		fillRotatedRect(2, 1, (i + 2) % ROTATE_MIDDLE_COUNT);
		setHue((color + 3) % HUE_COUNT);
		fillRotatedRect(3, 1, (i + 3) % ROTATE_MIDDLE_COUNT);
		flushClearDelay();
		incHue(color);
		incMiddle(i);
	}
}

void fourProps() {
	setSpeed(200);
	rotateMiddle();
	setDraw();
	while (isNotOver) {
		for (byte i = 0; i < ROTATE_MIDDLE_COUNT; i++) {
			setRGB(PWM_MAX, 0, 0);
			fillRotatedRectY(0, 1, i);
			setRGB(0, 0, PWM_MAX);
			fillRotatedRectX(3, 1, ROTATE_MIDDLE_MAX - (i + 5) % ROTATE_MIDDLE_COUNT);
			setRGB(PWM_MAX, PWM_MAX, 0);
			fillRotatedRectY(3, 1, ROTATE_MIDDLE_MAX - (i + 2) % ROTATE_MIDDLE_COUNT);
			setRGB(0, PWM_MAX, 0);
			fillRotatedRectX(0, 1, (i + 3) % ROTATE_MIDDLE_COUNT);
			flushClearDelay();
		}
	}
}

void sideRotats() {
	setSpeed(52);
	rotateSide();
	setDraw();
	nextHue();
	byte i = 0, pos = 0;
	bool stepUp = false;
	rotatedRectAxis(0);
	while (isNotOver) {
		if ((i % 6 == 0 && randBool())) {
			nextHue();
			stepUp = randBool();
			if (randBool()) {
				if (i % 12) {
					if (pos < 2) pos = 1 - pos;
					else {pos = 0; i -= 6;}
				}
				else {
					if (pos) pos = 3 - pos;
					else {pos = 2; i += 6;}
				}
				rotatedRectAxis(pos);
			}
		}
		fillRotatedRect(0, 4, i);
		flushClearDelay();
		if (stepUp) incSide(i);
		else 		decSide(i);
	}
}

void lineRotats() {
	setSpeed(38);
	rotateSide();
	setDraw();
	nextHue();
	byte i = 0, pos = 0;
	bool stepUp = true;
	while (isNotOver) {
		if ((i % 6 == 0)) {
			nextHue();
			stepUp = randBool();
			if (randBool()) {
				if      (i == 0) {
					if      (pos < 2)  pos = 1 - pos;
					else if (pos < 3) {pos = 0; i = 6;}
					else if (pos < 5) {pos = 4 - pos; i = 12;}
					else              {pos = 0; i = 18;}
				}
				else if (i == 6) {
					if      (pos < 1) {pos = 2; i = 0;}
					else if (pos < 3)  pos = 3 - pos;
					else if (pos < 4) {pos = 2; i = 12;}
					else              {pos = 6 - pos; i = 18;}
				}
				else if (i == 12) {
					if      (pos < 2) {pos = 4 - pos; i = 0;}
					else if (pos < 3) {pos = 3; i = 6;}
					else if (pos < 5)  pos = 7 - pos;
					else              {pos = 3; i = 18;}
				}
				else {
					if      (pos < 1) {pos = 5; i = 0;}
					else if (pos < 3) {pos = 6 - pos; i = 6;}
					else if (pos < 4) {pos = 5; i = 12;}
					else               pos = 9 - pos;
				}
				rotatedRectAxis(pos % AXIS_COUNT);
			}
		}

		if (pos < AXIS_COUNT) fillRotatedRect(0, 1, i);
		else                  fillRotatedRect(3, 1, i);
		flushClearDelay();
		if (stepUp) incSide(i);
		else 		decSide(i);
	}
}

//}

//{ Oters

bool randBool() {
	return random(2);
}

void flushClearDelay() {
	flushBuffer();
	clearBuffer();
	delay(animationSpeed);
}

void flushDelay() {
	flushBuffer();
	delay(animationSpeed);
}

void setSpeed(int speed) {
	isNotOver = true;
	animationSpeed = speed;
}

//}