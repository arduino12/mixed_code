#define AXIS_COUNT			(3)


void idle_delay(uint32_t ms);

uint16_t effect_delay_ms;
bool effect_running;
uint8_t effect_index;



bool effect_delay(uint32_t ms) {
	ms += millis();
	while (ms > millis()) {
		idle_delay(1);
		if (!effect_running) {
			effect_running = true;
			return true;
		}
	}
	return false;
}

bool rand_bool() {
	return random(2);
}

bool flush_clear_delay() {
	flash_cube();
	clear_cube();
	return effect_delay(effect_delay_ms);
}

bool flush_delay() {
	flash_cube();
	return effect_delay(effect_delay_ms);
}

void rain() {
	effect_delay_ms = 130;
	draw_mode = DRAW_REPLACE;
	
	uint8_t pX, pY, pZ = 3;
	bool dir = false;

	while (effect_running) {
		if (++pZ > 3) {
			pX = random(4); 
			pY = random(4); 
			dir = rand_bool();
			pZ = 0;
		}
		next_hue();
		if (dir) draw_pixel(pX, pY, pZ);
		else draw_pixel(pX, pY, 3 - pZ);
		if (flush_clear_delay()) return;
	}
}

void chaseTheDot() {
	effect_delay_ms = (80);
	draw_mode = DRAW_REPLACE;

	uint8_t pX = 0, pY = 0, pZ = 0;
	uint8_t *mover;
	while (effect_running) {
		next_hue();
		switch (random(3)) {
			case 0: mover = &pX; break;
			case 1: mover = &pY; break;
			case 2: mover = &pZ; break;
		}
		if (rand_bool()) {if (++*mover > 3) *mover = 2;}
		else             {if (--*mover > 3) *mover = 1;}

		draw_pixel(pX, pY, pZ);
		if (flush_clear_delay()) return;
	}
}

void movingCubes() {
	const uint8_t C_SIZE = 2;
	effect_delay_ms = 200;
	draw_mode = DRAW_ADD;
	char
		rX = 0, rY = 2, rZ = 0,
		gX = 0, gY = 0, gZ = 2,
		bX = 2, bY = 0, bZ = 0;

	char *mover;
	bool moved = true;
	while (effect_running) {
		if (moved)
			switch (random(9)) {
				case 0: mover = &bX; break;
				case 1: mover = &bY; break;
				case 2: mover = &bZ; break;
				case 3: mover = &rX; break;
				case 4: mover = &rY; break;
				case 5: mover = &rZ; break;
				case 6: mover = &gX; break;
				case 7: mover = &gY; break;
				case 8: mover = &gZ; break;
			}
		moved = !moved;
		*mover = (((*mover) + 2) % 4) - 1;
		set_rgb_color(MAX_BRIGHTNESS, 0, 0);
		fill_box(abs(gX), abs(gY), abs(gZ), C_SIZE, C_SIZE, C_SIZE);
		set_rgb_color(0, MAX_BRIGHTNESS, 0);
		fill_box(abs(rX), abs(rY), abs(rZ), C_SIZE, C_SIZE, C_SIZE);
		set_rgb_color(0, 0, MAX_BRIGHTNESS);
		fill_box(abs(bX), abs(bY), abs(bZ), C_SIZE, C_SIZE, C_SIZE);
		if (flush_clear_delay()) return;
	}
}

void hueCube() {
	effect_delay_ms = (random(4) * 500 + 200);
	draw_mode = DRAW_REPLACE;
	while (effect_running) {
		// next_hue();
		set_rgb_color(MAX_BRIGHTNESS, 0, 0);
		fill_cube();
		if (flush_clear_delay()) return;
		set_rgb_color(0, MAX_BRIGHTNESS, 0);
		fill_cube();
		if (flush_clear_delay()) return;
		set_rgb_color(0, 0, MAX_BRIGHTNESS);
		fill_cube();
		if (flush_clear_delay()) return;
		
	}
}

void fountian() {
	effect_delay_ms = (200);
	draw_mode = DRAW_REPLACE;
	while (effect_running) {
		next_hue();
		for (uint8_t z = 0; z < 4; z++) {
			fill_rect_z(1, 1, z, 2, 2);
			if (flush_clear_delay()) return;
		}
		for (uint8_t z = 4; z > 0; z--) {
			draw_rect_z(0, 0, z - 1, 4, 4);
			if (flush_clear_delay()) return;
		}
	}
}

void linesRot() {
	effect_delay_ms = (100);
	rotateSide();
	draw_mode = DRAW_REPLACE;
	rotatedRectAxis(random(AXIS_COUNT));
	while (effect_running) {
		next_hue();
		for (uint8_t i = 0; i < 7; i++) {
			set_hue();
			fillRotatedRect(0, 1, i);
			fillRotatedRect(2, 1, i);
			set_neg_hue();
			fillRotatedRect(1, 1, 12 - i);
			fillRotatedRect(3, 1, 12 - i);
			if (flush_clear_delay()) return;
		}
		for (uint8_t i = 7; i > 0; i--) {
			set_hue();
			fillRotatedRect(0, 1, i - 1);
			fillRotatedRect(2, 1, i - 1);
			set_neg_hue();
			fillRotatedRect(1, 1, 13 - i);
			fillRotatedRect(3, 1, 13 - i);
			if (flush_clear_delay()) return;
		}
	}
}

