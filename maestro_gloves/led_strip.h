#ifndef LED_STRIP_H
#define LED_STRIP_H

#define RGB_CHANNELS			(3)
#define STRIP_LED_COUNT			(120)
#define STRIP_LED_BUFFER_SIZE	(STRIP_LED_COUNT * RGB_CHANNELS)
#define STRIP_LED_DATA_PIN		(A0)


uint8_t led_strip_buffer[STRIP_LED_BUFFER_SIZE] = {0};

/* initialize smart pixel RGB LEDs */
void led_strip_init() {
	pinMode(STRIP_LED_DATA_PIN, OUTPUT);
	led_strip_refresh();
}

/* smart pixel RGB LEDs order: green red blue */
void led_strip_set_color(uint16_t index, uint32_t color) {
	index *= 3;
	led_strip_buffer[index + 1] = color & 0xFF;
	color >>= 8;
	led_strip_buffer[index + 0] = color & 0xFF;
	color >>= 8;
	led_strip_buffer[index + 2] = color & 0xFF;
	led_strip_refresh();
}

/* refresh the smart pixel RGB LEDs by setting their color to match the led_strip_buffer */
void led_strip_refresh() {
	uint8_t value, ctr, mask_low, mask_high = digitalPinToBitMask(STRIP_LED_DATA_PIN);
	mask_low = ~mask_high & portOutputRegister(digitalPinToPort(STRIP_LED_DATA_PIN));
	mask_high |= portOutputRegister(digitalPinToPort(STRIP_LED_DATA_PIN));
	
	#define SEND_LED_BYTE \
		asm volatile ( \
			"		ldi %0,8	\n\t" \
			"loop%=:out %2, %3	\n\t" \
			"lsl	%1			\n\t" \
			"dec	%0			\n\t" \
			"		rjmp .+0	\n\t" \
			"		brcs .+2	\n\t" \
			"		out %2,%4	\n\t" \
			"		rjmp .+0	\n\t" \
			"		nop			\n\t" \
			"		out %2,%4	\n\t" \
			"		breq end%=	\n\t" \
			"		rjmp .+0	\n\t" \
			"		rjmp .+0	\n\t" \
			"		rjmp .+0	\n\t" \
			"		rjmp loop%=	\n\t" \
			"end%=:				\n\t" \
			:	"=&d" (ctr)			  \
			:	"r" (value), "I" (_SFR_IO_ADDR(portOutputRegister(digitalPinToPort(STRIP_LED_DATA_PIN)))), \
			"r" (mask_high), "r" (mask_low) \
		)

	cli(); // disable global interrupts
	for (uint16_t i = 0; i < STRIP_LED_BUFFER_SIZE; i++) {
		value = led_strip_buffer[i];
		SEND_LED_BYTE;
	}
	sei(); // enable global interrupts
}

#endif /* LED_STRIP_H */
