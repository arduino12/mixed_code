/*
 * Synchronously rotating pendulum (servo motor) and wheel (step motor) using hall sensor.
 * Arad Eizen 21/04/18.
 */
#include <Servo.h>
#include <TM1638.h>
#include <serial_menu.h>


/* uart baud rate in bit per seconds */
#define SERIAL_BAUD_RATE			(115200)
/* set uart read timeout in milliseconds */
#define SERIAL_TIMEOUT				(1)

#define PENDULUM_SERVO_MOTOR_PIN	(2)
#define WHEEL_SENSOR_PIN			(3)
#define WHEEL_US1_PIN				(4)
#define WHEEL_US2_PIN				(5)
#define WHEEL_US3_PIN				(6)
#define WHEEL_DIR_PIN				(7)
#define WHEEL_STEP_PIN				(8)
#define WHEEL_ENABLE_PIN			(A3)
#define TM1638_DATA_PIN				(A0)
#define TM1638_CLOCK_PIN			(A2)
#define TM1638_STROBE_PIN			(A1)

#define PENDULUM_STEPS				(1)
#define PENDULUM_EADGE				(270) // 320 ?
#define PENDULUM_MIDDLE				(1490)
#define PENDULUM_UPDATE_US			(8000) // 13000 ?
#define WHEEL_OFFSET				(2260) // 2500 (short pendulum)
#define WHEEL_MICROSTEPS			(3)
#define WHEEL_UPDATE_US				(904) // 905 (short pendulum)
#define WHEEL_SLOW_UPDATE_US		(5000)
#define MENU_UPDATE_US				(10000)


Servo pendulum_servo;
TM1638 tm1638;


struct {
	uint32_t speed = PENDULUM_UPDATE_US;
	uint8_t steps = PENDULUM_STEPS;
	uint16_t eadge = PENDULUM_EADGE;
	uint16_t middle = PENDULUM_MIDDLE;
	uint16_t degrees;
	uint16_t count;
	int16_t position;
	uint32_t last_update;
} pendulum;

struct {
	uint32_t speed = WHEEL_UPDATE_US;
	/* 0=full, 1=half, 2=quarter, 3=eighth, 4=sixteenth */
	uint8_t microsteps = WHEEL_MICROSTEPS;
	/* -1=backward, 0=disabled, 1=forward */
	int8_t direction = 0;
	uint8_t step_bit;
	uint8_t *step_port;
	uint16_t offset = WHEEL_OFFSET;
	uint16_t position;
	uint32_t last_update;
} wheel;

struct {
	uint32_t speed = MENU_UPDATE_US;
	uint8_t mode = 0;
	uint8_t last_key;
	uint32_t last_update;
} menu;


/* prints serial menu commands help */
void print_help()
{
	Serial.println(F(
		"______________________________________\n"
		"\n"
		"  P E N D U L U M   I N T E R F A C E:\n"
		"______________________________________\n"
		"\n"
		"H\n"
		"    Print this help\n"
		"R\n"
		"    Reset the pendulum\n"
		"A <pendulum.speed[1-10]>\n"
		"    Sets the given pendulum speed\n"
		"B <pendulum.eadge[10-500]>\n"
		"    Sets the given pendulum.eadge\n"
		"C <pendulum.steps[1-10]>\n"
		"    Sets the given pendulum.steps\n"
		"D <pendulum.middle[10-500]>\n"
		"    Sets the given pendulum.middle\n"
		"E <wheel.speed[1-10]>\n"
		"    Sets the given wheel.speed\n"
		"F <wheel.offset[10-500]>\n"
		"    Sets the given wheel.offset\n"
	));
}

static inline void delay_micros(uint32_t delay_us)
{
	uint32_t start_us = micros();
	while (micros() - start_us < delay_us);
}

