/*
 * Shape Patterns
 * Static shapes and symbols including digits
 * VERSION 2.6 - Added comprehensive bounds checking and validation
 */

#ifndef PATTERNS_SHAPES_H
#define PATTERNS_SHAPES_H

#include "led_control.h"

// Forward declarations for global variables and functions
extern void setPatternEndTime(unsigned long duration);
extern PanelState panelState;
extern bool patternActive;

namespace Shapes {
  
  // Sprites for gaming animations
  const uint8_t pacman_open[] = { 
    B00111100, B01111110, B11000111, B11000011, 
    B11000111, B01111110, B00111100, B00000000 
  };
  const uint8_t pacman_closed[] = { 
    B00111100, B01111110, B11111111, B11111111, 
    B11111111, B01111110, B00111100, B00000000 
  };
  const uint8_t ghost[] = { 
    B00111100, B01111110, B10111011, B11111111, 
    B11111111, B10101010, B10101010, B00000000 
  };
  const uint8_t invader[] = { 
    B00011000, B00111100, B01111110, B11011011, 
    B11111111, B01011010, B00100100, B00000000 
  };

  // Shape definitions
  const uint8_t CROSS_PATTERN[8] = { 
    B00011000, B00111100, B01111110, B11111111, 
    B11111111, B01111110, B00111100, B00011000 
  };
  const uint8_t AI_LOGO[8] = { 
    B00111100, B01000010, B10111011, B10111011, 
    B10000001, B10000001, B01111110, B00000000 
  };
  const uint8_t HEART_PATTERN[8] = { 
    B00000000, B01100110, B11111111, B11111111, 
    B01111110, B00111100, B00011000, B00000000 
  };
  const uint8_t HEART_SMALL[8] = { 
    B00000000, B00000000, B00100100, B01111110, 
    B01111110, B00111100, B00011000, B00000000 
  };
  const uint8_t SMILEY_PATTERN[8] = { 
    B00111100, B01000010, B10100101, B10000001, 
    B10011001, B10100101, B01000010, B00111100 
  };
  const uint8_t SAD_PATTERN[8] = { 
    B00111100, B01000010, B10100101, B10000001, 
    B10100101, B10011001, B01000010, B00111100 
  };
  
  // Digit patterns (Centered 5x7)
  const uint8_t DIGITS[10][8] = {
    {B00000000,B00111000,B01000100,B01000100,B01000100,B01000100,B00111000,B00000000}, // 0
    {B00000000,B00010000,B00110000,B00010000,B00010000,B00010000,B00111000,B00000000}, // 1
    {B00000000,B00111000,B01000100,B00001000,B00010000,B00100000,B01111100,B00000000}, // 2
    {B00000000,B00111000,B01000100,B00110000,B00000100,B01000100,B00111000,B00000000}, // 3
    {B00000000,B00001000,B00011000,B00101000,B01111100,B00001000,B00001000,B00000000}, // 4
    {B00000000,B01111100,B01000000,B01111000,B00000100,B01000100,B00111000,B00000000}, // 5
    {B00000000,B00111000,B01000000,B01111000,B01000100,B01000100,B00111000,B00000000}, // 6
    {B00000000,B01111100,B00000100,B00001000,B00010000,B00100000,B00100000,B00000000}, // 7
    {B00000000,B00111000,B01000100,B00111000,B01000100,B01000100,B00111000,B00000000}, // 8
    {B00000000,B00111000,B01000100,B01000100,B00111100,B00000100,B00111000,B00000000}  // 9
  };

  // Draw sprite with comprehensive bounds checking - FIXED
  void drawSprite(int8_t x, int8_t y, const uint8_t sprite[8], CRGB color) {
    // Early exit if sprite is completely outside bounds
    if (x > MATRIX_WIDTH || x < -8 || y > MATRIX_HEIGHT || y < -8) return;
    
    // Validate sprite pointer
    if (!sprite) return;
    
    for (int sy = 0; sy < 8; sy++) {
      int8_t py = y + sy;
      // Skip row if outside bounds
      if (py < 0 || py >= MATRIX_HEIGHT) continue;
      
      for (int sx = 0; sx < 8; sx++) {
        int8_t px = x + sx;
        // Skip pixel if outside bounds
        if (px < 0 || px >= MATRIX_WIDTH) continue;
        
        // Check if pixel should be lit (with bounds check for sprite array)
        if (sy < 8 && sx < 8) {
          if ((sprite[sy] >> (7 - sx)) & 1) {
            LedControl::setPixel(px, py, color);
          }
        }
      }
    }
  }

