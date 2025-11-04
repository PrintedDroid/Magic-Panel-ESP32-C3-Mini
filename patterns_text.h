/*
 * Text Scrolling Patterns
 * Fonts and text scrolling functionality
 * VERSION 2.6 - Fixed buffer overflows, bounds checking, memory safety, and scroll completion
 */

#ifndef PATTERNS_TEXT_H
#define PATTERNS_TEXT_H

#include "led_control.h"
#include "patterns_shapes.h"

extern PanelState panelState;
extern bool patternActive;

namespace Text {
  
  // Scroll buffer for text animation - static allocation to avoid stack issues
  static unsigned long scrollBuffer[TEXT_SCROLL_BUFFER_SIZE] = {0};
  
  // Standard 5x7 Font definitions
  const unsigned char font5x7[] PROGMEM = {
    // Space (0x20)
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, 3,
    // ! (0x21)
    B01000000, B01000000, B01000000, B01000000, B01000000, B00000000, B01000000, 2,
    // " (0x22)
    B10100000, B10100000, B10100000, B00000000, B00000000, B00000000, B00000000, 4,
    // # (0x23)
    B01010000, B01010000, B11111000, B01010000, B11111000, B01010000, B01010000, 6,
    // $ (0x24)
    B00100000, B01111000, B10100000, B01110000, B00101000, B11110000, B00100000, 6,
    // % (0x25)
    B11000000, B11001000, B00010000, B00100000, B01000000, B10011000, B00011000, 6,
    // & (0x26)
    B01100000, B10010000, B10100000, B01000000, B10101000, B10010000, B01101000, 6,
    // ' (0x27)
    B11000000, B01000000, B10000000, B00000000, B00000000, B00000000, B00000000, 3,
    // ( (0x28)
    B00100000, B01000000, B10000000, B10000000, B10000000, B01000000, B00100000, 4,
    // ) (0x29)
    B10000000, B01000000, B00100000, B00100000, B00100000, B01000000, B10000000, 4,
    // * (0x2A)
    B00000000, B00100000, B10101000, B01110000, B10101000, B00100000, B00000000, 6,
    // + (0x2B)
    B00000000, B00100000, B00100000, B11111000, B00100000, B00100000, B00000000, 6,
    // , (0x2C)
    B00000000, B00000000, B00000000, B00000000, B11000000, B01000000, B10000000, 3,
    // - (0x2D)
    B00000000, B00000000, B11111000, B00000000, B00000000, B00000000, B00000000, 6,
    // . (0x2E)
    B00000000, B00000000, B00000000, B00000000, B00000000, B11000000, B11000000, 3,
    // / (0x2F)
    B00000000, B00001000, B00010000, B00100000, B01000000, B10000000, B00000000, 6,
    // 0-9 (0x30-0x39)
    B01110000, B10001000, B10011000, B10101000, B11001000, B10001000, B01110000, 6,
    B01000000, B11000000, B01000000, B01000000, B01000000, B01000000, B11100000, 4,
    B01110000, B10001000, B00001000, B00010000, B00100000, B01000000, B11111000, 6,
    B11111000, B00010000, B00100000, B00010000, B00001000, B10001000, B01110000, 6,
    B00010000, B00110000, B01010000, B10010000, B11111000, B00010000, B00010000, 6,
    B11111000, B10000000, B11110000, B00001000, B00001000, B10001000, B01110000, 6,
    B00110000, B01000000, B10000000, B11110000, B10001000, B10001000, B01110000, 6,
    B11111000, B10001000, B00001000, B00010000, B00100000, B00100000, B00100000, 6,
    B01110000, B10001000, B10001000, B01110000, B10001000, B10001000, B01110000, 6,
    B01110000, B10001000, B10001000, B01111000, B00001000, B00010000, B01100000, 6,
    // : (0x3A)
    B00000000, B11000000, B11000000, B00000000, B11000000, B11000000, B00000000, 3,
    // ; (0x3B)
    B00000000, B11000000, B11000000, B00000000, B11000000, B01000000, B10000000, 3,
    // < (0x3C)
    B00010000, B00100000, B01000000, B10000000, B01000000, B00100000, B00010000, 5,
    // = (0x3D)
    B00000000, B00000000, B11111000, B00000000, B11111000, B00000000, B00000000, 6,
    // > (0x3E)
    B10000000, B01000000, B00100000, B00010000, B00100000, B01000000, B10000000, 5,
    // ? (0x3F)
    B01110000, B10001000, B00001000, B00010000, B00100000, B00000000, B00100000, 6,
    // @ (0x40)
    B01110000, B10001000, B00001000, B01101000, B10101000, B10101000, B01110000, 6,
    // A-Z (0x41-0x5A)
    B01110000, B10001000, B10001000, B10001000, B11111000, B10001000, B10001000, 6,
    B11110000, B10001000, B10001000, B11110000, B10001000, B10001000, B11110000, 6,
    B01110000, B10001000, B10000000, B10000000, B10000000, B10001000, B01110000, 6,
    B11100000, B10010000, B10001000, B10001000, B10001000, B10010000, B11100000, 6,
    B11111000, B10000000, B10000000, B11110000, B10000000, B10000000, B11111000, 6,
    B11111000, B10000000, B10000000, B11110000, B10000000, B10000000, B10000000, 6,
    B01110000, B10001000, B10000000, B10111000, B10001000, B10001000, B01111000, 6,
    B10001000, B10001000, B10001000, B11111000, B10001000, B10001000, B10001000, 6,
    B11100000, B01000000, B01000000, B01000000, B01000000, B01000000, B11100000, 4,
    B00111000, B00010000, B00010000, B00010000, B00010000, B10010000, B01100000, 6,
    B10001000, B10010000, B10100000, B11000000, B10100000, B10010000, B10001000, 6,
    B10000000, B10000000, B10000000, B10000000, B10000000, B10000000, B11111000, 6,
    B10001000, B11011000, B10101000, B10101000, B10001000, B10001000, B10001000, 6,
    B10001000, B10001000, B11001000, B10101000, B10011000, B10001000, B10001000, 6,
    B01110000, B10001000, B10001000, B10001000, B10001000, B10001000, B01110000, 6,
    B11110000, B10001000, B10001000, B11110000, B10000000, B10000000, B10000000, 6,
    B01110000, B10001000, B10001000, B10001000, B10101000, B10010000, B01101000, 6,
    B11110000, B10001000, B10001000, B11110000, B10100000, B10010000, B10001000, 6,
    B01111000, B10000000, B10000000, B01110000, B00001000, B00001000, B11110000, 6,
    B11111000, B00100000, B00100000, B00100000, B00100000, B00100000, B00100000, 6,
    B10001000, B10001000, B10001000, B10001000, B10001000, B10001000, B01110000, 6,
    B10001000, B10001000, B10001000, B10001000, B10001000, B01010000, B00100000, 6,
    B10001000, B10001000, B10001000, B10101000, B10101000, B10101000, B01010000, 6,
    B10001000, B10001000, B01010000, B00100000, B01010000, B10001000, B10001000, 6,
    B10001000, B10001000, B10001000, B01010000, B00100000, B00100000, B00100000, 6,
    B11111000, B00001000, B00010000, B00100000, B01000000, B10000000, B11111000, 6,
    // [ (0x5B) to ~ (0x7E)
    B11100000, B10000000, B10000000, B10000000, B10000000, B10000000, B11100000, 4,
    B00000000, B10000000, B01000000, B00100000, B00010000, B00001000, B00000000, 6,
    B11100000, B00100000, B00100000, B00100000, B00100000, B00100000, B11100000, 4,
    B00100000, B01010000, B10001000, B00000000, B00000000, B00000000, B00000000, 6,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B11111000, 6,
    B10000000, B01000000, B00100000, B00000000, B00000000, B00000000, B00000000, 4,
    // a-z (0x61-0x7A)
    B00000000, B00000000, B01110000, B00001000, B01111000, B10001000, B01111000, 6,
    B10000000, B10000000, B10110000, B11001000, B10001000, B10001000, B11110000, 6,
    B00000000, B00000000, B01110000, B10001000, B10000000, B10001000, B01110000, 6,
    B00001000, B00001000, B01101000, B10011000, B10001000, B10001000, B01111000, 6,
    B00000000, B00000000, B01110000, B10001000, B11111000, B10000000, B01110000, 6,
    B00110000, B01001000, B01000000, B11100000, B01000000, B01000000, B01000000, 6,
    B00000000, B01111000, B10001000, B10001000, B01111000, B00001000, B01110000, 6,
    B10000000, B10000000, B10110000, B11001000, B10001000, B10001000, B10001000, 6,
    B01000000, B00000000, B11000000, B01000000, B01000000, B01000000, B11100000, 4,
    B00010000, B00000000, B00110000, B00010000, B00010000, B10010000, B01100000, 5,
    B10000000, B10000000, B10010000, B10100000, B11000000, B10100000, B10010000, 5,
    B11000000, B01000000, B01000000, B01000000, B01000000, B01000000, B11100000, 4,
    B00000000, B00000000, B11010000, B10101000, B10101000, B10001000, B10001000, 6,
    B00000000, B00000000, B10110000, B11001000, B10001000, B10001000, B10001000, 6,
    B00000000, B00000000, B01110000, B10001000, B10001000, B10001000, B01110000, 6,
    B00000000, B00000000, B11110000, B10001000, B11110000, B10000000, B10000000, 6,
    B00000000, B00000000, B01101000, B10011000, B01111000, B00001000, B00001000, 6,
    B00000000, B00000000, B10110000, B11001000, B10000000, B10000000, B10000000, 6,
    B00000000, B00000000, B01110000, B10000000, B01110000, B00001000, B11110000, 6,
    B01000000, B01000000, B11100000, B01000000, B01000000, B01001000, B00110000, 6,
    B00000000, B00000000, B10001000, B10001000, B10001000, B10011000, B01101000, 6,
    B00000000, B00000000, B10001000, B10001000, B10001000, B01010000, B00100000, 6,
    B00000000, B00000000, B10001000, B10101000, B10101000, B10101000, B01010000, 6,
    B00000000, B00000000, B10001000, B01010000, B00100000, B01010000, B10001000, 6,
    B00000000, B00000000, B10001000, B10001000, B01111000, B00001000, B01110000, 6,
    B00000000, B00000000, B11111000, B00010000, B00100000, B01000000, B11111000, 6,
    // { | } ~ (0x7B-0x7E)
    B00100000, B01000000, B01000000, B10000000, B01000000, B01000000, B00100000, 4,
    B10000000, B10000000, B10000000, B10000000, B10000000, B10000000, B10000000, 2,
    B10000000, B01000000, B01000000, B00100000, B01000000, B01000000, B10000000, 4,
    B00000000, B00000000, B00000000, B01101000, B10010000, B00000000, B00000000, 6,
    // DEL (0x7F)
    B01100000, B10010000, B10010000, B01100000, B00000000, B00000000, B00000000, 5,
    // Smiley (0x80)
    B00000000, B01100000, B01100110, B00000000, B10000001, B01100110, B00011000, 5
  };
  
