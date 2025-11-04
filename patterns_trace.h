/*
 * Trace Patterns
 * Line and fill movements in all directions
 * VERSION 2.6 - Added comprehensive bounds checking and validation
 */

#ifndef PATTERNS_TRACE_H
#define PATTERNS_TRACE_H

#include "led_control.h"

extern PanelState panelState;
extern bool patternActive;

namespace Trace {
  
  // Trace up with bounds checking
  void traceUp(int type = 1) { // Fill=1, Line=2
    // Validate type
    if (type < 1 || type > 2) type = 1;
    
    CRGB color = LedControl::getCurrentColor();
    
    // Iterate safely through rows
    for (int row = MATRIX_HEIGHT - 1; row >= 0; row--) {
      yield(); // Prevent watchdog reset
      
      if (type == 2) {
        LedControl::clearPanel();
      }
      
      // Safe row setting with bounds check
      if (row >= 0 && row < MATRIX_HEIGHT) {
        LedControl::safeSetRow(row, 0xFF, color);
      }
      
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_MEDIUM);
    }
  }
  
  // Trace down with bounds checking
  void traceDown(int type = 1) { // Fill=1, Line=2
    // Validate type
    if (type < 1 || type > 2) type = 1;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (uint8_t row = 0; row < MATRIX_HEIGHT; row++) {
      yield(); // Prevent watchdog reset
      
      if (type == 2) {
        LedControl::clearPanel();
      }
      
      // Bounds check is implicit in loop condition
      LedControl::safeSetRow(row, 0xFF, color);
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_MEDIUM);
    }
  }
  
  // Trace right with bounds checking
  void traceRight(int type = 1) { // Fill=1, Line=2
    // Validate type
    if (type < 1 || type > 2) type = 1;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (uint8_t col = 0; col < MATRIX_WIDTH; col++) {
      yield(); // Prevent watchdog reset
      
      if (type == 2) {
        LedControl::clearPanel();
      }
      
      // Bounds check is implicit in loop condition
      LedControl::safeSetColumn(col, 0xFF, color);
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_MEDIUM);
    }
  }
  
  // Trace left with bounds checking
  void traceLeft(int type = 1) { // Fill=1, Line=2
    // Validate type
    if (type < 1 || type > 2) type = 1;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (int col = MATRIX_WIDTH - 1; col >= 0; col--) {
      yield(); // Prevent watchdog reset
      
      if (type == 2) {
        LedControl::clearPanel();
      }
      
      // Safe column setting with bounds check
      if (col >= 0 && col < MATRIX_WIDTH) {
        LedControl::safeSetColumn(col, 0xFF, color);
      }
      
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_MEDIUM);
    }
  }
  
  // Compress inward with comprehensive bounds checking
  void compressIn(int type = 1) { // 1=Fill, 2=Fill then Clear
    // Validate type
    if (type < 1 || type > 2) type = 1;
    
    CRGB color = LedControl::getCurrentColor();
    LedControl::clearPanel();
    
    // Calculate safe iteration limit
    uint8_t maxIterations = min(MATRIX_WIDTH / 2, MATRIX_HEIGHT / 2);
    
    for (uint8_t i = 0; i < maxIterations; i++) {
      yield(); // Prevent watchdog reset
      
      // Calculate safe boundaries
      uint8_t minX = i;
      uint8_t maxX = MATRIX_WIDTH - i;
      uint8_t minY = i;
      uint8_t maxY = MATRIX_HEIGHT - i;
      
      // Ensure we don't exceed matrix bounds
      if (minX >= MATRIX_WIDTH || minY >= MATRIX_HEIGHT) break;
      if (maxX <= minX || maxY <= minY) break;
      
      // Draw the perimeter with bounds checking
      for (uint8_t x = minX; x < maxX && x < MATRIX_WIDTH; x++) {
        LedControl::setPixel(x, minY, color);
        if (maxY - 1 < MATRIX_HEIGHT) {
          LedControl::setPixel(x, maxY - 1, color);
        }
      }
      
      for (uint8_t y = minY; y < maxY && y < MATRIX_HEIGHT; y++) {
        LedControl::setPixel(minX, y, color);
        if (maxX - 1 < MATRIX_WIDTH) {
          LedControl::setPixel(maxX - 1, y, color);
        }
      }
      
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_MEDIUM);
    }
    
    if (type == 2) {
      delay(500);
      
      // Clear from outside in
      for (int i = maxIterations - 1; i >= 0; i--) {
        yield(); // Prevent watchdog reset
        
        // Calculate safe boundaries
        uint8_t minX = i;
        uint8_t maxX = MATRIX_WIDTH - i;
        uint8_t minY = i;
        uint8_t maxY = MATRIX_HEIGHT - i;
        
        // Ensure we don't exceed matrix bounds
        if (minX >= MATRIX_WIDTH || minY >= MATRIX_HEIGHT) continue;
        if (maxX <= minX || maxY <= minY) continue;
        
        // Clear the perimeter with bounds checking
        for (uint8_t x = minX; x < maxX && x < MATRIX_WIDTH; x++) {
          LedControl::setPixel(x, minY, CRGB::Black);
          if (maxY - 1 < MATRIX_HEIGHT) {
            LedControl::setPixel(x, maxY - 1, CRGB::Black);
          }
        }
        
        for (uint8_t y = minY; y < maxY && y < MATRIX_HEIGHT; y++) {
          LedControl::setPixel(minX, y, CRGB::Black);
          if (maxX - 1 < MATRIX_WIDTH) {
            LedControl::setPixel(maxX - 1, y, CRGB::Black);
          }
        }
        
        LedControl::updatePanel();
        delay(ANIMATION_DELAY_MEDIUM);
      }
    }
  }

  // Explode outward with comprehensive bounds checking
  void explodeOut(int type = 1) { // 1=Fill, 2=Fill then Clear
    // Validate type
    if (type < 1 || type > 2) type = 1;
    
    CRGB color = LedControl::getCurrentColor();
    LedControl::clearPanel();
    
    // Calculate safe iteration limit
    uint8_t maxIterations = min(MATRIX_WIDTH / 2, MATRIX_HEIGHT / 2);
    
    for (int i = maxIterations - 1; i >= 0; i--) {
      yield(); // Prevent watchdog reset
      
      // Calculate safe boundaries
      uint8_t minX = i;
      uint8_t maxX = MATRIX_WIDTH - i;
      uint8_t minY = i;
      uint8_t maxY = MATRIX_HEIGHT - i;
      
      // Ensure we don't exceed matrix bounds
      if (minX >= MATRIX_WIDTH || minY >= MATRIX_HEIGHT) continue;
      if (maxX <= minX || maxY <= minY) continue;
      
      // Draw the perimeter with bounds checking
      for (uint8_t x = minX; x < maxX && x < MATRIX_WIDTH; x++) {
        LedControl::setPixel(x, minY, color);
        if (maxY - 1 < MATRIX_HEIGHT) {
          LedControl::setPixel(x, maxY - 1, color);
        }
      }
      
      for (uint8_t y = minY; y < maxY && y < MATRIX_HEIGHT; y++) {
        LedControl::setPixel(minX, y, color);
        if (maxX - 1 < MATRIX_WIDTH) {
          LedControl::setPixel(maxX - 1, y, color);
        }
      }
      
      LedControl::updatePanel();
      delay(ANIMATION_DELAY_MEDIUM);
    }
    
    if (type == 2) {
      delay(500);
      
      // Clear from inside out
      for (uint8_t i = 0; i < maxIterations; i++) {
        yield(); // Prevent watchdog reset
        
        // Calculate safe boundaries
        uint8_t minX = i;
        uint8_t maxX = MATRIX_WIDTH - i;
        uint8_t minY = i;
        uint8_t maxY = MATRIX_HEIGHT - i;
        
        // Ensure we don't exceed matrix bounds
        if (minX >= MATRIX_WIDTH || minY >= MATRIX_HEIGHT) break;
        if (maxX <= minX || maxY <= minY) break;
        
        // Clear the perimeter with bounds checking
        for (uint8_t x = minX; x < maxX && x < MATRIX_WIDTH; x++) {
          LedControl::setPixel(x, minY, CRGB::Black);
          if (maxY - 1 < MATRIX_HEIGHT) {
            LedControl::setPixel(x, maxY - 1, CRGB::Black);
          }
        }
        
        for (uint8_t y = minY; y < maxY && y < MATRIX_HEIGHT; y++) {
          LedControl::setPixel(minX, y, CRGB::Black);
          if (maxX - 1 < MATRIX_WIDTH) {
            LedControl::setPixel(maxX - 1, y, CRGB::Black);
          }
        }
        
        LedControl::updatePanel();
        delay(ANIMATION_DELAY_MEDIUM);
      }
    }
  }
}

// Main dispatcher function for this category with validation
void runTracePattern(int patternId) {
  // Validate pattern ID range
  if (!((patternId >= 8 && patternId <= 15) || 
        (patternId >= 48 && patternId <= 51))) {
    LedControl::clearPanel();
    return;
  }
  
  switch (patternId) {
    case 8: Trace::traceUp(1); break;
    case 9: Trace::traceUp(2); break;
    case 10: Trace::traceDown(1); break;
    case 11: Trace::traceDown(2); break;
    case 12: Trace::traceRight(1); break;
    case 13: Trace::traceRight(2); break;
    case 14: Trace::traceLeft(1); break;
    case 15: Trace::traceLeft(2); break;
    case 48: Trace::compressIn(1); break;
    case 49: Trace::compressIn(2); break;
    case 50: Trace::explodeOut(1); break;
    case 51: Trace::explodeOut(2); break;
    default:
      // This should never be reached due to validation above
      LedControl::clearPanel();
      break;
  }
}

#endif // PATTERNS_TRACE_H