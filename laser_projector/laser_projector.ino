/*********************************** LICENCE **********************************\
| Laser Projector - arduino based blue and red laser projector software.       |
|                                                                              |
| Hardware:                                                                    |
|     * Two uno-polar step motors for X and Y axes,                            |
|       connected via NPNs to pins 4-7 and A0-A3.                              |
|     * Red and blue lasers, connected via MOSFATS to pins 8-9.                |
|                                                                              |
| Functions:                                                                   |
|     * relative/absolute steps/point                                          |
|     * set laser mask                                                         |
|     * set speed                                                              |
|     * set distance                                                           |
|     * set home                                                               |
|     * draw polynom with start angle, radius, kodkods                         |
|     * draw text x, y, size                                                   |
|                                                                              |
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

/* get int32_t from ascii serial terminal */
int32_t read_number() {
	while (!Serial.available());
	return Serial.parseInt();
}

/* read axes from serial into point16_t */
void read_point(point16_t * p) {
	p->x = read_number();
	p->y = read_number();
	print_point16(p);
}

/* read axes from serial into point32_t */
void read_steps(point32_t * s) {
	s->x = read_number();
	s->y = read_number();
	print_point32(s);
}

/* print point16_t axes to serial */
void (point16_t * p) {
	Serial.print(F("Got: ("));
	Serial.print(p->x, DEC);
	Serial.print(F(", "));
	Serial.print(p->y, DEC);
	Serial.print(F(")\n"));
}

/* print point32_t axes to serial */
void print_point32(point32_t * s) {
	Serial.print(F("Got: ("));
	Serial.print(s->x, DEC);
	Serial.print(F(", "));
	Serial.print(s->y, DEC);
	Serial.print(F(")\n"));
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

/* called once at power-on */
void setup() {
	Serial.begin(115200);
	Serial.setTimeout(1);
	PORTB &= ~LASERS_MASK;		// (8, 9) LOW for laser outputs
	set_lasers(0);				// turn off lasers
	DDRD |= 0xF0;				// (4 - 7) OUTPUTS
	DDRC |= 0x0F;				// (A0 - A3) OUTPUTS
	set_home();
}

/* called repeatedly after "setup" */
void loop() {	
	Serial.println(F("Enter command:\n"
	"0 for laser\n"
	"1 for z_sqare distance\n"
	"2 for steps delay\n"
	"3 for absolute point\n"
	"4 for relative point\n"
	"5 for absolute steps\n"
	"6 for relative steps\n"
	"7 for sqare\n"
	"8 for triangle\n"
	"9 for char\n"
	"10 for text"));
	uint8_t data = read_number();
	point16_t p16;
	point32_t p32;
	int32_t n;
	uint8_t i;
	point16_t ps[10];
	switch (data) {
		case 0:
			Serial.println(F("Enter laser state:"));
			data = read_number();
			set_lasers(data);
			Serial.print(F("Laser: "));
			switch (data) {
				case 0:
					Serial.println(F("Off"));
				break;
				case 1:
					Serial.println(F("Red"));
				break;
				case 2:
					Serial.println(F("Blue"));
				break;
				case 3:
					Serial.println(F("Red And Blue"));
				break;
			}
		break;
		case 1:
			Serial.println(F("Enter z_sqare distance:"));
			z_sqare = read_number();
			Serial.print(F("z_sqare = "));
			Serial.println(z_sqare, DEC);
		break;
		case 2:
			Serial.println(F("Enter steps delay:"));
			steps_delay_ms = read_number();
			Serial.print(F("steps delay = "));
			Serial.println(steps_delay_ms, DEC);
		break;
		case 3:
			Serial.println(F("Enter absolute point: (x, y)"));
			read_point(&p16);
			absolute_point(&p16);
		break;
		case 4:
			Serial.println(F("Enter relative point: (x, y)"));
			read_point(&p16);
			relative_point(&p16);
		break;
		case 5:
			Serial.println(F("Enter absolute steps: (x, y)"));
			read_steps(&p32);
			step_to_current_steps();
			absolute_steps(&p32);
			disable_axes();
		break;
		case 6:
			Serial.println(F("Enter relative steps: (x, y)"));
			read_steps(&p32);
			step_to_current_steps();
			relative_steps(&p32);
			disable_axes();
		break;
		case 7:
			n = read_number();
			ps[0].x = n; ps[0].y = 0;
			ps[1].x = 0; ps[1].y = n;
			ps[2].x = -n; ps[2].y = 0;
			ps[3].x = 0; ps[3].y = -n;
			for (i = 0; i < 4; i++)
				relative_point(&(ps[i % 4]));
		break;
		case 8:
			n = read_number();
			ps[0].x = n; ps[0].y = 0;
			ps[1].x = -n/2; ps[1].y = round(n * 0.86);
			ps[2].x = -n/2; ps[2].y = -round(n * 0.86);
			for (i = 0; i < 3; i++)
				relative_point(&(ps[i % 3]));
		break;
		case 9:
			/*Serial.println(F("Enter char(a-z) and size(0-4):"));
			i=read_number();
			Serial.println(draw_char(-20, -20, 'A', 2, LASER_BLUE_MASK), DEC);
			Serial.println(draw_char(0, -20, 'R', 2, LASER_BLUE_MASK), DEC);
			Serial.println(draw_char(20, -20, 'A', 2, LASER_BLUE_MASK), DEC);
			Serial.println(draw_char(40, -20, 'D', 2, LASER_BLUE_MASK), DEC);
			*/
			set_lasers(LASER_BLUE_MASK);
			draw_text(-20, -20, "ARAD!", 1);
		break;
		case 10:
			// draw_points [LASER_POINTS_COUNT];
			// draw_laser_masks  [LASER_MASKS_COUNT];
			// memset(draw_laser_masks, 0, LASER_MASKS_COUNT);
		break;
	}
}

void draw_poly(int16_t x, int16_t y, uint8_t size) {

}

void draw_text(int16_t x, int16_t y, char * text, double scale) {
	char c;
	
	while (c = *text++)
		x += draw_char(x, y, c, scale);
}

uint8_t draw_char(int16_t x, int16_t y, uint8_t c, double scale) {
	uint16_t next_index, index = 0;
	uint8_t width = 0, lasers_mask = current_lasers_mask;
	point16_t p;
	
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
