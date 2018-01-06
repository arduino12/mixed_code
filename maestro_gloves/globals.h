#ifndef GLOBALS_H
#define GLOBALS_H

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

#endif /* GLOBALS_H */
