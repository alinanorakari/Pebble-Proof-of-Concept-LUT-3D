#include <pebble.h>
// 435 bytes 3480 bits
// black and white image data wrapped in bitmaps
static uint8_t donutspecular[] = {
0b00000000,0b00000000,0b00111111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,
0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011111,0b11111111,0b11111111,0b11110000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11111111,0b11111111,0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00111111,0b11111111,0b11111111,0b11111111,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011111,0b11111111,
0b11111111,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111111,0b11111111,0b11111111,0b10000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b11111000,0b00111100,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b11111111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,
0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111100,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000001,0b11111111,0b11000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,
0b11111111,0b11110000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b11111100,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000001,0b11111111,0b11111111,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,
0b11111111,0b11111111,0b11000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b11111111,0b11100000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111111,0b11111111,0b11110000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00001111,0b11111111,0b11111111,0b11110000,0b00000000,0b00000000,0b00000000,0b00011111,0b11111111,0b11111111,
0b11111111,0b11111111,0b11111000,0b00000000,0b00000000,0b00011111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111000,
0b00000000,0b00000000,0b01111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111000,0b00000000,0b00000000,0b01111111,
0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11110000,0b00000000,0b00000000,0b00011111,0b11111111,0b11111111,0b11111111,
0b11111111,0b11111111,0b11100000,0b00000000,0b00000000,0b00000011,0b11111111,0b11111111,0b11111111,0b11111110,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b01111111,0b11111111,0b11111111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000111,0b11111111,0b11111111,0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11111111,
0b11110000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111110,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,
};
// 1083 bytes 8664 bits
// black and white image data wrapped in bitmaps
static uint8_t donutshadow[] = {
0b00000000,0b00000100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000010,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b10000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b01000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000100,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000010,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00010000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00001000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000010,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b10000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b11111111,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b01100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b11111111,0b11111111,0b11111110,
0b00000000,0b00000000,0b00000000,0b00000000,0b00011000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,
0b11111111,0b11111111,0b11111111,0b00000000,0b00000000,0b00000000,0b00000000,0b00001110,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000111,0b11111111,0b11111111,0b11111111,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000011,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111111,0b11111111,0b11111111,0b11111111,0b10000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b11000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111111,0b11111111,0b11111111,0b11111111,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111111,
0b11111111,0b11111111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001110,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000001,0b11111111,0b11111111,0b11111111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,
0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b11111111,0b11000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000001,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011110,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000111,0b11000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b11110000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111110,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b10000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000011,0b11110000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111111,0b10000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,
0b11110000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00001111,0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b11110000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b01111111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00011111,0b11111111,0b11000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111111,0b11111000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000110,0b11111111,0b11111111,
0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000011,0b11111111,0b11111111,0b11110000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b11111111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111011,0b11111111,0b11111111,0b11100000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111100,0b01111111,
0b11111111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b11111111,0b10111111,0b11111111,0b11111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b10000111,0b11111111,0b11111111,0b11111110,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b11100000,0b11111111,0b11111111,0b11111111,
0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b11111000,
0b00111111,0b11111111,0b11111111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00001111,0b11111111,0b11111000,0b00000111,0b11111111,0b11111111,0b11111111,0b11110000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00011111,0b11111111,0b11111100,0b00000001,0b11111111,0b11111111,0b11111111,0b11111111,0b11000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111111,0b11111111,0b11111110,0b00000000,0b00111111,0b11111111,
0b11111111,0b11111111,0b11111111,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b11111111,0b11111111,
0b10000000,0b00000111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b00000000,0b00000000,0b00000000,0b00000000,0b01111111,
0b11111111,0b11111111,0b11111111,0b11000000,0b00000001,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11000000,0b00000000,0b00011111,0b11111111,0b11111111,0b11111111,
0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11100000,0b00000000,0b00000111,
0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
0b11110000,0b00000000,0b00000000,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
0b11111111,0b11111111,0b11111111,0b11111100,0b00000000,0b00000000,0b00001111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111100,0b00000000,0b00000000,0b00000001,0b11111111,0b11111111,
0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111100,0b00000000,0b00000000,
0b00000000,0b00111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
0b11111100,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
0b11111111,0b11111111,0b11111111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11111111,0b11111111,0b11111111,
0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,
0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111110,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
0b11111111,0b11111111,0b11111111,0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011111,0b11111111,
0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000001,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11100000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111111,0b11111111,
0b11111111,0b11111111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00101111,0b11111111,0b11111111,0b11111111,0b11000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111010,0b11101111,0b01111100,0b00000000,0b00000000,0b00000000,0b00000000,
0b00000000,0b00000000,0b00000000,
};