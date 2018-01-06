#define KEY_POWER					(0x00)
#define KEY_PLAY					(0x01)
#define KEY_BRIGHTER				(0x02)
#define KEY_DARKER					(0x03)
#define KEY_RED						(0x04)
#define KEY_GREEN					(0x05)
#define KEY_BLUE					(0x06)
#define KEY_WHITE					(0x07)
#define KEY_QUICK					(0x08)
#define KEY_SLOW					(0x09)
#define KEY_AUTO					(0x0A)
#define KEY_FLASH					(0x0B)
#define KEY_JUMP3					(0x0C)
#define KEY_JUMP7					(0x0D)
#define KEY_FADE3					(0x0E)
#define KEY_FADE7					(0x0F)
#define KEY_COLOR_00				(0x10)
#define KEY_COLOR_01				(0x11)
#define KEY_COLOR_02				(0x12)
#define KEY_COLOR_03				(0x13)
#define KEY_COLOR_04				(0x14)
#define KEY_COLOR_05				(0x15)
#define KEY_COLOR_06				(0x16)
#define KEY_COLOR_07				(0x17)
#define KEY_COLOR_08				(0x18)
#define KEY_COLOR_09				(0x19)
#define KEY_COLOR_10				(0x1A)
#define KEY_COLOR_11				(0x1B)
#define KEY_COLOR_12				(0x1C)
#define KEY_COLOR_13				(0x1D)
#define KEY_COLOR_14				(0x1E)
#define KEY_COLOR_15				(0x1F)
#define KEY_DIY_1					(0x20)
#define KEY_DIY_2					(0x21)
#define KEY_DIY_3					(0x22)
#define KEY_DIY_4					(0x23)
#define KEY_DIY_5					(0x24)
#define KEY_DIY_6					(0x25)
#define KEY_R_UP					(0x26)
#define KEY_G_UP					(0x27)
#define KEY_B_UP					(0x28)
#define KEY_R_DOWN					(0x29)
#define KEY_G_DOWN					(0x2A)
#define KEY_B_DOWN					(0x2B)
#define KEY_ON						(0x2C)
#define KEY_OFF						(0x2D)
#define KEY_STROBE					(0x2E)
#define KEY_FADE					(0x2F)
#define KEY_SMOOTH					(0x30)


uint8_t remote_key_map_44(uint32_t value) {
	if ((value >> 16) != 0xFF)
		return KEY_NONE;
	value &= 0xFFFF;
	if (value == 0x3AC5) return KEY_BRIGHTER;	// Brightness +
	if (value == 0xBA45) return KEY_DARKER;		// Brightness -
	if (value == 0x827D) return KEY_PLAY;		// Play
	if (value == 0x02FD) return KEY_POWER;		// On / Off
	if (value == 0x1AE5) return KEY_RED;		// Red
	if (value == 0x2AD5) return KEY_COLOR_00;	// Color 0
	if (value == 0x0AF5) return KEY_COLOR_04;	// Color 4
	if (value == 0x38C7) return KEY_COLOR_08;	// Color 8
	if (value == 0x18E7) return KEY_COLOR_12;	// Color 12
	if (value == 0x9A65) return KEY_GREEN;		// Green
	if (value == 0xAA55) return KEY_COLOR_01;	// Color 1
	if (value == 0x8A75) return KEY_COLOR_05;	// Color 9
	if (value == 0xB847) return KEY_COLOR_09;	// Color 10
	if (value == 0x9867) return KEY_COLOR_13;	// Color 13
	if (value == 0xA25D) return KEY_BLUE;		// Blue
	if (value == 0x926D) return KEY_COLOR_02;	// Color 2
	if (value == 0xB24D) return KEY_COLOR_06;	// Color 6
	if (value == 0x7887) return KEY_COLOR_10;	// Color 10
	if (value == 0x58A7) return KEY_COLOR_14;	// Color 14
	if (value == 0x22DD) return KEY_WHITE;		// White
	if (value == 0x12ED) return KEY_COLOR_03;	// Color 3
	if (value == 0x32CD) return KEY_COLOR_07;	// Color 7
	if (value == 0xF807) return KEY_COLOR_11;	// Color 11
	if (value == 0xD827) return KEY_COLOR_15;	// Color 15
	if (value == 0x28D7) return KEY_R_UP;		// R+
	if (value == 0xA857) return KEY_G_UP;		// G+
	if (value == 0x6897) return KEY_B_UP;		// B+
	if (value == 0xE817) return KEY_QUICK;		// QUICK
	if (value == 0x08F7) return KEY_R_DOWN;		// R-
	if (value == 0x8877) return KEY_G_DOWN;		// G-
	if (value == 0x48B7) return KEY_B_DOWN;		// B-
	if (value == 0xC837) return KEY_SLOW;		// SLOW
	if (value == 0x30CF) return KEY_DIY_1;		// DIY1
	if (value == 0xB04F) return KEY_DIY_2;		// DIY2
	if (value == 0x708F) return KEY_DIY_3;		// DIY3
	if (value == 0xF00F) return KEY_AUTO;		// AUTO
	if (value == 0x10EF) return KEY_DIY_1;		// DIY4
	if (value == 0x906F) return KEY_DIY_2;		// DIY5
	if (value == 0x50AF) return KEY_DIY_3;		// DIY6
	if (value == 0xD02F) return KEY_FLASH;		// FLASH
	if (value == 0x20DF) return KEY_JUMP3;		// JUMP3
	if (value == 0xA05F) return KEY_JUMP7;		// JUMP7
	if (value == 0x609F) return KEY_FADE3;		// FADE3
	if (value == 0xE01F) return KEY_FADE7;		// FADE7
	return KEY_NONE;
}