void wheel_step()
{
	// digitalWrite(WHEEL_STEP_PIN, HIGH);
	*wheel.step_port |= wheel.step_bit;
	delay_micros(1);
	*wheel.step_port &= ~wheel.step_bit;
	// digitalWrite(WHEEL_STEP_PIN, LOW);
	delay_micros(1);
}
void wheel_update()
{
	const uint8_t microsteps_bits[] = {0, 1, 2, 3, 7};
	uint8_t ms_bits = microsteps_bits[wheel.microsteps];
	digitalWrite(WHEEL_US1_PIN, ms_bits & 1);
	digitalWrite(WHEEL_US2_PIN, ms_bits & 2);
	digitalWrite(WHEEL_US3_PIN, ms_bits & 4);
	digitalWrite(WHEEL_STEP_PIN, LOW);
	digitalWrite(WHEEL_DIR_PIN, wheel.direction == -1);
	digitalWrite(WHEEL_ENABLE_PIN, wheel.direction == 0);
}

bool wheel_sensor_on()
{
	return digitalRead(WHEEL_SENSOR_PIN);
}

void pendulum_move()
{
	pendulum_servo.writeMicroseconds(pendulum.middle + pendulum.position);
}

void calibration()
{
	uint8_t blink_state = 0, blink_count = 0;

	tm1638.clearSegments();
	tm1638.sendChars_P(PSTR("CAL"));
	/* move pendulum to start position */
	for (uint16_t i = 0; i <= PENDULUM_EADGE; i++) {
		pendulum.position = i;
		pendulum_move();
		delay(10);
		tm1638.sendLed(LED_COLOR_RED, i * 2 / PENDULUM_EADGE);
	}
	pendulum.degrees = 90;
	pendulum.count = 0;

	/* move wheel to start position */
	wheel.direction = -1;
	wheel_update();
	while (wheel_sensor_on()) {
		wheel_step();
		delay_micros(WHEEL_SLOW_UPDATE_US);
		if (blink_count++ > 70) {
			blink_count = 0;
			tm1638.sendLed(blink_state & LED_COLOR_RED, 3);
			blink_state = ~blink_state;
		}
	}
	wheel.direction = 1;
	wheel_update();
	while (!wheel_sensor_on()) {
		wheel_step();
		delay_micros(WHEEL_SLOW_UPDATE_US);
		if (blink_count++ > 70) {
			blink_count = 0;
			blink_state = ~blink_state;
			tm1638.sendLed(blink_state & LED_COLOR_RED, 3);
		}
	}
	tm1638.sendLed(LED_COLOR_RED, 3);
	blink_state = blink_count = 0;

	uint16_t wheel_offset = wheel.offset;
	for (uint32_t d = WHEEL_SLOW_UPDATE_US; d > wheel.speed; d -= 100) {
		for (uint16_t i = 0; i < 20; i++) {
			wheel_step();
			wheel_offset--;
			delay_micros(d);
		}
		if (blink_count++ > 10) {
			blink_count = 0;
			tm1638.sendLed(LED_COLOR_RED, 4 + blink_state++);
		}
	}
	tm1638.sendLed(LED_COLOR_RED, 7);

	while (wheel_offset--) {
		wheel_step();
		delay_micros(wheel.speed);
	}

	tm1638.clearLeds();
	tm1638.clearSegments();
}

