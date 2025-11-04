/*
 * Basic Patterns
 * Simple on/off patterns and alerts
 * VERSION 2.6 - Enhanced error handling and bounds checking
 */

#ifndef PATTERNS_BASIC_H
#define PATTERNS_BASIC_H

#include "led_control.h"

// Forward declaration for function in main .ino file
extern void setPatternEndTime(unsigned long duration);
extern PanelState panelState;
extern bool patternActive;

namespace Basic {
  
  // Basic on/off patterns with proper duration handling and validation
  void allOnTimed(unsigned long duration = 0) {
    // Validate and cap duration
    if (duration > MAX_PATTERN_DURATION) {
      duration = MAX_PATTERN_DURATION;
    }
    
    LedControl::allOn(LedControl::getCurrentColor());
    
    if (duration > 0) {
      setPatternEndTime(duration);
    } else {
      setPatternEndTime(3600000); // 1 hour default
    }
  }
  
  // Toggle top and bottom halves with bounds checking
  void toggle(int repeats = 10) {
    // Validate repeats
    if (repeats < 1) repeats = 1;
    if (repeats > 100) repeats = 100; // Cap maximum repeats
    
    CRGB color = LedControl::getCurrentColor();
    
    for (int i = 0; i < repeats; i++) {
      yield(); // Prevent watchdog reset
      
      // Clear panel safely
      LedControl::clearPanel();
      
      // Light top half with bounds checking
      for (uint8_t row = 0; row < 4 && row < MATRIX_HEIGHT; row++) {
        LedControl::safeSetRow(row, B11111111, color);
      }
      LedControl::updatePanel();
      delay(500);
      
      // Clear panel safely
      LedControl::clearPanel();
      
      // Light bottom half with bounds checking
      for (uint8_t row = 4; row < 8 && row < MATRIX_HEIGHT; row++) {
        LedControl::safeSetRow(row, B11111111, color);
      }
      LedControl::updatePanel();
      delay(500);
    }
  }
  
  // Alert pattern - rapid flashing with watchdog protection and validation
  void alert(unsigned long duration) {
    // Validate duration
    if (duration == 0) duration = 1000; // Minimum 1 second
    if (duration > MAX_PATTERN_DURATION) {
      duration = MAX_PATTERN_DURATION;
    }
    
    unsigned long startTime = millis();
    unsigned long elapsedTime = 0;
    CRGB color = LedControl::getCurrentColor();
    bool ledState = false;
    
    while(elapsedTime < duration) {
      yield(); // Prevent watchdog reset
      
      if (ledState) {
        LedControl::allOn(color);
      } else {
        LedControl::clearPanel();
      }
      ledState = !ledState;
      
      delay(ANIMATION_DELAY_FAST);
      
      // Update elapsed time with overflow protection
      unsigned long currentTime = millis();
      if (currentTime >= startTime) {
        elapsedTime = currentTime - startTime;
      } else {
        // Handle millis() overflow
        elapsedTime = duration; // Exit loop
      }
    }
    
    // Ensure LEDs are off at the end
    LedControl::clearPanel();
  }
}

// Main dispatcher function for this category with validation
void runBasicPattern(int patternId) {
  // Validate pattern ID
  if (patternId < 0 || patternId > 7) {
    LedControl::clearPanel();
    return;
  }
  
  switch (patternId) {
    case 0: 
      LedControl::clearPanel(); 
      break;
    case 1: 
      Basic::allOnTimed(0); 
      break;
    case 2: 
      Basic::allOnTimed(2000); 
      break;
    case 3: 
      Basic::allOnTimed(5000); 
      break;
    case 4: 
      Basic::allOnTimed(10000); 
      break;
    case 5: 
      Basic::toggle(panelState.alwaysOn ? 2 : 5); 
      break;
    case 6: 
      Basic::alert(4000); 
      break;
    case 7: 
      Basic::alert(10000); 
      break;
    default:
      // This should never be reached due to validation above
      LedControl::clearPanel();
      break;
  }
  
  if (!panelState.alwaysOn && !patternActive) {
    LedControl::clearPanel();
  }
}

#endif // PATTERNS_BASIC_H