
#include "cube_zipped_regs.h"

#define LEDS_PER_PIXEL				(3)
#define CUBE_SIZE					(4)
#define PIXELS_COUNT				(CUBE_SIZE * CUBE_SIZE * CUBE_SIZE)
#define LED_COUNT					(PIXELS_COUNT * LEDS_PER_PIXEL)

#define R_CHANNEL					(0 * PIXELS_COUNT)
#define G_CHANNEL					(1 * PIXELS_COUNT)
#define B_CHANNEL					(2 * PIXELS_COUNT)

#define BRIGHTNESS_LEVELS			(8) // 4
#define MAX_BRIGHTNESS				(BRIGHTNESS_LEVELS - 1)

#define HUE_LEVELS					(24)
#define HUE_BLACK					(HUE_LEVELS + 0)
#define HUE_GRAY					(HUE_LEVELS + 1)
#define HUE_WHITE					(HUE_LEVELS + 2)

#define DRAW_REPLACE				(0)
#define DRAW_ADD					(1)
#define DRAW_SUB					(2)

#define ROTATE_MIDDLE_COUNT			(6)
#define ROTATE_SIDE_COUNT			(24)
#define ROTATE_MIDDLE_MAX			(ROTATE_MIDDLE_COUNT - 1)
#define ROTATE_SIDE_MAX				(ROTATE_SIDE_COUNT - 1)

#define LED_CUBE_UPDATE_US			(22) // 22


uint8_t draw_buffer[LED_COUNT];
uint8_t scan_buffer[LED_COUNT];
uint8_t scan_led_h_regs[LED_COUNT];
uint8_t scan_led_l_regs[LED_COUNT];
uint8_t draw_color[LEDS_PER_PIXEL];
uint8_t pwm_on_ticks[BRIGHTNESS_LEVELS];
uint8_t pwm_off_ticks[BRIGHTNESS_LEVELS];
uint8_t draw_mode, hue_color, scan_count;
volatile uint8_t cur_led, cur_pwm;

void (*rotatePos)(uint8_t &l, uint8_t p, uint8_t &x1, uint8_t &y1, uint8_t &x2, uint8_t &y2, uint8_t r);
void (*fillRotatedRect)(uint8_t a, uint8_t l, uint8_t r);

void toggle_cur_led() {
	uint8_t l = scan_led_l_regs[cur_led], h = scan_led_h_regs[cur_led];
	if (l & 0x80)      DDRC ^= l & 0x0F;
	else if (l & 0x40) DDRB ^= l & 0x3F;
	else               DDRD ^= l << 2;
	if (h & 0x80)      {h &= 0x0F; DDRC ^= h; PORTC ^= h;}
	else if (h & 0x40) {h &= 0x3F; DDRB ^= h; PORTB ^= h;}
	else               {h <<= 2; DDRD ^= h; PORTD ^= h;}
}

void clear_leds() {
	DISABLE_LED_CUBE_UPDATE();
	DDRB &= 0xC0;
	DDRC &= 0xF0;
	DDRD &= 0x03;
	PORTB &= 0xC0;
	PORTC &= 0xF0;
	PORTD &= 0x03;
	cur_pwm = 0;
	cur_led = 0;
	scan_count = 0;
}

uint8_t unzip_regs(uint8_t regs) {
	if (regs < 6)
		return (uint8_t)1 << regs;
	if (regs < 12)
		return 0x40 | ((uint8_t)1 << (regs - 6));
	return 0x80 | ((uint8_t)1 << (regs - 12));
}

void flash_cube() {
	clear_leds();

	for (uint8_t i = 0, r, d; i < LED_COUNT; i++) {
		d = draw_buffer[i];
		if (d) {
			r = pgm_read_byte(cube_zipped_regs + i);
			scan_led_l_regs[scan_count] = unzip_regs(r & 0x0F);
			scan_led_h_regs[scan_count] = unzip_regs(r >> 4);
			scan_buffer[scan_count] = d;
			scan_count++;
		}
	}

	if (!scan_count)
		return;
	scan_count--;
	ENABLE_LED_CUBE_UPDATE();
}

ISR(TIMER2_COMPA_vect) {
	if (cur_pwm) {
		toggle_cur_led();
		if (cur_pwm < MAX_BRIGHTNESS) {
			OCR2A = TCNT2 + pwm_off_ticks[cur_pwm];
			cur_pwm = 0;
			return;
		}
	}
	if (cur_led < scan_count)
		cur_led++;
	else
		cur_led = 0;
	cur_pwm = scan_buffer[cur_led];
	toggle_cur_led();
	OCR2A = TCNT2 + pwm_on_ticks[cur_pwm];
}

