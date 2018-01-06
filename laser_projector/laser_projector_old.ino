/*********************************** LICENCE **********************************\
| Laser Projector - arduino based blue and red laser projector software.       |
|                                                                              |
| Hardware:                                                                    |
|     * Two uno-polar step motors for X and Y axes,                            |
|       connected via NPNs to pins 4-7 and A0-A3.                              |
|     * Red and blue lasers, connected via MOSFATS to pins 8-9.                |
|                                                                              |
| Functions:                                                                   |
|     * relative/absolute steps/point (x, y)                                   |
|     * set lasers mask (lasers mask)                                          |
|     * set speed (speed)                                                      |
|     * set distance (distance)                                                |
|     * set home                                                               |
|     * draw polygon (x, y, cornners, [size, start angle])                     |
|     * draw text (x, y, text, [size])                                         |
|                                                                              |
| Protocol:                                                                    |
|     * use textual single line protocol ending with \n and binary protocol!   |
|     * binary starts with barker and then like textual...                     |
|     * max length 60 bytes                                                    |
|     * text: no spaces - for two words need two commands                      |
|     * listen for serial in loop,                                             |
|     * when full command recived - execut                                     |
|     * while executing, check buffer size, if not 0, return                   |
|                                                                              |
| By A.E.TEC (Arad Eizen) 2016.                                                |
|	                                                                           |
\******************************************************************************/
#include <math.h>					// asin
#include "two_axes.h"				// TwoAxes
#include "hershey_font.h"			// HERSHEY_FONT

#define X_AXIS_LIMIT_MIN			(-200)
#define Y_AXIS_LIMIT_MIN			(-100)
#define X_AXIS_LIMIT_MAX			(200)
#define Y_AXIS_LIMIT_MAX			(300)

#define SERIAL_BAUDRATE				(115200)
#define SERIAL_BUFFER_SIZE			(60)

#define STEPS_PER_RADIAN			(648.68)
#define STEP_DELAY_MS				(20)
#define LASER_POINTS_COUNT			(256)
#define LASER_MASKS_COUNT			(LASER_POINTS_COUNT / 8)

#define X_AXIS_A_PIN				(4)
#define X_AXIS_B_PIN				(5)
#define X_AXIS_C_PIN				(6)
#define X_AXIS_D_PIN				(7)
#define LASER_BLUE_PIN				(8)
#define LASER_RED_PIN				(9)
#define Y_AXIS_A_PIN				(A0)
#define Y_AXIS_B_PIN				(A1)
#define Y_AXIS_C_PIN				(A2)
#define Y_AXIS_D_PIN				(A3)

#define LASER_RED_MASK				(0x01)
#define LASER_BLUE_MASK				(0x02)
#define LASERS_MASK					(LASER_BLUE_MASK | LASER_RED_MASK)

const uint8_t STEPS_MASKS[] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b1000, 0b1001};

typedef struct point8_t {
	int8_t x;
	int8_t y;
};

typedef struct point16_t {
	int16_t x;
	int16_t y;
};

typedef struct point32_t {
	int32_t x;
	int32_t y;
};

// 15 bits = axes value, 1 LSB = laser mask
//point_t draw_points[LASER_POINTS_COUNT];
//uint8_t draw_laser_masks[LASER_MASKS_COUNT];

//uint16_t draw_index, draw_count;
uint8_t steps_delay_ms = STEP_DELAY_MS;
uint8_t current_lasers_mask;
uint8_t current_mode;
uint32_t z_sqare = 19600;

point16_t current_point;
point32_t current_steps;

/* set the current laser position to (0, 0) for trigo to work */
void set_home() {
	current_point.x = 0;
	current_point.y = 0;
	current_steps.x = 0;
	current_steps.y = 0;
	step_to_current_steps();
	delay(STEP_DELAY_MS);
	disable_axes();
}

/* cuts the power to the motors (they will hold thier positions) 
	must call after done steping the motors with "step_to_current_steps" */
void disable_axes() {
	PORTC &= 0xF0;
	PORTD &= 0x0F;
}

/* sets the motors positions to match "current_steps" global
    must call with each change of 1 step in "current_steps" global */
void step_to_current_steps() {
	PORTC = (PORTC & 0xF0) | STEPS_MASKS[current_steps.x & 7];
	PORTD = (PORTD & 0x0F) | (STEPS_MASKS[current_steps.y & 7] << 4);
}

/* turn lasers on or off (pull mosfets sorce down to turn the laser on) */
void set_lasers(uint8_t mask) {
	mask &= LASERS_MASK;
	current_lasers_mask = mask;
	DDRB &= ~LASERS_MASK ^ mask;
	DDRB |= mask;
}