  // Aurebesh 5x7 Font definitions  
  const unsigned char aurebesh5x7[] PROGMEM = {
    // Space (0x20)
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, 3,
    // ! (0x21)
    B00000000, B01000000, B10000000, B01000000, B10000000, B00000000, B00000000, 3,
    // " (0x22)
    B00000000, B10100000, B11000000, B10000000, B00000000, B00000000, B00000000, 4,
    // # (0x23)
    B01010000, B01010000, B11111000, B01010000, B11111000, B01010000, B01010000, 6,
    // $ (0x24)
    B00000000, B01010000, B11111110, B01010100, B00001000, B00010000, B00000000, 7,
    // % (0x25)
    B11000000, B11001000, B00010000, B00100000, B01000000, B10011000, B00011000, 6,
    // & (0x26)
    B01100000, B10010000, B10100000, B01000000, B10101000, B10010000, B01101000, 6,
    // ' (0x27)
    B00000000, B11000000, B01000000, B01000000, B00000000, B00000000, B00000000, 3,
    // ( (0x28)
    B00000000, B01000000, B01000000, B11000000, B01000000, B01000000, B00000000, 3,
    // ) (0x29)
    B00000000, B10000000, B10000000, B11000000, B10000000, B10000000, B00000000, 3,
    // * (0x2A)
    B00000000, B00100000, B10101000, B01110000, B10101000, B00100000, B00000000, 6,
    // + (0x2B)
    B00000000, B00100000, B00100000, B11111000, B00100000, B00100000, B00000000, 6,
    // , (0x2C)
    B00000000, B00000000, B00000000, B00000000, B01000000, B01000000, B01000000, 3,
    // - (0x2D)
    B00000000, B01100000, B00000000, B00000000, B00000000, B00000000, B00000000, 4,
    // . (0x2E)
    B00000000, B00000000, B00000000, B00000000, B10100000, B10100000, B10100000, 4,
    // / (0x2F)
    B00000000, B01000000, B01000000, B10000000, B10000000, B00000000, B00000000, 3,
    // 0-9 (same as regular font)
    B01110000, B10001000, B10011000, B10101000, B11001000, B10001000, B01110000, 6,
    B01000000, B11000000, B01000000, B01000000, B01000000, B01000000, B11100000, 4,
    B01110000, B10001000, B00001000, B00010000, B00100000, B01000000, B11111000, 6,
    B11111000, B00010000, B00100000, B00010000, B00001000, B10001000, B01110000, 6,
    B00010000, B00110000, B01010000, B10010000, B11111000, B00010000, B00010000, 6,
    B11111000, B10000000, B11110000, B00001000, B00001000, B10001000, B01110000, 6,
    B00110000, B01000000, B10000000, B11110000, B10001000, B10001000, B01110000, 6,
    B11111000, B10001000, B00001000, B00010000, B00100000, B00100000, B00100000, 6,
    B01110000, B10001000, B10001000, B01110000, B10001000, B10001000, B01110000, 6,
    B01110000, B10001000, B10001000, B01111000, B00001000, B00010000, B01100000, 6,
    // : to @ (0x3A-0x40)
    B00000000, B00000000, B10000000, B01000000, B11100000, B00000000, B00000000, 4,
    B00000000, B10000000, B10000000, B10000000, B10000000, B10000000, B00000000, 3,
    B00010000, B00100000, B01000000, B10000000, B01000000, B00100000, B00010000, 5,
    B00000000, B00000000, B11111000, B00000000, B11111000, B00000000, B00000000, 6,
    B10000000, B01000000, B00100000, B00010000, B00100000, B01000000, B10000000, 5,
    B00000000, B11000000, B10100000, B00100000, B00100000, B01000000, B00000000, 5,
    B01110000, B10001000, B00001000, B01101000, B10101000, B10101000, B01110000, 6,
    // A-Z (Aurebesh characters)
    B00000000, B10000110, B11111000, B00000000, B11111000, B10000110, B00000000, 7,
    B00000000, B01111100, B10000010, B00111000, B10000010, B01111100, B00000000, 7,
    B00000000, B01000000, B01000000, B01010100, B01010100, B00000100, B00000100, 7,
    B00000000, B11111110, B00000100, B01111000, B00010000, B00100000, B01000000, 7,
    B00000000, B10001110, B10001100, B01010100, B01010100, B00100100, B00100100, 7,
    B00000000, B00000000, B00010010, B11111100, B10010000, B11111110, B00000000, 7,
    B00000000, B10111110, B10100010, B10000100, B10000100, B10001000, B11110000, 7,
    B00000000, B01111110, B00000000, B00111100, B00000000, B01111110, B00000000, 7,
    B00000000, B00100000, B01100000, B00100000, B00100000, B00100000, B00000000, 4,
    B00000000, B00000010, B00001100, B11111000, B00010000, B11100000, B00000000, 7,
    B00000000, B00000000, B11111100, B00000100, B00000100, B11111100, B00000000, 7,
    B00000000, B00010000, B00010000, B10010000, B01010000, B00110000, B00000000, 7,
    B00000000, B00000000, B00011100, B00100000, B01000000, B01111100, B00000000, 7,
    B00000000, B01001000, B10001000, B10010100, B10100001, B01000001, B00000000, 7,
    B00000000, B00111000, B01000100, B10000010, B10000010, B01111100, B00000000, 7,
    B00000000, B01101000, B10001000, B10001000, B10001000, B01111000, B00000000, 7,
    B00000000, B00000000, B01111100, B01000100, B01000000, B01110000, B00000000, 7,
    B00000000, B11111110, B00000100, B00001000, B00010000, B00100000, B01000000, 7,
    B00000000, B10000100, B01000100, B00100100, B00010100, B10001100, B01100100, 7,
    B00000000, B00010000, B00010000, B01010100, B00111000, B00010000, B00000000, 7,
    B00000000, B10011100, B10100100, B10000100, B10000100, B11111100, B00000000, 7,
    B00000000, B01000100, B00101000, B00010000, B00010000, B00010000, B00000000, 7,
    B00000000, B00000000, B01111110, B01000010, B01000010, B01111110, B00000000, 7,
    B00000000, B00000000, B00010000, B00101000, B01000100, B00111000, B00000000, 7,
    B00000000, B11100010, B10100010, B01000100, B00101000, B00010000, B00000000, 7,
    B00000000, B00000100, B00011100, B00100100, B10000100, B11111100, B00000000, 7,
    // [ to DEL (0x5B-0x7F) - same as regular font
    B11100000, B10000000, B10000000, B10000000, B10000000, B10000000, B11100000, 4,
    B00000000, B10000000, B01000000, B00100000, B00010000, B00001000, B00000000, 6,
    B11100000, B00100000, B00100000, B00100000, B00100000, B00100000, B11100000, 4,
    B00100000, B01010000, B10001000, B00000000, B00000000, B00000000, B00000000, 6,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B11111000, 6,
    B00000000, B11000000, B10000000, B10000000, B00000000, B00000000, B00000000, 3,
    // a-z lowercase (same as standard font for simplicity)
    B00000000, B00000000, B01110000, B00001000, B01111000, B10001000, B01111000, 6,
    B10000000, B10000000, B10110000, B11001000, B10001000, B10001000, B11110000, 6,
    B00000000, B00000000, B01110000, B10001000, B10000000, B10001000, B01110000, 6,
    B00001000, B00001000, B01101000, B10011000, B10001000, B10001000, B01111000, 6,
    B00000000, B00000000, B01110000, B10001000, B11111000, B10000000, B01110000, 6,
    B00110000, B01001000, B01000000, B11100000, B01000000, B01000000, B01000000, 6,
    B00000000, B01111000, B10001000, B10001000, B01111000, B00001000, B01110000, 6,
    B10000000, B10000000, B10110000, B11001000, B10001000, B10001000, B10001000, 6,
    B01000000, B00000000, B11000000, B01000000, B01000000, B01000000, B11100000, 4,
    B00010000, B00000000, B00110000, B00010000, B00010000, B10010000, B01100000, 5,
    B10000000, B10000000, B10010000, B10100000, B11000000, B10100000, B10010000, 5,
    B11000000, B01000000, B01000000, B01000000, B01000000, B01000000, B11100000, 4,
    B00000000, B00000000, B11010000, B10101000, B10101000, B10001000, B10001000, 6,
    B00000000, B00000000, B10110000, B11001000, B10001000, B10001000, B10001000, 6,
    B00000000, B00000000, B01110000, B10001000, B10001000, B10001000, B01110000, 6,
    B00000000, B00000000, B11110000, B10001000, B11110000, B10000000, B10000000, 6,
    B00000000, B00000000, B01101000, B10011000, B01111000, B00001000, B00001000, 6,
    B00000000, B00000000, B10110000, B11001000, B10000000, B10000000, B10000000, 6,
    B00000000, B00000000, B01110000, B10000000, B01110000, B00001000, B11110000, 6,
    B01000000, B01000000, B11100000, B01000000, B01000000, B01001000, B00110000, 6,
    B00000000, B00000000, B10001000, B10001000, B10001000, B10011000, B01101000, 6,
    B00000000, B00000000, B10001000, B10001000, B10001000, B01010000, B00100000, 6,
    B00000000, B00000000, B10001000, B10101000, B10101000, B10101000, B01010000, 6,
    B00000000, B00000000, B10001000, B01010000, B00100000, B01010000, B10001000, 6,
    B00000000, B00000000, B10001000, B10001000, B01111000, B00001000, B01110000, 6,
    B00000000, B00000000, B11111000, B00010000, B00100000, B01000000, B11111000, 6,
    // { | } ~ (0x7B-0x7E)
    B00100000, B01000000, B01000000, B10000000, B01000000, B01000000, B00100000, 4,
    B10000000, B10000000, B10000000, B10000000, B10000000, B10000000, B10000000, 2,
    B10000000, B01000000, B01000000, B00100000, B01000000, B01000000, B10000000, 4,
    B00000000, B00000000, B00000000, B01101000, B10010000, B00000000, B00000000, 6,
    // DEL (0x7F)
    B01100000, B10010000, B10010000, B01100000, B00000000, B00000000, B00000000, 5,
    // Smiley (0x80)
    B00000000, B01100000, B01100110, B00000000, B10000001, B01100110, B00011000, 5
  };
  
