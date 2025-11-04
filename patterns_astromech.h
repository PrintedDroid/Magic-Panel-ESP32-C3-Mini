/*
 * Astromech-Specific Patterns
 * R2-D2 PSI and community patterns for Magic Panel
 */

#ifndef PATTERNS_ASTROMECH_H
#define PATTERNS_ASTROMECH_H

#include "led_control.h"

// Forward declarations
extern PanelState panelState;
extern bool patternActive;
extern void setPatternEndTime(unsigned long duration);

namespace Astromech {
  
  // PSI color states for R2-D2 processor indicators
  enum PSIColors {
    PSI_RED = 0,
    PSI_BLUE = 1,
    PSI_GREEN = 2,
    PSI_YELLOW = 3,
    PSI_CYAN = 4,
    PSI_MAGENTA = 5,
    PSI_WHITE = 6
  };
  
  // Get PSI color based on state
  CRGB getPSIColor(uint8_t colorState) {
    switch(colorState % 7) {
      case PSI_RED: return CRGB::Red;
      case PSI_BLUE: return CRGB::Blue;
      case PSI_GREEN: return CRGB::Green;
      case PSI_YELLOW: return CRGB::Yellow;
      case PSI_CYAN: return CRGB::Cyan;
      case PSI_MAGENTA: return CRGB::Magenta;
      case PSI_WHITE: return CRGB::White;
      default: return CRGB::Blue;
    }
  }
  
  // PSI Solid Color Display (100-106)
  void psiSolid(uint8_t colorState) {
    CRGB color = getPSIColor(colorState);
    LedControl::allOn(color);
    setPatternEndTime(5000); // Display for 5 seconds
  }
  
  // PSI Color Wipe (107-108)
  void psiColorWipe(int direction = 1) {
    static uint8_t currentColor = 0;
    CRGB color = getPSIColor(currentColor);
    
    if (direction == 1) {
      // Wipe down
      for (uint8_t row = 0; row < MATRIX_HEIGHT; row++) {
        LedControl::safeSetRow(row, 0xFF, color);
        LedControl::updatePanel();
        delay(50);
        yield();
      }
    } else {
      // Wipe up
      for (int row = MATRIX_HEIGHT - 1; row >= 0; row--) {
        if (row >= 0 && row < MATRIX_HEIGHT) {
          LedControl::safeSetRow(row, 0xFF, color);
          LedControl::updatePanel();
          delay(50);
        }
        yield();
      }
    }
    
    currentColor = (currentColor + 1) % 7;
    delay(500);
    
    // Fade out
    for (int i = 0; i < MATRIX_HEIGHT; i++) {
      LedControl::safeSetRow(i, 0x00, CRGB::Black);
      LedControl::updatePanel();
      delay(30);
      yield();
    }
  }
  
  // PSI Random Flicker (109)
  void psiRandomFlicker(unsigned long duration = 5000) {
    if (duration == 0) duration = 5000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    
    while (millis() - startTime < duration) {
      yield();
      
      // Random color from PSI palette
      CRGB color = getPSIColor(random(7));
      
      // Random intensity
      uint8_t brightness = random(50, 255);
      color.nscale8(brightness);
      
      // Random pixels
      for (int i = 0; i < random(5, 20); i++) {
        uint8_t pixel = random(NUM_LEDS);
        if (LedControl::isValidIndex(pixel)) {
          leds[pixel] = color;
        }
      }
      
      LedControl::updatePanel();
      delay(random(30, 100));
      
      // Fade all pixels
      for (int i = 0; i < NUM_LEDS; i++) {
        if (LedControl::isValidIndex(i)) {
          leds[i].fadeToBlackBy(random(20, 80));
        }
      }
    }
    
    LedControl::clearPanel();
  }
  
  // PSI Pulse (110)
  void psiPulse(unsigned long duration = 5000) {
    if (duration == 0) duration = 5000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    uint8_t colorIndex = 0;
    
    while (millis() - startTime < duration) {
      yield();
      
      CRGB color = getPSIColor(colorIndex);
      
      // Pulse in
      for (int brightness = 0; brightness < 255; brightness += 5) {
        CRGB fadedColor = color;
        fadedColor.nscale8(brightness);
        LedControl::safeFill(fadedColor);
        delay(5);
        yield();
      }
      
      // Pulse out
      for (int brightness = 255; brightness >= 0; brightness -= 5) {
        CRGB fadedColor = color;
        fadedColor.nscale8(brightness);
        LedControl::safeFill(fadedColor);
        delay(5);
        yield();
      }
      
      colorIndex = (colorIndex + 1) % 7;
      delay(200);
    }
    
    LedControl::clearPanel();
  }
  
