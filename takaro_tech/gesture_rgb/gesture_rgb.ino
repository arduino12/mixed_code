#include <FastLED.h>
#include <Wire.h>
#include <SparkFun_APDS9960.h>


/* rgb channels count: red, green, blue */
#define LED_DATA_PIN					(3)
/* total amount of rgb channels */
#define RGB_COUNT						(3)
/* led strip type */
#define LED_CHIPSET						WS2812B
/* led strip color rgb channels order */
#define LED_COLOR_ORDER					(GRB)
/* max brightness for rgb channel */
#define LED_BRIGHTNESS					(64)
/* total amount of rgb leds */
#define MATRIX_SIZE						(16)
/* total amount of rgb leds */
#define LED_COUNT						(MATRIX_SIZE * MATRIX_SIZE)

#define WIDTH							(MATRIX_SIZE)
#define HEIGHT							(MATRIX_SIZE)
#define WIDTH_MAX						(WIDTH - 1)
#define HEIGHT_MAX						(HEIGHT - 1)

#define APDS9960_INT    2 // Needs to be an interrupt pin

SparkFun_APDS9960 apds = SparkFun_APDS9960();
CRGBArray<LED_COUNT> leds;
#include "graphics.h"



#define XO_SIZE							(3)
uint8_t xo_map[XO_SIZE][XO_SIZE];
uint8_t xo_curser_x;
uint8_t xo_curser_y;
uint8_t xo_player;
uint8_t xo_ticks;

CRGB xo_players_colors[] = {CRGB::Red, CRGB::Green};
CRGB xo_board_color = CRGB(0, 0, 32);
CRGB xo_curser_color = CRGB(60, 60, 0);



bool xoIsWin() {
	// const int BOARD_SIZE = XO_BOARD_X + XO_SIZE * XO_BOARD_S + 2;
	// const int TILE_SIZE = XO_BOARD_S + 1;
	
	if (xoIsTic(0, 0) && xoIsTic(1, 1) && xoIsTic(2, 2))
	{
		// drawLine(XO_BOARD_X - 1, XO_BOARD_Y, BOARD_SIZE - 1, BOARD_SIZE, xoPlayer);
		// drawLine(XO_BOARD_X, XO_BOARD_Y - 1, BOARD_SIZE, BOARD_SIZE - 1, xoPlayer);
		return true;
	}
	if (xoIsTic(0, 2) && xoIsTic(1, 1) && xoIsTic(2, 0))
	{
		// drawLine(XO_BOARD_X,BOARD_SIZE,BOARD_SIZE, XO_BOARD_Y, xoPlayer);
		// drawLine(XO_BOARD_X - 1, BOARD_SIZE - 1,BOARD_SIZE - 1, XO_BOARD_Y - 1, xoPlayer);
		return true;
	}
	if (xoIsTic(0, 0) && xoIsTic(1, 0) && xoIsTic(2, 0))
	{
		// xoDrawHLine(0);
		return true;
	}
	if (xoIsTic(0, 1) && xoIsTic(1, 1) && xoIsTic(2, 1))
	{
		// xoDrawHLine(TILE_SIZE);
		return true;
	}
	if (xoIsTic(0, 2) && xoIsTic(1, 2) && xoIsTic(2, 2))
	{
		// xoDrawHLine(TILE_SIZE * 2);
		return true;
	}
	if (xoIsTic(0, 0) && xoIsTic(0, 1) && xoIsTic(0, 2))
	{
		// xoDrawVLine(0);
		return true;
	}
	if (xoIsTic(1, 0) && xoIsTic(1, 1) && xoIsTic(1, 2))
	{
		// xoDrawVLine(TILE_SIZE);
		return true;
	}
	if (xoIsTic(2, 0) && xoIsTic(2, 1) && xoIsTic(2, 2))
	{
		// xoDrawVLine(TILE_SIZE * 2);
		return true;
	}
	return false;
}

bool xoIsTic(byte x, byte y) {
	return xo_map[x][y] == xo_player;
}

bool xoIsEmpty() {
	return !xo_map[xo_curser_x][xo_curser_y];
}

void xo_curser_press()
{
	if (!xoIsEmpty())
		return;
	xo_map[xo_curser_x][xo_curser_y] = xo_player;
	xo_ticks++;
	xo_draw_player();
	if (xoIsWin())
	{
		xo_init();
	}
	else if (xo_ticks >= XO_SIZE * XO_SIZE)
	{
		xo_init();
	}
	else
	{
		xo_draw_player();
		xo_player = 3 - xo_player;
		FastLED.show();
	}
}

// void xoDrawHLine(byte y) {
	// drawFastHLine(XO_BOARD_X - 1, XO_BOARD_Y + 3 + y, XO_SIZE * XO_BOARD_S + 4, xoPlayer);
	// drawFastHLine(XO_BOARD_X - 1, XO_BOARD_Y + 4 + y, XO_SIZE * XO_BOARD_S + 4, xoPlayer);
// }
// void xoDrawVLine(byte x) {
	// drawFastVLine(XO_BOARD_X + 3 + x, XO_BOARD_Y - 1, XO_SIZE * XO_BOARD_S + 4, xoPlayer);
	// drawFastVLine(XO_BOARD_X + 4 + x, XO_BOARD_Y - 1, XO_SIZE * XO_BOARD_S + 4, xoPlayer);
