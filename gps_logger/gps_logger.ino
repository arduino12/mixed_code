/*
	GPS-Logger V1.0 Software!
	=========================
	This program logs GPS data to SD card,
	and shows the meta GPS data on the display!
	Main board: Arduino-Pro-Mini.
	
	Raw GPS NMEA data to Google Earth converter:
	* http://www.gpsvisualizer.com/map_input?form=googleearth
	
	Must change:
	* line 42 in C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\SoftwareSerial\src\SoftwareSerial.h 
		#define _SS_MAX_RX_BUFF 128 // RX buffer size
	* line 59 in C:\Program Files (x86)\Arduino\libraries\SD\src\SD.h
		public:  
*/

//{ includes:
#include <Wire.h>				// for LiquidCrystal_I2C class
#include <LiquidCrystal_I2C.h>	// for i2c lcd display module
#include <SoftwareSerial.h>		// for gps serial comunication
#include <TinyGPSPlus.h>		// for gps module
#include <SPI.h>				// for sd spi comunication
#include <SD.h>					// for sd card module
#include "texts.h"				// for hebrew texts
//}

//{ defines:
#define SD_EJECT_PIN			(2)
#define SD_CS_PIN				(10)
#define SD_MOSI_PIN				(11)
#define SD_MISO_PIN				(12)
#define SD_CLK_PIN				(13)
#define GPS_TX_PIN				(A2)
#define GPS_RX_PIN				(A3)
#define LCD_SDA_PIN				(A4)
#define LCD_SCL_PIN				(A5)

#define LCD_I2C_ADDRESS			(0x27)
#define LCD_WIDTH				(16)
#define LCD_HEIGHT				(2)
#define GPS_BAUDRATE			(9600)
#define GPS_WRITE_BUFFER_SIZE	(_SS_MAX_RX_BUFF)
#define NEXT_SCREEN_MS			(3500)
#define LOGGER_FILE_NAME		("NMEA_00.txt")

#define SD_EJECT_STATE_IDEL		(0)
#define SD_EJECT_STATE_EJECTING	(1)

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
//}