  // PSI Rainbow (111)
  void psiRainbow(unsigned long duration = 5000) {
    if (duration == 0) duration = 5000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    uint8_t hue = 0;
    
    while (millis() - startTime < duration) {
      yield();
      
      for (int i = 0; i < NUM_LEDS; i++) {
        if (LedControl::isValidIndex(i)) {
          leds[i] = CHSV(hue + (i * 2), 255, 255);
        }
      }
      
      LedControl::updatePanel();
      hue += 3;
      delay(20);
    }
    
    LedControl::clearPanel();
  }
  
  // PSI March (112-116)
  void psiMarch(uint8_t variant = 0, unsigned long duration = 5000) {
    if (duration == 0) duration = 5000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    uint8_t position = 0;
    uint8_t colorIndex = variant % 5; // Different starting colors for variants
    
    while (millis() - startTime < duration) {
      yield();
      
      LedControl::clearPanel();
      
      switch (variant) {
        case 0: // Horizontal march
          for (uint8_t col = 0; col < MATRIX_WIDTH; col++) {
            CRGB color = getPSIColor((colorIndex + col) % 7);
            if ((col + position) % 3 == 0) {
              LedControl::safeSetColumn(col, 0xFF, color);
            }
          }
          break;
          
        case 1: // Vertical march
          for (uint8_t row = 0; row < MATRIX_HEIGHT; row++) {
            CRGB color = getPSIColor((colorIndex + row) % 7);
            if ((row + position) % 3 == 0) {
              LedControl::safeSetRow(row, 0xFF, color);
            }
          }
          break;
          
        case 2: // Diagonal march
          for (uint8_t i = 0; i < NUM_LEDS; i++) {
            uint8_t x, y;
            LedControl::indexToXY(i, x, y);
            if (((x + y + position) % 4) == 0) {
              leds[i] = getPSIColor((x + y) % 7);
            }
          }
          break;
          
        case 3: // Checkerboard march
          for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
            for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
              if (((x + y + position) % 2) == 0) {
                LedControl::setPixel(x, y, getPSIColor((position) % 7));
              }
            }
          }
          break;
          
        case 4: // Spiral march
          {
            uint8_t spiralOrder[NUM_LEDS];
            uint8_t idx = 0;
            uint8_t left = 0, right = MATRIX_WIDTH - 1;
            uint8_t top = 0, bottom = MATRIX_HEIGHT - 1;
            
            // Generate spiral order
            while (left <= right && top <= bottom && idx < NUM_LEDS) {
              // Top row
              for (uint8_t x = left; x <= right && idx < NUM_LEDS; x++) {
                spiralOrder[idx++] = LedControl::xyToIndex(x, top);
              }
              top++;
              
              // Right column
              for (uint8_t y = top; y <= bottom && idx < NUM_LEDS; y++) {
                spiralOrder[idx++] = LedControl::xyToIndex(right, y);
              }
              right--;
              
              // Bottom row
              if (top <= bottom) {
                for (int x = right; x >= left && idx < NUM_LEDS; x--) {
                  spiralOrder[idx++] = LedControl::xyToIndex(x, bottom);
                }
                bottom--;
              }
              
              // Left column
              if (left <= right) {
                for (int y = bottom; y >= top && idx < NUM_LEDS; y--) {
                  spiralOrder[idx++] = LedControl::xyToIndex(left, y);
                }
                left++;
              }
            }
            
            // Light up spiral with marching colors
            for (uint8_t i = 0; i < NUM_LEDS; i++) {
              if ((i + position) % 8 < 3) {
                if (LedControl::isValidIndex(spiralOrder[i])) {
                  leds[spiralOrder[i]] = getPSIColor((i / 8) % 7);
                }
              }
            }
          }
          break;
      }
      
