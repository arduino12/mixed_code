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
*/

/*
pass string with args to func
parse all args and pass to func
while moving left or right or both, fingers must be up! also ignor fingers and side commands
the two commands can run at the same time are left and right with no collusion
and oposit side fingers
*/

//{ includes:
#include "globals.h"
#include "decoder.h"
#include "hands.h"
#include "fingers.h"
#include "led_strip.h"
//}

//{ defines:
//}

//{ globals
//}

//{ helpers
//}

//{ setup and loop
void setup() {
	/* initialize the debug UART */
	DEBUG_INIT;
	DEBUG_PRINTLN(F("setup"));
	/* initialize the servo motors I2C expender */
	fingers_init();
	/* initialize the DC motors with IR encoders */
	hands_init();
	/* initialize the RGB LED strip */
	led_strip_init();
	/* start infinity loop */
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