  // Draw shape with bounds checking - FIXED
  void drawShape(const uint8_t shape[8]) {
    // Validate shape pointer
    if (!shape) return;
    
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < 8 && i < MATRIX_HEIGHT; i++) {
      LedControl::safeSetRow(i, shape[i], color);
    }
    LedControl::updatePanel();
  }
  
  // Show digit with validation - FIXED
  void showDigit(int digit) {
    // Validate digit range
    if (digit < 0 || digit > 9) {
      LedControl::clearPanel(); // Show nothing for invalid digits
      return;
    }
    drawShape(DIGITS[digit]);
  }

  // Countdown with validation and bounds checking - FIXED
  void countdown(int start) {
    // Validate and constrain start value
    if (start < 0) start = 0;
    if (start > 9) start = 9;
    
    for (int i = start; i >= 0; i--) {
      yield(); // Prevent watchdog reset
      showDigit(i);
      delay(1000);
    }
  }

  // 2GWD Logo animation with bounds checking
  void twoGWDLogo() {
    showDigit(2); 
    delay(1000);
    
    const uint8_t G[] = {0, 0x3E, 0x41, 0x41, 0x49, 0x49, 0x3E, 0}; 
    drawShape(G); 
    delay(1000);
    
    const uint8_t W[] = {0, 0x81, 0x81, 0x99, 0x99, 0x5A, 0x3C, 0}; 
    drawShape(W); 
    delay(1000);
    
    const uint8_t D[] = {0, 0x7C, 0x82, 0x81, 0x81, 0x82, 0x7C, 0}; 
    drawShape(D);
  }

  // Checkerboard with validation
  void checkerboard(int flashes = 4) {
    // Validate flashes
    if (flashes < 1) flashes = 1;
    if (flashes > 20) flashes = 20;
    
    CRGB color1 = LedControl::getCurrentColor();
    CRGB color2 = panelState.rainbowMode ? LedControl::getRandomColor() : CRGB::Black;
    
    for (int flash = 0; flash < flashes; flash++) {
      yield(); // Prevent watchdog reset
      
      // Pattern 1
      for (int i = 0; i < NUM_LEDS; i++) {
        if (LedControl::isValidIndex(i)) {
          leds[i] = (i % 2 == (i / 8) % 2) ? color1 : color2;
        }
      }
      FastLED.show(); 
      delay(ANIMATION_DELAY_MEDIUM);
      
      // Pattern 2 (inverted)
      for (int i = 0; i < NUM_LEDS; i++) {
        if (LedControl::isValidIndex(i)) {
          leds[i] = (i % 2 != (i / 8) % 2) ? color1 : color2;
        }
      }
      FastLED.show(); 
      delay(ANIMATION_DELAY_MEDIUM);
    }
  }

  // Animated heart with bounds checking
  void animatedHeart() {
    CRGB color = LedControl::getCurrentColor();
    for (int beat = 0; beat < 3; beat++) {
      yield(); // Prevent watchdog reset
      
      drawShape(HEART_SMALL); 
      delay(ANIMATION_DELAY_MEDIUM);
      drawShape(HEART_PATTERN); 
      delay(300);
      drawShape(HEART_SMALL); 
      delay(150);
      LedControl::clearPanel(); 
      delay(400);
    }
  }

  // Random alert with validation
  void randomAlert(int repeats = 20) {
    // Validate repeats
    if (repeats < 1) repeats = 1;
    if (repeats > 100) repeats = 100;
    
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < repeats; i++) {
      yield(); // Prevent watchdog reset
      
      LedControl::allOn(color);
      delay(random(5, 40));
      LedControl::clearPanel();
      delay(random(3, 25));
    }
  }

  // Random pixel with validation
  void randomPixel(int duration = 40) {
    // Validate duration
    if (duration < 1) duration = 1;
    if (duration > 200) duration = 200;
    
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < duration; i++) {
      yield(); // Prevent watchdog reset
      
      uint8_t pixel = random(NUM_LEDS);
      if (LedControl::isValidIndex(pixel)) {
        leds[pixel] = color;
      }
      FastLED.show();
      delay(ANIMATION_DELAY_MEDIUM);
      LedControl::clearPanel();
    }
  }

  // Test fill with bounds checking
  void testFill(int pixelDelay = 20) {
    // Validate delay
    if (pixelDelay < 5) pixelDelay = 5;
    if (pixelDelay > 100) pixelDelay = 100;
    
    CRGB color = LedControl::getCurrentColor();
    
    // Fill
    for (int i = 0; i < NUM_LEDS; i++) { 
      yield(); // Prevent watchdog reset in long loops
      if (LedControl::isValidIndex(i)) {
        leds[i] = color;
      }
      FastLED.show(); 
      delay(pixelDelay); 
    }
    
    // Clear
    for (int i = 0; i < NUM_LEDS; i++) { 
      yield(); // Prevent watchdog reset in long loops
      if (LedControl::isValidIndex(i)) {
        leds[i] = CRGB::Black;
      }
      FastLED.show(); 
      delay(pixelDelay); 
    }
  }

  // Test pixel with bounds checking
  void testPixel(int pixelDelay = 20) {
    // Validate delay
    if (pixelDelay < 5) pixelDelay = 5;
    if (pixelDelay > 100) pixelDelay = 100;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (int i = 0; i < NUM_LEDS; i++) {
      yield(); // Prevent watchdog reset in long loops
      LedControl::clearPanel();
      if (LedControl::isValidIndex(i)) {
        leds[i] = color;
      }
      FastLED.show();
      delay(pixelDelay);
    }
  }

  // Quadrant animation with comprehensive bounds checking
  void quadrant(int type = 1, int repeats = 2) {
    // Validate parameters
    if (type < 1 || type > 4) type = 1;
    if (repeats < 1) repeats = 1;
    if (repeats > 20) repeats = 20;
    
    CRGB color = LedControl::getCurrentColor();
    uint8_t quadrants[4][2] = {{0,0}, {4,0}, {4,4}, {0,4}}; // TL, TR, BR, BL
    uint8_t order[4];

    // Define quadrant orders
    switch(type) {
      case 1: { uint8_t o[] = {0,1,2,3}; memcpy(order, o, 4); break; } // TL, TR, BR, BL
      case 2: { uint8_t o[] = {1,0,3,2}; memcpy(order, o, 4); break; } // TR, TL, BL, BR
      case 3: { uint8_t o[] = {1,2,3,0}; memcpy(order, o, 4); break; } // TR, BR, BL, TL
      case 4: { uint8_t o[] = {0,3,2,1}; memcpy(order, o, 4); break; } // TL, BL, BR, TR
      default: { uint8_t o[] = {0,1,2,3}; memcpy(order, o, 4); break; }
    }

    for(int r = 0; r < repeats; r++) {
      for(int i = 0; i < 4; i++) {
        yield(); // Prevent watchdog reset
        
        LedControl::clearPanel();
        uint8_t q_index = order[i];
        
        // Validate quadrant index
        if (q_index >= 4) continue;
        
        // Light up the selected quadrant with bounds checking
        uint8_t startX = quadrants[q_index][0];
        uint8_t startY = quadrants[q_index][1];
        
        for(uint8_t y = startY; y < startY + 4 && y < MATRIX_HEIGHT; y++) {
          for(uint8_t x = startX; x < startX + 4 && x < MATRIX_WIDTH; x++) {
            if (LedControl::isValidCoordinate(x, y)) {
              LedControl::setPixel(x, y, color);
            }
          }
        }
        LedControl::updatePanel();
        delay(ANIMATION_DELAY_MEDIUM);
      }
    }
  }
}

