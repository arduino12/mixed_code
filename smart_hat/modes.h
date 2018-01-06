#ifndef _MODES_H_
#define _MODES_H_

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
#include "voice.h"

extern Player player;
extern TinyGPSPlus gps;
extern Adafruit_BMP085 pressure_sensor;
extern BH1750 light_sensor;
extern Ultrasonic distance_sensor;
extern Buttons buttons;
extern SoftwareSerial gps_serial;

extern bitmask_t buttons_bitmask;
extern uint8_t folder_index;
extern uint8_t folders_count;
extern uint8_t track_index;
extern uint8_t tracks_count;

extern uint8_t dis_word;
extern uint8_t led_power;
extern uint8_t fan_power;
extern uint8_t dis_trigger;
extern uint8_t led_trigger;
extern uint8_t fan_trigger;
extern bool is_led_on;
extern bool is_fan_on;



void updateDisWord() {
	voice_begin_word(HAT_VOICE_STOP + dis_word);
}

void sensorsUpdate() {
	if (dis_trigger > distance_sensor.ping_cm()) {
			if (!isPlaying()) {
				updateDisWord();
			}
	}
	if (led_trigger > light_sensor.readLightLevel()) {
		if (!is_led_on) {
			analogWrite(LED_PIN, led_power);
			is_led_on = true;
		}
	}
	else {
		if (is_led_on) {
			analogWrite(LED_PIN, 0);
			is_led_on = false;
		}
	}
	if (fan_trigger < pressure_sensor.readTemperature()) {
		if (!is_fan_on) {
			analogWrite(FAN_PIN, fan_power);
			is_fan_on = true;
		}
	}
	else {
		if (is_fan_on) {
			analogWrite(FAN_PIN, 0);
			is_fan_on = false;
		}
	}
}

void updateDisTrigger() {
	sensorsUpdate();
	voice_begin_number(dis_trigger);
	buttons.setNextPulseMillis(500);
}

void updateLightPower() {
	is_led_on = !is_led_on;
	sensorsUpdate();
	voice_begin_number(led_power);
}

void updateFanPower() {
	is_fan_on = !is_fan_on;
	sensorsUpdate();
	voice_begin_number(fan_power);
}

void updateLightTrigger() {
	sensorsUpdate();
	voice_begin_number(led_trigger);
	buttons.setNextPulseMillis(500);
}

void updateFanTrigger() {
	sensorsUpdate();
	voice_begin_number(fan_trigger);
	buttons.setNextPulseMillis(500);
}

void onMusicShortRelease() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		track_index++;
		if (track_index > tracks_count) track_index = 1;
		//player.playTrack(folder_index, track_index);
		player.repeatFolder(folder_index);
		/*for (uint8_t i = 1; i < tracks_count; i++) {
			player.next();
			delay(5);
		}*/
		//player.next();
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		if (track_index > 1)	track_index--;
		else					track_index = tracks_count;
		//player.playTrack(folder_index, track_index);
		player.repeatFolder(folder_index);
		//player.prev(); 
	}
	else if	(BUTTON_ENTER_MASK == buttons_bitmask) {
		if (isPlaying())
			player.pause();
		else {
			//player.playTrack(folder_index, track_index);
			player.repeatFolder(folder_index);
		}
		//player.repeatFolder(folder_index);
	}
	else if	(BUTTON_UP_DOWN_MASK == buttons_bitmask) {
		voice_begin_word(HAT_VOICE_FIX_ORDER);
	}
}

void onMusicLongPressAndPulse() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		player.volumeUp();
		buttons.setNextPulseMillis(500);
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		player.volumeDown();
		buttons.setNextPulseMillis(500);
	}
	else if	(BUTTON_ENTER_MASK == buttons_bitmask) {
		uint8_t equalizer = player.getEqualizer();
		voice_begin_word(HAT_VOICE_EQ_NORMAL + equalizer);
		equalizer++;
		if (equalizer >= EQ_COUNT) equalizer = 0;
		player.setEqualizer((eq_type_t)equalizer);
	}
	else if	(BUTTON_UP_DOWN_MASK == buttons_bitmask) {
		folder_index++;
		if (folder_index > folders_count)	folder_index = 2;
		voice_begin_word(HAT_VOICE_FOLDER_NUMBER);
		voice_add_number(folder_index);
		buttons.setNextPulseMillis(4000);
		updateFolder();
	}
}