/* convert absolute point to absolute axes steps */
void point_to_steps(point16_t * p, point32_t * s) {
	double n = sqrt(p->x * p->x + p->y * p->y + z_sqare);
	double a = asin(p->y / n);
	s->x = round(STEPS_PER_RADIAN * asin(p->x / n / cos(a)));
	s->y = round(STEPS_PER_RADIAN * a);
}

/* go to the given axes values from current point */
void absolute_point(point16_t * p) {
	point16_t point = {p->x - current_point.x, p->y - current_point.y};
	relative_point(&point);
}

/* add/inc the given axes values from the current point */
void relative_point(point16_t * p) {
	TwoAxes two_axes_point(p->x, p->y);
	point32_t steps;
	step_to_current_steps();
	for (uint32_t i = 0; i < two_axes_point.steps; i++) {
		uint8_t axes_mask = two_axes_point.get_next_step_mask();
		if (axes_mask & X_AXIS_MASK) current_point.x += two_axes_point.sx;
		if (axes_mask & Y_AXIS_MASK) current_point.y += two_axes_point.sy;
		point_to_steps(&current_point, &steps);
		absolute_steps(&steps);
	}
	disable_axes();
}

/* go to the given axes steps (like degrees) from current motors position */
void absolute_steps(point32_t * s) {
	point32_t steps = {s->x - current_steps.x, s->y - current_steps.y};
	relative_steps(&steps);
}

/* add/inc the motors current position (like degrees) by the given steps */
void relative_steps(point32_t * s) {
	TwoAxes two_axes_steps(s->x, s->y);
	for (uint32_t i = 0; i < two_axes_steps.steps; i++) {
		uint8_t axes_mask = two_axes_steps.get_next_step_mask();
		if (axes_mask & X_AXIS_MASK) current_steps.x += two_axes_steps.sx;
		if (axes_mask & Y_AXIS_MASK) current_steps.y += two_axes_steps.sy;
		step_to_current_steps();
		delay(steps_delay_ms);
	}
}

/* turn lasers off and put projector in home position */
void go_home() {
	point32_t p32 = {0, 0};
	set_lasers(0);
	absolute_steps(&p32);
}

/* draw regular polygon x, y are the center point with absolute_steps */
void draw_polygon(int16_t x, int16_t y, uint8_t corners, int16_t size, int16_t start_angle) {
	point32_t p32;
	double current_angle = PI * start_angle / 180.0;
	double step_angle = PI * 2.0 / corners;
	corners++;
	while (corners) {
		p32.x = x + round(size * cos(current_angle));
		p32.y = y + round(size * sin(current_angle));
		absolute_steps(&p32);
		current_angle += step_angle;
		corners--;
	}
}

/* draw a NULL terminated string with absolute_steps */
void draw_text(int16_t x, int16_t y, char * text, double scale) {
	while (*text)
		x += draw_char(x, y, *text++, scale) + 1;
}

/* draw character with absolute_steps*/
uint8_t draw_char(int16_t x, int16_t y, uint8_t c, double scale) {
	uint16_t next_index, index = 0;
	uint8_t width = 0, lasers_mask = current_lasers_mask;
	point32_t p;
	
	index = pgm_read_byte(HERSHEY_FONT);
	if ((c < index) || (c >= (index + pgm_read_byte(HERSHEY_FONT + 1))))
		return 70;
	index = (c - index) * 2 + 2;
	next_index = pgm_read_word(HERSHEY_FONT + index + 2);
	index = pgm_read_word(HERSHEY_FONT + index);
	
	while (index < next_index) {
		p.x = (int8_t)(pgm_read_byte(HERSHEY_FONT + index));
		p.y = (int8_t)(pgm_read_byte(HERSHEY_FONT + index + 1));
		
		if ((p.x < 0) && (p.y < 0)) {
			p.x = -p.x;
			p.y = -p.y;
			set_lasers(0);
		}
		else set_lasers(lasers_mask);
		
		p.x *= scale;
		p.y *= scale;
		
		if (width < p.x) width = p.x;
		p.x += x;
		p.y += y;
		absolute_steps(&p);
		index += 2;
	}
	
	return width;
}

/* called once at power-on */
void setup() {
	Serial.begin(SERIAL_BAUDRATE);
	current_mode = 0;
	//Serial.setTimeout(1);
	PORTB &= ~LASERS_MASK;		// (8, 9) LOW for laser outputs
	set_lasers(0);				// turn off lasers
	DDRD |= 0xF0;				// (4 - 7) OUTPUTS
	DDRC |= 0x0F;				// (A0 - A3) OUTPUTS
	set_home();
}

