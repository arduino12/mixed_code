#include <soft_bcm.h>
#include <serial_menu.h>

/* uart baud rate in bit per seconds */
#define SERIAL_BAUD_RATE		(115200)
/* set uart read timeout in milliseconds */
#define SERIAL_TIMEOUT			(1)
/* rgb channels count: red, green, blue */
#define RGB_SIZE				(3)
/* total amount of rgb leds */
#define RGB_LED_COUNT			(6)

#define COLOR_BLACK				(0x00)
#define COLOR_RED				(0x01)
#define COLOR_GREEN				(0x02)
#define COLOR_BLUE				(0x04)
#define COLOR_YELLO				(0x03)
#define COLOR_CYAN				(0x06)
#define COLOR_MAGENTA			(0x05)
#define COLOR_WHITE				(0x07)

uint8_t rgb_on_values[RGB_SIZE] = {70, 210, 210};
uint8_t rgb_led_colors[RGB_LED_COUNT] = {0};


/* prints serial menu commands help */
void print_help() {
	Serial.println(F(
		"______________________________________\n"
		"\n"
		"  R G B   L E D   I N T E R F A C E:\n"
		"______________________________________\n"
		"\n"
		"H\n"
		"    Print this help\n"
		"A <led_index[0-5] color_index[0-7]>\n"
		"    Sets the given led color\n"
	));
}

/* set leds colors from rgb_led_colors global colors array */
void update_leds() {
	for (uint8_t i = 0; i < RGB_LED_COUNT; i++)
		for (uint8_t j = 0; j < RGB_SIZE; j++)
			bcm_values[i * RGB_SIZE + j] = (rgb_led_colors[i] & (1 << j)) ? rgb_on_values[j] : 0;
	bcm_update_values();
}

void setup() {
	/* initialize uart for serial communication */
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.setTimeout(SERIAL_TIMEOUT);
	/* initialize pseudo random generator */
	srand(analogRead(7));
	/* initialize bcm */
	bcm_init();
	print_help();
}

void loop() {
	uint16_t a = 0, b = 0;

	if (serial_menu_update()) {
		switch (command.buffer[0]) {
			case 'H':
				print_help();
			break;
			case 'A':
				sscanf(command.buffer + 2, "%d %d", &a, &b);
				Serial.print(a);
				Serial.print(' ');
				Serial.println(b);
				rgb_led_colors[a] = b;
				update_leds();
			break;
		}
	}
}
