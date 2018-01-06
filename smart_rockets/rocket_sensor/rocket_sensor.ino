
#include <SparkFunMPU9250-DMP.h>

#define SERIAL_BAUD_RATE		(115200)
#define SERIAL_TIMEOUT			(1)

#define COMMAND_BUFFER_SIZE		(100)
#define COMMAND_TIMEOUT_MS		(150)
#define SCREEN_SAVER_S			(40)


struct {
	uint8_t buffer[COMMAND_BUFFER_SIZE];
	uint8_t buffer_index;
	uint32_t timeout;
} command;

MPU9250_DMP imu;

uint32_t screen_saver_ms = SCREEN_SAVER_S * 1000;
float accelerometer_threshold = 0.05;
float idle_error = 0;
uint32_t old_millis;
uint32_t stat_ms = 0;

float dv = 0;
float dv_max = 0;
bool only_accs = false;


uint16_t _interactive_menu_read_16(uint16_t number) {
	uint16_t i = number;
	sscanf(command.buffer + 2, "%d", &i);
	return i;
}

void print_help() {
	Serial.println(F(
		"________________________________________________\n"
		"\n"
		"  R o c k e t   S e n s o r   I n t e r f a c e:\n"
		"________________________________________________\n"
		"\n"
		"A <accelerometer_threshold[1-16]>\n"
		"    Set accelerometer threshold\n"
		"H <>\n"
		"    Print help\n"
	));
}

void setup() {
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.setTimeout(SERIAL_TIMEOUT);

	while (imu.begin() != INV_SUCCESS) {
		Serial.println(F("Unable to communicate with MPU-9250"));
		delay(5000);
	}

	// The sample rate of the accel/gyro can be set using
	// setSampleRate. Acceptable values range from 4Hz to 1kHz
	imu.setSampleRate(100); // Set sample rate to 100Hz

	// Use configureFifo to set which sensors should be stored in the buffer.  
	// Parameter to this function can be: INV_XYZ_GYRO, 
	// INV_XYZ_ACCEL, INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
	imu.configureFifo(INV_XYZ_GYRO |INV_XYZ_ACCEL);
	imu.setAccelFSR(16);
	// print_help();
	old_millis = millis();
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
					accelerometer_threshold = _interactive_menu_read_16(accelerometer_threshold);
					Serial.print(F("accelerometer_threshold = ")); Serial.println(accelerometer_threshold);
				break;
				case 'B':
					Serial.println(F("Calibrating..."));
					stat_ms = 0;
					dv = 0;
					dv_max = 0;
					for (uint8_t i = 0; i < 100; i++) {
						while (!imu.fifoAvailable());
						while (imu.updateFifo() != INV_SUCCESS)
						idle_error += imu.calcAccel(imu.ay);
					}
					idle_error /= 100;
					Serial.print(F("idle_error: ")); Serial.println(idle_error);
				break;
				case 'C':
					only_accs = _interactive_menu_read_16(0);
					Serial.print(F("only_accs = ")); Serial.println(only_accs);
				break;
			}
			command.buffer_index = 0;
		}
	}
	
	// fifoAvailable returns the number of bytes in the FIFO
	// The FIFO is 512 bytes max. We'll read when it reaches
	// half of that.
	if (imu.fifoAvailable() >= 256) {
		// Then read while there is data in the FIFO
		while (imu.fifoAvailable() > 0) {
			// Call updateFifo to update ax, ay, az, gx, gy, and/or gz
			if (imu.updateFifo() == INV_SUCCESS) {
				f = imu.calcAccel(imu.ay) - idle_error;
				if (only_accs) {
					if (f > accelerometer_threshold) {
							Serial.println(f);
					}
				}
				else {
					// dv += f * (cur_millis - old_millis);
					dv += f * 0.01;

					if (dv > dv_max)
						dv_max = dv;

					float v_exhust = dv_max * (425 / 15);
					stat_ms += (cur_millis - old_millis);
					if (stat_ms > 500) {
						stat_ms = 0;
						Serial.print(F("dv = ")); Serial.println(dv);
						Serial.print(F("dv_max = ")); Serial.println(dv_max);
						Serial.print(F("v_exhust = ")); Serial.println(v_exhust);
						Serial.print(F("range 80% fule = ")); Serial.println(v_exhust * v_exhust * 0.164);
					}
					old_millis = cur_millis;
				}
			}
		}
	}
	
}
