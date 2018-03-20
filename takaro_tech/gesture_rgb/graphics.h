
// #define WIDTH							(MATRIX_SIZE)
// #define HEIGHT							(MATRIX_SIZE)
// #define WIDTH_MAX						(WIDTH - 1)
// #define HEIGHT_MAX						(HEIGHT - 1)

extern void draw_pixel(uint8_t x, uint8_t y, CRGB c);


void swap(int16_t &a, int16_t &b)
{
	a ^= b; b = a ^ b; a ^= b;
}

bool isBadX(int16_t x)
{
	return ((x < 0) || (x > WIDTH_MAX));
}

bool isBadY(int16_t y)
{
	return ((y < 0) || (y > HEIGHT_MAX));
}

bool isBadPoint(int16_t x, int16_t y)
{
	return isBadX(x) || isBadY(y);
}

void drawPixel(int16_t x, int16_t y, CRGB c)
{
	if(isBadPoint(x, y)) return;
	draw_pixel(x, y, c);
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, CRGB c)
{
	if(isBadY(y)) return;
	if (x < 0)
	{
		w += x;
		x = 0;
	}
	else if (x > WIDTH_MAX) return;
	if (w < 1) return;
	if (w + x > WIDTH) w = WIDTH - x;
	
	while (w)
	{
		drawPixel(x, y, c);
		x++;
		w--;
	}
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, CRGB c)
{
	if(isBadX(x)) return;
	if (y < 0)
	{
		h += y;
		y = 0;
	}
	else if (y > HEIGHT_MAX) return;
	if (h < 1) return;
	if (h + y > HEIGHT) h = HEIGHT - y;

	while (h)
	{
		drawPixel(x, y, c);
		y++;
		h--;
	}
}

void fillScreen(CRGB c)
{
	leds.fill_solid(c);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, CRGB c)
{
	int16_t steeps = abs(y1 - y0) > abs(x1 - x0);
	
	if (steeps)
	{
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) ystep = 1;
	else 		 ystep = -1;

	while (x0 <= x1)
	{
		if (steeps) drawPixel(y0, x0, c);
		else 	    drawPixel(x0, y0, c);
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
		x0++;
	}
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint8_t c)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4)
		{
			drawPixel(x0 + x, y0 + y, c);
			drawPixel(x0 + y, y0 + x, c);
		} 
		if (cornername & 0x2)
		{
			drawPixel(x0 + x, y0 - y, c);
			drawPixel(x0 + y, y0 - x, c);
		}
		if (cornername & 0x8)
		{
			drawPixel(x0 - y, y0 + x, c);
			drawPixel(x0 - x, y0 + y, c);
		}
		if (cornername & 0x1)
		{
			drawPixel(x0 - y, y0 - x, c);
			drawPixel(x0 - x, y0 - y, c);
		}
	}
}

void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint8_t c)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x1)
		{
			drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, c);
			drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, c);
		}
		if (cornername & 0x2)
		{
			drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, c);
			drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, c);
		}
	}
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, CRGB c)
{
	for (int16_t i = x; i < x + w; i++)
		drawFastVLine(i, y, h, c); 
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, CRGB c)
{
	if ((w == 0) || (h == 0)) return;
	drawFastVLine(x,         y,         h,   c);
	drawFastVLine(x++ + --w, y,         h,   c);
	drawFastHLine(x,         y,         --w, c);
	drawFastHLine(x,         y + h - 1, w,   c);

}

void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t r, CRGB c)
{
	drawFastHLine(x + r    , y        , w - 2 * r, c);
	drawFastHLine(x + r    , y + h - 1, w - 2 * r, c);
	drawFastVLine(x        , y + r    , h - 2 * r, c);
	drawFastVLine(x + w - 1, y + r    , h - 2 * r, c);
	drawCircleHelper(x + r        , y + r        , r, 1, c);
	drawCircleHelper(x + w - r - 1, y + r        , r, 2, c);
	drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, c);
	drawCircleHelper(x + r        , y + h - r - 1, r, 8, c);
}

void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t r, CRGB c)
{
	fillRect(x + r, y, w - 2 * r, h, c);
	fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, c);
	fillCircleHelper(x + r        , y + r, r, 2, h - 2 * r - 1, c);
}

void drawCircle(int16_t x, int16_t y, uint8_t r, CRGB c)
{
	int16_t xx = -r, yy = 0,rr = r, err = 2 - 2 * r;
	do
	{
		drawPixel(x - xx, y + yy, c);
		drawPixel(x - yy, y - xx, c);
		drawPixel(x + xx, y - yy, c);
		drawPixel(x + yy, y + xx, c);
		rr = err;
		if (rr >  xx) err += ++xx * 2 + 1;
		if (rr <= yy) err += ++yy * 2 + 1;
	} while (xx < 0);
}

void drawEllipse(int16_t x0, int16_t y0, int16_t x1, int16_t y1, CRGB c)
{
	int16_t a = abs(x1 - x0), b = abs(y1 - y0), b1 = b & 1;
	int16_t dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;
	int16_t err = dx + dy + b1 * a * a, e2;

	if (x0 > x1) {x0 = x1; x1 += a;}
	if (y0 > y1) y0 = y1;
	y0 += (b + 1) / 2;
	y1 = y0 - b1;
	a *= 8 * a; 
	b1 = 8 * b * b;

	do
	{
		drawPixel(x1, y0, c);
		drawPixel(x0, y0, c);
		drawPixel(x0, y1, c);
		drawPixel(x1, y1, c);
		e2 = 2 * err;
		if (e2 >= dx) {x0++; x1--; err += dx += b1;}
		if (e2 <= dy) {y0++; y1--; err += dy += a;}
	} while (x0 <= x1);

	while (y0 - y1 < b)
	{
		drawPixel(x0 - 1, ++y0, c);
		drawPixel(x0 - 1, --y1, c); 
	}
}

void fillCircle(int16_t x, int16_t y, uint8_t r, CRGB c)
{
	drawFastVLine(x, y - r, 2 * r + 1, c);
	fillCircleHelper(x, y, r, 3, 0, c);
}

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, CRGB c)
{
	drawLine(x0, y0, x1, y1, c);
	drawLine(x1, y1, x2, y2, c);
	drawLine(x2, y2, x0, y0, c);

}
void fillTriangle( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, CRGB c)
{
	int16_t a, b, y, last;

	if (y0 > y1) {swap(y0, y1); swap(x0, x1);}
	if (y1 > y2) {swap(y2, y1); swap(x2, x1);}
	if (y0 > y1) {swap(y0, y1); swap(x0, x1);}

	if(y0 == y2)
	{
		a = b = x0;
		if(x1 < a)      a = x1;
		else if(x1 > b) b = x1;
		if(x2 < a)      a = x2;
		else if(x2 > b) b = x2;
		drawFastHLine(a, y0, b - a + 1, c);
		return;
	}

	int16_t
		dx01 = x1 - x0,
		dy01 = y1 - y0,
		dx02 = x2 - x0,
		dy02 = y2 - y0,
		dx12 = x2 - x1,
		dy12 = y2 - y1,
		sa   = 0,
		sb   = 0;

	last = y1;
	if(y1 != y2) last--;

	for(y = y0; y <= last; y++)
	{
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		if(a > b) swap(a,b);
		drawFastHLine(a, y, b-a+1, c);
	}

	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	while(y <= y2)
	{
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if(a > b) swap(a,b);
		drawFastHLine(a, y++, b - a + 1, c);
	}
}
