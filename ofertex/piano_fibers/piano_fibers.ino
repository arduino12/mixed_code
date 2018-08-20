#include <adc_touch.h>
#include <common.h>

#define SPEAKER_PIN	(2)
#define TOUCH_FIBER_PIN_0			(A0)
#define TOUCH_FIBER_PIN_1			(A1)
#define TOUCH_FIBER_PIN_2			(A2)
#define TOUCH_FIBER_PIN_3			(A3)
#define WATER_FIBER_PIN				(A6)
#define RESISTIVE_FIBER_PIN			(A7)
#define RESISTIVE_FIBER_THRESHOLD	(100)
#define TOUCH_FIBER_THRESHOLD		(100)
#define WATER_FIBER_THRESHOLD		(700)

uint8_t touch_pins[] = {A0, A1, A2, A3};
#define TOUCH_COUNT					(ARRAY_SIZE(touch_pins))

uint16_t notes[] = {
	262, 294, 330, 349, 392, 440, 494,
	523, 587, 659, 698, 784, 880, 988,
	1047, 1175, 1319, 1397, 1568, 1760, 1976,
};
#define NOTES_COUNT					(ARRAY_SIZE(notes))

int16_t touch_ref[TOUCH_COUNT];


void note_beep(uint8_t index) {
	if (index < NOTES_COUNT)
		tone(SPEAKER_PIN, notes[index]);
	else
		noTone(SPEAKER_PIN);
}

void setup() {
	Serial.begin(115200);
	Serial.println("start!");
	for (uint8_t i = 0; i < TOUCH_COUNT; i++) {
		touch_ref[i] = adc_touch_read(touch_pins[i], 500) + TOUCH_FIBER_THRESHOLD;
		Serial.print("touch_ref[");
		Serial.print(i);
		Serial.print("]=");
		Serial.println(touch_ref[i]);
	}
	pinMode(WATER_FIBER_PIN, INPUT_PULLUP);
	pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
	bool no_note = true;
	int16_t v;

	for (uint8_t i = 0; i < TOUCH_COUNT; i++) {
		v = adc_touch_read(touch_pins[i], 50) - touch_ref[i];
		if (v > RESISTIVE_FIBER_THRESHOLD) {
			note_beep(map(i, 0, TOUCH_COUNT, 0, NOTES_COUNT - 1));
			Serial.print("touch_val[");
			Serial.print(i);
			Serial.print("]=");
			Serial.println(v);
			no_note = false;
		}
	}

	v = analogRead(RESISTIVE_FIBER_PIN);
	if (v > RESISTIVE_FIBER_THRESHOLD) {
		note_beep(map(v, 0, 1023, 0, NOTES_COUNT - 1));
		Serial.print("resistive_val=");
		Serial.println(v);
		no_note = false;
	}

	if (no_note)
		note_beep(NOTES_COUNT);

	v = analogRead(WATER_FIBER_PIN);
	digitalWrite(LED_BUILTIN, v < WATER_FIBER_THRESHOLD);
	// delay(100);
}