      LedControl::updatePanel();
      position++;
      delay(150);
    }
    
    LedControl::clearPanel();
  }
  
  // R2-D2 Communication Pattern - rapid color changes simulating data transfer
  void r2Communication(unsigned long duration = 3000) {
    if (duration == 0) duration = 3000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    
    while (millis() - startTime < duration) {
      yield();
      
      // Simulate data packets with different colors
      for (int packet = 0; packet < 4; packet++) {
        CRGB color = getPSIColor(random(7));
        
        // Random pattern to simulate data
        for (int i = 0; i < NUM_LEDS; i++) {
          if (random(100) < 30) {
            leds[i] = color;
          } else {
            leds[i] = CRGB::Black;
          }
        }
        
        LedControl::updatePanel();
        delay(50);
      }
      
      // Brief pause between transmissions
      LedControl::clearPanel();
      delay(100);
    }
  }
  
  // R2-D2 Thinking Pattern - gentle swirling colors
  void r2Thinking(unsigned long duration = 5000) {
    if (duration == 0) duration = 5000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    float angle = 0;
    
    while (millis() - startTime < duration) {
      yield();
      
      for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
          // Create swirling pattern
          float distance = sqrt((x - 3.5) * (x - 3.5) + (y - 3.5) * (y - 3.5));
          uint8_t hue = (uint8_t)(distance * 30 + angle) % 255;
          uint8_t brightness = 100 + 155 * sin(distance / 2 - angle / 50);
          
          if (LedControl::isValidCoordinate(x, y)) {
            LedControl::setPixel(x, y, CHSV(hue, 200, brightness));
          }
        }
      }
      
      LedControl::updatePanel();
      angle += 0.1;
      delay(30);
    }
    
    LedControl::clearPanel();
  }
  
  // R2-D2 Alert Pattern - urgent flashing
  void r2Alert(unsigned long duration = 3000) {
    if (duration == 0) duration = 3000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    bool state = false;
    
    while (millis() - startTime < duration) {
      yield();
      
      if (state) {
        // Flash red and white alternating quadrants
        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
          for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
            bool isTopLeft = (x < 4 && y < 4);
            bool isBottomRight = (x >= 4 && y >= 4);
            
            if (isTopLeft || isBottomRight) {
              LedControl::setPixel(x, y, CRGB::Red);
            } else {
              LedControl::setPixel(x, y, CRGB::White);
            }
          }
        }
      } else {
        LedControl::clearPanel();
      }
      
      state = !state;
      LedControl::updatePanel();
      delay(100);
    }
    
    LedControl::clearPanel();
  }
}

// Main dispatcher function for Astromech patterns
void runAstromechPattern(int patternId) {
  // Validate pattern ID range (100-119 reserved for Astromech)
  if (patternId < 100 || patternId > 119) {
    LedControl::clearPanel();
    return;
  }
  
  switch (patternId) {
    // PSI Solid Colors (100-106)
    case 100: Astromech::psiSolid(0); break; // Red
    case 101: Astromech::psiSolid(1); break; // Blue
    case 102: Astromech::psiSolid(2); break; // Green
    case 103: Astromech::psiSolid(3); break; // Yellow
    case 104: Astromech::psiSolid(4); break; // Cyan
    case 105: Astromech::psiSolid(5); break; // Magenta
    case 106: Astromech::psiSolid(6); break; // White
    
    // PSI Animations (107-116)
    case 107: Astromech::psiColorWipe(1); break;  // Wipe down
    case 108: Astromech::psiColorWipe(2); break;  // Wipe up
    case 109: Astromech::psiRandomFlicker(); break;
    case 110: Astromech::psiPulse(); break;
    case 111: Astromech::psiRainbow(); break;
    case 112: Astromech::psiMarch(0); break; // Horizontal
    case 113: Astromech::psiMarch(1); break; // Vertical
    case 114: Astromech::psiMarch(2); break; // Diagonal
    case 115: Astromech::psiMarch(3); break; // Checkerboard
    case 116: Astromech::psiMarch(4); break; // Spiral
    
    // R2-D2 Special Patterns (117-119)
    case 117: Astromech::r2Communication(); break;
    case 118: Astromech::r2Thinking(); break;
    case 119: Astromech::r2Alert(); break;
    
    default:
      LedControl::clearPanel();
      break;
  }
  
  if (!panelState.alwaysOn && !patternActive) {
    LedControl::clearPanel();
  }
}

#endif // PATTERNS_ASTROMECH_H