void clear_cube() {
	memset(draw_buffer, 0, LED_COUNT);
}

void invert_cube() {
	for (uint8_t i = 0; i < LED_COUNT; i++)
		draw_buffer[i] = MAX_BRIGHTNESS - draw_buffer[i];
}

void swap_cube_rgb_channels(bool dir) {
	uint8_t t[PIXELS_COUNT];
	uint8_t a1 = (dir ? 0 : PIXELS_COUNT);
	uint8_t a2 = PIXELS_COUNT - a1;
	memcpy(t, draw_buffer + B_CHANNEL, PIXELS_COUNT);	
	memcpy(draw_buffer + B_CHANNEL, draw_buffer + a1, PIXELS_COUNT);
	memcpy(draw_buffer + a1, draw_buffer + a2, PIXELS_COUNT);
	memcpy(draw_buffer + a2, t, PIXELS_COUNT);
}

void fill_cube() {
	uint8_t * ptr = draw_buffer;
	for (uint8_t i = 0; i < LEDS_PER_PIXEL; i++, ptr += PIXELS_COUNT)
		memset(ptr, draw_color[i], PIXELS_COUNT);
}

void draw_pixel(uint8_t * ptr) {
	switch (draw_mode) {
		case DRAW_REPLACE:
			for (uint8_t i = 0; i < LEDS_PER_PIXEL; i++, ptr += PIXELS_COUNT)
				*ptr = draw_color[i];
		break;
		case DRAW_ADD:
			for (uint8_t i = 0; i < LEDS_PER_PIXEL; i++, ptr += PIXELS_COUNT)
				*ptr = min(draw_color[i] + *ptr, MAX_BRIGHTNESS);
		break;
		case DRAW_SUB:
			for (uint8_t i = 0; i < LEDS_PER_PIXEL; i++, ptr += PIXELS_COUNT)
				*ptr = max((int8_t)draw_color[i] - *ptr, 0);
		break;
		// case DRAW_HUE:
		// case DRAW_RANDOM:
	}
}

void draw_pixel(uint8_t x, uint8_t y, uint8_t z) {
	if (x >= CUBE_SIZE || y >= CUBE_SIZE || z >= CUBE_SIZE)
		return;
	draw_pixel(draw_buffer + x + y * CUBE_SIZE * CUBE_SIZE + z * CUBE_SIZE);
}

void set_rgb_color(uint8_t r, uint8_t g, uint8_t b) {
	draw_color[0] = r;
	draw_color[1] = g;
	draw_color[2] = b;
}

void set_zipped_color(uint8_t c) {
	for (uint8_t i = 0; i < LEDS_PER_PIXEL; i++, c >>= 2)
		draw_color[i] = c & MAX_BRIGHTNESS;
}

void set_hue_color(uint8_t h) {
	uint8_t t;
	if (h < HUE_LEVELS) {
		t = h % BRIGHTNESS_LEVELS;
		switch (h / BRIGHTNESS_LEVELS) {
			case 0: set_rgb_color(MAX_BRIGHTNESS, t, 0); break;
			case 1: set_rgb_color(MAX_BRIGHTNESS - t, MAX_BRIGHTNESS, 0); break;
			case 2: set_rgb_color(0, MAX_BRIGHTNESS, t); break;
			case 3: set_rgb_color(0, MAX_BRIGHTNESS - t, MAX_BRIGHTNESS); break;
			case 4: set_rgb_color(t, 0, MAX_BRIGHTNESS); break;
			case 5: set_rgb_color(MAX_BRIGHTNESS, 0, MAX_BRIGHTNESS - t); break;
		}
	}
	else {
		t = (h - HUE_LEVELS) % MAX_BRIGHTNESS;
		set_rgb_color(t, t, t);
	}
}

void set_hue() {
	set_hue_color(hue_color);
}

void set_neg_hue() {
	set_hue_color((hue_color + HUE_LEVELS / 2) % HUE_LEVELS);
}

void inc_hue(uint8_t &c) {
	if (++c >= HUE_LEVELS) c = 0;
}

void dec_hue(uint8_t &c) {
	if (--c >= HUE_LEVELS) c = HUE_LEVELS - 1;
}

void inc_rgb(uint8_t channel) {
	uint8_t * ptr = draw_color + channel;
	if (*ptr < MAX_BRIGHTNESS)
		(*ptr)++;
}

void dec_rgb(uint8_t channel) {
	uint8_t * ptr = draw_color + channel;
	if (*ptr)
		(*ptr)--;
}

void next_hue() {
	inc_hue(hue_color);
	set_hue_color(hue_color);
}

