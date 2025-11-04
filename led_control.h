/*
 * LED Control Utilities
 * Basic functions for controlling the LED matrix
 * VERSION 2.6 - Added comprehensive bounds checking and validation
 */

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <FastLED.h>
#include "config.h"

extern CRGB leds[NUM_LEDS];
extern PanelState panelState;

namespace LedControl {
  
  // Basic LED functions with bounds checking
  void setPixel(uint8_t x, uint8_t y, CRGB color);
  void setPixel(uint8_t index, CRGB color);
  CRGB getPixel(uint8_t x, uint8_t y);
  CRGB getPixel(uint8_t index);
  
  // Row and column operations with validation
  void setRow(uint8_t row, uint8_t pattern, CRGB color = CRGB::Red);
  void setColumn(uint8_t col, uint8_t pattern, CRGB color = CRGB::Red);
  void safeSetRow(uint8_t row, uint8_t pattern, CRGB color = CRGB::Red);
  void safeSetColumn(uint8_t col, uint8_t pattern, CRGB color = CRGB::Red);
  
  // Panel operations
  void clearPanel();
  void allOn(CRGB color = CRGB::Red);
  void updatePanel();
  void fadeOutEffect(uint8_t delay_ms = 5);
  void safeFill(CRGB color);
  
  // Utility functions with validation
  uint8_t xyToIndex(uint8_t x, uint8_t y);
  void indexToXY(uint8_t index, uint8_t &x, uint8_t &y);
  bool isValidCoordinate(uint8_t x, uint8_t y);
  bool isValidIndex(uint8_t index);
  
  // Color utilities
  CRGB getCurrentColor();
  CRGB getRandomColor();
  CRGB getRainbowColor(uint8_t position);
  CRGB validateColor(CRGB color);
}

// Implementation
namespace LedControl {
  
  inline bool isValidCoordinate(uint8_t x, uint8_t y) {
    return (x < MATRIX_WIDTH && y < MATRIX_HEIGHT);
  }
  
  inline bool isValidIndex(uint8_t index) {
    return (index < NUM_LEDS);
  }
  
  inline void setPixel(uint8_t x, uint8_t y, CRGB color) {
    if (isValidCoordinate(x, y)) {
      uint8_t idx = xyToIndex(x, y);
      if (isValidIndex(idx)) {
        leds[idx] = validateColor(color);
      }
    }
  }
  
  inline void setPixel(uint8_t index, CRGB color) {
    if (isValidIndex(index)) {
      leds[index] = validateColor(color);
    }
  }
  
  inline CRGB getPixel(uint8_t x, uint8_t y) {
    if (isValidCoordinate(x, y)) {
      uint8_t idx = xyToIndex(x, y);
      if (isValidIndex(idx)) {
        return leds[idx];
      }
    }
    return CRGB::Black;
  }
  
  inline CRGB getPixel(uint8_t index) {
    if (isValidIndex(index)) {
      return leds[index];
    }
    return CRGB::Black;
  }
  
  // Standard progressive layout: row by row from left to right
  inline uint8_t xyToIndex(uint8_t x, uint8_t y) {
    // Add bounds checking even in conversion
    if (!isValidCoordinate(x, y)) {
      return 0; // Return first LED as safe default
    }
    return (y * MATRIX_WIDTH) + x;
  }
  
  // Convert index back to x,y coordinates with validation
  inline void indexToXY(uint8_t index, uint8_t &x, uint8_t &y) {
    if (!isValidIndex(index)) {
      x = 0;
      y = 0;
      return;
    }
    y = index / MATRIX_WIDTH;
    x = index % MATRIX_WIDTH;
    
    // Additional validation
    if (!isValidCoordinate(x, y)) {
      x = 0;
      y = 0;
    }
  }
  
  inline void setRow(uint8_t row, uint8_t pattern, CRGB color) {
    if (row >= MATRIX_HEIGHT) return; // Bounds check
    
    for (uint8_t col = 0; col < MATRIX_WIDTH; col++) {
      // Bit 0 is right, Bit 7 is left. We need to reverse it.
      uint8_t bitPos = 7 - col;
      if (bitPos < 8) { // Additional safety check
        if (pattern & (1 << bitPos)) {
          setPixel(col, row, color);
        } else {
          setPixel(col, row, CRGB::Black);
        }
      }
    }
  }
  
  inline void setColumn(uint8_t col, uint8_t pattern, CRGB color) {
    if (col >= MATRIX_WIDTH) return; // Bounds check
    
    for (uint8_t row = 0; row < MATRIX_HEIGHT; row++) {
      // Bit 0 is top, Bit 7 is bottom.
      if (row < 8) { // Additional safety check
        if (pattern & (1 << row)) {
          setPixel(col, row, color);
        } else {
          setPixel(col, row, CRGB::Black);
        }
      }
    }
  }
  
  // Safe versions with full validation
  inline void safeSetRow(uint8_t row, uint8_t pattern, CRGB color) {
    if (row < MATRIX_HEIGHT) {
      color = validateColor(color);
      setRow(row, pattern, color);
    }
  }
  
  inline void safeSetColumn(uint8_t col, uint8_t pattern, CRGB color) {
    if (col < MATRIX_WIDTH) {
      color = validateColor(color);
      setColumn(col, pattern, color);
    }
  }
  
  inline void clearPanel() {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
  
  inline void allOn(CRGB color) {
    color = validateColor(color);
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
  }
  
  inline void safeFill(CRGB color) {
    color = validateColor(color);
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[i] = color;
    }
    FastLED.show();
  }
  
  inline void updatePanel() {
    FastLED.show();
  }
  
  inline void fadeOutEffect(uint8_t delay_ms) {
    uint8_t currentBrightness = FastLED.getBrightness();
    
    // Validate delay
    if (delay_ms > 100) delay_ms = 100; // Cap maximum delay
    
    for (int i = currentBrightness; i >= 0; i--) {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(delay_ms);
      yield(); // Prevent watchdog
    }
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    FastLED.setBrightness(currentBrightness);
  }

  inline CRGB getCurrentColor() {
    if (panelState.rainbowMode) {
      static uint8_t hue = 0;
      hue += 2;
      return CHSV(hue, 255, 255);
    }
    return validateColor(panelState.currentColor);
  }
  
  inline CRGB getRandomColor() {
    return CHSV(random8(), 255, 255);
  }
  
  inline CRGB getRainbowColor(uint8_t position) {
    // Ensure position is within valid range
    if (position >= NUM_LEDS) {
      position = position % NUM_LEDS;
    }
    return CHSV(position * 255 / NUM_LEDS, 255, 255);
  }
  
  inline CRGB validateColor(CRGB color) {
    // Ensure color values are within valid range (they already are for CRGB)
    // This is more for future expansion if needed
    return color;
  }
}

#endif // LED_CONTROL_H