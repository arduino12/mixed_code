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
void print_help()
{
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
void update_leds()
{
	for (uint8_t i = 0; i < RGB_LED_COUNT; i++)
		for (uint8_t j = 0; j < RGB_SIZE; j++)
			bcm_values[i * RGB_SIZE + j] = (rgb_led_colors[i] & (1 << j)) ? rgb_on_values[j] : 0;
	bcm_update_values();
}

void set_led(uint8_t index, uint8_t color, uint8_t ms)
{
	rgb_led_colors[index] = color;
	for (uint8_t j = 0; j < RGB_SIZE; j++)
		bcm_values[index * RGB_SIZE + j] = (rgb_led_colors[index] & (1 << j)) ? rgb_on_values[j] : 0;
	bcm_update_values();
	delay(ms);
}

void set_leds(uint8_t color, uint8_t ms)
{
	memset(rgb_led_colors, color, RGB_LED_COUNT);
	update_leds();
	delay(ms);
}

void setup()
{
	/* initialize uart for serial communication */
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.setTimeout(SERIAL_TIMEOUT);
	/* initialize pseudo random generator */
	srand(analogRead(7));
	/* initialize bcm */
	bcm_init();
	print_help();
}

void loop()
{
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
			case 'B':
				sscanf(command.buffer + 2, "%d %d", &a, &b);
				Serial.print(a);
				Serial.print(' ');
				Serial.println(b);
				while (a--) {
					set_led(0, 1, b);
					set_led(0, 2, b);
					set_led(0, 4, b);
				}
				set_led(0, 0, 0);
			break;
			case 'C':
				sscanf(command.buffer + 2, "%d %d", &a, &b);
				Serial.print(a);
				Serial.print(' ');
				Serial.println(b);
				while (a--) {
					set_led(0, 1, b);
					set_led(0, 6, b);
				}
				set_led(0, 0, 0);
			break;
			case 'D':
				sscanf(command.buffer + 2, "%d %d", &a, &b);
				Serial.print(a);
				Serial.print(' ');
				Serial.println(b);
				while (a--) {
					set_led(0, 7, b);
					set_led(0, 0, b);
				}
			break;
			case 'E':
				for (a = 0; a < 600; a++) {
					set_led(0, 1, 6);
					set_led(0, 2, 6);
					set_led(0, 4, 6);
				}
				for (b = 1; b < 8; b <<= 1)
					for (a = 0; a < 500; a++) {
						set_led(0, b, 8);
						set_led(0, 7 - b, 8);
					}
				set_led(0, 0, 0);
			break;
		}
	}
	
	for (a = 0; a < 300; a++) {
		set_leds(1, 6);
		set_leds(2, 6);
		set_leds(4, 6);
	}
	for (b = 1; b < 8; b <<= 1)
		for (a = 0; a < 200; a++) {
			set_leds(b, 8);
			set_leds(7 - b, 8);
		}
	set_leds(0, 0);
}