//{ globals
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_WIDTH, LCD_HEIGHT);
SoftwareSerial gps_serial(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus gps;
volatile uint8_t sd_eject_state = SD_EJECT_STATE_IDEL;
char gps_write_buffer[GPS_WRITE_BUFFER_SIZE];
char logger_file_name[] = LOGGER_FILE_NAME;
uint16_t gps_write_buffer_size = 0;
uint16_t sd_saved_records_count = 0;
uint32_t next_screen_ms;
//}

//{ helpers
void lcd_printf_P(uint8_t line_index, const char * format, ...) {
	char buffer[LCD_WIDTH + 1];
	
	/* call sprintf_P with the given va_list */
	va_list argp;
	va_start(argp, format);
	vsprintf_P(buffer, format, argp);
	va_end(argp);
	
	/* print the formated text to lcd */
	lcd.setCursor(0, line_index);
	lcd.print(buffer);
}

void print_next_screen() {
	static uint8_t screen_index = 0;
	char buffer[LCD_WIDTH + 1];
	uint32_t up_time = millis() / 1000;
	uint8_t satellites_count = gps.satellites.value();
	
	/* set next screen index */
	screen_index++;
	if		(satellites_count == 0) {
		screen_index = 0;
	}
	else if	(satellites_count < 4) {
		if (screen_index > 1) screen_index = 0;
	}
	else {
		if (screen_index > 4) screen_index = 0;
	}

	/* print the screen with match index to lcd */
	switch (screen_index) {
		case 0:
			lcd_printf_P(0, PSTR("%8d %s"),
				satellites_count,
				TEXT_SATELLITES);
			lcd_printf_P(1, PSTR("%02d:%02d:%02d %s"),
				(uint8_t)(up_time / 3600 % 60),
				(uint8_t)(up_time / 60 % 60),
				(uint8_t)(up_time % 60),
				TEXT_DURATION);
		break;
		case 1:
			lcd_printf_P(0, PSTR("%02d:%02d:%02d %s"),
				gps.time.hour(),
				gps.time.minute(),
				gps.time.second(),
				TEXT_UTC_TIME);
			lcd_printf_P(1, PSTR("%02d/%02d/%02d %2d %s"),
				gps.date.day(),
				gps.date.month(),
				gps.date.year() % 100,
				(uint8_t)gps.hdop.value() % 100,
				TEXT_ACCURACY);	
		break;
		case 2:
			dtostrf(gps.location.lat(), 8, 5, buffer);
			lcd_printf_P(0, PSTR("%s %s"),
				buffer,
				TEXT_LATITUDE);
			dtostrf(gps.location.lng(), 8, 5, buffer);
			lcd_printf_P(1, PSTR("%s %s"),
				buffer,
				TEXT_LONGITUDE);
		break;
		case 3:
			lcd_printf_P(0, PSTR("%4d %s"),
				(uint16_t)gps.altitude.meters(),
				TEXT_ALTITUDE);
			dtostrf(gps.speed.kmph(), 6, 2, buffer);
			lcd_printf_P(1, PSTR("   %s %s"),
				buffer,
				TEXT_SPEED);
		break;
		case 4:
			lcd_printf_P(0, PSTR("       %3d %s"),
				(uint16_t)gps.course.deg(),
				TEXT_DIRECTION);
			lcd_printf_P(1, PSTR("%5d %s"),
				sd_saved_records_count,
				TEXT_SAVED_LANDMARKS);
		break;
	}
}

void sd_eject_interrupt() {
	sd_eject_state = SD_EJECT_STATE_EJECTING;
}
//}

//{ setup and loop
void setup() {
	/* initialize the debug UART */
	DEBUG_INIT;
	DEBUG_PRINTLN(F(TEXT_INIT_1));
	
	/* initialize the lcd and its I2C expender */
	lcd.init(); 
	lcd.backlight();
	
	/* print the init screen to lcd */
	lcd_printf_P(0, PSTR(TEXT_INIT_1));
	lcd_printf_P(1, PSTR(TEXT_INIT_2));
	
	/* attach interrupt for SD card ejection */
	pinMode(SD_EJECT_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(SD_EJECT_PIN), sd_eject_interrupt, FALLING);
	
	/* initialize the SD flash card */
	if (SD.begin()) {
		delay(NEXT_SCREEN_MS);
		DEBUG_PRINTLN(F("SD card found!"));
		lcd_printf_P(0, PSTR(TEXT_FOUND_SD));
		lcd_printf_P(1, PSTR("SD%d FAT%2d %2dGb !"),
			(uint8_t)(SD.card.type()),
			(uint8_t)(SD.volume.fatType()),
			(uint8_t)((uint32_t)SD.volume.blocksPerCluster() * SD.volume.clusterCount() / 1024 / 1024 / 2 + 1));

		/* open a new file that doesn't exist */
		for (uint8_t i = 0; i < 100; i++) {
			logger_file_name[5] = '0' + i / 10;
			logger_file_name[6] = '0' + i % 10;
			if (!SD.exists(logger_file_name)) break;
		}
		DEBUG_PRINTLN(F("The logger file name:"));
		DEBUG_PRINTLN(logger_file_name);
		delay(NEXT_SCREEN_MS);
	}
	
	/* initialize the GPS UART */
	gps_serial.begin(GPS_BAUDRATE);
	sd_eject_state = SD_EJECT_STATE_IDEL;
}

void loop() {
	static uint16_t old_gps_records_count = 0;
	uint32_t current_ms = millis();
	
	/* check for gps raw data */
	if (gps_serial.available()) {
		/* is there space in gps_write_buffer? */
		if (gps_write_buffer_size < (GPS_WRITE_BUFFER_SIZE - 1)) {
			/* encode it and add it to the gps_write_buffer */
			gps.encode(gps_serial.peek());
			gps_write_buffer[gps_write_buffer_size++] = gps_serial.read();
		}
		/* flush the gps_write_buffer to serial and SD card */
		else {
			DEBUG_WRITE(gps_write_buffer, gps_write_buffer_size);
			/* can write to SD card? */
			if ((sd_eject_state == SD_EJECT_STATE_IDEL) && gps.location.isValid()) {
				File logger_file = SD.open(logger_file_name, FILE_WRITE);
				if (logger_file) {
					if (logger_file.write(gps_write_buffer, gps_write_buffer_size) == gps_write_buffer_size)
						sd_saved_records_count += gps.sentencesWithFix() - old_gps_records_count;
					logger_file.close();
				}
			}
			old_gps_records_count = gps.sentencesWithFix();
			gps_write_buffer_size = 0;
		}
	}
	
	/* did the eject button was pressed? */
	if (sd_eject_state == SD_EJECT_STATE_EJECTING) {
		DEBUG_PRINTLN(F("SD card can be or removed..!"));
		lcd_printf_P(0, PSTR(TEXT_REMOVE_SD_1));
		lcd_printf_P(1, PSTR(TEXT_REMOVE_SD_2));
		delay(5000);
		sd_eject_state = SD_EJECT_STATE_IDEL;
		current_ms = millis();
	}
	
	/* is it time to print the next screen to the lcd display? */
	if ((current_ms - next_screen_ms) > NEXT_SCREEN_MS) {
		next_screen_ms = current_ms;
		print_next_screen();
	}
}
//}
