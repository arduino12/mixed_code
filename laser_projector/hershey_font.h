/*
	Font Name           : hershey_font
	Date                : 06/04/2016
	Font size in bytes  : 2393
	Font first char     : 33
	Font char count     : 94

	Written by Arad Eizen 2016
*/

static const int8_t HERSHEY_FONT[] PROGMEM = {
	0x21, // first char
	0x5E, // chars count

	// characters size table: [MSB index, LSB index] * chars count
	0xc0,0x00, // !
	0xce,0x00, // "
	0xd6,0x00, // #
	0xe6,0x00, // $
	0x16,0x01, // %
	0x50,0x01, // &
	0x94,0x01, // '
	0xa2,0x01, // (
	0xb6,0x01, // )
	0xca,0x01, // *
	0xd6,0x01, // +
	0xde,0x01, // ,
	0xee,0x01, // -
	0xf2,0x01, // .
	0xfc,0x01, // /
	0x00,0x02, // 0
	0x22,0x02, // 1
	0x2a,0x02, // 2
	0x46,0x02, // 3
	0x64,0x02, // 4
	0x6e,0x02, // 5
	0x90,0x02, // 6
	0xbe,0x02, // 7
	0xc6,0x02, // 8
	0x00,0x03, // 9
	0x2e,0x03, // :
	0x42,0x03, // ;
	0x5c,0x03, // <
	0x62,0x03, // =
	0x6a,0x03, // >
	0x70,0x03, // ?
	0x96,0x03, // @
	0xfe,0x03, // A
	0x0a,0x04, // B
	0x34,0x04, // C
	0x58,0x04, // D
	0x74,0x04, // E
	0x84,0x04, // F
	0x90,0x04, // G
	0xba,0x04, // H
	0xc6,0x04, // I
	0xca,0x04, // J
	0xde,0x04, // K
	0xea,0x04, // L
	0xf2,0x04, // M
	0x02,0x05, // N
	0x0e,0x05, // O
	0x38,0x05, // P
	0x50,0x05, // Q
	0x7e,0x05, // R
	0x9a,0x05, // S
	0xc2,0x05, // T
	0xca,0x05, // U
	0xde,0x05, // V
	0xe6,0x05, // W
	0xf6,0x05, // X
	0xfe,0x05, // Y
	0x08,0x06, // Z
	0x14,0x06, // [
	0x24,0x06, // \ (line can't end with \)
	0x28,0x06, // ]
	0x38,0x06, // ^
	0x48,0x06, // _
	0x4c,0x06, // `
	0x5a,0x06, // a
	0x7a,0x06, // b
	0x9a,0x06, // c
	0xb6,0x06, // d
	0xd6,0x06, // e
	0xf8,0x06, // f
	0x06,0x07, // g
	0x30,0x07, // h
	0x42,0x07, // i
	0x50,0x07, // j
	0x64,0x07, // k
	0x70,0x07, // l
	0x74,0x07, // m
	0x94,0x07, // n
	0xa6,0x07, // o
	0xc8,0x07, // p
	0xe8,0x07, // q
	0x08,0x08, // r
	0x16,0x08, // s
	0x38,0x08, // t
	0x46,0x08, // u
	0x58,0x08, // v
	0x60,0x08, // w
	0x70,0x08, // x
	0x78,0x08, // y
	0x88,0x08, // z
	0x94,0x08, // {
	0xde,0x08, // |
	0xe2,0x08, // }
	0x2c,0x09, // ~
	0x58,0x09, // font total size
	// characters data: [data...] * chars count
	-5, -28, 5, 14, -5, -9, 4, 8, 5, 7, 6, 8, 5, 9, // !
	-4, -28, 4, 21, -12, -28, 12, 21, // "
	-11, -32, 4, 0, -17, -32, 10, 0, -4, -19, 18, 19, -3, -13, 17, 13, // #
	-8, -32, 8, 3, -12, -32, 12, 3, -17, -25, 15, 27, 12, 28, 8, 28, 5, 27, 3, 25, 3, 23, 4, 21, 5, 20, 7, 19, 13, 17, 15, 16, 16, 15, 17, 13, 17, 10, 15, 8, 12, 7, 8, 7, 5, 8, 3, 10, // $
	-21, -28, 3, 7, -8, -28, 10, 26, 10, 24, 9, 22, 7, 21, 5, 21, 3, 23, 3, 25, 4, 27, 6, 28, 8, 28, 10, 27, 13, 26, 16, 26, 19, 27, 21, 28, -17, -14, 15, 13, 14, 11, 14, 9, 16, 7, 18, 7, 20, 8, 21, 10, 21, 12, 19, 14, 17, 14, // %
	-23, -19, 23, 20, 22, 21, 21, 21, 20, 20, 19, 18, 17, 13, 15, 10, 13, 8, 11, 7, 7, 7, 5, 8, 4, 9, 3, 11, 3, 13, 4, 15, 5, 16, 12, 20, 13, 21, 14, 23, 14, 25, 13, 27, 11, 28, 9, 27, 8, 25, 8, 23, 9, 20, 11, 17, 16, 10, 18, 8, 20, 7, 22, 7, 23, 8, 23, 9, // &
	-5, -26, 4, 27, 5, 28, 6, 27, 6, 25, 5, 23, 4, 22, // '
	-11, -32, 9, 30, 7, 27, 5, 23, 4, 18, 4, 14, 5, 9, 7, 5, 9, 2, 11, 0, // (
	-3, -32, 5, 30, 7, 27, 9, 23, 10, 18, 10, 14, 9, 9, 7, 5, 5, 2, 3, 0, // )
	-8, -28, 8, 16, -3, -25, 13, 19, -13, -25, 3, 19, // *
	-13, -25, 13, 7, -4, -16, 22, 16, // +
	-6, -8, 5, 7, 4, 8, 5, 9, 6, 8, 6, 6, 5, 4, 4, 3, // ,
	-4, -16, 22, 16, // -
	-5, -9, 4, 8, 5, 7, 6, 8, 5, 9, // .
	-20, -32, 2, 0, // /
	-9, -28, 6, 27, 4, 24, 3, 19, 3, 16, 4, 11, 6, 8, 9, 7, 11, 7, 14, 8, 16, 11, 17, 16, 17, 19, 16, 24, 14, 27, 11, 28, 9, 28, // 0
	-6, -24, 8, 25, 11, 28, 11, 7, // 1
	-4, -23, 4, 24, 5, 26, 6, 27, 8, 28, 12, 28, 14, 27, 15, 26, 16, 24, 16, 22, 15, 20, 13, 17, 3, 7, 17, 7, // 2
	-5, -28, 16, 28, 10, 20, 13, 20, 15, 19, 16, 18, 17, 15, 17, 13, 16, 10, 14, 8, 11, 7, 8, 7, 5, 8, 4, 9, 3, 11, // 3
	-13, -28, 3, 14, 18, 14, -13, -28, 13, 7, // 4
	-15, -28, 5, 28, 4, 19, 5, 20, 8, 21, 11, 21, 14, 20, 16, 18, 17, 15, 17, 13, 16, 10, 14, 8, 11, 7, 8, 7, 5, 8, 4, 9, 3, 11, // 5
	-16, -25, 15, 27, 12, 28, 10, 28, 7, 27, 5, 24, 4, 19, 4, 14, 5, 10, 7, 8, 10, 7, 11, 7, 14, 8, 16, 10, 17, 13, 17, 14, 16, 17, 14, 19, 11, 20, 10, 20, 7, 19, 5, 17, 4, 14, // 6
	-17, -28, 7, 7, -3, -28, 17, 28, // 7
	-8, -28, 5, 27, 4, 25, 4, 23, 5, 21, 7, 20, 11, 19, 14, 18, 16, 16, 17, 14, 17, 11, 16, 9, 15, 8, 12, 7, 8, 7, 5, 8, 4, 9, 3, 11, 3, 14, 4, 16, 6, 18, 9, 19, 13, 20, 15, 21, 16, 23, 16, 25, 15, 27, 12, 28, 8, 28, // 8
	-16, -21, 15, 18, 13, 16, 10, 15, 9, 15, 6, 16, 4, 18, 3, 21, 3, 22, 4, 25, 6, 27, 9, 28, 10, 28, 13, 27, 15, 25, 16, 21, 16, 16, 15, 11, 13, 8, 10, 7, 8, 7, 5, 8, 4, 10, // 9
	-5, -21, 4, 20, 5, 19, 6, 20, 5, 21, -5, -9, 4, 8, 5, 7, 6, 8, 5, 9, // :
	-5, -21, 4, 20, 5, 19, 6, 20, 5, 21, -6, -8, 5, 7, 4, 8, 5, 9, 6, 8, 6, 6, 5, 4, 4, 3, // ;
	-20, -25, 4, 16, 20, 7, // <
	-4, -19, 22, 19, -4, -13, 22, 13, // =
	-4, -25, 20, 16, 4, 7, // >
	-3, -23, 3, 24, 4, 26, 5, 27, 7, 28, 11, 28, 13, 27, 14, 26, 15, 24, 15, 22, 14, 20, 13, 19, 9, 17, 9, 14, -9, -9, 8, 8, 9, 7, 10, 8, 9, 9, // ?
	-18, -20, 17, 22, 15, 23, 12, 23, 10, 22, 9, 21, 8, 18, 8, 15, 9, 13, 11, 12, 14, 12, 16, 13, 17, 15, -12, -23, 10, 21, 9, 18, 9, 15, 10, 13, 11, 12, -18, -23, 17, 15, 17, 13, 19, 12, 21, 12, 23, 14, 24, 17, 24, 19, 23, 22, 22, 24, 20, 26, 18, 27, 15, 28, 12, 28, 9, 27, 7, 26, 5, 24, 4, 22, 3, 19, 3, 16, 4, 13, 5, 11, 7, 9, 9, 8, 12, 7, 15, 7, 18, 8, 20, 9, 21, 10, -19, -23, 18, 15, 18, 13, 19, 12, // @
	-9, -28, 1, 7, -9, -28, 17, 7, -4, -14, 14, 14, // A
	-4, -28, 4, 7, -4, -28, 13, 28, 16, 27, 17, 26, 18, 24, 18, 22, 17, 20, 16, 19, 13, 18, -4, -18, 13, 18, 16, 17, 17, 16, 18, 14, 18, 11, 17, 9, 16, 8, 13, 7, 4, 7, // B
	-18, -23, 17, 25, 15, 27, 13, 28, 9, 28, 7, 27, 5, 25, 4, 23, 3, 20, 3, 15, 4, 12, 5, 10, 7, 8, 9, 7, 13, 7, 15, 8, 17, 10, 18, 12, // C
	-4, -28, 4, 7, -4, -28, 11, 28, 14, 27, 16, 25, 17, 23, 18, 20, 18, 15, 17, 12, 16, 10, 14, 8, 11, 7, 4, 7, // D
	-4, -28, 4, 7, -4, -28, 17, 28, -4, -18, 12, 18, -4, -7, 17, 7, // E
	-4, -28, 4, 7, -4, -28, 17, 28, -4, -18, 12, 18, // F
	-18, -23, 17, 25, 15, 27, 13, 28, 9, 28, 7, 27, 5, 25, 4, 23, 3, 20, 3, 15, 4, 12, 5, 10, 7, 8, 9, 7, 13, 7, 15, 8, 17, 10, 18, 12, 18, 15, -13, -15, 18, 15, // G
	-4, -28, 4, 7, -18, -28, 18, 7, -4, -18, 18, 18, // H
	-4, -28, 4, 7, // I
	-12, -28, 12, 12, 11, 9, 10, 8, 8, 7, 6, 7, 4, 8, 3, 9, 2, 12, 2, 14, // J
	-4, -28, 4, 7, -18, -28, 4, 14, -9, -19, 18, 7, // K
	-4, -28, 4, 7, -4, -7, 16, 7, // L
	-4, -28, 4, 7, -4, -28, 12, 7, -20, -28, 12, 7, -20, -28, 20, 7, // M
	-4, -28, 4, 7, -4, -28, 18, 7, -18, -28, 18, 7, // N
	-9, -28, 7, 27, 5, 25, 4, 23, 3, 20, 3, 15, 4, 12, 5, 10, 7, 8, 9, 7, 13, 7, 15, 8, 17, 10, 18, 12, 19, 15, 19, 20, 18, 23, 17, 25, 15, 27, 13, 28, 9, 28, // O
	-4, -28, 4, 7, -4, -28, 13, 28, 16, 27, 17, 26, 18, 24, 18, 21, 17, 19, 16, 18, 13, 17, 4, 17, // P
	-9, -28, 7, 27, 5, 25, 4, 23, 3, 20, 3, 15, 4, 12, 5, 10, 7, 8, 9, 7, 13, 7, 15, 8, 17, 10, 18, 12, 19, 15, 19, 20, 18, 23, 17, 25, 15, 27, 13, 28, 9, 28, -12, -11, 18, 5, // Q
	-4, -28, 4, 7, -4, -28, 13, 28, 16, 27, 17, 26, 18, 24, 18, 22, 17, 20, 16, 19, 13, 18, 4, 18, -11, -18, 18, 7, // R
	-17, -25, 15, 27, 12, 28, 8, 28, 5, 27, 3, 25, 3, 23, 4, 21, 5, 20, 7, 19, 13, 17, 15, 16, 16, 15, 17, 13, 17, 10, 15, 8, 12, 7, 8, 7, 5, 8, 3, 10, // S
	-8, -28, 8, 7, -1, -28, 15, 28, // T
	-4, -28, 4, 13, 5, 10, 7, 8, 10, 7, 12, 7, 15, 8, 17, 10, 18, 13, 18, 28, // U
	-1, -28, 9, 7, -17, -28, 9, 7, // V
	-2, -28, 7, 7, -12, -28, 7, 7, -12, -28, 17, 7, -22, -28, 17, 7, // W
	-3, -28, 17, 7, -17, -28, 3, 7, // X
	-1, -28, 9, 18, 9, 7, -17, -28, 9, 18, // Y
	-17, -28, 3, 7, -3, -28, 17, 28, -3, -7, 17, 7, // Z
	-4, -32, 4, 0, -5, -32, 5, 0, -4, -32, 11, 32, -4, 0, 11, 0, // [
	0, -28, 14, 4, // \ (line can't end with \)
	-9, -32, 9, 0, -10, -32, 10, 0, -3, -32, 10, 32, -3, 0, 10, 0, // ]
	-6, -22, 8, 25, 10, 22, -3, -19, 8, 24, 13, 19, -8, -24, 8, 7, // ^
	0, -5, 16, 5, // _
	-6, -28, 5, 27, 4, 25, 4, 23, 5, 22, 6, 23, 5, 24, // `
	-15, -21, 15, 7, -15, -18, 13, 20, 11, 21, 8, 21, 6, 20, 4, 18, 3, 15, 3, 13, 4, 10, 6, 8, 8, 7, 11, 7, 13, 8, 15, 10, // a
	-4, -28, 4, 7, -4, -18, 6, 20, 8, 21, 11, 21, 13, 20, 15, 18, 16, 15, 16, 13, 15, 10, 13, 8, 11, 7, 8, 7, 6, 8, 4, 10, // b
	-15, -18, 13, 20, 11, 21, 8, 21, 6, 20, 4, 18, 3, 15, 3, 13, 4, 10, 6, 8, 8, 7, 11, 7, 13, 8, 15, 10, // c
	-15, -28, 15, 7, -15, -18, 13, 20, 11, 21, 8, 21, 6, 20, 4, 18, 3, 15, 3, 13, 4, 10, 6, 8, 8, 7, 11, 7, 13, 8, 15, 10, // d
	-3, -15, 15, 15, 15, 17, 14, 19, 13, 20, 11, 21, 8, 21, 6, 20, 4, 18, 3, 15, 3, 13, 4, 10, 6, 8, 8, 7, 11, 7, 13, 8, 15, 10, // e
	-10, -28, 8, 28, 6, 27, 5, 24, 5, 7, -2, -21, 9, 21, // f
	-15, -21, 15, 5, 14, 2, 13, 1, 11, 0, 8, 0, 6, 1, -15, -18, 13, 20, 11, 21, 8, 21, 6, 20, 4, 18, 3, 15, 3, 13, 4, 10, 6, 8, 8, 7, 11, 7, 13, 8, 15, 10, // g
	-4, -28, 4, 7, -4, -17, 7, 20, 9, 21, 12, 21, 14, 20, 15, 17, 15, 7, // h
	-3, -28, 4, 27, 5, 28, 4, 29, 3, 28, -4, -21, 4, 7, // i
	-5, -28, 6, 27, 7, 28, 6, 29, 5, 28, -6, -21, 6, 4, 5, 1, 3, 0, 1, 0, // j
	-4, -28, 4, 7, -14, -21, 4, 11, -8, -15, 15, 7, // k
	-4, -28, 4, 7, // l
	-4, -21, 4, 7, -4, -17, 7, 20, 9, 21, 12, 21, 14, 20, 15, 17, 15, 7, -15, -17, 18, 20, 20, 21, 23, 21, 25, 20, 26, 17, 26, 7, // m
	-4, -21, 4, 7, -4, -17, 7, 20, 9, 21, 12, 21, 14, 20, 15, 17, 15, 7, // n
	-8, -21, 6, 20, 4, 18, 3, 15, 3, 13, 4, 10, 6, 8, 8, 7, 11, 7, 13, 8, 15, 10, 16, 13, 16, 15, 15, 18, 13, 20, 11, 21, 8, 21, // o
	-4, -21, 4, 0, -4, -18, 6, 20, 8, 21, 11, 21, 13, 20, 15, 18, 16, 15, 16, 13, 15, 10, 13, 8, 11, 7, 8, 7, 6, 8, 4, 10, // p
	-15, -21, 15, 0, -15, -18, 13, 20, 11, 21, 8, 21, 6, 20, 4, 18, 3, 15, 3, 13, 4, 10, 6, 8, 8, 7, 11, 7, 13, 8, 15, 10, // q
	-4, -21, 4, 7, -4, -15, 5, 18, 7, 20, 9, 21, 12, 21, // r
	-14, -18, 13, 20, 10, 21, 7, 21, 4, 20, 3, 18, 4, 16, 6, 15, 11, 14, 13, 13, 14, 11, 14, 10, 13, 8, 10, 7, 7, 7, 4, 8, 3, 10, // s
	-5, -28, 5, 11, 6, 8, 8, 7, 10, 7, -2, -21, 9, 21, // t
	-4, -21, 4, 11, 5, 8, 7, 7, 10, 7, 12, 8, 15, 11, -15, -21, 15, 7, // u
	-2, -21, 8, 7, -14, -21, 8, 7, // v
	-3, -21, 7, 7, -11, -21, 7, 7, -11, -21, 15, 7, -19, -21, 15, 7, // w
	-3, -21, 14, 7, -14, -21, 3, 7, // x
	-2, -21, 8, 7, -14, -21, 8, 7, 6, 3, 4, 1, 2, 0, 1, 0, // y
	-14, -21, 3, 7, -3, -21, 14, 21, -3, -7, 14, 7, // z
	-9, -32, 7, 31, 6, 30, 5, 28, 5, 26, 6, 24, 7, 23, 8, 21, 8, 19, 6, 17, -7, -31, 6, 29, 6, 27, 7, 25, 8, 24, 9, 22, 9, 20, 8, 18, 4, 16, 8, 14, 9, 12, 9, 10, 8, 8, 7, 7, 6, 5, 6, 3, 7, 1, -6, -15, 8, 13, 8, 11, 7, 9, 6, 8, 5, 6, 5, 4, 6, 2, 7, 1, 9, 0, // {
	-4, -32, 4, 0, // |
	-5, -32, 7, 31, 8, 30, 9, 28, 9, 26, 8, 24, 7, 23, 6, 21, 6, 19, 8, 17, -7, -31, 8, 29, 8, 27, 7, 25, 6, 24, 5, 22, 5, 20, 6, 18, 10, 16, 6, 14, 5, 12, 5, 10, 6, 8, 7, 7, 8, 5, 8, 3, 7, 1, -8, -15, 6, 13, 6, 11, 7, 9, 8, 8, 9, 6, 9, 4, 8, 2, 7, 1, 5, 0, // }
	-3, -13, 3, 15, 4, 18, 6, 19, 8, 19, 10, 18, 14, 15, 16, 14, 18, 14, 20, 15, 21, 17, -3, -15, 4, 17, 6, 18, 8, 18, 10, 17, 14, 14, 16, 13, 18, 13, 20, 14, 21, 17, 21, 19, // ~
};