/* called repeatedly after "setup" */
void loop() {	
	char buffer[SERIAL_BUFFER_SIZE] = {0};
	char text[SERIAL_BUFFER_SIZE] = {0};
	char * ptr = buffer + 2;
	int16_t i, j, c, x, y;
	point16_t p16;
	point32_t p32;
	
	uint8_t len = Serial.readBytesUntil('\n', buffer, SERIAL_BUFFER_SIZE - 1);
	if (0 == len) return;
	buffer[len] = 0;
	Serial.flush();
	
	switch (buffer[0]) {
		case '1':
			p32.x = 0;
			p32.y = 1;
			relative_steps(&p32);
		break;
		case '2':
			p32.x = 1;
			p32.y = 1;
			relative_steps(&p32);
		break;
		case '3':
			p32.x = 1;
			p32.y = 0;
			relative_steps(&p32);
		break;
		case '4':
			p32.x = 1;
			p32.y = -1;
			relative_steps(&p32);
		break;
		case '5':
			p32.x = 0;
			p32.y = -1;
			relative_steps(&p32);
		break;
		case '6':
			p32.x = -1;
			p32.y = -1;
			relative_steps(&p32);
		break;
		case '7':
			p32.x = -1;
			p32.y = 0;
			relative_steps(&p32);
		break;
		case '8':
			p32.x = -1;
			p32.y = 1;
			relative_steps(&p32);
		break;
		case '?':
			Serial.println(F(
				"Enter command:\n"
				"set lasers:         l + lasers mask\n"
				"set speed:          s + speed in ms\n"
				"set distance:       d + distance\n"
				"set home:           h\n"
				"set mode:           m + absolute / relative + steps / point\n"
				"go:                 g + x + y\n"
				"draw poligon:       p + x + y + corners + size + start_angle\n"
				"draw text:          t + x + y + text + size\n"
				"print this help:    ?\n"
				"print kinnernet:    k\n"
				"print unit:         u\n"
				"single step:        1-8\n"
			));
		break;
		case 'h':
			set_home();
			Serial.println(F("Home"));
		break;
		case 'l':
			sscanf(ptr, "%d", &i);
			i &= LASERS_MASK;
			set_lasers(i);
			Serial.print(F("Lasers: "));
			Serial.println(i);
		break;
		case 's':
			sscanf(ptr, "%d", &i);
			steps_delay_ms = i;
			Serial.print(F("Speed: "));
			Serial.println(i);
		break;
		case 'd':
			sscanf(ptr, "%d", &i);
			z_sqare = i;
			Serial.print(F("Distance: "));
			Serial.println(i);
		break;
		case 'm':
			sscanf(ptr, "%d", &i);
			current_mode = i;
			Serial.print(F("Mode: "));
			Serial.println(i);
		break;
		case 'g':
			x = y = 0;
			sscanf(ptr, "%d %d", &x, &y);
			Serial.print(F("Go: ("));
			Serial.print(x);
			Serial.print(F(", "));
			Serial.print(y);
			Serial.print(F(")\n"));
			p16.x = (int16_t)x;
			p16.y = (int16_t)y;
			p32.x = (int32_t)x;
			p32.y = (int32_t)y;
			
			switch (current_mode) {
				case 0: absolute_steps(&p32); break;
				case 1: relative_steps(&p32); break;
				case 2: absolute_point(&p16); break;
				case 3: relative_point(&p16); break;
			}
		break;
		case 't':
			i = 10;
			sscanf(ptr, "%d %d %s %d", &x, &y, &text, &i);
			Serial.print(F("Print: "));
			Serial.print(text);
			Serial.print(F(" At: ("));
			Serial.print(x);
			Serial.print(F(", "));
			Serial.print(y);
			Serial.print(F(")\n"));
			set_lasers(LASER_BLUE_MASK);
			draw_text(x, y, text, i / 10.0);
			go_home();
		break;
		case 'p':
			c = 4;
			i = 20;
			j = 0;
			sscanf(ptr, "%d %d %d %d %d", &x, &y, &c, &i, &j);
			Serial.print(F("Poligen:\nAt: ("));
			Serial.print(x);
			Serial.print(F(", "));
			Serial.print(y);
			Serial.print(F("), Corners: "));
			Serial.print(c);
			Serial.print(F(", Size: "));
			Serial.print(i);
			Serial.print(F(", Start Angle: "));
			Serial.println(j);
			p32.x = round(x + i * cos(PI * j / 180.0));
			p32.y = round(y + i * sin(PI * j / 180.0));
			absolute_steps(&p32);
			set_lasers(LASER_BLUE_MASK);
			draw_polygon(x, y, c, i, j);
			go_home();
		break;
		case 'k':
			set_lasers(LASER_BLUE_MASK);
			draw_text(-20, -20, "Kinnernet", 1);
			draw_text(0, -70, "2016", 1.5);
			go_home();
		break;
		case 'u':
			set_lasers(LASER_BLUE_MASK);
			draw_text(0, 0, "oooo", 1);
			draw_text(8, -15, "ooo", 1);
			draw_text(16, -30, "oo", 1);
			draw_text(0, -60, "8153", 1);
			go_home();
		break;
	}
}