uint8_t remote_key_map_24(uint32_t value) {
	if ((value >> 16) != 0xF7)
		return KEY_NONE;
	value &= 0xFFFF;
	if (value == 0x00FF) return KEY_BRIGHTER;	// Brightness +
	if (value == 0x807F) return KEY_DARKER;		// Brightness -
	if (value == 0x40BF) return KEY_OFF;		// OFF
	if (value == 0xC03F) return KEY_ON;			// ON
	if (value == 0x20DF) return KEY_RED;		// Red
	if (value == 0xA05F) return KEY_GREEN;		// Green
	if (value == 0x609F) return KEY_BLUE;		// Blue
	if (value == 0xE01F) return KEY_WHITE;		// White
	if (value == 0x10EF) return KEY_COLOR_00;	// Color 0
	if (value == 0x906F) return KEY_COLOR_04;	// Color 4
	if (value == 0x50AF) return KEY_COLOR_08;	// Color 8
	if (value == 0xD02F) return KEY_FLASH;		// FLASH
	if (value == 0x30CF) return KEY_COLOR_01;	// Color 1
	if (value == 0xB04F) return KEY_COLOR_05;	// Color 5
	if (value == 0x708F) return KEY_COLOR_09;	// Color 9
	if (value == 0xF00F) return KEY_STROBE;		// STROBE
	if (value == 0x08F7) return KEY_COLOR_02;	// Color 2
	if (value == 0x8877) return KEY_COLOR_06;	// Color 6
	if (value == 0x48B7) return KEY_COLOR_10;	// Color 10
	if (value == 0xC837) return KEY_FADE;		// FADE
	if (value == 0x28D7) return KEY_COLOR_03;	// Color 3
	if (value == 0xA857) return KEY_COLOR_07;	// Color 7
	if (value == 0x6897) return KEY_COLOR_11;	// Color 11
	if (value == 0xE817) return KEY_SMOOTH;		// SMOOTH
	return KEY_NONE;
}

uint8_t remote_key_map_21(uint32_t value) {
	value &= 0xFFFF;
	if (value == 0x6897) return 0;	// 0
	if (value == 0x30CF) return 1;	// 1
	if (value == 0x18E7) return 2;	// 2
	if (value == 0x7A85) return 3;	// 3
	if (value == 0x10EF) return 4;	// 4
	if (value == 0x38C7) return 5;	// 5
	if (value == 0x5AA5) return 6;	// 6
	if (value == 0x42BD) return 7;	// 7
	if (value == 0x4AB5) return 8;	// 8
	if (value == 0x52AD) return 9;	// 9
	if (value == 0xA25D) return 10;	// CH-
	if (value == 0x629D) return 11;	// CH
	if (value == 0xE21D) return 12;	// CH+
	if (value == 0x22DD) return 13;	// |<<
	if (value == 0x02FD) return 14;	// >>|
	if (value == 0xC23D) return 15;	//  >|
	if (value == 0xE01F) return 16;	// -
	if (value == 0xA857) return 17;	// +
	if (value == 0x906F) return 18;	// EQ
	if (value == 0x9867) return 19;	// @
	if (value == 0xB04F) return 20;	// U/SD		
	return KEY_NONE;
}
