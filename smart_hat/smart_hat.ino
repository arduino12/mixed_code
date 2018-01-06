/*
	Smart Hat V0.0.5.0 Software!
	==========================
	Copyright (C) 2015 Arad Eizen (A.E Tec) LTD INC.
	This Program Is The Operating System For My Smart Hat.
	Working With: GPS, MP3, LEDS, Fan, sensors(pressure, light, distance) and buttons.
	The Power Source Is A Solar Portable Charger.
	Main Board: Arduino Pro Mini.
	Language: cpp.
*/

#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <Player.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <BH1750.h>
#include <Ultrasonic.h>
#include <Buttons.h>
#include "globals.h"
#include "modes.h"
#include "voice.h"

/*
	TODO: add eeprom, fix playlists
*/

Player player(PLAYER_RX_PIN, PLAYER_TX_PIN);
Adafruit_BMP085 pressure_sensor;
BH1750 light_sensor;
Ultrasonic distance_sensor;
Buttons buttons;

TinyGPSPlus gps;
SoftwareSerial gps_serial(GPS_RX_PIN, GPS_TX_PIN);

bitmask_t buttons_bitmask;
bool	is_first_long_press;
uint8_t hat_mode;
uint8_t folder_index;
uint8_t folders_count;
uint8_t track_index;
uint8_t tracks_count;

uint8_t talking_list[TALKING_LIST_SIZE];
uint8_t talking_index;
uint8_t talking_size;

uint8_t dis_word;
uint8_t dis_trigger;

uint8_t led_power;
uint8_t led_trigger;
bool is_led_on;

uint8_t fan_power;
uint8_t fan_trigger;
bool is_fan_on;

uint8_t loop_counter;

event_t onModeShortRelease[] = {
	onMusicShortRelease,
	onDistanceShortRelease,
	onLightShortRelease,
	onFanShortRelease,
	onInformationShortRelease,
};

event_t onModeLongPressAndPulse[] = {
	onMusicLongPressAndPulse,
	onDistanceLongPressAndPulse,
	onLightLongPressAndPulse,
	onFanLongPressAndPulse,
	onInformationLongPressAndPulse,
};
/*
event_t onModeLongRelease[] = {
	NULL,
	onDistanceLongRelease,
	onLightLongRelease,
	onFanLongRelease,
	NULL,
};
*/
void setup() {
#ifdef DEBUG
	Serial.begin(DEBUG_BAUDRATE);
#endif
	DDRB  |= 0x0C;				// (10, 11)	OUTPUT
	PORTB &= 0xF3;				// (10, 11)	LOW
	DDRC  &= 0xF0;				// (A0 - A3)		INPUT
	PORTC |= 0x0F;				// (A0 - A3)		PULLUP
	
	buttons.begin(getButtonsBitmask, 100, 700, 2000);		
	pressure_sensor.begin();
	light_sensor.begin();
	distance_sensor.begin(US_TRIG_PIN, US_ECHO_PIN, US_MAX_DISTANCE);
	gps_serial.begin(GPS_BAUDRATE);
	player.setVolume(22);
	player.setEqualizer(EQ_NORMAL);
	
	/*folders_count = player.getFoldersCount();
	DEBUG_PRINT(F("folders_count ="));
	DEBUG_PRINTLN(folders_count, DEC);*/

	talking_index	= 0;
	talking_size	= 0;
	hat_mode		= HAT_MODE_MUSIC;
	
	folders_count	= 5;
	folder_index	= 2;
	updateFolder();
	
	voice_begin_word(1);

	dis_word = 0;
	dis_trigger = 0;
	led_power = 0;
	led_trigger = 100;
	is_led_on = false;
	fan_power = 100;
	fan_trigger = 20;
	is_fan_on = false;

	DEBUG_PRINTLN(F("start!"));
}

void loop() {
	buttons.update();
	buttons_state_t buttons_state = buttons.getState();
	
	if		(BUTTONS_STATE_UP == buttons_state) {
		buttons_bitmask = 0;
		is_first_long_press = true;
	}
	else if	(BUTTONS_STATE_SHORT_PRESS == buttons_state) {
		buttons_bitmask = getButtonsBitmask();
	}
	else if	(BUTTONS_STATE_SHORT_RELEASE == buttons_state) {
		if (BUTTON_MODE_MASK == buttons_bitmask) {
			hat_mode++;
			hat_mode %= HAT_MODES_COUNT;
			DEBUG_PRINT(F("hat_mode: "));
			DEBUG_PRINTLN(hat_mode, DEC);
			voice_begin_word(hat_mode + 1);
		}
		else {
			onModeShortRelease[hat_mode]();
		}
	}
	else if	(((BUTTONS_STATE_LONG_PRESS == buttons_state) && is_first_long_press)||
			BUTTONS_STATE_LONG_PULSE == buttons_state) {
		is_first_long_press = false;
		onModeLongPressAndPulse[hat_mode]();
	}
	/*else if (BUTTONS_STATE_LONG_RELEASE == buttons_state) {
		if (NULL != onModeLongRelease[hat_mode]) onModeLongRelease[hat_mode]();
	}*/

	if (loop_counter % 50 == 0) sensorsUpdate();

	updateVoice();
	
	delayGPS(5); //delay(5);
	loop_counter++;
}

bitmask_t getButtonsBitmask() {
	return ~PINC & 0x0F;
}
