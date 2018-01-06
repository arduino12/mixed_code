
#define SERIAL_BAUD_RATE		(115200)
#define SERIAL_TIMEOUT			(1)
#define ACS712_PIN				(A3)
#define INDUCTION_RELAY_PIN		(2)

#define COMMAND_BUFFER_SIZE		(100)
#define COMMAND_TIMEOUT_MS		(150)
#define SCREEN_SAVER_S			(40)

#define ACS712_MAX_CURRENT		(30)
#define ACS712_SAMPLES			(10)

#if ACS712_MAX_CURRENT == 5
	#define ACS712_MUL			(0.0264)
	#define ACS712_SUB			(13.51)
#elif ACS712_MAX_CURRENT == 20
	#define ACS712_MUL			(0.19)
	#define ACS712_SUB			(25)
#elif ACS712_MAX_CURRENT == 30
	#define ACS712_MUL			(0.044)
	#define ACS712_SUB			(3.78)
#endif


struct {
	uint8_t buffer[COMMAND_BUFFER_SIZE];
	uint8_t buffer_index;
	uint32_t timeout;
} command;

uint32_t screen_saver_ms = SCREEN_SAVER_S * 1000;
float idle_induction_current = 0;
bool is_auto_induction = false;

uint16_t _interactive_menu_read_16(uint16_t number) {
	uint16_t i = number;
	sscanf(command.buffer + 2, "%d", &i);
	return i;
}

void print_help() {
	Serial.println(F(
		"____________________________________________________\n"
		"\n"
		"  R o c k e t   L a u n c h e r   I n t e r f a c e:\n"
		"____________________________________________________\n"
		"\n"
		"A <is_induction_on[0-1]>\n"
		"    Set induction power\n"
		"B <>\n"
		"    Read induction current\n"
		"C <idle_induction_current[0.1-1K]>\n"
		"    Set idle induction current\n"
		"D <>\n"
		"    Auto launch!\n"
		"H <>\n"
		"    Print help\n"
	));
}

void set_induction_power(bool is_on) {
	digitalWrite(INDUCTION_RELAY_PIN, !is_on);
}

float read_induction_current() {
	float average = 0;
	for(uint16_t i = 0; i < ACS712_SAMPLES; i++) {
		average += (ACS712_MUL * analogRead(ACS712_PIN) - ACS712_SUB) / ACS712_SAMPLES;
		delay(1);
	}
	return average;
}

void setup() {
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.setTimeout(SERIAL_TIMEOUT);
	pinMode(ACS712_PIN, INPUT);
	pinMode(INDUCTION_RELAY_PIN, OUTPUT);
	set_induction_power(false);
	print_help();
}

void loop() {
	uint32_t cur_millis = millis();
	uint16_t i;
	float f;

	if (cur_millis > command.timeout) {
		if (command.buffer_index)
			command.buffer_index = 0;
		else if ((cur_millis - command.timeout) > screen_saver_ms) {
			command.timeout = cur_millis;
			/* screen_saver */
		}
	}

	if (Serial.available()) {
		if (!command.buffer_index)
			command.timeout = cur_millis + COMMAND_TIMEOUT_MS;

		command.buffer_index += Serial.readBytes(command.buffer + command.buffer_index,
			sizeof(command.buffer) - command.buffer_index);

		i = command.buffer[command.buffer_index - 1];
		if (command.buffer_index >= 2 && i == '\n' || i == '\r') {
			i = command.buffer[command.buffer_index - 2];
			i = ((i == '\n' || i == '\r') ? 3 : 2);
			while (i--)
				command.buffer[command.buffer_index - i] = '\0';

			switch (command.buffer[0]) {
				case 'H':
					print_help();
				break;
				case 'A':
					i = _interactive_menu_read_16(0);
					set_induction_power(i);
					Serial.print(F("induction_power = ")); Serial.println(i);
				break;
				case 'B':
					Serial.print(F("induction_current = ")); Serial.println(read_induction_current());
				break;
				case 'C':
					idle_induction_current = _interactive_menu_read_16(0);
					if (!idle_induction_current) {
						set_induction_power(true);
						delay(500);
						idle_induction_current = read_induction_current();
						set_induction_power(false);
					}
					Serial.print(F("idle_induction_current = ")); Serial.println(idle_induction_current);
				break;
				case 'D':
					set_induction_power(true);
					is_auto_induction = true;
					Serial.println(F("starting auto induction"));
					delay(1000);
				break;
			}
			command.buffer_index = 0;
		}
	}
	if (is_auto_induction) {
		f = read_induction_current();
		if ((cur_millis % 20) == 0) {
			Serial.print(F("induction_current = ")); Serial.println(f);
		}
		if ((f - idle_induction_current) < 0.1) {
			set_induction_power(false);
			is_auto_induction = false;
			Serial.println(F("\ndone auto induction"));
		}
	}
}
