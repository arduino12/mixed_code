
#define KEY_NONE					(0xFF)
#define KEY_REPEAT					(0xFE)

#define IR_ERROR					(0)
#define IR_REPEAT					(0xFFFFFFFF)

#define IR_STATE_IDLE				(0)
#define IR_STATE_MARK				(1)
#define IR_STATE_SPACE				(2)
#define IR_STATE_STOP				(3)

#define IR_BUFFER_SIZE				(100)

#define IR_TICK_US					(50) // microseconds per clock interrupt tick
#define IR_MARK_EXCESS				(100) // marks tend to be 100us too long, and spaces 100us too short...
#define IR_GAP_TICKS				(5000 / IR_TICK_US) // minimum gap between transmissions
#define IR_TOLERANCE				(0.25)
#define IR_TICKS_LOW(us)			(uint16_t)((us) * (1.0 - IR_TOLERANCE) / IR_TICK_US)
#define IR_TICKS_HIGH(us)			(uint16_t)((us) * (1.0 + IR_TOLERANCE) / IR_TICK_US + 1)

#define NEC_HDR_MARK				(9000)
#define NEC_HDR_SPACE				(4500)
#define NEC_BIT_MARK				(560)
#define NEC_ONE_SPACE				(1600)
#define NEC_ZERO_SPACE				(560)
#define NEC_RPT_SPACE				(2250)
#define NEC_BITS					(32)

#define IR_REMOTE_UPDATE_US			(IR_TICK_US)


uint8_t ir_pin, *ir_port; // ir sensor pin and port for fast readings
volatile uint8_t ir_state; // current ir receiving state
volatile uint16_t ir_ticks;
volatile uint8_t ir_buffer_size;
volatile uint16_t ir_buffer[IR_BUFFER_SIZE]; // raw duration buffer
uint8_t (*ir_key_map)(uint32_t value);


bool match(uint16_t measured, uint16_t desired) {
	return measured >= IR_TICKS_LOW(desired) && measured <= IR_TICKS_HIGH(desired);
}

bool matchMark(uint16_t measured_ticks, uint16_t desired_us) {
	return match(measured_ticks, (desired_us + IR_MARK_EXCESS));
}

bool matchSpace(uint16_t measured_ticks, uint16_t desired_us) {
	return match(measured_ticks, (desired_us - IR_MARK_EXCESS));
}

uint32_t decode_nec() {
	uint32_t data = 0;
	uint8_t offset = 1; // Skip first space
	if (!matchMark(ir_buffer[offset], NEC_HDR_MARK))
		return IR_ERROR; // Initial mark
	offset++;
	// Check for IR_REPEAT
	if (ir_buffer_size == 4 && matchSpace(ir_buffer[offset], NEC_RPT_SPACE) && matchMark(ir_buffer[offset + 1], NEC_BIT_MARK))
		return IR_REPEAT;
	if (ir_buffer_size < 2 * NEC_BITS + 4)
		return IR_ERROR;
	if (!matchSpace(ir_buffer[offset], NEC_HDR_SPACE))
		return IR_ERROR;
	offset++;
	for (uint8_t i = 0; i < NEC_BITS; i++) {
		if (!matchMark(ir_buffer[offset], NEC_BIT_MARK))
			return IR_ERROR;
		offset++;
		if (matchSpace(ir_buffer[offset], NEC_ONE_SPACE))
			data = (data << 1) | 1;
		else if (matchSpace(ir_buffer[offset], NEC_ZERO_SPACE))
			data <<= 1;
		else
			return IR_ERROR;
		offset++;
	}
	return data;
}

void ir_remote_init(uint8_t pin, uint8_t (*remote_key_map)(uint32_t value)) {
	ir_buffer_size = 0;
	ir_state = IR_STATE_IDLE;
	ir_key_map = remote_key_map;
	pinMode(pin, INPUT);
	ir_pin = digitalPinToBitMask(pin);
	ir_port = portInputRegister(digitalPinToPort(pin));
}

void ir_remote_resume() {
	if (ir_state != IR_STATE_STOP)
		return;

	/* resume ir state machin */
	ir_state = IR_STATE_IDLE;
	ir_buffer_size = 0;
	ir_ticks = 0;
	ENABLE_IR_REMOTE_UPDATE();
}

uint8_t ir_remote_get_key() {
	if (ir_state != IR_STATE_STOP)
		return KEY_NONE;

	uint32_t value = decode_nec();

	/* special keys */
	if (value == IR_ERROR)
		return KEY_NONE;
	if (value == IR_REPEAT)
		return KEY_REPEAT;

	Serial.println(value, HEX);
	/* return key index */
	return ir_key_map(value);
}

void ir_remote_update() {
	uint8_t is_space = *ir_port & ir_pin;

	ir_ticks++; // One more 50us tick
	if (ir_buffer_size >= IR_BUFFER_SIZE)
		ir_state = IR_STATE_STOP; // Buffer overflow
	
	if(ir_state == IR_STATE_IDLE) { // In the middle of a gap
		if (!is_space) {
			if (ir_ticks < IR_GAP_TICKS)
				ir_ticks = 0; // Not big enough to be a gap.
			else {
				ir_buffer_size = 0;	// gap just ended, record duration and start recording transmission
				ir_buffer[ir_buffer_size++] = ir_ticks;
				ir_ticks = 0;
				ir_state = IR_STATE_MARK;
			}
		}
	}
	else if(ir_state == IR_STATE_MARK) { // timing MARK
		if (is_space) { // MARK ended, record time
			ir_buffer[ir_buffer_size++] = ir_ticks;
			ir_ticks = 0;
			ir_state = IR_STATE_SPACE;
		}
	}
	else if(ir_state == IR_STATE_SPACE) { // timing SPACE
		if (is_space) { // SPACE
			if (ir_ticks > IR_GAP_TICKS)
				ir_state = IR_STATE_STOP;
		} 
		else { // SPACE just ended, record it
			ir_buffer[ir_buffer_size++] = ir_ticks;
			ir_ticks = 0;
			ir_state = IR_STATE_MARK;
		} 
	}
	else if(ir_state == IR_STATE_STOP) { // waiting, measuring gap
		if (!is_space)
			ir_ticks = 0; // reset gap ir_ticks
		DISABLE_IR_REMOTE_UPDATE();
	}
}
