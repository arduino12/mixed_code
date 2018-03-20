#include <WiFi.h>
#include <WifiTmp2Net.h>
#define FASTLED_INTERNAL
#include <FastLED.h>


#define ARRAY_COUNT(arr)			(sizeof(arr) / sizeof(*arr))

#define WIFI_SSID					"KokorikoBlaBla"
#define WIFI_PASSWORD				"MooMiauMeeLoLoLi"
#if (1)
	#define STRIPS_COUNT				7
	#define LED_PER_STRIP				56
	#define LED_BRIGHTNESS				64 // max 255!
#else
	#define STRIPS_COUNT				4
	#define LED_PER_STRIP				512
	#define LED_BRIGHTNESS				64 // max 255!
#endif
#define LED_COUNT					STRIPS_COUNT * LED_PER_STRIP
#define LED_TYPE					WS2812B
#define LED_COLOR_ORDER				GRB
#define LED_CORRECTION				TypicalLEDStrip
#define RGB_CHANNELS_COUNT			3


WifiTmp2Net tmp2net;
CRGB leds[LED_COUNT];
uint16_t last_data_index = 0;


void ConnectWifi(void)
{
	Serial.println();
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

void tmp2net_callback(uint8_t packet_id, uint8_t packets_count, uint8_t *data, uint16_t data_size)
{
	if ((last_data_index + data_size) <= sizeof(leds))
		memcpy((uint8_t*)(&leds) + last_data_index, data, data_size);
	
	// Serial.print("packet_id: ");
	// Serial.println(packet_id);
	// Serial.print("packet_couny: ");
	// Serial.println(packets_count);
	// Serial.print("data_size: ");
	// Serial.println(data_size);

	last_data_index += data_size;
	if (packet_id < packets_count)
		return;
	// Serial.println("4");
	FastLED.show();
	last_data_index = 0;
}

void setup()
{
	Serial.begin(115200);
	Serial.println("Initializing...");
	FastLED.addLeds<LED_TYPE, 13, LED_COLOR_ORDER>(leds, 0 * LED_PER_STRIP, LED_PER_STRIP).setCorrection(LED_CORRECTION);
#if STRIPS_COUNT > 1
	FastLED.addLeds<LED_TYPE, 14, LED_COLOR_ORDER>(leds, 1 * LED_PER_STRIP, LED_PER_STRIP).setCorrection(LED_CORRECTION);
#endif
#if STRIPS_COUNT > 2
	FastLED.addLeds<LED_TYPE, 15, LED_COLOR_ORDER>(leds, 2 * LED_PER_STRIP, LED_PER_STRIP).setCorrection(LED_CORRECTION);
#endif
#if STRIPS_COUNT > 3
	FastLED.addLeds<LED_TYPE, 16, LED_COLOR_ORDER>(leds, 3 * LED_PER_STRIP, LED_PER_STRIP).setCorrection(LED_CORRECTION);
#endif
#if STRIPS_COUNT > 4
	FastLED.addLeds<LED_TYPE, 17, LED_COLOR_ORDER>(leds, 4 * LED_PER_STRIP, LED_PER_STRIP).setCorrection(LED_CORRECTION);
#endif
#if STRIPS_COUNT > 5
	FastLED.addLeds<LED_TYPE, 18, LED_COLOR_ORDER>(leds, 5 * LED_PER_STRIP, LED_PER_STRIP).setCorrection(LED_CORRECTION);
#endif
#if STRIPS_COUNT > 6
	FastLED.addLeds<LED_TYPE, 19, LED_COLOR_ORDER>(leds, 6 * LED_PER_STRIP, LED_PER_STRIP).setCorrection(LED_CORRECTION);
#endif
#if STRIPS_COUNT > 7
	FastLED.addLeds<LED_TYPE, 21, LED_COLOR_ORDER>(leds, 7 * LED_PER_STRIP, LED_PER_STRIP).setCorrection(LED_CORRECTION);
#endif
	FastLED.setBrightness(LED_BRIGHTNESS);
	// leds[0] = CRGB(255, 255, 255);
	// FastLED.show();
	// delay(100);
	ConnectWifi();
	tmp2net.begin(tmp2net_callback);
	Serial.println("Init complete");
}

void loop()
{
	tmp2net.read();
}