void swap_color_rgb_channels(bool dir) {
	if (dir)
		set_rgb_color(draw_color[1], draw_color[2], draw_color[0]);
	else
		set_rgb_color(draw_color[2], draw_color[0], draw_color[1]);
}

uint8_t round_clostest(uint8_t numerator, uint8_t denominator) {
	numerator = (numerator << 1) / denominator;
	return (numerator >> 1) + (numerator % 2);
}

void swap(uint8_t &one, uint8_t &two) {
	one ^= two; two = one ^ two; one ^= two;
}

void fix_len(uint8_t &l, uint8_t p) {
	l = min(l, CUBE_SIZE - p);
}

void colorize_cube() {
	for (uint8_t i = 0; i < PIXELS_COUNT; i++)
		if (draw_color[i + R_CHANNEL] || draw_color[i + G_CHANNEL] || draw_color[i + B_CHANNEL])
			draw_pixel(draw_color + i);
}

void draw_line_x(uint8_t x, uint8_t y, uint8_t z, uint8_t l) {
	fix_len(l, x);
	while(l--) draw_pixel(x + l, y, z);
}

void draw_line_y(uint8_t x, uint8_t y, uint8_t z, uint8_t l) {
	fix_len(l, y);
	while(l--) draw_pixel(x, y + l, z);
}

void draw_line_z(uint8_t x, uint8_t y, uint8_t z, uint8_t l) {
	fix_len(l, z);
	while(l--) draw_pixel(x, y, z + l);
}

void draw_line(uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2) {
	bool reverseX = false, reverseY = false, reverseZ = false;
	if (x1 > x2) {swap(x1, x2); reverseX = true;}
	if (y1 > y2) {swap(y1, y2); reverseY = true;}
	if (z1 > z2) {swap(z1, z2); reverseZ = true;}

	uint8_t delx = x2 - x1;
	uint8_t dely = y2 - y1;
	uint8_t delz = z2 - z1;

	uint8_t longest = max(delx, max(dely, delz));
	for (uint8_t x, y, z, i = 0; i < longest; i++) {
		x = (reverseX ? longest - i : i);
		y = (reverseY ? longest - i : i);
		z = (reverseZ ? longest - i : i);
		draw_pixel(round_clostest(x * delx, longest) + x1, round_clostest(y * dely, longest) + y1, round_clostest(z * delz, longest) + z1);
	}

	if (reverseX) swap(x1, x2);
	if (reverseY) swap(y1, y2);
	if (reverseZ) swap(z1, z2);
	draw_pixel(x2, y2, z2);
}

void draw_rect_x(uint8_t x, uint8_t y, uint8_t z, uint8_t yl, uint8_t zl) {
	if ((yl-- == 0) || (zl-- == 0)) return;
	draw_line_y(x, y , z, yl);
	draw_line_y(x, y + 1, z + zl, yl);
	draw_line_z(x, y , z + 1, zl);
	draw_line_z(x, y + yl, z, zl);
}
void draw_rect_y(uint8_t x, uint8_t y, uint8_t z, uint8_t xl, uint8_t zl) {
	if ((xl-- == 0) || (zl-- == 0)) return;
	draw_line_x(x, y , z, xl);
	draw_line_x(x + 1, y, z + zl, xl);
	draw_line_z(x, y , z + 1, zl);
	draw_line_z(x + xl, y, z, zl);
}
void draw_rect_z(uint8_t x, uint8_t y, uint8_t z, uint8_t xl, uint8_t yl) {
	if ((xl-- == 0) || (yl-- == 0)) return;
	draw_line_x(x, y , z, xl);
	draw_line_x(x + 1, y + yl, z, xl);
	draw_line_y(x, y + 1, z, yl);
	draw_line_y(x + xl, y, z, yl);
}

void fill_rect_x(uint8_t x, uint8_t y, uint8_t z, uint8_t yl, uint8_t zl) {
	fix_len(zl, z);
	while(zl--) draw_line_y(x, y , z + zl, yl);
}
void fill_rect_y(uint8_t x, uint8_t y, uint8_t z, uint8_t xl, uint8_t zl) {
	fix_len(zl, z);
	while(zl--) draw_line_x(x, y , z + zl, xl);
}
void fill_rect_z(uint8_t x, uint8_t y, uint8_t z, uint8_t xl, uint8_t yl) {
	fix_len(yl, y);
	while(yl--) draw_line_x(x, y + yl, z, xl);
}

void fill_box(uint8_t x, uint8_t y, uint8_t z, uint8_t xl, uint8_t yl, uint8_t zl) {
	uint8_t ly, lx;
	fix_len(xl, x); fix_len(yl, y); fix_len(zl, z);
	while(zl--) {
		ly = yl;
		while(ly--) {
			lx = xl;
			while(lx--)
				draw_pixel(x + lx, y + ly, z + zl);
		}
	}
}

