#define WIRE_1_PIN				(6)
#define WIRE_2_PIN				(5)
#define WIRE_3_PIN				(4)
#define WIRE_4_PIN				(3)
#define WIRE_5_PIN				(2)
#define MIC_PIN					(A7)
#define MIC_THRESHOLD			(100)
#define MIC_DELAY_MS			(500)

#define Z						(0x00)
#define A						(0x10)
#define B						(0x08 | A)
#define C						(0x04 | A)
#define D						(0x02 | A)
#define E						(0x01 | A)


uint8_t mode = 0;
uint32_t next_mode_ms;


bool delay2(uint32_t ms)
{
	while (ms) {
		ms--;
		delay(1);
		if (analogRead(MIC_PIN) > MIC_THRESHOLD && millis() > next_mode_ms) {
			Serial.println("mode!");
			mode++;
			next_mode_ms = millis() + MIC_DELAY_MS;
			return true;
		}
	}
	return false;
}

bool set_el_wires(uint8_t mask, uint32_t ms)
{
	PORTD = (PORTD & 0x03) | (~mask << 2);
	// DDRD = (DDRD & 0x03) | (mask << 2);
	return delay2(ms);
}

bool loop_el_wires_2(uint8_t count, uint8_t mask1, uint32_t ms1, uint8_t mask2, uint32_t ms2)
{
	while (count) {
		count--;
		if(set_el_wires(mask1, ms1))
			return true;
		if(set_el_wires(mask2, ms2))
			return true;
	}
	return false;
}

void setup()
{
	Serial.begin(115200);
	Serial.println("start!");
	DDRD |= 0x7C;	/* (2 - 6 OUTPUT) */
	// PORTD &= 0x03;
	set_el_wires(0, 0);
}

void loop()
{
	uint8_t i;
	switch (mode % 4) {
	case 0:
		if(loop_el_wires_2(14, Z, 50, A, 50))
			return;
		if(loop_el_wires_2(14, A, 50, B, 50))
			return;
		if(loop_el_wires_2(14, B, 50, C, 50))
			return;
		for (i = 0; i < 14; i++) {
			if(set_el_wires(B, 50))
				return;
			if(set_el_wires(C, 50))
				return;
			if(set_el_wires(D, 50))
				return;
		}
		if(loop_el_wires_2(14, D|B, 50, E|C, 50))
			return;
		break;
	case 1:
		if(loop_el_wires_2(8, A|B|C, 100, D, 100))
			return;
		if(set_el_wires(E, 1000))
				return;
		if(loop_el_wires_2(8, A|B|C|D, 80, A|B|C|E, 80))
			return;
		for (i = 0; i < 8; i++) {
			if(set_el_wires(C, 90))
				return;
			if(set_el_wires(B, 90))
				return;
			if(set_el_wires(D|E, 90))
				return;
		}
		break;
	case 2:
		for (i = 0; i < 8; i++) {
			if(set_el_wires(C, 90))
				return;
			if(set_el_wires(B, 90))
				return;
			if(set_el_wires(D|E, 90))
				return;
		}
		for (i = 0; i < 10; i++) {
			if(set_el_wires(C, 70))
				return;
			if(set_el_wires(B, 70))
				return;
			if(set_el_wires(D|E, 70))
				return;
		}
		for (i = 0; i < 20; i++) {
			if(set_el_wires(C, 60))
				return;
			if(set_el_wires(B, 60))
				return;
			if(set_el_wires(D|E, 60))
				return;
		}
		for (i = 0; i < 20; i++) {
			if(set_el_wires(C, 30))
				return;
			if(set_el_wires(B, 40))
				return;
			if(set_el_wires(D|E, 70))
				return;
			if(set_el_wires(B, 50))
				return;
		}
		break;
	case 3:
		if(loop_el_wires_2(18, Z, 70, B|C|D|E, 70))
			return;
		for (i = 0; i < 8; i++) {
			if(set_el_wires(Z, 140))
				return;
			if(set_el_wires(A, 140))
				return;
			if(set_el_wires(B, 120))
				return;
			if(set_el_wires(D, 100))
				return;
			if(set_el_wires(E, 140))
				return;
		}
		for (i = 0; i < 8; i++) {
			if(set_el_wires(Z, 60))
				return;
			if(set_el_wires(B|C, 80))
				return;
			if(set_el_wires(Z, 60))
				return;
			if(set_el_wires(D|E, 80))
				return;
		}
		break;
	}
}