void colorProp() {
	effect_delay_ms = (140);
	rotateMiddle();
	draw_mode = DRAW_REPLACE;
	uint8_t i = 0;
	uint8_t color = 0;
	rotatedRectAxis(random(AXIS_COUNT));
	while (effect_running) {
		set_hue_color(color);
		fillRotatedRect(0, 1, i);
		set_hue_color((color + 1) % HUE_LEVELS);
		fillRotatedRect(1, 1, (i + 1) % ROTATE_MIDDLE_COUNT);
		set_hue_color((color + 2) % HUE_LEVELS);
		fillRotatedRect(2, 1, (i + 2) % ROTATE_MIDDLE_COUNT);
		set_hue_color((color + 3) % HUE_LEVELS);
		fillRotatedRect(3, 1, (i + 3) % ROTATE_MIDDLE_COUNT);
		if (flush_clear_delay()) return;
		inc_hue(color);
		incMiddle(i);
	}
}

void fourProps() {
	effect_delay_ms = (200);
	rotateMiddle();
	draw_mode = DRAW_REPLACE;
	while (effect_running) {
		for (uint8_t i = 0; i < ROTATE_MIDDLE_COUNT; i++) {
			set_rgb_color(MAX_BRIGHTNESS, 0, 0);
			fillRotatedRectY(0, 1, i);
			set_rgb_color(0, 0, MAX_BRIGHTNESS);
			fillRotatedRectX(3, 1, ROTATE_MIDDLE_MAX - (i + 5) % ROTATE_MIDDLE_COUNT);
			set_rgb_color(MAX_BRIGHTNESS, MAX_BRIGHTNESS, 0);
			fillRotatedRectY(3, 1, ROTATE_MIDDLE_MAX - (i + 2) % ROTATE_MIDDLE_COUNT);
			set_rgb_color(0, MAX_BRIGHTNESS, 0);
			fillRotatedRectX(0, 1, (i + 3) % ROTATE_MIDDLE_COUNT);
			if (flush_clear_delay()) return;
		}
	}
}

void sideRotats() {
	effect_delay_ms = (52);
	rotateSide();
	draw_mode = DRAW_REPLACE;
	next_hue();
	uint8_t i = 0, pos = 0;
	bool stepUp = false;
	rotatedRectAxis(0);
	while (effect_running) {
		if ((i % 6 == 0 && rand_bool())) {
			next_hue();
			stepUp = rand_bool();
			if (rand_bool()) {
				if (i % 12) {
					if (pos < 2) pos = 1 - pos;
					else {pos = 0; i -= 6;}
				}
				else {
					if (pos) pos = 3 - pos;
					else {pos = 2; i += 6;}
				}
				rotatedRectAxis(pos);
			}
		}
		fillRotatedRect(0, 4, i);
		if (flush_clear_delay()) return;
		if (stepUp) incSide(i);
		else 		decSide(i);
	}
}

void lineRotats() {
	effect_delay_ms = (38);
	rotateSide();
	draw_mode = DRAW_REPLACE;
	next_hue();
	uint8_t i = 0, pos = 0;
	bool stepUp = true;
	while (effect_running) {
		if ((i % 6 == 0)) {
			next_hue();
			stepUp = rand_bool();
			if (rand_bool()) {
				if      (i == 0) {
					if      (pos < 2)  pos = 1 - pos;
					else if (pos < 3) {pos = 0; i = 6;}
					else if (pos < 5) {pos = 4 - pos; i = 12;}
					else              {pos = 0; i = 18;}
				}
				else if (i == 6) {
					if      (pos < 1) {pos = 2; i = 0;}
					else if (pos < 3)  pos = 3 - pos;
					else if (pos < 4) {pos = 2; i = 12;}
					else              {pos = 6 - pos; i = 18;}
				}
				else if (i == 12) {
					if      (pos < 2) {pos = 4 - pos; i = 0;}
					else if (pos < 3) {pos = 3; i = 6;}
					else if (pos < 5)  pos = 7 - pos;
					else              {pos = 3; i = 18;}
				}
				else {
					if      (pos < 1) {pos = 5; i = 0;}
					else if (pos < 3) {pos = 6 - pos; i = 6;}
					else if (pos < 4) {pos = 5; i = 12;}
					else               pos = 9 - pos;
				}
				rotatedRectAxis(pos % AXIS_COUNT);
			}
		}

		if (pos < AXIS_COUNT) fillRotatedRect(0, 1, i);
		else                  fillRotatedRect(3, 1, i);
		if (flush_clear_delay()) return;
		if (stepUp) incSide(i);
		else 		decSide(i);
	}
}

void test() {
	uint16_t d = 400;
	int8_t x = 0;
	int8_t y = 0;
	int8_t z = 0;
	
	// set_rgb_color(MAX_BRIGHTNESS, 0, 0);
	// draw_pixel(0, 0, 0);
	// effect_delay_ms = 500;
	// if (flush_clear_delay()) return;
	
	for (uint8_t i = 0; i < 8; i++) {
		set_rgb_color(MAX_BRIGHTNESS, 0, 0);
		draw_pixel(x, y, z);
		if (flush_clear_delay()) return;
		idle_delay(d);
		set_rgb_color(0, MAX_BRIGHTNESS, 0);
		draw_pixel(x, y, z);
		if (flush_clear_delay()) return;
		idle_delay(d);
		set_rgb_color(0, 0, MAX_BRIGHTNESS);
		draw_pixel(x, y, z);
		if (flush_clear_delay()) return;
		idle_delay(d);
		set_rgb_color(0, 0, 0);
		draw_pixel(x, y, z);
		if (flush_clear_delay()) return;
		idle_delay(1000);
	}
}

void effect_run(uint8_t index) {
	effect_running = true;
	switch (index) {
		case 0: linesRot();	break;
		case 1: fourProps();	break;
		case 2: hueCube();		break;
		case 3: fountian();		break;
		case 4: movingCubes();		break;
		case 5: colorProp();	break;
		case 6: chaseTheDot();	break;
		case 7: sideRotats();	break;
		case 8: lineRotats();	break;
		case 9: rain();			break;
	}
}