  // Test messages - Added spaces for completion safety
  const unsigned char testMessage[] PROGMEM = "THIS IS A TEST MESSAGE. MAGIC PANEL V2.4! ";
  const unsigned char aurebeshTest[] PROGMEM = " AUREBESH TEST. CAN YOU READ THIS? ";
  
  // Forward declarations
  void loadCharacter(int ascii, int fontType);
  void rotateBuffer();
  void displayBuffer();
  int loadCharacterToBuffer(int ascii, int fontType);
  bool isValidFontIndex(int ascii, int fontType);
  int calculateTextWidth(const char* text, int length, int fontType);
  void clearBuffer();

  // Clear buffer safely
  void clearBuffer() {
    for (int i = 0; i < TEXT_SCROLL_BUFFER_SIZE; i++) {
      scrollBuffer[i] = 0;
    }
  }

  // Validate font index to prevent buffer overflow
  bool isValidFontIndex(int ascii, int fontType) {
    if (ascii < 0x20 || ascii > 0x80) return false;
    
    // Calculate required buffer size
    int charIndex = (ascii - 0x20) * 8;
    int maxIndex = charIndex + 7;
    
    // Check against font array size (96 characters * 8 bytes = 768 bytes)
    const int FONT_SIZE = 96 * 8;
    return (maxIndex < FONT_SIZE);
  }

