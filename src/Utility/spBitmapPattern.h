#pragma once

#include <inttypes.h>
#include <binary.h>

namespace StarterPack {

namespace BitmapPattern {

//
// 3 BITS
//
static constexpr uint8_t ___ = B000;
static constexpr uint8_t __O = B001;
static constexpr uint8_t _O_ = B010;
static constexpr uint8_t _OO = B011;
static constexpr uint8_t O__ = B100;
static constexpr uint8_t O_O = B101;
static constexpr uint8_t OO_ = B110;
static constexpr uint8_t OOO = B111;

//
// 4 BITS
//
static constexpr uint8_t ____ = B0000;
static constexpr uint8_t ___O = B0001;
static constexpr uint8_t __O_ = B0010;
static constexpr uint8_t __OO = B0011;
static constexpr uint8_t _O__ = B0100;
static constexpr uint8_t _O_O = B0101;
static constexpr uint8_t _OO_ = B0110;
static constexpr uint8_t _OOO = B0111;
static constexpr uint8_t O___ = B1000;
static constexpr uint8_t O__O = B1001;
static constexpr uint8_t O_O_ = B1010;
static constexpr uint8_t O_OO = B1011;
static constexpr uint8_t OO__ = B1100;
static constexpr uint8_t OO_O = B1101;
static constexpr uint8_t OOO_ = B1110;
static constexpr uint8_t OOOO = B1111;

//
// 5 BITS
//
static constexpr uint8_t _____ = B00000;
static constexpr uint8_t ____O = B00001;
static constexpr uint8_t ___O_ = B00010;
static constexpr uint8_t ___OO = B00011;
static constexpr uint8_t __O__ = B00100;
static constexpr uint8_t __O_O = B00101;
static constexpr uint8_t __OO_ = B00110;
static constexpr uint8_t __OOO = B00111;
static constexpr uint8_t _O___ = B01000;
static constexpr uint8_t _O__O = B01001;
static constexpr uint8_t _O_O_ = B01010;
static constexpr uint8_t _O_OO = B01011;
static constexpr uint8_t _OO__ = B01100;
static constexpr uint8_t _OO_O = B01101;
static constexpr uint8_t _OOO_ = B01110;
static constexpr uint8_t _OOOO = B01111;
static constexpr uint8_t O____ = B10000;
static constexpr uint8_t O___O = B10001;
static constexpr uint8_t O__O_ = B10010;
static constexpr uint8_t O__OO = B10011;
static constexpr uint8_t O_O__ = B10100;
static constexpr uint8_t O_O_O = B10101;
static constexpr uint8_t O_OO_ = B10110;
static constexpr uint8_t O_OOO = B10111;
static constexpr uint8_t OO___ = B11000;
static constexpr uint8_t OO__O = B11001;
static constexpr uint8_t OO_O_ = B11010;
static constexpr uint8_t OO_OO = B11011;
static constexpr uint8_t OOO__ = B11100;
static constexpr uint8_t OOO_O = B11101;
static constexpr uint8_t OOOO_ = B11110;
static constexpr uint8_t OOOOO = B11111;

//
// 6 BITS
//
static constexpr uint8_t ______ = B000000;
static constexpr uint8_t _____O = B000001;
static constexpr uint8_t ____O_ = B000010;
static constexpr uint8_t ____OO = B000011;
static constexpr uint8_t ___O__ = B000100;
static constexpr uint8_t ___O_O = B000101;
static constexpr uint8_t ___OO_ = B000110;
static constexpr uint8_t ___OOO = B000111;
static constexpr uint8_t __O___ = B001000;
static constexpr uint8_t __O__O = B001001;
static constexpr uint8_t __O_O_ = B001010;
static constexpr uint8_t __O_OO = B001011;
static constexpr uint8_t __OO__ = B001100;
static constexpr uint8_t __OO_O = B001101;
static constexpr uint8_t __OOO_ = B001110;
static constexpr uint8_t __OOOO = B001111;
static constexpr uint8_t _O____ = B010000;
static constexpr uint8_t _O___O = B010001;
static constexpr uint8_t _O__O_ = B010010;
static constexpr uint8_t _O__OO = B010011;
static constexpr uint8_t _O_O__ = B010100;
static constexpr uint8_t _O_O_O = B010101;
static constexpr uint8_t _O_OO_ = B010110;
static constexpr uint8_t _O_OOO = B010111;
static constexpr uint8_t _OO___ = B011000;
static constexpr uint8_t _OO__O = B011001;
static constexpr uint8_t _OO_O_ = B011010;
static constexpr uint8_t _OO_OO = B011011;
static constexpr uint8_t _OOO__ = B011100;
static constexpr uint8_t _OOO_O = B011101;
static constexpr uint8_t _OOOO_ = B011110;
static constexpr uint8_t _OOOOO = B011111;
static constexpr uint8_t O_____ = B100000;
static constexpr uint8_t O____O = B100001;
static constexpr uint8_t O___O_ = B100010;
static constexpr uint8_t O___OO = B100011;
static constexpr uint8_t O__O__ = B100100;
static constexpr uint8_t O__O_O = B100101;
static constexpr uint8_t O__OO_ = B100110;
static constexpr uint8_t O__OOO = B100111;
static constexpr uint8_t O_O___ = B101000;
static constexpr uint8_t O_O__O = B101001;
static constexpr uint8_t O_O_O_ = B101010;
static constexpr uint8_t O_O_OO = B101011;
static constexpr uint8_t O_OO__ = B101100;
static constexpr uint8_t O_OO_O = B101101;
static constexpr uint8_t O_OOO_ = B101110;
static constexpr uint8_t O_OOOO = B101111;
static constexpr uint8_t OO____ = B110000;
static constexpr uint8_t OO___O = B110001;
static constexpr uint8_t OO__O_ = B110010;
static constexpr uint8_t OO__OO = B110011;
static constexpr uint8_t OO_O__ = B110100;
static constexpr uint8_t OO_O_O = B110101;
static constexpr uint8_t OO_OO_ = B110110;
static constexpr uint8_t OO_OOO = B110111;
static constexpr uint8_t OOO___ = B111000;
static constexpr uint8_t OOO__O = B111001;
static constexpr uint8_t OOO_O_ = B111010;
static constexpr uint8_t OOO_OO = B111011;
static constexpr uint8_t OOOO__ = B111100;
static constexpr uint8_t OOOO_O = B111101;
static constexpr uint8_t OOOOO_ = B111110;
static constexpr uint8_t OOOOOO = B111111;

//
// 7 BITS
//
static constexpr uint8_t _______ = B0000000;
static constexpr uint8_t ______O = B0000001;
static constexpr uint8_t _____O_ = B0000010;
static constexpr uint8_t _____OO = B0000011;
static constexpr uint8_t ____O__ = B0000100;
static constexpr uint8_t ____O_O = B0000101;
static constexpr uint8_t ____OO_ = B0000110;
static constexpr uint8_t ____OOO = B0000111;
static constexpr uint8_t ___O___ = B0001000;
static constexpr uint8_t ___O__O = B0001001;
static constexpr uint8_t ___O_O_ = B0001010;
static constexpr uint8_t ___O_OO = B0001011;
static constexpr uint8_t ___OO__ = B0001100;
static constexpr uint8_t ___OO_O = B0001101;
static constexpr uint8_t ___OOO_ = B0001110;
static constexpr uint8_t ___OOOO = B0001111;
static constexpr uint8_t __O____ = B0010000;
static constexpr uint8_t __O___O = B0010001;
static constexpr uint8_t __O__O_ = B0010010;
static constexpr uint8_t __O__OO = B0010011;
static constexpr uint8_t __O_O__ = B0010100;
static constexpr uint8_t __O_O_O = B0010101;
static constexpr uint8_t __O_OO_ = B0010110;
static constexpr uint8_t __O_OOO = B0010111;
static constexpr uint8_t __OO___ = B0011000;
static constexpr uint8_t __OO__O = B0011001;
static constexpr uint8_t __OO_O_ = B0011010;
static constexpr uint8_t __OO_OO = B0011011;
static constexpr uint8_t __OOO__ = B0011100;
static constexpr uint8_t __OOO_O = B0011101;
static constexpr uint8_t __OOOO_ = B0011110;
static constexpr uint8_t __OOOOO = B0011111;
static constexpr uint8_t _O_____ = B0100000;
static constexpr uint8_t _O____O = B0100001;
static constexpr uint8_t _O___O_ = B0100010;
static constexpr uint8_t _O___OO = B0100011;
static constexpr uint8_t _O__O__ = B0100100;
static constexpr uint8_t _O__O_O = B0100101;
static constexpr uint8_t _O__OO_ = B0100110;
static constexpr uint8_t _O__OOO = B0100111;
static constexpr uint8_t _O_O___ = B0101000;
static constexpr uint8_t _O_O__O = B0101001;
static constexpr uint8_t _O_O_O_ = B0101010;
static constexpr uint8_t _O_O_OO = B0101011;
static constexpr uint8_t _O_OO__ = B0101100;
static constexpr uint8_t _O_OO_O = B0101101;
static constexpr uint8_t _O_OOO_ = B0101110;
static constexpr uint8_t _O_OOOO = B0101111;
static constexpr uint8_t _OO____ = B0110000;
static constexpr uint8_t _OO___O = B0110001;
static constexpr uint8_t _OO__O_ = B0110010;
static constexpr uint8_t _OO__OO = B0110011;
static constexpr uint8_t _OO_O__ = B0110100;
static constexpr uint8_t _OO_O_O = B0110101;
static constexpr uint8_t _OO_OO_ = B0110110;
static constexpr uint8_t _OO_OOO = B0110111;
static constexpr uint8_t _OOO___ = B0111000;
static constexpr uint8_t _OOO__O = B0111001;
static constexpr uint8_t _OOO_O_ = B0111010;
static constexpr uint8_t _OOO_OO = B0111011;
static constexpr uint8_t _OOOO__ = B0111100;
static constexpr uint8_t _OOOO_O = B0111101;
static constexpr uint8_t _OOOOO_ = B0111110;
static constexpr uint8_t _OOOOOO = B0111111;
static constexpr uint8_t O______ = B1000000;
static constexpr uint8_t O_____O = B1000001;
static constexpr uint8_t O____O_ = B1000010;
static constexpr uint8_t O____OO = B1000011;
static constexpr uint8_t O___O__ = B1000100;
static constexpr uint8_t O___O_O = B1000101;
static constexpr uint8_t O___OO_ = B1000110;
static constexpr uint8_t O___OOO = B1000111;
static constexpr uint8_t O__O___ = B1001000;
static constexpr uint8_t O__O__O = B1001001;
static constexpr uint8_t O__O_O_ = B1001010;
static constexpr uint8_t O__O_OO = B1001011;
static constexpr uint8_t O__OO__ = B1001100;
static constexpr uint8_t O__OO_O = B1001101;
static constexpr uint8_t O__OOO_ = B1001110;
static constexpr uint8_t O__OOOO = B1001111;
static constexpr uint8_t O_O____ = B1010000;
static constexpr uint8_t O_O___O = B1010001;
static constexpr uint8_t O_O__O_ = B1010010;
static constexpr uint8_t O_O__OO = B1010011;
static constexpr uint8_t O_O_O__ = B1010100;
static constexpr uint8_t O_O_O_O = B1010101;
static constexpr uint8_t O_O_OO_ = B1010110;
static constexpr uint8_t O_O_OOO = B1010111;
static constexpr uint8_t O_OO___ = B1011000;
static constexpr uint8_t O_OO__O = B1011001;
static constexpr uint8_t O_OO_O_ = B1011010;
static constexpr uint8_t O_OO_OO = B1011011;
static constexpr uint8_t O_OOO__ = B1011100;
static constexpr uint8_t O_OOO_O = B1011101;
static constexpr uint8_t O_OOOO_ = B1011110;
static constexpr uint8_t O_OOOOO = B1011111;
static constexpr uint8_t OO_____ = B1100000;
static constexpr uint8_t OO____O = B1100001;
static constexpr uint8_t OO___O_ = B1100010;
static constexpr uint8_t OO___OO = B1100011;
static constexpr uint8_t OO__O__ = B1100100;
static constexpr uint8_t OO__O_O = B1100101;
static constexpr uint8_t OO__OO_ = B1100110;
static constexpr uint8_t OO__OOO = B1100111;
static constexpr uint8_t OO_O___ = B1101000;
static constexpr uint8_t OO_O__O = B1101001;
static constexpr uint8_t OO_O_O_ = B1101010;
static constexpr uint8_t OO_O_OO = B1101011;
static constexpr uint8_t OO_OO__ = B1101100;
static constexpr uint8_t OO_OO_O = B1101101;
static constexpr uint8_t OO_OOO_ = B1101110;
static constexpr uint8_t OO_OOOO = B1101111;
static constexpr uint8_t OOO____ = B1110000;
static constexpr uint8_t OOO___O = B1110001;
static constexpr uint8_t OOO__O_ = B1110010;
static constexpr uint8_t OOO__OO = B1110011;
static constexpr uint8_t OOO_O__ = B1110100;
static constexpr uint8_t OOO_O_O = B1110101;
static constexpr uint8_t OOO_OO_ = B1110110;
static constexpr uint8_t OOO_OOO = B1110111;
static constexpr uint8_t OOOO___ = B1111000;
static constexpr uint8_t OOOO__O = B1111001;
static constexpr uint8_t OOOO_O_ = B1111010;
static constexpr uint8_t OOOO_OO = B1111011;
static constexpr uint8_t OOOOO__ = B1111100;
static constexpr uint8_t OOOOO_O = B1111101;
static constexpr uint8_t OOOOOO_ = B1111110;
static constexpr uint8_t OOOOOOO = B1111111;

//
// 8 BITS
//
static constexpr uint8_t ________ = B00000000;
static constexpr uint8_t _______O = B00000001;
static constexpr uint8_t ______O_ = B00000010;
static constexpr uint8_t ______OO = B00000011;
static constexpr uint8_t _____O__ = B00000100;
static constexpr uint8_t _____O_O = B00000101;
static constexpr uint8_t _____OO_ = B00000110;
static constexpr uint8_t _____OOO = B00000111;
static constexpr uint8_t ____O___ = B00001000;
static constexpr uint8_t ____O__O = B00001001;
static constexpr uint8_t ____O_O_ = B00001010;
static constexpr uint8_t ____O_OO = B00001011;
static constexpr uint8_t ____OO__ = B00001100;
static constexpr uint8_t ____OO_O = B00001101;
static constexpr uint8_t ____OOO_ = B00001110;
static constexpr uint8_t ____OOOO = B00001111;
static constexpr uint8_t ___O____ = B00010000;
static constexpr uint8_t ___O___O = B00010001;
static constexpr uint8_t ___O__O_ = B00010010;
static constexpr uint8_t ___O__OO = B00010011;
static constexpr uint8_t ___O_O__ = B00010100;
static constexpr uint8_t ___O_O_O = B00010101;
static constexpr uint8_t ___O_OO_ = B00010110;
static constexpr uint8_t ___O_OOO = B00010111;
static constexpr uint8_t ___OO___ = B00011000;
static constexpr uint8_t ___OO__O = B00011001;
static constexpr uint8_t ___OO_O_ = B00011010;
static constexpr uint8_t ___OO_OO = B00011011;
static constexpr uint8_t ___OOO__ = B00011100;
static constexpr uint8_t ___OOO_O = B00011101;
static constexpr uint8_t ___OOOO_ = B00011110;
static constexpr uint8_t ___OOOOO = B00011111;
static constexpr uint8_t __O_____ = B00100000;
static constexpr uint8_t __O____O = B00100001;
static constexpr uint8_t __O___O_ = B00100010;
static constexpr uint8_t __O___OO = B00100011;
static constexpr uint8_t __O__O__ = B00100100;
static constexpr uint8_t __O__O_O = B00100101;
static constexpr uint8_t __O__OO_ = B00100110;
static constexpr uint8_t __O__OOO = B00100111;
static constexpr uint8_t __O_O___ = B00101000;
static constexpr uint8_t __O_O__O = B00101001;
static constexpr uint8_t __O_O_O_ = B00101010;
static constexpr uint8_t __O_O_OO = B00101011;
static constexpr uint8_t __O_OO__ = B00101100;
static constexpr uint8_t __O_OO_O = B00101101;
static constexpr uint8_t __O_OOO_ = B00101110;
static constexpr uint8_t __O_OOOO = B00101111;
static constexpr uint8_t __OO____ = B00110000;
static constexpr uint8_t __OO___O = B00110001;
static constexpr uint8_t __OO__O_ = B00110010;
static constexpr uint8_t __OO__OO = B00110011;
static constexpr uint8_t __OO_O__ = B00110100;
static constexpr uint8_t __OO_O_O = B00110101;
static constexpr uint8_t __OO_OO_ = B00110110;
static constexpr uint8_t __OO_OOO = B00110111;
static constexpr uint8_t __OOO___ = B00111000;
static constexpr uint8_t __OOO__O = B00111001;
static constexpr uint8_t __OOO_O_ = B00111010;
static constexpr uint8_t __OOO_OO = B00111011;
static constexpr uint8_t __OOOO__ = B00111100;
static constexpr uint8_t __OOOO_O = B00111101;
static constexpr uint8_t __OOOOO_ = B00111110;
static constexpr uint8_t __OOOOOO = B00111111;
static constexpr uint8_t _O______ = B01000000;
static constexpr uint8_t _O_____O = B01000001;
static constexpr uint8_t _O____O_ = B01000010;
static constexpr uint8_t _O____OO = B01000011;
static constexpr uint8_t _O___O__ = B01000100;
static constexpr uint8_t _O___O_O = B01000101;
static constexpr uint8_t _O___OO_ = B01000110;
static constexpr uint8_t _O___OOO = B01000111;
static constexpr uint8_t _O__O___ = B01001000;
static constexpr uint8_t _O__O__O = B01001001;
static constexpr uint8_t _O__O_O_ = B01001010;
static constexpr uint8_t _O__O_OO = B01001011;
static constexpr uint8_t _O__OO__ = B01001100;
static constexpr uint8_t _O__OO_O = B01001101;
static constexpr uint8_t _O__OOO_ = B01001110;
static constexpr uint8_t _O__OOOO = B01001111;
static constexpr uint8_t _O_O____ = B01010000;
static constexpr uint8_t _O_O___O = B01010001;
static constexpr uint8_t _O_O__O_ = B01010010;
static constexpr uint8_t _O_O__OO = B01010011;
static constexpr uint8_t _O_O_O__ = B01010100;
static constexpr uint8_t _O_O_O_O = B01010101;
static constexpr uint8_t _O_O_OO_ = B01010110;
static constexpr uint8_t _O_O_OOO = B01010111;
static constexpr uint8_t _O_OO___ = B01011000;
static constexpr uint8_t _O_OO__O = B01011001;
static constexpr uint8_t _O_OO_O_ = B01011010;
static constexpr uint8_t _O_OO_OO = B01011011;
static constexpr uint8_t _O_OOO__ = B01011100;
static constexpr uint8_t _O_OOO_O = B01011101;
static constexpr uint8_t _O_OOOO_ = B01011110;
static constexpr uint8_t _O_OOOOO = B01011111;
static constexpr uint8_t _OO_____ = B01100000;
static constexpr uint8_t _OO____O = B01100001;
static constexpr uint8_t _OO___O_ = B01100010;
static constexpr uint8_t _OO___OO = B01100011;
static constexpr uint8_t _OO__O__ = B01100100;
static constexpr uint8_t _OO__O_O = B01100101;
static constexpr uint8_t _OO__OO_ = B01100110;
static constexpr uint8_t _OO__OOO = B01100111;
static constexpr uint8_t _OO_O___ = B01101000;
static constexpr uint8_t _OO_O__O = B01101001;
static constexpr uint8_t _OO_O_O_ = B01101010;
static constexpr uint8_t _OO_O_OO = B01101011;
static constexpr uint8_t _OO_OO__ = B01101100;
static constexpr uint8_t _OO_OO_O = B01101101;
static constexpr uint8_t _OO_OOO_ = B01101110;
static constexpr uint8_t _OO_OOOO = B01101111;
static constexpr uint8_t _OOO____ = B01110000;
static constexpr uint8_t _OOO___O = B01110001;
static constexpr uint8_t _OOO__O_ = B01110010;
static constexpr uint8_t _OOO__OO = B01110011;
static constexpr uint8_t _OOO_O__ = B01110100;
static constexpr uint8_t _OOO_O_O = B01110101;
static constexpr uint8_t _OOO_OO_ = B01110110;
static constexpr uint8_t _OOO_OOO = B01110111;
static constexpr uint8_t _OOOO___ = B01111000;
static constexpr uint8_t _OOOO__O = B01111001;
static constexpr uint8_t _OOOO_O_ = B01111010;
static constexpr uint8_t _OOOO_OO = B01111011;
static constexpr uint8_t _OOOOO__ = B01111100;
static constexpr uint8_t _OOOOO_O = B01111101;
static constexpr uint8_t _OOOOOO_ = B01111110;
static constexpr uint8_t _OOOOOOO = B01111111;
static constexpr uint8_t O_______ = B10000000;
static constexpr uint8_t O______O = B10000001;
static constexpr uint8_t O_____O_ = B10000010;
static constexpr uint8_t O_____OO = B10000011;
static constexpr uint8_t O____O__ = B10000100;
static constexpr uint8_t O____O_O = B10000101;
static constexpr uint8_t O____OO_ = B10000110;
static constexpr uint8_t O____OOO = B10000111;
static constexpr uint8_t O___O___ = B10001000;
static constexpr uint8_t O___O__O = B10001001;
static constexpr uint8_t O___O_O_ = B10001010;
static constexpr uint8_t O___O_OO = B10001011;
static constexpr uint8_t O___OO__ = B10001100;
static constexpr uint8_t O___OO_O = B10001101;
static constexpr uint8_t O___OOO_ = B10001110;
static constexpr uint8_t O___OOOO = B10001111;
static constexpr uint8_t O__O____ = B10010000;
static constexpr uint8_t O__O___O = B10010001;
static constexpr uint8_t O__O__O_ = B10010010;
static constexpr uint8_t O__O__OO = B10010011;
static constexpr uint8_t O__O_O__ = B10010100;
static constexpr uint8_t O__O_O_O = B10010101;
static constexpr uint8_t O__O_OO_ = B10010110;
static constexpr uint8_t O__O_OOO = B10010111;
static constexpr uint8_t O__OO___ = B10011000;
static constexpr uint8_t O__OO__O = B10011001;
static constexpr uint8_t O__OO_O_ = B10011010;
static constexpr uint8_t O__OO_OO = B10011011;
static constexpr uint8_t O__OOO__ = B10011100;
static constexpr uint8_t O__OOO_O = B10011101;
static constexpr uint8_t O__OOOO_ = B10011110;
static constexpr uint8_t O__OOOOO = B10011111;
static constexpr uint8_t O_O_____ = B10100000;
static constexpr uint8_t O_O____O = B10100001;
static constexpr uint8_t O_O___O_ = B10100010;
static constexpr uint8_t O_O___OO = B10100011;
static constexpr uint8_t O_O__O__ = B10100100;
static constexpr uint8_t O_O__O_O = B10100101;
static constexpr uint8_t O_O__OO_ = B10100110;
static constexpr uint8_t O_O__OOO = B10100111;
static constexpr uint8_t O_O_O___ = B10101000;
static constexpr uint8_t O_O_O__O = B10101001;
static constexpr uint8_t O_O_O_O_ = B10101010;
static constexpr uint8_t O_O_O_OO = B10101011;
static constexpr uint8_t O_O_OO__ = B10101100;
static constexpr uint8_t O_O_OO_O = B10101101;
static constexpr uint8_t O_O_OOO_ = B10101110;
static constexpr uint8_t O_O_OOOO = B10101111;
static constexpr uint8_t O_OO____ = B10110000;
static constexpr uint8_t O_OO___O = B10110001;
static constexpr uint8_t O_OO__O_ = B10110010;
static constexpr uint8_t O_OO__OO = B10110011;
static constexpr uint8_t O_OO_O__ = B10110100;
static constexpr uint8_t O_OO_O_O = B10110101;
static constexpr uint8_t O_OO_OO_ = B10110110;
static constexpr uint8_t O_OO_OOO = B10110111;
static constexpr uint8_t O_OOO___ = B10111000;
static constexpr uint8_t O_OOO__O = B10111001;
static constexpr uint8_t O_OOO_O_ = B10111010;
static constexpr uint8_t O_OOO_OO = B10111011;
static constexpr uint8_t O_OOOO__ = B10111100;
static constexpr uint8_t O_OOOO_O = B10111101;
static constexpr uint8_t O_OOOOO_ = B10111110;
static constexpr uint8_t O_OOOOOO = B10111111;
static constexpr uint8_t OO______ = B11000000;
static constexpr uint8_t OO_____O = B11000001;
static constexpr uint8_t OO____O_ = B11000010;
static constexpr uint8_t OO____OO = B11000011;
static constexpr uint8_t OO___O__ = B11000100;
static constexpr uint8_t OO___O_O = B11000101;
static constexpr uint8_t OO___OO_ = B11000110;
static constexpr uint8_t OO___OOO = B11000111;
static constexpr uint8_t OO__O___ = B11001000;
static constexpr uint8_t OO__O__O = B11001001;
static constexpr uint8_t OO__O_O_ = B11001010;
static constexpr uint8_t OO__O_OO = B11001011;
static constexpr uint8_t OO__OO__ = B11001100;
static constexpr uint8_t OO__OO_O = B11001101;
static constexpr uint8_t OO__OOO_ = B11001110;
static constexpr uint8_t OO__OOOO = B11001111;
static constexpr uint8_t OO_O____ = B11010000;
static constexpr uint8_t OO_O___O = B11010001;
static constexpr uint8_t OO_O__O_ = B11010010;
static constexpr uint8_t OO_O__OO = B11010011;
static constexpr uint8_t OO_O_O__ = B11010100;
static constexpr uint8_t OO_O_O_O = B11010101;
static constexpr uint8_t OO_O_OO_ = B11010110;
static constexpr uint8_t OO_O_OOO = B11010111;
static constexpr uint8_t OO_OO___ = B11011000;
static constexpr uint8_t OO_OO__O = B11011001;
static constexpr uint8_t OO_OO_O_ = B11011010;
static constexpr uint8_t OO_OO_OO = B11011011;
static constexpr uint8_t OO_OOO__ = B11011100;
static constexpr uint8_t OO_OOO_O = B11011101;
static constexpr uint8_t OO_OOOO_ = B11011110;
static constexpr uint8_t OO_OOOOO = B11011111;
static constexpr uint8_t OOO_____ = B11100000;
static constexpr uint8_t OOO____O = B11100001;
static constexpr uint8_t OOO___O_ = B11100010;
static constexpr uint8_t OOO___OO = B11100011;
static constexpr uint8_t OOO__O__ = B11100100;
static constexpr uint8_t OOO__O_O = B11100101;
static constexpr uint8_t OOO__OO_ = B11100110;
static constexpr uint8_t OOO__OOO = B11100111;
static constexpr uint8_t OOO_O___ = B11101000;
static constexpr uint8_t OOO_O__O = B11101001;
static constexpr uint8_t OOO_O_O_ = B11101010;
static constexpr uint8_t OOO_O_OO = B11101011;
static constexpr uint8_t OOO_OO__ = B11101100;
static constexpr uint8_t OOO_OO_O = B11101101;
static constexpr uint8_t OOO_OOO_ = B11101110;
static constexpr uint8_t OOO_OOOO = B11101111;
static constexpr uint8_t OOOO____ = B11110000;
static constexpr uint8_t OOOO___O = B11110001;
static constexpr uint8_t OOOO__O_ = B11110010;
static constexpr uint8_t OOOO__OO = B11110011;
static constexpr uint8_t OOOO_O__ = B11110100;
static constexpr uint8_t OOOO_O_O = B11110101;
static constexpr uint8_t OOOO_OO_ = B11110110;
static constexpr uint8_t OOOO_OOO = B11110111;
static constexpr uint8_t OOOOO___ = B11111000;
static constexpr uint8_t OOOOO__O = B11111001;
static constexpr uint8_t OOOOO_O_ = B11111010;
static constexpr uint8_t OOOOO_OO = B11111011;
static constexpr uint8_t OOOOOO__ = B11111100;
static constexpr uint8_t OOOOOO_O = B11111101;
static constexpr uint8_t OOOOOOO_ = B11111110;
static constexpr uint8_t OOOOOOOO = B11111111;

}

}
