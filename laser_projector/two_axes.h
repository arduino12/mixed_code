#ifndef two_axes_h
#define two_axes_h

#include <Arduino.h>

#define X_AXIS_MASK		(0x01)
#define Y_AXIS_MASK		(0x02)

class TwoAxes {
	public:
		TwoAxes(int32_t x, int32_t y);
		uint8_t get_next_step_mask();
		int32_t sx, sy, steps;
	private:
		int32_t get_diraction(int32_t n);
		int32_t dx, dy, err;
};

#endif 
