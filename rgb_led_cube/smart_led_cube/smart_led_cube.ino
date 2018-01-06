
#define ENABLE_LED_CUBE_UPDATE()	({ TIMSK2 |= _BV(OCIE2A); })
#define DISABLE_LED_CUBE_UPDATE()	({ TIMSK2 &= ~_BV(OCIE2A); })

#define ENABLE_IR_REMOTE_UPDATE()	({ TIMSK2 |= _BV(OCIE2B); })
#define DISABLE_IR_REMOTE_UPDATE()	({ TIMSK2 &= ~_BV(OCIE2B); })

#define UPDATE_TIMER_PRESCALER		(8)
#define US_TO_UPDATE_TICKS(us)		(F_CPU * us / 1000000 / UPDATE_TIMER_PRESCALER)

#define LED_CUBE_UPDATE_TICKS		(US_TO_UPDATE_TICKS(LED_CUBE_UPDATE_US))
#define IR_REMOTE_UPDATE_TICKS		(US_TO_UPDATE_TICKS(IR_REMOTE_UPDATE_US))

#include "led_cube.h"
#include "ir_remote.h"
#include "effects.h"
#include "remote_key_map.h"


#define UART_BAUD_RATE				(115200)
#define IR_REMOTE_PIN				(A4)


// ISR(TIMER2_COMPA_vect) {
	// DISABLE_LED_CUBE_UPDATE();
	/* charlieple the led cube */
	// led_cube_update();
	/* schedule next interrupt time */
	// OCR2A = TCNT2 + LED_CUBE_UPDATE_TICKS;
	// ENABLE_LED_CUBE_UPDATE();
// }

ISR(TIMER2_COMPB_vect) {
	/* recive ir remote commands */
	ir_remote_update();
	/* schedule next interrupt time */
	OCR2B = TCNT2 + IR_REMOTE_UPDATE_TICKS;
}

void timer_2_init() {
	/* disable global interrupts */
	cli();
	/* set timer_2 mode to normal */
	TCCR2A = 0;
	/* set timer_2 prescaler to 8 */
	TCCR2B = _BV(CS21);
	/* enable global interrupts */
	sei();
}

void setup() {
	/* initialize uart for serial communication */
	Serial.begin(UART_BAUD_RATE);
	Serial.print(F("\napp started!\n============\n"));
	/* initialize pseudo random generator with random io reading */
	srand(analogRead(7));
	/* initialize rgb led cube */
	led_cube_init();
	/* initialize ir remote reciver */
	ir_remote_init(IR_REMOTE_PIN, remote_key_map_44);
	/* initialize timer_2 to update both cube and ir remote */
	timer_2_init();
	/* start charlieplexing the led cube and reciving ir remote commands */
	// ENABLE_LED_CUBE_UPDATE();
	ENABLE_IR_REMOTE_UPDATE();
	// moving_cubes();
}

void idle_delay(uint32_t ms) {
	ms += millis();
	while (ms > millis()) {
		uint8_t key = ir_remote_get_key();
		switch (key) {
			case KEY_REPEAT:
				Serial.print('.');
			break;
			case KEY_PLAY:
				Serial.println("KEY_PLAY");
				effect_running = false;
				effect_index++;
				effect_index %= 10;
			break;
			case KEY_DIY_2:
				Serial.println("DIY_2");
			break;
			// case KEY_DIY_3:
				// Serial.println("DIY_3");
			// break;
			// case KEY_R_UP:
			// case KEY_G_UP:
			// case KEY_B_UP:
				// Serial.println("INC");
				// inc_rgb(key - KEY_R_UP);
				// clear_cube();
				// // fill_rect_y(0, 0, 0, CUBE_SIZE, CUBE_SIZE);
				// fill_cube();
				// flash_cube();
				// Serial.println(draw_color[0]);
				// Serial.println(draw_color[1]);
				// Serial.println(draw_color[2]);
			// break;
			// case KEY_R_DOWN:
			// case KEY_G_DOWN:
			// case KEY_B_DOWN:
				// Serial.println("DEC");
				// dec_rgb(key - KEY_R_DOWN);
				// clear_cube();
				// fill_cube();
				// // fill_rect_y(0, 0, 0, CUBE_SIZE, CUBE_SIZE);
				// flash_cube();
			// break;
		}
		ir_remote_resume();
		// delay(1);
	}
}

void loop() {
	Serial.println(effect_index);
	effect_run(effect_index);
	idle_delay(1);
}
