#ifndef HANDS_H
#define HANDS_H

#define LEFT_IR_PIN				(2)
#define RIGHT_IR_PIN			(3)
#define LEFT_LIMIT_PIN			(4)
#define LEFT_ENABLE_PIN			(5)
#define LEFT_A_PIN				(6)
#define LEFT_B_PIN				(7)
#define RIGHT_LIMIT_PIN			(8)
#define RIGHT_ENABLE_PIN		(9)
#define RIGHT_A_PIN				(10)
#define RIGHT_B_PIN				(11)


void calibrate() {
	motors_status = MOVE_LEFT | MOVE_RIGHT;
	// TODO: blocking or not..?
}

void move_left(uint32_t pos) {
	// TODO
}

void move_right(uint32_t pos) {
	// TODO
}

#endif /* HANDS_H */
