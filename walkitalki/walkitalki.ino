#include "Buttons.h"
#include <SoftwareSerial.h>
#include "Player2.h"
#include <EEPROM.h>

#define FACTORY_RESET_PIN			(3)
#define BATTERY_VOLTAGE_PIN_2		(A6)
#define BATTERY_VOLTAGE_PIN			(A7)
#define AMPLIFAIER_RELAY_PIN		(2)
#define PTT_RELAY_PIN				(4)
#define VHF_RADIO_PIN				(5)
#define UHF_RADIO_PIN				(6)
#define INC_DEC_CHANNEL_PIN			(7)
#define CHARGER_RESET_PIN			(8)
#define PLAYER_TX_PIN				(9)
#define PLAYER_RX_PIN				(10)

#define BATTERY_VOLTAGE_VOICE		(1)
#define VOLT_VOICE					(2)
#define VHF_CHANNEL_VOICE			(3)
#define UHF_CHANNEL_VOICE			(4)
#define DOT_VOICE					(5)
#define NUMBER_ZERO_VOICE			(6)

#define CHARGER_RESET_TIME			(30 * 60)
#define AMPLIFAIER_TEST_TIME		(1 * 60)
#define CONTROLLER_STABILIZE_TIME	(30000)

#define ANALOG_SAMPLES_COUNT		(10)
#define ANALOG_SAMPLES_TIMEOUT		(100)

#define VHF_CHANNEL_EEPROM			(1)
#define UHF_CHANNEL_EEPROM			(2)

#define CHANNEL_MIN					(1)
#define CHANNEL_MAX					(16)

#define AMPLIFAIER_MIN_VOLTAGE		(1070)
#define AMPLIFAIER_START_VOLTAGE	(1100)

Player2 player(PLAYER_RX_PIN, PLAYER_TX_PIN);
Buttons dtmf;
Buttons factory;

uint32_t old_millis = 0;
bitmask_t old_dtmf = 0;


uint16_t readVccValue() {
	uint16_t result;
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	delay(2);
	ADCSRA |= _BV(ADSC);
	while (bit_is_set(ADCSRA,ADSC));
	result = (ADCL | (ADCH << 8));
	//result = 1126400L / result; // Back-calculate AVcc in mV
	return result;
}

uint16_t readBatteryValue_old() {
	uint16_t new_value, old_value = 0xFFFF;
	
	for (uint8_t i = 0, j = 0; i < ANALOG_SAMPLES_COUNT; i++, j++) {
		delay(1);
		new_value = analogRead(BATTERY_VOLTAGE_PIN);
		if (old_value == new_value) continue;
		if (j > ANALOG_SAMPLES_TIMEOUT) break;
		old_value = new_value;
		i = 0;
	}
	
	return old_value;
}

uint16_t readBatteryValue() {
	uint16_t return_value = 0;
	analogRead(BATTERY_VOLTAGE_PIN);
	for (uint8_t i = 0; i < ANALOG_SAMPLES_COUNT; i++) {
		delay(1);
		return_value += analogRead(BATTERY_VOLTAGE_PIN);
	}
	return (return_value / ANALOG_SAMPLES_COUNT) / 0.337; // 0.345
}

uint16_t readBatteryValue2() {
	uint16_t return_value = 0;
	analogRead(BATTERY_VOLTAGE_PIN_2);
	for (uint8_t i = 0; i < ANALOG_SAMPLES_COUNT; i++) {
		delay(1);
		return_value += analogRead(BATTERY_VOLTAGE_PIN_2);
	}
	return (return_value / ANALOG_SAMPLES_COUNT) / 0.344; // 0.352
}

void setup() {
	Serial.begin(115200);
	Serial.println(F("start ()"));
	
	pinMode(BATTERY_VOLTAGE_PIN,	INPUT);
	pinMode(PTT_RELAY_PIN,			OUTPUT);
	pinMode(UHF_RADIO_PIN,			OUTPUT);
	pinMode(VHF_RADIO_PIN,			OUTPUT);
	pinMode(INC_DEC_CHANNEL_PIN,	OUTPUT);
	pinMode(CHARGER_RESET_PIN,		OUTPUT);

	PINC &= 0xF0;
	DDRC &= 0xF0;					// (A0 - A3) INPUTS
	
	Serial.println(F("waiting for controller to stabilize"));
	delay(CONTROLLER_STABILIZE_TIME);
	
	player.reset();
	player.setVolume(20);
	
	dtmf.begin(
		getDtmfBitmaskFunc
	);
	dtmf.setPressMillis(
		100,
		60000,
		60000
	);
	dtmf.setEventsFuncs(
		onLongPress,
		onLongRelease
	);
	
	uint8_t button_pin[] = {FACTORY_RESET_PIN};
	factory.begin(
		button_pin,
		sizeof(button_pin)
	);
	dtmf.setPressMillis(
		30,
		60000,
		60000
	);
	factory.setShortReleaseFunc(
		factoryShortRelease
	);
	
	Serial.println(F("loop ()"));
}