// }

void xo_draw_curser(CRGB c)
{
	drawRect(xo_curser_x * 5, xo_curser_y * 5, 6, 6, c);
}

void xo_draw_player()
{
	uint8_t x = xo_curser_x * 5 + 1;
	uint8_t y = xo_curser_y * 5 + 1;
	CRGB c = xo_players_colors[xo_player - 1];

	if (xo_player == 1) {
		drawPixel(x + 0, y + 0, c);
		drawPixel(x + 1, y + 1, c);
		drawPixel(x + 2, y + 2, c);
		drawPixel(x + 3, y + 3, c);
		drawPixel(x + 3, y + 0, c);
		drawPixel(x + 2, y + 1, c);
		drawPixel(x + 1, y + 2, c);
		drawPixel(x + 0, y + 3, c);
	} else {
		drawPixel(x + 1, y + 0, c);
		drawPixel(x + 2, y + 0, c);
		drawPixel(x + 1, y + 3, c);
		drawPixel(x + 2, y + 3, c);
		drawPixel(x + 0, y + 1, c);
		drawPixel(x + 0, y + 2, c);
		drawPixel(x + 3, y + 1, c);
		drawPixel(x + 3, y + 2, c);
	}
}

void xo_curser_move(int dir)
{
	xo_draw_curser(xo_board_color);
	switch (dir)
	{
		case DIR_UP:    if (--xo_curser_y >= XO_SIZE) xo_curser_y = XO_SIZE - 1; break;
		case DIR_DOWN:  if (++xo_curser_y >= XO_SIZE) xo_curser_y = 0; break;
		case DIR_LEFT:  if (--xo_curser_x >= XO_SIZE) xo_curser_x = XO_SIZE - 1; break;
		case DIR_RIGHT: if (++xo_curser_x >= XO_SIZE) xo_curser_x = 0; break;
	}
	xo_draw_curser(xo_curser_color);
	FastLED.show();
}

void xo_init()
{
	memset(xo_map, 0, sizeof(xo_map));
	xo_curser_x = xo_curser_y = 1;
	xo_player = 1;
	xo_ticks = 0;

	fillScreen(CRGB::Black);
	for (uint8_t i = 0; i < MATRIX_SIZE; i+= 5) {
		drawFastHLine(0, i, MATRIX_SIZE, xo_board_color);
		drawFastVLine(i, 0, MATRIX_SIZE, xo_board_color);
	}
	xo_draw_curser(xo_curser_color);
	FastLED.show();
}

void xo_dir()
{
    if (!apds.isGestureAvailable())
		return;
	int dir = apds.readGesture();

	switch (dir) {
	case DIR_UP:
		Serial.println("UP");
		xo_curser_move(dir);
		break;
	case DIR_DOWN:
		Serial.println("DOWN");
		xo_curser_move(dir);
		break;
	case DIR_LEFT:
		Serial.println("LEFT");
		xo_curser_move(dir);
		break;
	case DIR_RIGHT:
		Serial.println("RIGHT");
		xo_curser_move(dir);
		break;
	case DIR_NEAR:
		Serial.println("NEAR");
		xo_curser_press();
		break;
	case DIR_FAR:
		Serial.println("FAR");
		xo_curser_press();
		break;
	default:
		Serial.println("NONE");
		return;
		break;
	}
}

uint16_t xy(uint8_t x, uint8_t y)
{
	uint16_t i = (MATRIX_SIZE - 1 - x) * MATRIX_SIZE;

	if (x >= MATRIX_SIZE || y >= MATRIX_SIZE)
		return 0;

	i += (x & 0x01 ? MATRIX_SIZE - 1 - y: y);

	return i;
}

void draw_pixel(uint8_t x, uint8_t y, CRGB c)
{
	leds[xy(x, y)] = c;
}

void setup()
{
	Serial.begin(115200);

	// Set interrupt pin as input
	pinMode(APDS9960_INT, INPUT);

	// Initialize APDS-9960 (configure I2C and initial values)
	if (apds.init())
		Serial.println(F("APDS-9960 initialization complete"));
	else
		Serial.println(F("Something went wrong during APDS-9960 init!"));

	// Start running the APDS-9960 gesture sensor engine
	if (apds.enableGestureSensor(false))
		Serial.println(F("Gesture sensor is now running"));
	else
		Serial.println(F("Something went wrong during gesture sensor init!"));

	// Adjust the Proximity sensor gain
	if (!apds.setProximityGain(PGAIN_2X))
		Serial.println(F("Something went wrong trying to set PGAIN"));

	// Start running the APDS-9960 proximity sensor (no interrupts)
	if (apds.enableProximitySensor(false))
		Serial.println(F("Proximity sensor is now running"));
	else
		Serial.println(F("Something went wrong during sensor init!"));

	FastLED.addLeds<LED_CHIPSET, LED_DATA_PIN, LED_COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(LED_BRIGHTNESS);
	FastLED.show();
	
	xo_init();
}

void loop()
{
	// draw_dir();
	xo_dir();
}