  // Calculate text width in pixels for accurate scrolling
  int calculateTextWidth(const char* text, int length, int fontType) {
    if (!text || length <= 0) return 0;
    if (fontType < 1 || fontType > 2) fontType = 1;
    
    int totalWidth = 0;
    const unsigned char* fontPtr = (fontType == 1) ? font5x7 : aurebesh5x7;
    
    for (int i = 0; i < length; i++) {
      char c = text[i];
      if (isValidFontIndex(c, fontType)) {
        int charIndex = (c - 0x20) * 8;
        if (charIndex >= 0 && charIndex < 96 * 8) {
          byte charWidth = pgm_read_byte_near(fontPtr + charIndex + 7);
          totalWidth += constrain(charWidth, 0, 8);
        }
      }
    }
    
    return totalWidth;
  }

  // Core scrolling function with FIXED completion tracking
  void scrollMessage(const unsigned char* message, int fontType) {
    if (!message) return;
    
    // Validate font type
    if (fontType < 1 || fontType > 2) fontType = 1;
    
    int counter = 0;
    char myChar;
    const int MAX_MESSAGE_LENGTH = 256;
    
    // Clear buffer before starting
    Text::clearBuffer();
    
    // Load all characters
    do {
      yield();
      
      if (counter >= MAX_MESSAGE_LENGTH) break;
      
      myChar = pgm_read_byte_near(message + counter);
      if (myChar != 0) {
        if (isValidFontIndex(myChar, fontType)) {
          loadCharacter(myChar, fontType);
        }
      }
      counter++;
    } while (myChar != 0);
    
    // FIXED: Ensure COMPLETE scrolling with enough iterations
    // Need extra iterations to scroll entire text off screen
    for (int i = 0; i < 40; i++) {  // Increased to 40 for complete scrolling
      yield();
      rotateBuffer();
      displayBuffer();
      long dynamicDelay = map(panelState.animationSpeed, 1, 100, 150, 10);
      delay(constrain(dynamicDelay, 10, 200));
    }
    
    // Extra safety: ensure buffer is clear
    clearBuffer();
    LedControl::clearPanel();
  }
  
