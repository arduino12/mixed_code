/*
	Glediator to WS2812 pixel converter.
	Receives serial data in Glediator protocol format @ 1 MBit/s
	and distributes it to a connectect chain of WS2812 pixels.
	In the Glediator software set output mode to "Glediator_Protocol",
	color order to "GRB" and baud rate to "1000000".
*/

#define DATA_PORT		PORTD
#define DATA_DDR		DDRD
#define DATA_PIN		(6)
#define LED_COUNT		(256) // 512 max
#define LED_BUFF_SIZE	(LED_COUNT * 3)


uint16_t led_index = 0;
volatile uint8_t send_flag = 0;
uint8_t led_buff[LED_BUFF_SIZE] = {0};


void setup() {
	DATA_DDR |= _BV(DATA_PIN);
	UCSR0A |= _BV(U2X0);
	UCSR0B |= _BV(RXEN0)  | _BV(TXEN0) | _BV(RXCIE0);
	UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00);
	UBRR0H = 0;
	UBRR0L = 1; // 1 MBit baudrate at F_CPU = 16MHz
}

void loop() {
	for (;;)
		if (send_flag == 1) {
			cli();
			send_ws2812_buff(led_buff, sizeof(led_buff));
			sei();
			send_flag = 0;
		}
}

ISR(USART_RX_vect) {
	uint8_t b = UDR0;

	if (b == 1) {
		led_index = 0;
		return;
	}

	if (led_index < LED_BUFF_SIZE)
		led_buff[led_index++] = b;

	if (led_index == LED_BUFF_SIZE)
		send_flag = 1;
}

void send_ws2812_buff(uint8_t * buff, uint16_t buff_size) {
	uint8_t i, data;
	uint8_t set_mask = _BV(DATA_PIN) | DATA_PORT;
	uint8_t clr_mask =~ _BV(DATA_PIN) & DATA_PORT;

	while (buff_size--) {
		data = *buff++;
		asm volatile (
			"		ldi %0,8	\n\t"	// 0
			"loop%=:out %2, %3	\n\t"	// 1
			"lsl	%1			\n\t"	// 2
			"dec	%0			\n\t"	// 3
			"		rjmp .+0	\n\t"	// 5
			"		brcs .+2	\n\t"	// 6l / 7h
			"		out %2,%4	\n\t"	// 7l / -
			"		rjmp .+0	\n\t"	// 9
			"		nop			\n\t"	// 10
			"		out %2,%4	\n\t"	// 11
			"		breq end%=	\n\t"	// 12 nt. 13 taken
			"		rjmp .+0	\n\t"	// 14
			"		rjmp .+0	\n\t"	// 16
			"		rjmp .+0	\n\t"	// 18
			"		rjmp loop%=	\n\t"	// 20
			"end%=:				\n\t" 
			:		"=&d" (i)
			:		"r" (data), "I" (_SFR_IO_ADDR(DATA_PORT)), "r" (set_mask), "r" (clr_mask)
		);
	}
}
