#include <FastLED.h>


/* uart baud rate in bit per seconds */
#define SERIAL_BAUD_RATE				(115200)
/* set uart read timeout in milliseconds */
#define SERIAL_TIMEOUT					(1)
/* rgb channels count: red, green, blue */
#define LED_DATA_PIN					(5)
/* total amount of rgb channels */
#define RGB_COUNT						(3)
/* led strip type */
#define LED_CHIPSET						WS2812B
/* led strip color rgb channels order */
#define LED_COLOR_ORDER					(GRB)
/* max brightness for rgb channel */
#define LED_BRIGHTNESS					(255)
/* total amount of rgb leds */
#define LED_COUNT						(33)
/* leds per bar */
#define LED_CHANNEL_COUNT				(10)
/* turn off timeout in seconds */
#define LED_TIMEOUT_S					(10)
/* add / sub this amount in each enc step */
#define ENC_CHANNEL_STEPS				(18)

#define ENC_A_PIN						(2)
#define ENC_B_PIN						(3)
#define ENC_E_PIN						(4)
#define ENC_LEFT 						(1)
#define ENC_RIGHT						(2)
#define ENC_ENTER						(4)


uint32_t old_ms = 0;
uint8_t old_enc = 0;
uint8_t led_on_s = 0;
uint8_t cur_rgb_channel = 0;
CRGB rgb_channels[RGB_COUNT] = {CRGB::Black};
CRGB rgb_channels_steps[] = {CRGB(ENC_CHANNEL_STEPS, 0, 0), CRGB(0, ENC_CHANNEL_STEPS, 0), CRGB(0, 0, ENC_CHANNEL_STEPS)};

CRGBArray<LED_COUNT> leds;

CRGBSet leds_channels[] = {
	leds(LED_CHANNEL_COUNT * 0, LED_CHANNEL_COUNT * 1 - 1),
	leds(LED_CHANNEL_COUNT * 1, LED_CHANNEL_COUNT * 2 - 1),
	leds(LED_CHANNEL_COUNT * 2, LED_CHANNEL_COUNT * 3 - 1),
}; 

CRGBSet leds_color_mix = leds(LED_CHANNEL_COUNT * 3, LED_CHANNEL_COUNT * 3 + RGB_COUNT - 1);


void update_leds() {
	CRGB c = rgb_channels[cur_rgb_channel];
	leds_channels[cur_rgb_channel].fill_solid(CRGB::Black);
	leds_channels[cur_rgb_channel](0, (c.r + c.g + c.b) / 28).fill_solid(rgb_channels[cur_rgb_channel]);
	leds_color_mix[cur_rgb_channel] = rgb_channels[cur_rgb_channel];
	FastLED.show();
	led_on_s = 0;
}

void enc_left() {
	rgb_channels[cur_rgb_channel] -= rgb_channels_steps[cur_rgb_channel];
	update_leds();
}

void enc_right() {
	rgb_channels[cur_rgb_channel] += rgb_channels_steps[cur_rgb_channel];
	update_leds();
}

void enc_press() {
	if (cur_rgb_channel < (RGB_COUNT - 1))	cur_rgb_channel++;
	else									cur_rgb_channel = 0;
}
	
void setup() {
	pinMode(ENC_E_PIN, INPUT_PULLUP);
	pinMode(ENC_A_PIN, INPUT_PULLUP);
	pinMode(ENC_B_PIN, INPUT_PULLUP);
	
	FastLED.addLeds<LED_CHIPSET, LED_DATA_PIN, LED_COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(LED_BRIGHTNESS);
	FastLED.show();
}

void loop() {
	uint8_t cur_enc = 0;
	uint32_t cur_ms = millis();

	if (!digitalRead(ENC_A_PIN)) cur_enc |= ENC_LEFT;
	if (!digitalRead(ENC_B_PIN)) cur_enc |= ENC_RIGHT;
	if (!digitalRead(ENC_E_PIN)) cur_enc |= ENC_ENTER;
	if (old_enc != cur_enc) {
		if (cur_enc == 0) {
			if      (old_enc == ENC_LEFT)  enc_left();
			else if (old_enc == ENC_RIGHT) enc_right();
		}
		else if (cur_enc == ENC_ENTER) enc_press();

		old_enc = cur_enc;
	}

	if ((cur_ms - old_ms) > 1000) {
		old_ms = cur_ms;
		if (led_on_s < LED_TIMEOUT_S)
			led_on_s++;
		else {
			leds.fill_solid(CRGB::Black);
			fill_solid(rgb_channels, RGB_COUNT, CRGB::Black);
			update_leds();
		}
	}

	delay(1);
}