  // Load character with comprehensive bounds checking
  void loadCharacter(int ascii, int fontType) {
    if (!isValidFontIndex(ascii, fontType)) return;
    
    // Validate font type
    if (fontType < 1 || fontType > 2) fontType = 1;
    
    const unsigned char* fontPtr = (fontType == 1) ? font5x7 : aurebesh5x7;
    int charIndex = (ascii - 0x20) * 8;
    
    // Additional bounds check
    if (charIndex < 0 || charIndex >= 96 * 8) return;
    
    // Load character data into buffer with bounds checking
    for (int a = 0; a < 7 && a < TEXT_SCROLL_BUFFER_SIZE/2; a++) {
      int bufferIndex = a * 2;
      if (bufferIndex < TEXT_SCROLL_BUFFER_SIZE) {
        scrollBuffer[bufferIndex] |= pgm_read_byte_near(fontPtr + charIndex + a);
      }
    }
    
    // Get character width and scroll it
    byte count = pgm_read_byte_near(fontPtr + charIndex + 7);
    
    // Validate count
    if (count > 8) count = 8;
    
    for (byte x = 0; x < count; x++) {
      yield();
      rotateBuffer();
      displayBuffer();
      long dynamicDelay = map(panelState.animationSpeed, 1, 100, 150, 10);
      delay(constrain(dynamicDelay, 10, 200));
    }
    
    // Add small spacing between characters
    rotateBuffer();
    displayBuffer();
    delay(10);
  }
  
