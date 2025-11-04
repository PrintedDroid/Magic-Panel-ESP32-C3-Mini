/*
 * Effect Patterns
 * Special effects like expand, compress, fade, and other visual textures.
 * VERSION 2.6 - Fixed memory management, bounds checking, and buffer allocation
 */

#ifndef PATTERNS_EFFECTS_H
#define PATTERNS_EFFECTS_H

#include "led_control.h"
#include "patterns_basic.h"

// Forward declarations for global variables and functions
extern PanelState panelState;
extern bool patternActive;
extern void setPatternEndTime(unsigned long duration);

// Static buffer for fire effect to avoid stack allocation - FIXED
static byte fireHeatBuffer[FIRE_EFFECT_BUFFER_SIZE];
static bool fireBufferInitialized = false;

namespace Effects {
  
  // Expand from center with bounds checking
  void expand(int repeats = 1, int type = 1) { // Fill=1, Ring=2
    // Validate parameters
    if (repeats < 1) repeats = 1;
    if (repeats > 20) repeats = 20;
    if (type < 1 || type > 2) type = 1;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (int i = 0; i < repeats; i++) {
      yield(); // Prevent watchdog reset
      
      if (type == 1) {
        // Fill mode with bounds checking
        LedControl::clearPanel();
        if (3 < MATRIX_HEIGHT && 4 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(3, B00011000, color); 
          LedControl::safeSetRow(4, B00011000, color);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        if (2 < MATRIX_HEIGHT && 5 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(2, B00111100, color); 
          LedControl::safeSetRow(5, B00111100, color);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        if (1 < MATRIX_HEIGHT && 6 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(1, B01111110, color); 
          LedControl::safeSetRow(6, B01111110, color);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        if (0 < MATRIX_HEIGHT && 7 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(0, B11111111, color); 
          LedControl::safeSetRow(7, B11111111, color);
        }
        LedControl::updatePanel(); 
        delay(150);
      } else {
        // Ring mode with bounds checking
        LedControl::clearPanel();
        if (3 < MATRIX_HEIGHT && 4 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(3, B00011000, color); 
          LedControl::safeSetRow(4, B00011000, color);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        LedControl::clearPanel();
        if (2 < MATRIX_HEIGHT && 3 < MATRIX_HEIGHT && 4 < MATRIX_HEIGHT && 5 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(2, B00111100, color); 
          LedControl::safeSetRow(3, B00100100, color); 
          LedControl::safeSetRow(4, B00100100, color); 
          LedControl::safeSetRow(5, B00111100, color);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        LedControl::clearPanel();
        if (1 < MATRIX_HEIGHT && 2 < MATRIX_HEIGHT && 5 < MATRIX_HEIGHT && 6 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(1, B01111110, color); 
          LedControl::safeSetRow(6, B01111110, color); 
          LedControl::safeSetRow(2, B01000010, color); 
          LedControl::safeSetRow(5, B01000010, color);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        LedControl::clearPanel();
        if (0 < MATRIX_HEIGHT && 1 < MATRIX_HEIGHT && 6 < MATRIX_HEIGHT && 7 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(0, B11111111, color); 
          LedControl::safeSetRow(7, B11111111, color); 
          LedControl::safeSetRow(1, B10000001, color); 
          LedControl::safeSetRow(6, B10000001, color);
        }
        LedControl::updatePanel(); 
        delay(150);
      }
      delay(ANIMATION_DELAY_MEDIUM);
      LedControl::clearPanel();
    }
  }
  
  // Compress to center with bounds checking
  void compress(int repeats = 1, int type = 1) { // Fill=1, Ring=2
    // Validate parameters
    if (repeats < 1) repeats = 1;
    if (repeats > 20) repeats = 20;
    if (type < 1 || type > 2) type = 1;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (int i = 0; i < repeats; i++) {
      yield(); // Prevent watchdog reset
      
      if (type == 1) {
        // Fill mode
        LedControl::allOn(color);
        delay(150);
        
        if (0 < MATRIX_HEIGHT && 7 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(0, B00000000); 
          LedControl::safeSetRow(7, B00000000);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        if (1 < MATRIX_HEIGHT && 6 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(1, B00000000); 
          LedControl::safeSetRow(6, B00000000);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        if (2 < MATRIX_HEIGHT && 5 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(2, B00000000); 
          LedControl::safeSetRow(5, B00000000);
        }
        LedControl::updatePanel(); 
        delay(150);
      } else {
        // Ring mode
        if (0 < MATRIX_HEIGHT && 1 < MATRIX_HEIGHT && 6 < MATRIX_HEIGHT && 7 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(0, B11111111, color); 
          LedControl::safeSetRow(7, B11111111, color); 
          LedControl::safeSetRow(1, B10000001, color); 
          LedControl::safeSetRow(6, B10000001, color);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        LedControl::clearPanel();
        if (1 < MATRIX_HEIGHT && 2 < MATRIX_HEIGHT && 5 < MATRIX_HEIGHT && 6 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(1, B01111110, color); 
          LedControl::safeSetRow(6, B01111110, color); 
          LedControl::safeSetRow(2, B01000010, color); 
          LedControl::safeSetRow(5, B01000010, color);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        LedControl::clearPanel();
        if (2 < MATRIX_HEIGHT && 3 < MATRIX_HEIGHT && 4 < MATRIX_HEIGHT && 5 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(2, B00111100, color); 
          LedControl::safeSetRow(3, B00100100, color); 
          LedControl::safeSetRow(4, B00100100, color); 
          LedControl::safeSetRow(5, B00111100, color);
        }
        LedControl::updatePanel(); 
        delay(150);
        
        LedControl::clearPanel();
        if (3 < MATRIX_HEIGHT && 4 < MATRIX_HEIGHT) {
          LedControl::safeSetRow(3, B00011000, color); 
          LedControl::safeSetRow(4, B00011000, color);
        }
        LedControl::updatePanel(); 
        delay(150);
      }
      delay(ANIMATION_DELAY_MEDIUM);
      LedControl::clearPanel();
    }
  }
  
  // Fade out and optionally back in with validation
  void fadeOutIn(int type = 1) { // 1=Out/In, 2=Out only
    // Validate type
    if (type < 1 || type > 2) type = 1;
    
    CRGB color = LedControl::getCurrentColor();
    LedControl::allOn(color);
    
    uint8_t savedBrightness = panelState.brightness;
    
    for (int i = savedBrightness; i >= 0; i--) {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(5);
      yield(); // Prevent watchdog
    }
    
    if (type == 1) {
      delay(ANIMATION_DELAY_MEDIUM);
      for (int i = 0; i <= savedBrightness; i++) {
        FastLED.setBrightness(i);
        FastLED.show();
        delay(5);
        yield(); // Prevent watchdog
      }
    }
    FastLED.setBrightness(savedBrightness); // Restore brightness
  }

  // Flash All
  void flashAll(int repeats = 8) {
    // Validate repeats
    if (repeats < 1) repeats = 1;
    if (repeats > 50) repeats = 50;
    
    CRGB color = LedControl::getCurrentColor();
    
    for(int i = 0; i < repeats; i++) {
      yield(); // Prevent watchdog reset
      LedControl::allOn(color);
      delay(ANIMATION_DELAY_FAST);
      LedControl::clearPanel();
      delay(ANIMATION_DELAY_FAST);
    }
  }

  // Flash Vertical
  void flashV(unsigned long duration) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    CRGB color = LedControl::getCurrentColor();
    unsigned long startTime = millis();
    
    while(millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      // Left half
      for (int c = 0; c < 4 && c < MATRIX_WIDTH; c++) {
        LedControl::safeSetColumn(c, 0xFF, color);
      }
      for (int c = 4; c < 8 && c < MATRIX_WIDTH; c++) {
        LedControl::safeSetColumn(c, 0x00, color);
      }
      FastLED.show(); 
      delay(ANIMATION_DELAY_MEDIUM);
      
      // Right half
      for (int c = 0; c < 4 && c < MATRIX_WIDTH; c++) {
        LedControl::safeSetColumn(c, 0x00, color);
      }
      for (int c = 4; c < 8 && c < MATRIX_WIDTH; c++) {
        LedControl::safeSetColumn(c, 0xFF, color);
      }
      FastLED.show(); 
      delay(ANIMATION_DELAY_MEDIUM);
    }
  }

  // Flash Quadrants with bounds checking
  void flashQ(int repeats = 8) {
    // Validate repeats
    if (repeats < 1) repeats = 1;
    if (repeats > 50) repeats = 50;
    
    CRGB color = LedControl::getCurrentColor();
    
    for(int i = 0; i < repeats; i++) {
      yield(); // Prevent watchdog reset
      
      LedControl::clearPanel();
      // Top-Left & Bottom-Right
      for(uint8_t y = 0; y < 4 && y < MATRIX_HEIGHT; y++) {
        for(uint8_t x = 0; x < 4 && x < MATRIX_WIDTH; x++) {
          LedControl::setPixel(x, y, color);
        }
      }
      for(uint8_t y = 4; y < 8 && y < MATRIX_HEIGHT; y++) {
        for(uint8_t x = 4; x < 8 && x < MATRIX_WIDTH; x++) {
          LedControl::setPixel(x, y, color);
        }
      }
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_MEDIUM);

      LedControl::clearPanel();
      // Top-Right & Bottom-Left
      for(uint8_t y = 0; y < 4 && y < MATRIX_HEIGHT; y++) {
        for(uint8_t x = 4; x < 8 && x < MATRIX_WIDTH; x++) {
          LedControl::setPixel(x, y, color);
        }
      }
      for(uint8_t y = 4; y < 8 && y < MATRIX_HEIGHT; y++) {
        for(uint8_t x = 0; x < 4 && x < MATRIX_WIDTH; x++) {
          LedControl::setPixel(x, y, color);
        }
      }
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_MEDIUM);
    }
  }

  // Rainbow cycle effect with duration validation
  void rainbowCycle(unsigned long duration = 5000) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    uint8_t hue = 0;
    
    while (millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      for (int i = 0; i < NUM_LEDS; i++) {
        if (LedControl::isValidIndex(i)) {
          uint8_t adjustedHue = hue + (i * 4);
          leds[i] = CHSV(adjustedHue, 255, 255);
        }
      }
      LedControl::updatePanel();
      hue += 2;
      delay(20);
    }
  }
  
  // Fire effect with proper memory management and bounds checking - FIXED
  void fireEffect(unsigned long duration = 5000) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    
    // Initialize heat buffer if needed
    if (!fireBufferInitialized) {
      memset(fireHeatBuffer, 0, sizeof(fireHeatBuffer));
      fireBufferInitialized = true;
    }
    
    while (millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      // Step 1. Cool down every cell a little with bounds checking
      for(int i = 0; i < NUM_LEDS && i < FIRE_EFFECT_BUFFER_SIZE; i++) {
        uint8_t cooling = random8(0, ((55 * 10) / NUM_LEDS) + 2);
        fireHeatBuffer[i] = qsub8(fireHeatBuffer[i], cooling);
      }
      
      // Step 2. Heat from bottom with bounds checking
      int bottomStart = NUM_LEDS - MATRIX_WIDTH;
      if (bottomStart < 0) bottomStart = 0;
      
      for(int k = bottomStart; k < NUM_LEDS && k < FIRE_EFFECT_BUFFER_SIZE; k++) {
        fireHeatBuffer[k] = qadd8(fireHeatBuffer[k], random8(70, 130));
      }
      
      // Step 3. Spread heat upwards with comprehensive bounds checking
      for(int y = 0; y < MATRIX_HEIGHT; y++){
        for(int x = 0; x < MATRIX_WIDTH; x++){
          if (LedControl::isValidCoordinate(x, y)) {
            int index = LedControl::xyToIndex(x, y);
            if (index >= 0 && index < FIRE_EFFECT_BUFFER_SIZE) {
              if(y > 0){
                uint8_t belowY = y - 1;
                if (LedControl::isValidCoordinate(x, belowY)) {
                  int below = LedControl::xyToIndex(x, belowY);
                  if (below >= 0 && below < FIRE_EFFECT_BUFFER_SIZE) {
                    fireHeatBuffer[below] = (fireHeatBuffer[below] + fireHeatBuffer[index] * 2) / 3;
                  }
                }
              }
            }
          }
        }
      }
      
      // Step 4. Map heat to color using Lava palette with bounds checking
      for(int j = 0; j < NUM_LEDS && j < FIRE_EFFECT_BUFFER_SIZE; j++) {
        if (LedControl::isValidIndex(j)) {
          leds[j] = ColorFromPalette(LavaColors_p, fireHeatBuffer[j]);
        }
      }
      
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_VERY_FAST);
    }
  }
  
  // Twinkle effect with bounds checking
  void twinkle(unsigned long duration = 5000) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    LedControl::clearPanel();
    
    while (millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      if (random(100) < 20) {
        uint8_t pixel = random(NUM_LEDS);
        if (LedControl::isValidIndex(pixel)) {
          leds[pixel] = LedControl::getRandomColor();
        }
      }
      
      for (int i = 0; i < NUM_LEDS; i++) {
        if (LedControl::isValidIndex(i)) {
          leds[i].fadeToBlackBy(15);
        }
      }
      
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_VERY_FAST);
    }
  }
  
  // Plasma effect with bounds checking
  void plasma(unsigned long duration = 5000) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    int16_t time = 0;
    
    while (millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
          if (LedControl::isValidCoordinate(x, y)) {
            int16_t h = (sin8(x * 20 + time / 2) + cos8(y * 15 + time / 2) + sin8((x + y) * 15 + time / 2));
            uint8_t idx = LedControl::xyToIndex(x, y);
            if (LedControl::isValidIndex(idx)) {
              leds[idx] = CHSV(h, 255, 255);
            }
          }
        }
      }
      LedControl::updatePanel();
      time += 6;
      delay(20);
    }
  }

  // Kaleidoscope with comprehensive bounds checking
  void kaleidoscope(unsigned long duration) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    float angle1 = 0, angle2 = 0, angle3 = 0;
    float speed1 = 0.05, speed2 = 0.08, speed3 = 0.03;
    
    while(millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      angle1 += speed1; 
      angle2 += speed2; 
      angle3 += speed3;
      
      // Only process the first quadrant and mirror
      uint8_t halfWidth = MATRIX_WIDTH / 2;
      uint8_t halfHeight = MATRIX_HEIGHT / 2;
      
      for (uint8_t y = 0; y < halfHeight && y < 4; y++) {
        for (uint8_t x = 0; x < halfWidth && x < 4; x++) {
          uint8_t hue = (sin(x * 0.5 + angle1) + cos(y * 0.5 + angle2) + sin((x + y) * 0.5 + angle3) + 4) * 32;
          CRGB color = CHSV(hue, 255, 255);
          
          // Mirror to all quadrants with bounds checking
          LedControl::setPixel(x, y, color);
          
          uint8_t mirrorX = MATRIX_WIDTH - 1 - x;
          uint8_t mirrorY = MATRIX_HEIGHT - 1 - y;
          
          if (LedControl::isValidCoordinate(mirrorX, y)) {
            LedControl::setPixel(mirrorX, y, color);
          }
          if (LedControl::isValidCoordinate(x, mirrorY)) {
            LedControl::setPixel(x, mirrorY, color);
          }
          if (LedControl::isValidCoordinate(mirrorX, mirrorY)) {
            LedControl::setPixel(mirrorX, mirrorY, color);
          }
        }
      }
      LedControl::updatePanel();
      delay(20);
    }
  }

  // Raindrops effect with bounds checking
  void raindrops(unsigned long duration) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    
    while(millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      if(random(10) == 0) {
        uint8_t x = random(8);
        uint8_t y = random(8);
        if (LedControl::isValidCoordinate(x, y)) {
          uint8_t idx = LedControl::xyToIndex(x, y);
          if (LedControl::isValidIndex(idx)) {
            leds[idx] = LedControl::getCurrentColor();
          }
        }
      }
      
      for(int i = 0; i < NUM_LEDS; i++) {
        if (LedControl::isValidIndex(i)) {
          leds[i].fadeToBlackBy(10);
        }
      }
      
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_VERY_FAST);
    }
  }

  // Drip effect with bounds checking
  void dripEffect(unsigned long duration) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    uint8_t drips[MATRIX_WIDTH];
    
    // Initialize drip positions safely
    for(int i = 0; i < MATRIX_WIDTH && i < sizeof(drips); i++) {
      drips[i] = MATRIX_HEIGHT;
    }

    while(millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      for(int i = 0; i < MATRIX_WIDTH && i < sizeof(drips); i++) {
        if(drips[i] >= MATRIX_HEIGHT && random(100) == 0) {
          drips[i] = 0;
        }
        if(drips[i] < MATRIX_HEIGHT) {
          if (LedControl::isValidCoordinate(i, drips[i])) {
            LedControl::setPixel(i, drips[i], LedControl::getCurrentColor());
          }
          drips[i]++;
        }
      }
      
      LedControl::updatePanel();
      
      for(int i = 0; i < NUM_LEDS; i++) {
        if (LedControl::isValidIndex(i)) {
          leds[i].fadeToBlackBy(30);
        }
      }
      
      delay(ANIMATION_DELAY_FAST);
    }
  }
}