void onDistanceShortRelease() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		dis_word++;
		if (dis_word > 2) dis_word = 2;
		updateDisWord();
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		if (dis_word) dis_word--;
		updateDisWord();
	}
	else if	(BUTTON_ENTER_MASK == buttons_bitmask) {
		playDistance();
	}
}

void onDistanceLongPressAndPulse() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		if (dis_trigger == 255) return;
		dis_trigger++;
		updateDisTrigger();
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		if (dis_trigger == 0) return;
		dis_trigger--;
		updateDisTrigger();
	}
}

void onDistanceLongRelease() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		
	}
}

void onLightShortRelease() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		if (led_power == 250) return;
		led_power += 50;
		updateLightPower();
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		if (led_power == 0) return;
		led_power -= 50;
		updateLightPower();
	}
	else if	(BUTTON_ENTER_MASK == buttons_bitmask) {
		playLightLevel();
	}
}

void onLightLongPressAndPulse() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		if (led_trigger == 255) return;
		led_trigger++;
		updateLightTrigger();
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		if (led_trigger == 0) return;
		led_trigger--;
		updateLightTrigger();
	}
	else if	(BUTTON_ENTER_MASK == buttons_bitmask) {
		if (is_led_on)	voice_begin_word(HAT_VOICE_LED_ON);
		else			voice_begin_word(HAT_VOICE_LED_OFF);
	}
}

void onLightLongRelease() {
	if 		((BUTTON_UP_MASK == buttons_bitmask) ||
			(BUTTON_DOWN_MASK == buttons_bitmask)) {
		voice_begin_word(fan_trigger);
	}
}

void onFanShortRelease() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		if (fan_power == 250) return;
		fan_power += 50;
		updateFanPower();
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		if (fan_power == 0) return;
		fan_power -= 50;
		updateFanPower();
	}
	else if	(BUTTON_ENTER_MASK == buttons_bitmask) {
		playTemperature();
	}
}

void onFanLongPressAndPulse() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		if (fan_trigger == 255) return;
		fan_trigger++;
		updateFanTrigger();
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		if (fan_trigger == 0) return;
		fan_trigger--;
		updateFanTrigger();
	}
	else if	(BUTTON_ENTER_MASK == buttons_bitmask) {
		if (is_fan_on)	voice_begin_word(HAT_VOICE_FAN_ON);
		else			voice_begin_word(HAT_VOICE_FAN_OFF);
	}
}

void onFanLongRelease() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		
	}
}

void onInformationShortRelease() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		playTime();
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		if (isBadGPS()) return;
		voice_add_word(HAT_VOICE_SPEED);
		voice_add_number(gps.speed.kmph());
		voice_add_word(HAT_VOICE_KPH);
		
		voice_add_word(HAT_VOICE_DIRECTION);
		voice_add_number(gps.course.deg());
		voice_add_word(HAT_VOICE_DEGREES);		
	}
	else if	(BUTTON_ENTER_MASK == buttons_bitmask) {
		playLightLevel();
		playTemperature(false);
	}
}

void onInformationLongPressAndPulse() {
	if 		(BUTTON_UP_MASK == buttons_bitmask) {
		playDate();
	}
	else if	(BUTTON_DOWN_MASK == buttons_bitmask) {
		if (isBadGPS()) return;
		voice_begin_number(gps.satellites.value());
		voice_add_word(HAT_VOICE_SATELLITES);
		
		voice_add_word(HAT_VOICE_ALTITUDE);
		voice_add_number(gps.altitude.meters());
		voice_add_word(HAT_VOICE_METERS);

		voice_add_word(HAT_VOICE_NORTH);
		voice_add_word(HAT_VOICE_SOUTH);
		voice_add_number(gps.location.lat());
		voice_add_word(HAT_VOICE_EAST);
		voice_add_word(HAT_VOICE_WEST);
		voice_add_number(gps.location.lng());
	}
	else if	(BUTTON_ENTER_MASK == buttons_bitmask) {
		playDistance();
		playAltitude(false);
		playPressure(false);
	}
	buttons.setNextPulseMillis(99999);
}

#endif