  // Rotate buffer with bounds checking
  void rotateBuffer() {
    for (int a = 0; a < 7 && a < TEXT_SCROLL_BUFFER_SIZE/2; a++) {
      int idx1 = a * 2;
      int idx2 = a * 2 + 1;
      
      // Bounds check for buffer access
      if (idx1 < TEXT_SCROLL_BUFFER_SIZE && idx2 < TEXT_SCROLL_BUFFER_SIZE) {
        unsigned long x = scrollBuffer[idx1];
        byte b = bitRead(x, 31);
        x <<= 1;
        scrollBuffer[idx1] = x;
        
        x = scrollBuffer[idx2];
        x <<= 1;
        bitWrite(x, 0, b);
        scrollBuffer[idx2] = x;
      }
    }
  }
  
  // Display buffer with bounds checking
  void displayBuffer() {
    CRGB color = LedControl::getCurrentColor();
    
    for (int a = 0; a < 7 && a < MATRIX_HEIGHT; a++) {
      int bufferIndex = a * 2 + 1;
      if (bufferIndex < TEXT_SCROLL_BUFFER_SIZE) {
        uint8_t rowData = scrollBuffer[bufferIndex] & 0xFF;
        LedControl::safeSetRow(a, rowData, color);
      }
    }
    LedControl::updatePanel();
  }
   
