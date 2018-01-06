#ifndef FINGERS_H
#define FINGERS_H

#define SERVO_COUNT				(4)
#define SERVO_MIN				(0)
#define SERVO_MAX				(180)
#define EFECT_STEPS				(10)

Servo servos[SERVO_COUNT];


void calibrate() {
	motors_status = MOVE_LEFT | MOVE_RIGHT;
	// TODO: blocking or not..?
}

void move_fingers(uint8_t fingers_mask) {
	// TODO
}



void fingers_wave() {
	/*
	0000
	1000
	2100
	3210
	2321
	1232
	0123
	0012
	0001
	*/
	static uint8_t i_offset = 0;
	uint8_t j = 0;

	for (uint8_t i = SERVO_COUNT - 1; i > 0; i--)
		servos[i].write(servos[i - 1].read());
	
	if		(i_offset == 1 || i_offset == 5)	j = 1 * ((SERVO_MAX - SERVO_MIN) / 3);
	else if	(i_offset == 2 || i_offset == 4)	j = 2 * ((SERVO_MAX - SERVO_MIN) / 3);
	else if	(i_offset == 3)						j = 3 * ((SERVO_MAX - SERVO_MIN) / 3);
	servos[0].write(j);
	if (++i_offset >= 9) i_offset = 0;
}

void fingers_all() {
	static uint8_t pos = SERVO_MIN;

	for (uint8_t i = 0; i < SERVO_COUNT; i++)
		servos[i].write(pos);

	if (++pos >= SERVO_MAX) pos = SERVO_MIN;
}

#endif /* FINGERS_H */