void draw_box(uint8_t x, uint8_t y, uint8_t z, uint8_t xl, uint8_t yl, uint8_t zl, uint8_t s) {
	fix_len(xl, x); fix_len(yl, y); fix_len(zl, z);
	if ((xl-- == 0) || (yl-- == 0) || (zl-- == 0)) return;
	uint8_t ex = x + xl, ey = y + yl, ez = z + zl;
	
	for (uint8_t i = x; i <= ex; i++)
		for (uint8_t j = y; j <= ey; j++)
			for (uint8_t k = z; k <= ez; k++)
				if ((i == x) + (i == ex) + (j == y) + (j == ey) + (k == z) + (k == ez) > s)
					draw_pixel(i, j, k);
}




void incSide(uint8_t &r) {
	if (++r >= ROTATE_SIDE_COUNT) r = 0;
}
void decSide(uint8_t &r) {
	if (--r >= ROTATE_SIDE_COUNT) r = ROTATE_SIDE_MAX;
}
void incMiddle(uint8_t &r) {
	if (++r >= ROTATE_MIDDLE_COUNT) r = 0;
}
void decMiddle(uint8_t &r) {
	if (--r >= ROTATE_MIDDLE_COUNT) r = ROTATE_MIDDLE_MAX;
}

void fillRotatedRectX(uint8_t x, uint8_t l, uint8_t r) {
	uint8_t y1, z1, y2, z2;
	rotatePos(l, x, y1, z1, y2, z2, r);
	while(l--) draw_line(x + l, y1, z1, x + l, y2, z2);
}
void fillRotatedRectY(uint8_t y, uint8_t l, uint8_t r) {
	uint8_t x1, z1, x2, z2;
	rotatePos(l, y, x1, z1, x2, z2, r);
	while(l--) draw_line(x1, y + l, z1, x2, y + l, z2);
}
void fillRotatedRectZ(uint8_t z, uint8_t l, uint8_t r) {
	uint8_t x1, y1, x2, y2;
	rotatePos(l, z, x1, y1, x2, y2, r);
	while(l--) draw_line(x1, y1, z + l, x2, y2, z + l);
}

void rotatedRectAxis(uint8_t a) {
	if      (a == 0) fillRotatedRect = &fillRotatedRectX;
	else if (a == 1) fillRotatedRect = &fillRotatedRectY;
	else             fillRotatedRect = &fillRotatedRectZ;
}

void rotateMiddle(uint8_t &l, uint8_t p, uint8_t &x1, uint8_t &y1, uint8_t &x2, uint8_t &y2, uint8_t r) {
	fix_len(l, p);
	if (r < 3) { x1 = 0;     y1 = 3 - r; x2 = 3;     y2 = r; }
	else       { x1 = r - 3; y1 = 0;     x2 = 6 - r; y2 = 3; }
}
void rotateSide(uint8_t &l, uint8_t p, uint8_t &x1, uint8_t &y1, uint8_t &x2, uint8_t &y2, uint8_t r) {
	fix_len(l, p);
	if (r < 3)       { x1 = 0;      y1 = 0;      x2 = r;      y2 = 3; }
	else if (r < 6)  { x1 = 0;      y1 = 0;      x2 = 3;      y2 = 6 - r; }
	else if (r < 9)  { x1 = 0;      y1 = r - 6;  x2 = 3;      y2 = 0; }
	else if (r < 12) { x1 = r - 9;  y1 = 3;      x2 = 3;      y2 = 0; }
	else if (r < 15) { x1 = 3;      y1 = 3;      x2 = 15 - r; y2 = 0; }
	else if (r < 18) { x1 = 3;      y1 = 3;      x2 = 0;      y2 = r - 15; }
	else if (r < 21) { x1 = 3;      y1 = 21 - r; x2 = 0;      y2 = 3; }
	else             { x1 = 24 - r; y1 = 0;      x2 = 0;      y2 = 3; }
}

void rotateMiddle() {
	rotatePos = &rotateMiddle;
}
void rotateSide() {
	rotatePos = &rotateSide;
}

void led_cube_init() {
	/* map pwm level to ticks */
	for (uint8_t i = 0; i < BRIGHTNESS_LEVELS; i++)
		pwm_off_ticks[MAX_BRIGHTNESS - i] = pwm_on_ticks[i] = LED_CUBE_UPDATE_TICKS * i;
	/* initialize cube buffers and flags */
	clear_cube();
	flash_cube();
	draw_mode = DRAW_REPLACE;
	set_zipped_color(0);
	rotateMiddle();
}
