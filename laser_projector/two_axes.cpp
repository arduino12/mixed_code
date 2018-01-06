#include "two_axes.h"

TwoAxes::TwoAxes(int32_t x, int32_t y) {
	dx = abs(x);
	dy = abs(y);
	sx = get_diraction(x);
	sy = get_diraction(y);
	steps = max(dx, dy);
	err = (dx > dy ? dx : -dy) / 2;
}

uint8_t TwoAxes::get_next_step_mask() {
	int32_t e = err;
	uint8_t mask = 0;
	if (e >= -dx) {
		err -= dy;
		mask |= X_AXIS_MASK;
	}
	if (e < dy) {
		err += dx;
		mask |= Y_AXIS_MASK;
	}
	return mask;
}

int32_t TwoAxes::get_diraction(int32_t n) {
	if (n > 0) return 1;
	if (n < 0) return -1;
	return 0;
}
