/**
  ******************************************************************************
  * @file   Player.cpp
  * @author  A.E Tec
  * @version V1.0
  * @date    23/4/15
  * @brief   This file provides all the Uctronics MP3 module serial functions.
  ******************************************************************************
    @note
         This driver is for Uctronics MP3 module (http://www.ebay.com/itm/281387900991)
  ******************************************************************************
  * @section  HISTORY
  
    V1.0    Initial version.

  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, ELECHOUSE SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 ELECHOUSE</center></h2>
  ******************************************************************************
  */

#include "Player2.h"

Player2::Player2(uint8_t receivePin, uint8_t transmitPin) : SoftwareSerial(receivePin, transmitPin) {
	SoftwareSerial::begin(9600);
}

void Player2::play() {
	writeCommand(0x0D);
}

void Player2::pause() {
	writeCommand(0x0E);
}

void Player2::stop() {
	writeCommand(0x16);
}

void Player2::next() {
	writeCommand(0x01);
}

void Player2::prev() {
	writeCommand(0x02);
}

void Player2::volumeUp() {
	writeCommand(0x04);
}

void Player2::volumeDown() {
	writeCommand(0x05);
}

void Player2::setVolume(uint8_t volume) {
	if (volume > MAX_VOLUME) volume = MAX_VOLUME;
	writeCommand(0x06, volume);
	delay(100);
}

void Player2::setEqualizer(eq_type_t eq_type) {
	writeCommand(0x07, eq_type);
}

void Player2::reset() {
	writeCommand(0x0C);
	delay(1000);
	writeCommand(0x09, 0x04);
	delay(1000);
}

void Player2::setPower(bool is_on) {
	writeCommand(is_on ? 0x0B : 0x0A);
	delay(1000);
}

void Player2::playTrack(uint16_t track_index) {
	writeCommand(0x03, track_index >> 8, track_index);
}

void Player2::playTrack(uint8_t folder_index, uint8_t track_index) {
	writeCommand(0x12, folder_index, track_index);
}

void Player2::repeatTrack(uint16_t track_index) {
	writeCommand(0x08, track_index >> 8, track_index);
}

void Player2::repeatTrack(uint8_t folder_index, uint8_t track_index) {
	playTrack(folder_index, track_index);
	repeatingTrack();
}

void Player2::repeatTracks(repeat_mode_t repeat_mode) {
	writeCommand(0x11, repeat_mode);
}

void Player2::repeatShuffledTracks() {
	writeCommand(0x18);
}

void Player2::repeatingTrack(bool is_repeated) {
	writeCommand(0x19, is_repeated ? 0 : 1);
	delay(80);
}

void Player2::repeatFolder(uint8_t folder_index) {
	writeCommand(0x17, folder_index);
}

int8_t Player2::waitForTrack(uint16_t timeout) {
	return readCommand((uint32_t)timeout * 1000);
}

int8_t Player2::waitForTracks(uint16_t tracks_count, uint16_t timeout) {
	int8_t return_value = 0;
	while(0 < tracks_count) {
		return_value = waitForTrack(timeout);
		if (return_value) goto l_cleanup;
		tracks_count--;
	}
	
l_cleanup:
	return return_value;
}

bool Player2::isPlaying() {
	delay(80);
	writeCommand(0x42);
	uint8_t status = readCommand();
	return 1 == status;
}

uint8_t Player2::getVolume() {
	writeCommand(0x43);
	return readCommand();
}

eq_type_t Player2::getEqualizer() {
	writeCommand(0x44);
	return (eq_type_t)readCommand();
}

uint16_t Player2::getTracksCount() {
	writeCommand(0x49);
	return readCommand();
}

void Player2::writeCommandStart(uint8_t len, uint8_t id) {
	while(available()) flush();
	write(0x7E);
	write(len);
	write(id);
}

void Player2::writeCommandEnd() {
	while(available()) flush();
	write(0xEF);
	delay(100);
}

void Player2::writeCommand(uint8_t id) {
	writeCommandStart(0x02, id);
	writeCommandEnd();
}

void Player2::writeCommand(uint8_t id, uint8_t a) {
	writeCommandStart(0x03, id);
	write(a);
	writeCommandEnd();
}

void Player2::writeCommand(uint8_t id, uint8_t a, uint8_t b) {
	writeCommandStart(0x04, id);
	write(a);
	write(b);
	writeCommandEnd();
}

uint16_t Player2::readCommand(uint32_t timeout) {
	char buffer[8] = {0};
	uint8_t buffer_size = readToBuffer(buffer, 8, timeout);
	if (0 == buffer_size)
		return 0;
	return strtoul(buffer, NULL, 16);
}

uint16_t Player2::readToBuffer(char * buff, uint16_t len, uint32_t timeout) {
	uint16_t read_bytes = 0;
	uint32_t start_millis = millis();
	
	while ((millis() - start_millis) < timeout) {
		if (read_bytes >= len) goto l_cleanup;
		if (available()) buff[read_bytes++] = read();
		delay(1);
	}
	
l_cleanup:
	return read_bytes;
}