void loop() {
	if (0 == (old_millis++ % ((uint32_t)100 * CHARGER_RESET_TIME))) {
		old_millis = 1;
		changeRelay(CHARGER_RESET_PIN, true);
		delay(2000);
		sayBatteryVoltage();
		changeRelay(CHARGER_RESET_PIN, false);
	}
	if (0 == (old_millis % ((uint32_t)100 * AMPLIFAIER_TEST_TIME))) {
		Serial.println(F("AMPLIFAIER_TEST"));
		uint16_t battery_voltage = readBatteryValue2();
		Serial.println(battery_voltage, DEC);
		if (battery_voltage < AMPLIFAIER_MIN_VOLTAGE) {
			Serial.println(F("MIN"));
			changeRelay(AMPLIFAIER_RELAY_PIN, true);
		}
		else if (battery_voltage > AMPLIFAIER_START_VOLTAGE) {
			Serial.println(F("START"));
			changeRelay(AMPLIFAIER_RELAY_PIN, false);
		}
	}
	
	dtmf.update();
	factory.update();
	delay(10);
}

void changeRelay(uint8_t pin, bool is_on) {
	digitalWrite(pin, is_on);
	delay(300);
}

void playFile(uint8_t file, uint16_t ms) {
	player.playTrack(file);
	delay(ms);
}

void playNumber(uint8_t number, uint16_t ms) {
	player.playTrack(NUMBER_ZERO_VOICE + number);
	delay(ms);
}

uint8_t changeChannel(bool is_vhf, bool is_inc) {
	uint8_t channel = EEPROM.read(is_vhf ? VHF_CHANNEL_EEPROM : UHF_CHANNEL_EEPROM);
	
	if (is_inc)	channel = (channel < CHANNEL_MAX ? channel + 1: CHANNEL_MIN);
	else		channel = (channel > CHANNEL_MIN ? channel - 1: CHANNEL_MAX);
	EEPROM.update(is_vhf ? VHF_CHANNEL_EEPROM : UHF_CHANNEL_EEPROM, channel);
	
	sayChannelNumber(is_vhf, channel);
	
	if (is_inc) changeRelay(INC_DEC_CHANNEL_PIN, true);
	changeRelay(is_vhf ? VHF_RADIO_PIN : UHF_RADIO_PIN, true);
	changeRelay(is_vhf ? VHF_RADIO_PIN : UHF_RADIO_PIN, false);
	if (is_inc) changeRelay(INC_DEC_CHANNEL_PIN, false);
	return channel;
}

void sayChannelNumber(bool is_vhf, uint8_t channe) {
	dtmf.update();
	if (dtmf.isDown()) return;
	changeRelay(PTT_RELAY_PIN, true);
	playFile(is_vhf ? VHF_CHANNEL_VOICE : UHF_CHANNEL_VOICE, 2300);
	playNumber(channe, 1600);
	changeRelay(PTT_RELAY_PIN, false);
}

void sayBatteryVoltage() {
	dtmf.update();
	if (dtmf.isDown()) return;
	//uint16_t battery_voltage = analogRead(BATTERY_VOLTAGE_PIN) / 0.356;
	uint16_t battery_voltage = readBatteryValue();
	uint16_t battery_voltage_2 = readBatteryValue2();
	changeRelay(PTT_RELAY_PIN, true);
	sayVoltage(battery_voltage);
	sayVoltage(battery_voltage_2);
	changeRelay(PTT_RELAY_PIN, false);
}

void sayVoltage(uint16_t battery_voltage) {
	playFile(BATTERY_VOLTAGE_VOICE, 1800);
	playNumber(battery_voltage / 100, 800);
	if (battery_voltage >= 10)
		delay(600);
	playFile(DOT_VOICE, 1300);
	if ((battery_voltage % 100) < 10)
		playNumber(0, 500);
	playNumber(battery_voltage % 100, 1600);
	playFile(VOLT_VOICE, 900);
}

bitmask_t getDtmfBitmaskFunc() {
	return ~PINC & 0x0F;
}

void factoryShortRelease() {
	Serial.println('C');
	EEPROM.update(VHF_CHANNEL_EEPROM, CHANNEL_MIN);
	EEPROM.update(UHF_CHANNEL_EEPROM, CHANNEL_MIN);
}

void onLongPress() {
	Serial.println('A');
	old_dtmf = getDtmfBitmaskFunc();
}

void onLongRelease() {
	Serial.println('B');
	if (old_dtmf == 0x04){
		Serial.print(F("battery voltage: "));
		//Serial.print((float)analogRead(BATTERY_VOLTAGE_PIN) / (float)35.6);
		Serial.print(readBatteryValue() * 10);
		Serial.println(F(" volts"));
		Serial.print(F("battery voltage 2: "));
		Serial.print(readBatteryValue2() * 10);
		Serial.println(F(" volts"));
		sayBatteryVoltage();
	}	
	else if (old_dtmf == 0x01) {
		Serial.print(F("vhf: "));
		Serial.println(changeChannel(true, false));
	}
	else if (old_dtmf == 0x05) {
		Serial.print(F("VHF: "));
		Serial.println(changeChannel(true, true));
	}
	else if (old_dtmf == 0x09) {
		Serial.print(F("uhf: "));
		Serial.println(changeChannel(false, false));
	}
	else if (old_dtmf == 0x0D) {
		Serial.print(F("UHF: "));
		Serial.println(changeChannel(false, true));
	}
}