  // Custom text scrolling with FIXED completion
  void scrollCustomText(String text, int fontType) {
    // Validate input
    if (text.length() == 0 || text.length() > MAX_TEXT_LENGTH) return;
    if (fontType < 1 || fontType > 2) fontType = 1;
    
    clearBuffer();
    text.toUpperCase();
    
    // Calculate total pixel width for accurate scrolling
    int totalPixelWidth = 0;
    
    for (unsigned int i = 0; i < text.length() && i < MAX_TEXT_LENGTH; i++) {
      yield();
      
      char c = text.charAt(i);
      if (isValidFontIndex(c, fontType)) {
        loadCharacter(c, fontType);
        
        // Track width
        const unsigned char* fontPtr = (fontType == 1) ? font5x7 : aurebesh5x7;
        int charIndex = (c - 0x20) * 8;
        if (charIndex >= 0 && charIndex < 96 * 8) {
          byte charWidth = pgm_read_byte_near(fontPtr + charIndex + 7);
          totalPixelWidth += constrain(charWidth, 0, 8);
        }
      }
    }
    
    // FIXED: Calculate required iterations based on text length
    // Minimum 30, add more for longer texts
    int requiredIterations = 30 + (text.length() * 2);
    requiredIterations = constrain(requiredIterations, 30, 60);
    
    for (int i = 0; i < requiredIterations; i++) {
      yield();
      rotateBuffer();
      displayBuffer();
      long dynamicDelay = map(panelState.animationSpeed, 1, 100, 150, 10);
      delay(constrain(dynamicDelay, 10, 200));
    }
    
    clearBuffer();
    LedControl::clearPanel();
    delay(TEXT_SCROLL_PAUSE);
  }

  // Load character to buffer with bounds checking
  int loadCharacterToBuffer(int ascii, int fontType) {
    if (!isValidFontIndex(ascii, fontType)) return 0;
    
    // Validate font type
    if (fontType < 1 || fontType > 2) fontType = 1;
    
    const unsigned char* fontPtr = (fontType == 1) ? font5x7 : aurebesh5x7;
    int charIndex = (ascii - 0x20) * 8;
    
    // Additional bounds check
    if (charIndex < 0 || charIndex >= 96 * 8) return 0;
    
    for (int a = 0; a < 7 && a < TEXT_SCROLL_BUFFER_SIZE/2; a++) {
      int bufferIndex = a * 2;
      if (bufferIndex < TEXT_SCROLL_BUFFER_SIZE) {
        scrollBuffer[bufferIndex] |= pgm_read_byte_near(fontPtr + charIndex + a);
      }
    }
    
    byte width = pgm_read_byte_near(fontPtr + charIndex + 7);
    return constrain(width, 0, 8);
  }