// Main dispatcher function for this category with validation
void runEffectPattern(int patternId) {
  // Validate pattern ID
  if (!((patternId >= 16 && patternId <= 19) ||
        (patternId >= 24 && patternId <= 28) ||
        (patternId >= 57 && patternId <= 60) ||
        (patternId >= 64 && patternId <= 66))) {
    LedControl::clearPanel();
    return;
  }
  
  int repeats = panelState.alwaysOn ? 1 : 4;
  unsigned long duration = panelState.alwaysOn ? 15000 : 8000;
  
  // Validate repeats and duration
  if (repeats < 1) repeats = 1;
  if (repeats > 20) repeats = 20;
  if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;

  switch (patternId) {
    case 16: Effects::expand(repeats, 1); break;
    case 17: Effects::expand(repeats, 2); break;
    case 18: Effects::compress(repeats, 1); break;
    case 19: Effects::compress(repeats, 2); break;
    case 24: Effects::fadeOutIn(1); break;
    case 25: Effects::fadeOutIn(2); break;
    case 26: Effects::flashAll(duration / 100); break;
    case 27: Effects::flashV(duration); break;
    case 28: Effects::flashQ(repeats); break;
    case 57: Effects::rainbowCycle(duration); break;
    case 58: Effects::fireEffect(duration); break;
    case 59: Effects::twinkle(duration); break;
    case 60: Effects::plasma(duration); break;
    case 64: Effects::kaleidoscope(duration); break;
    case 65: Effects::raindrops(duration); break;
    case 66: Effects::dripEffect(duration); break;
    default:
      LedControl::clearPanel();
      break;
  }
  
  if (!panelState.alwaysOn) {
    LedControl::clearPanel();
  }
}

#endif // PATTERNS_EFFECTS_H