// Main dispatcher function for this category with validation
void runShapePattern(int patternId) {
  // Validate pattern ID range
  if (!((patternId == 20) || 
        (patternId >= 31 && patternId <= 47) || 
        (patternId == 56))) {
    LedControl::clearPanel();
    return;
  }
  
  unsigned long duration = 3000;

  switch (patternId) {
    case 20: 
      Shapes::drawShape(Shapes::CROSS_PATTERN); 
      setPatternEndTime(duration); 
      break;
    case 31: 
      Shapes::testFill(); 
      break;
    case 32: 
      Shapes::testPixel(); 
      break;
    case 33: 
      Shapes::drawShape(Shapes::AI_LOGO); 
      setPatternEndTime(duration); 
      break;
    case 34: 
      Shapes::twoGWDLogo(); 
      setPatternEndTime(4000); 
      break;
    case 35: 
      Shapes::quadrant(1); 
      break;
    case 36: 
      Shapes::quadrant(2); 
      break;
    case 37: 
      Shapes::quadrant(3); 
      break;
    case 38: 
      Shapes::quadrant(4); 
      break;
    case 39: 
      Shapes::randomPixel(); 
      break;
    case 40: 
      Shapes::countdown(9); 
      break;
    case 41: 
      Shapes::countdown(3); 
      break;
    case 42: 
      Shapes::randomAlert(20); 
      break;
    case 43: 
      Shapes::randomAlert(40); 
      break;
    case 44: 
      Shapes::drawShape(Shapes::SMILEY_PATTERN); 
      setPatternEndTime(duration); 
      break;
    case 45: 
      Shapes::drawShape(Shapes::SAD_PATTERN); 
      setPatternEndTime(duration); 
      break;
    case 46: 
      Shapes::drawShape(Shapes::HEART_PATTERN); 
      setPatternEndTime(duration); 
      break;
    case 47: 
      Shapes::checkerboard(); 
      break;
    case 56: 
      Shapes::animatedHeart(); 
      break;
    default:
      LedControl::clearPanel();
      break;
  }
  
  if (!panelState.alwaysOn && !patternActive) {
    LedControl::clearPanel();
  }
}

#endif // PATTERNS_SHAPES_H