void menu_update()
{
	uint16_t a = 0, b = 0;
	uint8_t menu_key = tm1638.getSingleButton();

	if (serial_menu_update()) {
		switch (command.buffer[0]) {
			case 'H':
				print_help();
				break;
			case 'R':
				calibration();
				break;
			case 'A':
				a = pendulum.speed;
				sscanf(command.buffer + 2, "%d", &a);
				Serial.print(F("pendulum.speed="));
				Serial.println(a);
				pendulum.speed = a;
				break;
			case 'B':
				a = pendulum.eadge;
				sscanf(command.buffer + 2, "%d", &a);
				Serial.print(F("pendulum.eadge="));
				Serial.println(a);
				pendulum.eadge = a;
				break;
			case 'C':
				a = pendulum.steps;
				sscanf(command.buffer + 2, "%d", &a);
				Serial.print(F("pendulum.steps="));
				Serial.println(a);
				pendulum.steps = a;
				break;
			case 'D':
				a = pendulum.middle;
				sscanf(command.buffer + 2, "%d", &a);
				Serial.print(F("pendulum.middle="));
				Serial.println(a);
				pendulum.middle = a;
				break;
			case 'E':
				a = wheel.speed;
				sscanf(command.buffer + 2, "%d", &a);
				Serial.print(F("wheel.speed="));
				Serial.println(a);
				wheel.speed = a;
				break;
			case 'F':
				a = wheel.offset;
				sscanf(command.buffer + 2, "%d", &a);
				Serial.print(F("wheel.offset="));
				Serial.println(a);
				wheel.offset = a;
				break;
		}
		return;
	}

	switch (menu_key) {
		case 1:
			if (menu.last_key)
				break;
			if (menu.mode) {
				tm1638.sendChars_P(PSTR("OFF"));
				menu.mode = 0;
				pendulum_servo.detach();
				wheel.direction = 0;
				wheel_update();
			}
			else {
				menu.mode = 1;
				pendulum_servo.attach(PENDULUM_SERVO_MOTOR_PIN);
				calibration();
				tm1638.sendChars_P(PSTR("On"));
			}
			break;
	}
	menu.last_key = menu_key;
}

void setup()
{
	/* initialize uart for serial communication */
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.setTimeout(SERIAL_TIMEOUT);

	/* initialize tm1638 display */
	tm1638.begin(TM1638_DATA_PIN, TM1638_CLOCK_PIN, TM1638_STROBE_PIN);
	tm1638.clearLeds();
	tm1638.clearSegments();

	/* initialize pendulum and wheel motors */
	// pendulum_servo.attach(PENDULUM_SERVO_MOTOR_PIN);
	pinMode(WHEEL_US1_PIN, OUTPUT);
	pinMode(WHEEL_US2_PIN, OUTPUT);
	pinMode(WHEEL_US3_PIN, OUTPUT);
	pinMode(WHEEL_STEP_PIN, OUTPUT);
	pinMode(WHEEL_DIR_PIN, OUTPUT);
	pinMode(WHEEL_ENABLE_PIN, OUTPUT);
	wheel.step_bit = digitalPinToBitMask(WHEEL_STEP_PIN);
	wheel.step_port = portInputRegister(digitalPinToPort(WHEEL_STEP_PIN));
	wheel_update();
	
	/* calibrate wheel and pendulum positions */
	// calibration();

	/* print banner to screen */
	char *buffer = "ArAd 2018";
	for (int8_t i = SEGMENTS_COUNT; i > -SEGMENTS_COUNT; i--) {
		tm1638.sendChars(buffer, i);
		delay(300);
		tm1638.clearSegments(i);
	}

	tm1638.sendChars_P(PSTR("OFF"));
	/* print commands to terminal */
	print_help();
}

void loop()
{
	uint32_t cur_ms = micros();

	if (menu.last_update < cur_ms) {
		menu.last_update = cur_ms + menu.speed;
		menu_update();
	}

	if (menu.mode) {
		if (wheel.last_update < cur_ms) {
			wheel.last_update = cur_ms + wheel.speed;
			wheel_step();
		}
		if (pendulum.last_update < cur_ms) {
			pendulum.last_update = cur_ms + pendulum.speed;
			pendulum.degrees += pendulum.steps;
			if (pendulum.degrees >= 360) {
				pendulum.degrees -= 360;
				pendulum.count++;
				tm1638.printIntegr(pendulum.count, 4, 4, true);
			}
			pendulum.position = sin(radians(pendulum.degrees)) * pendulum.eadge;
			pendulum_move();
		}
	}
}
