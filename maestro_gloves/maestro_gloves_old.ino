/*
	Maestro Gloves V1.0 Software!
	=============================
	This program controls 2 DC motors with IR encoders and 8 servo motors.
	Main board: Arduino-Pro-Mini.

	Commands:
		* calibrate				C
		* move left				L + number
		* move right			R + number
		* move fingers			K + number (bitmask of down fingers)
		
	After each command endded send telemetry (0 = OK, 1 = ERROR):
		* 						C + 0
		* 						L + 0
		* 						R + 0
		* 						K + 0

	Protocol:
		* use textual single line protocol ending with \n and binary protocol!
		* max length 60 bytes
		* listen for serial in loop
		* when full command recived - execut

	Mahanet 2016,
	Maestro Gloves team:
	Arad Barak Shalev Tzahi Yossi.

pass string with args to func

parse all args and pass to func

while moving left or right or both, fingers must be up! also ignor fingers and side commands

the two commands can run at the same time are left and right with no collusion

and oposit side fingers
	
*/

//{ includes:
#include <Servo.h>
//}

//{ defines:
#define DEBUG					// debug to uart (serial) if defined
#ifdef DEBUG
	#define DEBUG_INIT			Serial.begin(115200)
	#define DEBUG_WRITE(...)	Serial.write(__VA_ARGS__)
	#define DEBUG_PRINT(...)	Serial.print(__VA_ARGS__)
	#define DEBUG_PRINTLN(...)	Serial.println(__VA_ARGS__)
#else
	#define DEBUG_INIT
	#define DEBUG_WRITE(...)
	#define DEBUG_PRINT(...)
	#define DEBUG_PRINTLN(...)
#endif

#define SERVO_COUNT				(4)
#define SERVO_MIN				(0)
#define SERVO_MAX				(180)
#define EFECT_STEPS				(10)

#define LEFT_IR_PIN				(2)
#define RIGHT_IR_PIN			(3)
#define LEFT_LIMIT_PIN			(4)
#define LEFT_ENABLE_PIN			(5)
#define LEFT_A_PIN				(6)
#define LEFT_B_PIN				(7)
#define RIGHT_LIMIT_PIN			(8)
#define RIGHT_ENABLE_PIN		(9)
#define RIGHT_A_PIN				(10)
#define RIGHT_B_PIN				(11)
#define SDA_PIN					(A4)
#define SCL_PIN					(A5)
//}

//{ globals
Servo servos[SERVO_COUNT];
//}

//{ helpers
void fingers_wave() {
	/*
	0000
	1000
	2100
	3210
	2321
	1232
	0123
	0012
	0001
	*/
	static uint8_t i_offset = 0;
	uint8_t j = 0;

	for (uint8_t i = SERVO_COUNT - 1; i > 0; i--)
		servos[i].write(servos[i - 1].read());
	
	if		(i_offset == 1 || i_offset == 5)	j = 1 * ((SERVO_MAX - SERVO_MIN) / 3);
	else if	(i_offset == 2 || i_offset == 4)	j = 2 * ((SERVO_MAX - SERVO_MIN) / 3);
	else if	(i_offset == 3)						j = 3 * ((SERVO_MAX - SERVO_MIN) / 3);
	servos[0].write(j);
	if (++i_offset >= 9) i_offset = 0;
}

void fingers_all() {
	static uint8_t pos = SERVO_MIN;

	for (uint8_t i = 0; i < SERVO_COUNT; i++)
		servos[i].write(pos);

	if (++pos >= SERVO_MAX) pos = SERVO_MIN;
}

bool decode_integer(char c, uint32_t * number) {
	if (c >= '0' && c <= '9') {
		*number *= 10;
		*number += c - '0';
		return false;
	}
	return true;
}

void decode_command(char * command_buffer, uint8_t command_buffer_size) {
	switch (command_buffer[0]) {
		case 'C':
			arguments_count = 0;
		break;
		case 'L':
			arguments_count = 1;
			// sscanf_P(command_buffer, PSTR("%d"), &num);
		break;
		case 'R':
			arguments_count = 1;
		break;
		case 'K':
			arguments_count = 1;
		break;
	}
}

void decode_command(char c) {
	static char command_buffer[MAX_COMAND_SIZE];
	static uint8_t command_buffer_size = 0;
	if (c == END_COMMAND) {
		
		command_buffer_size = 0;
	}
	else if (command_buffer_size >= (MAX_COMAND_SIZE - 1)) {
		command_buffer_size = 0;
	}
	else {
		command_buffer_size[command_buffer_size] = c;
		command_buffer_size++;
	}
}

void decode_command_id(char c) {
	if		(c == 'C') {
		arguments_count = 0;
	}
	else if	(c == 'L') {
		arguments_count = 1;
	}
	else if	(c == 'R') {
		arguments_count = 1;
	}
	else if	(c == 'K') {
		arguments_count = 1;
	}
	// decoder state...
}

void decoder_feed(char c) {
	static uint8_t command_state = 0;
	static uint8_t command_tag = 0;
	static char command_buffer[MAX_COMAND_SIZE];
	static uint8_t command_buffer_size = 0;
	switch (c) {
		case ' ':
			command_arguments++;
		break;
		case '\n':
			command_state = 0;
		break;
	}
	
	if		(c == 'C') {
		arguments_count = 0;
	}
	else if	(c == 'L') {
		arguments_count = 1;
	}
	else if	(c == 'R') {
		arguments_count = 1;
	}
	else if	(c == 'K') {
		arguments_count = 1;
	}
	else if	(c == ' ') {
		if (arguments_count) arguments_count--;
		//else ...
	}
	else if	(c == '\n') {
		
	}
	else if	(c >= '0' && c <= '9') {
		
	}
	else {
		DEBUG_PRINT("decoder error: ");
		DEBUG_PRINTLN(c);
	}
}

void calibrate() {
	motors_status = MOVE_LEFT | MOVE_RIGHT;
	// TODO: blocking or not..?
}

void move_left(uint32_t pos) {
	// TODO
}

void move_right(uint32_t pos) {
	// TODO
}

void move_fingers(uint8_t fingers_mask) {
	// TODO
}

//}

//{ setup and loop
void setup() {
	/* initialize the debug UART */
	DEBUG_INIT;
	DEBUG_PRINTLN(F("setup"));

	/* initialize the servo motors I2C expender */
	for (uint8_t i = 0; i < SERVO_COUNT; i++) {
		servos[i].attach(i + 10);
		servos[i].write(0);
	}
	
	/* attach interrupt for IR encoders */
	// pinMode(SD_EJECT_PIN, INPUT_PULLUP);
	// attachInterrupt(digitalPinToInterrupt(SD_EJECT_PIN), sd_eject_interrupt, FALLING);	

	DEBUG_PRINTLN(F("loop"));
}

void loop() {
	/*
	uint32_t current_ms = millis();
	if (Serial.available()) decoder_feed(Serial.read());
	*/
	fingers_wave();
	// fingers_all();
	delay(300);
}
//}
