#ifndef _VOICE_H_
#define _VOICE_H_

#include <Arduino.h>

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

extern Player player;
extern TinyGPSPlus gps;
extern Adafruit_BMP085 pressure_sensor;
extern BH1750 light_sensor;
extern Ultrasonic distance_sensor;
extern SoftwareSerial gps_serial;

extern uint8_t talking_list[TALKING_LIST_SIZE];
extern uint8_t talking_index;
extern uint8_t talking_size;

extern uint8_t folder_index;
extern uint8_t folders_count;
extern uint8_t track_index;
extern uint8_t tracks_count;

uint8_t avg;
uint32_t old_millis;


void delayGPS(uint32_t ms) {
	uint32_t start = millis();
	do  {
		while (gps_serial.available())
			gps.encode(gps_serial.read());
	} while (millis() - start < ms);
}


void updateFolder() {
	track_index = 1;
	/*tracks_count = player.getTrackCount(folder_index);
	DEBUG_PRINT(F("tracks_count ="));
	DEBUG_PRINTLN(tracks_count, DEC);*/
	tracks_count = 10;
}

bool isPlaying() {
	uint8_t avg = 0;
	for (uint8_t i = 0; i < 5; i++) {
		if (digitalRead(PLAYER_BUSY_PIN)) avg++;
		delayGPS(20);
	}
	return avg < 3;
}

void updateVoice() {
	if(talking_size) {
		if (!isPlaying()) {
			player.playTrack(1, talking_list[talking_index]);
			talking_size--;
			talking_index++;
			delayGPS(100);
		}
	}
	else if (talking_index) {
		if (!isPlaying()) {
			player.stop();
			talking_index = 0;
			delayGPS(100);
		}
	}
}

void voice_begin_word(uint8_t track_index) {
	player.stop();
	talking_list[0] = track_index;
	talking_size = 1;
	talking_index = 0;
	delayGPS(20);
}

void voice_add_word(uint8_t track_index) {
	talking_list[talking_size] = track_index;
	talking_size++;
}

void voice_say_number(uint32_t number, void (*func)(uint8_t)) {
	if (number <= HAT_VOICES_MAX_NUMBER)
		func((uint8_t)(number + HAT_VOICES_NUMBER_ZERO));
	else {
		uint8_t len = 0;
		uint32_t reverse = 0;
		
		while (number) {
			reverse *= 10;
			reverse += (number % 10);
			number /= 10;
			len++;
		}
		while (len) {
			func((uint8_t)(reverse % 10 + HAT_VOICES_NUMBER_ZERO));
			reverse /= 10;
			func = &voice_add_word;
			len--;
		}
	}
}

void voice_begin_number(uint32_t number) {
	voice_say_number(number, &voice_begin_word);
}

void voice_add_number(uint32_t number) {
	voice_say_number(number, &voice_add_word);
}




void playDistance() {
	voice_begin_word(HAT_VOICE_DISTANCE_SENSOR);
	voice_add_number(distance_sensor.ping_cm());
	voice_add_word(HAT_VOICE_CENTIMETERS);
}

void playLightLevel() {
	voice_begin_word(HAT_VOICE_LIGHT_SENSOR);
	voice_add_number(light_sensor.readLightLevel());
	voice_add_word(HAT_VOICE_LUX);
}

void playTemperature(bool begin = true) {
	if (begin)	voice_begin_word(HAT_VOICE_TEMPERATURE_SENSOR);
	else		voice_add_word(HAT_VOICE_TEMPERATURE_SENSOR);
	voice_add_number(pressure_sensor.readTemperature());
	voice_add_word(HAT_VOICE_DEGREES_CELSIUS);
}

void playPressure(bool begin = true) {
	if (begin)	voice_begin_word(HAT_VOICE_PRESSURE_SENSOR);
	else		voice_add_word(HAT_VOICE_PRESSURE_SENSOR);
	voice_add_number(pressure_sensor.readPressure());
	voice_add_word(HAT_VOICE_PSI);
}

void playAltitude(bool begin = true) {
	if (begin)	voice_begin_word(HAT_VOICE_ALTITUDE);
	else		voice_add_word(HAT_VOICE_ALTITUDE);
	voice_add_number(pressure_sensor.readAltitude());
	voice_add_word(HAT_VOICE_METERS);
}

void playTime() {
	if (!gps.time.isValid()) {
		voice_begin_word(HAT_VOICE_NO_SATELLITE);
		return;
	}
	uint8_t h = (gps.time.hour()+3) % 24;
	uint8_t m = gps.time.minute();
	
	voice_begin_word(HAT_VOICE_THE_TIME_IS);
	
	uint8_t hh = h % 12;
	if (hh == 0) hh = 12;
	voice_add_number(hh);

	if (m == 15)
		voice_add_word(HAT_VOICE_AND_QUARTER);
	else if (m == 30)
		voice_add_word(HAT_VOICE_AND_HALF);
	else if (m == 1)
		voice_add_word(HAT_VOICE_AND_MINUTE);
	else if (m > 19)
		voice_add_number(m);
	else if (m)
	{
		voice_add_word(HAT_VOICE_AND);
		voice_add_number(m);
		voice_add_word(HAT_VOICE_MINUTES);
	}
	
	if (h < 3 || h > 21)
		voice_add_word(HAT_VOICE_NIGHT);
	else if (h < 12)
		voice_add_word(HAT_VOICE_MORNING);
	else if (h < 18)
		voice_add_word(HAT_VOICE_AFTERNOON);
	else
		voice_add_word(HAT_VOICE_EVENING);
}

void playDate() {
	if (!gps.date.isValid()) {
		voice_begin_word(HAT_VOICE_NO_SATELLITE);
		return;
	}
	uint8_t d = gps.date.day();
	uint8_t m = gps.date.month();
	uint8_t y = gps.date.year()-2000;

	voice_begin_word(HAT_VOICE_TODAY);
	voice_add_word(HAT_VOICE_TODAY + 6);
	
	voice_add_number(d);
	voice_add_word(HAT_VOICE_JANUARY + m - 1);
	voice_add_word(HAT_VOICE_THOUSANDS);
	if (y < 20) voice_add_word(HAT_VOICE_AND);
	voice_add_number(y);
}

bool isBadGPS() {
	if (!gps.location.isValid()) {
		voice_begin_word(HAT_VOICE_NO_SATELLITE);
		return true;
	}
	return false;
}

#endif
