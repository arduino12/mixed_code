/*
 * Sends touch values via bluetooth serial.
 * Arad Eizen 13/05/18.
 */
#include <BluetoothSerial.h> // https://github.com/espressif/arduino-esp32


/* uart baud rate in bit per seconds */
#define SERIAL_BAUD_RATE		(115200)
/* uart read timeout in milliseconds */
#define SERIAL_TIMEOUT			(1)
/* bluetooth serial device name */
#define BT_SERIAL_NAME			("Ofertex Box")

#define FAST_OUTPUT_INTERVAL	(50)
#define SLOW_OUTPUT_INTERVAL	(100)

/* touch gpios T0 - T9: 4, 0, 2, 15, 13, 12, 14, 27, 33, 32 */
#define WATER_SENSOR_A_PIN		(T3) // GPIO15
#define WATER_SENSOR_B_PIN		(T4) // GPIO13
#define BUILTIN_LED				(22) // GPIO22


enum output_data_format_e {
	RAW_DATA_FORMAT,
	A_SENSOR_DATA_FORMAT,
	B_SENSOR_DATA_FORMAT,
};

BluetoothSerial bt_serial;

bool led_status;
uint8_t output_data_format = RAW_DATA_FORMAT;
uint16_t output_interval_ms = SLOW_OUTPUT_INTERVAL;
uint32_t next_output_ms;
uint16_t last_a;
uint16_t last_b;

void read_input_command()
{
	char c = '\0';

	/*
		BluetoothSerial BUG (as of 06/04/18):
		Starting from the first received byte,
		all transmited bytes will slow down after about 10 seconds...
		Quick fix is to send another one each 5 seconds..
	*/
	if (bt_serial.available()) {
		c = bt_serial.read();
	}

	if (Serial.available()) {
		c = Serial.read();
	}

	switch (c) {
	case 'A':
		output_data_format = RAW_DATA_FORMAT;
		break;
	case 'B':
		output_data_format = A_SENSOR_DATA_FORMAT;
		break;
	case 'C':
		output_data_format = B_SENSOR_DATA_FORMAT;
		break;
	}
}

void write_output_data()
{
	char buf[20];
	uint16_t a = touchRead(WATER_SENSOR_A_PIN);
	uint16_t b = touchRead(WATER_SENSOR_B_PIN);

	if (!a && last_a) {
		a = last_a;
		last_a = 0;
	}
	else last_a = a;

	if (!b && last_b) {
		b = last_b;
		last_b = 0;
	}
	else last_b = b;

	switch (output_data_format) {
	case RAW_DATA_FORMAT:
		sprintf(buf, "%03d,%03d", a, b);
		break;
	case A_SENSOR_DATA_FORMAT:
		sprintf(buf, "%03d", a);
		break;
	case B_SENSOR_DATA_FORMAT:
		sprintf(buf, "%03d", b);
		break;
	}

	Serial.println(buf);
	bt_serial.println(buf);
}

void setup()
{
	/* initialize usb serial (uart) communication */
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.setTimeout(SERIAL_TIMEOUT);

	/* initialize bluetooth serial communication */
	bt_serial.begin(BT_SERIAL_NAME);
	
	pinMode(BUILTIN_LED, OUTPUT);
}

void loop()
{
	uint32_t cur_ms = millis();

	/* read user command from bt or usb serial */
	read_input_command();

	/* write output data to bt and usb serial if needed */
	if (cur_ms > next_output_ms) {
		next_output_ms = cur_ms + output_interval_ms;
		write_output_data();
		led_status = !led_status;
		digitalWrite(BUILTIN_LED, led_status);
	}

	// yield();
	delay(1);
}