  // Rainbow text scrolling with FIXED completion
  void scrollRainbowText(String text, int fontType) {
    // Validate input
    if (text.length() == 0 || text.length() > MAX_TEXT_LENGTH) return;
    if (fontType < 1 || fontType > 2) fontType = 1;
    
    clearBuffer();
    text.toUpperCase();
    static uint8_t hue = 0;
    long dynamicDelay = map(panelState.animationSpeed, 1, 100, 150, 10);
    dynamicDelay = constrain(dynamicDelay, 10, 200);
    
    int totalPixelWidth = 0;

    // Process each character
    for (unsigned int i = 0; i < text.length() && i < MAX_TEXT_LENGTH; i++) {
      yield();
      
      char c = text.charAt(i);
      if (!isValidFontIndex(c, fontType)) continue;
      
      int charWidth = loadCharacterToBuffer(c, fontType);
      totalPixelWidth += charWidth;
      
      // Scroll character in
      for (int j = 0; j < charWidth && j < 8; j++) {
        yield();
        rotateBuffer();
        LedControl::clearPanel();
        
        // Draw rainbow pixels
        for (int y = 0; y < 7 && y < MATRIX_HEIGHT; y++) {
          int bufferIndex = y * 2 + 1;
          if (bufferIndex < TEXT_SCROLL_BUFFER_SIZE) {
            for (int x = 0; x < 8 && x < MATRIX_WIDTH; x++) {
              if ((scrollBuffer[bufferIndex] >> (7 - x)) & 1) {
                if (LedControl::isValidCoordinate(x, y)) {
                  LedControl::setPixel(x, y, CHSV(hue + (x * 16), 255, 255));
                }
              }
            }
          }
        }
        LedControl::updatePanel();
        hue += 3;
        delay(dynamicDelay);
      }
    }
    
    // FIXED: Ensure complete scroll-out
    // Minimum 30, add more for longer texts
    int requiredIterations = 30 + (text.length() * 2);
    requiredIterations = constrain(requiredIterations, 30, 60);
    
    for (int i = 0; i < requiredIterations; i++) {
      yield();
      rotateBuffer();
      LedControl::clearPanel();
      
      // Continue drawing rainbow effect
      for (int y = 0; y < 7 && y < MATRIX_HEIGHT; y++) {
        int bufferIndex = y * 2 + 1;
        if (bufferIndex < TEXT_SCROLL_BUFFER_SIZE) {
          for (int x = 0; x < 8 && x < MATRIX_WIDTH; x++) {
            if ((scrollBuffer[bufferIndex] >> (7 - x)) & 1) {
              if (LedControl::isValidCoordinate(x, y)) {
                LedControl::setPixel(x, y, CHSV(hue + (x * 16), 255, 255));
              }
            }
          }
        }
      }
      LedControl::updatePanel();
      hue += 3;
      delay(dynamicDelay);
    }
    
    clearBuffer();
    LedControl::clearPanel();
    delay(TEXT_SCROLL_PAUSE);
  }

  // Bouncing text with comprehensive bounds checking
  void bouncingText(String text, int fontType) {
    // Validate input
    if (text.length() == 0 || text.length() > MAX_TEXT_LENGTH) return;
    if (fontType < 1 || fontType > 2) fontType = 1;
    
    CRGB color = LedControl::getCurrentColor();
    text.toUpperCase();
    
    for(unsigned int i = 0; i < text.length() && i < MAX_TEXT_LENGTH; i++) {
      yield();
      char c = text.charAt(i);
      
      if (c == ' ') { 
        delay(ANIMATION_DELAY_MEDIUM); 
        continue; 
      }
      
      if (!isValidFontIndex(c, fontType)) continue;
      
      const unsigned char* fontPtr = (fontType == 1) ? font5x7 : aurebesh5x7;
      int charIndex = (c - 0x20) * 8;
      
      // Additional bounds check
      if (charIndex < 0 || charIndex >= 96 * 8) continue;
      
      uint8_t char_data[8] = {0};
      
      // Load character data with bounds checking
      for(int j = 0; j < 8; j++) { 
        char_data[j] = pgm_read_byte_near(fontPtr + charIndex + j); 
      }
      
      // Bounce animation with bounds checking
      for(int y = -7; y <= 0; y++) { 
        LedControl::clearPanel(); 
        // Only draw if sprite would be visible
        if (y + 7 >= 0) {
          Shapes::drawSprite(0, y, char_data, color); 
        }
        LedControl::updatePanel(); 
        delay(ANIMATION_DELAY_VERY_FAST); 
      }
      
      // Show character at rest positions
      LedControl::clearPanel(); 
      Shapes::drawSprite(0, -1, char_data, color); 
      LedControl::updatePanel(); 
      delay(60);
      
      LedControl::clearPanel(); 
      Shapes::drawSprite(0, 0, char_data, color); 
      LedControl::updatePanel();
      delay(800);
    }
  }
}

// Main dispatcher function for text patterns with validation
void runTextPattern(int patternId) {
  // Validate pattern ID
  if (!((patternId == 80) || (patternId >= 97 && patternId <= 98))) {
    LedControl::clearPanel();
    return;
  }
  
  switch (patternId) {
    case 97:
      Text::clearBuffer();
      Text::scrollMessage(Text::testMessage, 1);
      LedControl::clearPanel();
      break;
    case 98:
      Text::clearBuffer();
      Text::scrollMessage(Text::aurebeshTest, 2);
      LedControl::clearPanel();
      break;
    case 80: 
      Text::bouncingText("BOUNCE", panelState.useAurebesh ? 2 : 1); 
      break;
    default:
      LedControl::clearPanel();
      break;
  }
}


#endif // PATTERNS_TEXT_H