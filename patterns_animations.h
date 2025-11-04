/*
 * Animation Patterns
 * Complex animations like Cylon, loops, VU meters, Game of Life, and more.
 * VERSION 2.6 - Fixed memory management, bounds checking, and stack usage
 */

#ifndef PATTERNS_ANIMATIONS_H
#define PATTERNS_ANIMATIONS_H

#include "led_control.h"
#include "patterns_shapes.h"

// Forward declarations for global variables and functions
extern PanelState panelState;
extern bool patternActive;

// Static buffers for animations to avoid stack allocation - FIXED
static byte gameOfLifeWorld[NUM_LEDS];
static byte gameOfLifeNextWorld[NUM_LEDS];
static bool gameOfLifeInitialized = false;

namespace Animations {

  // Conway's Game of Life with proper memory management - FIXED
  void gameOfLife(int generations = 50) {
    // Validate generations
    if (generations < 1) generations = 1;
    if (generations > 200) generations = 200;

    // Initialize world with random pattern if needed
    if (!gameOfLifeInitialized) {
      for(int i = 0; i < NUM_LEDS && i < sizeof(gameOfLifeWorld); i++) {
        gameOfLifeWorld[i] = random(0, 2);
      }
      gameOfLifeInitialized = true;
    } else {
      // Re-randomize for subsequent calls
      for(int i = 0; i < NUM_LEDS && i < sizeof(gameOfLifeWorld); i++) {
        gameOfLifeWorld[i] = random(0, 2);
      }
    }

    CRGB color = LedControl::getCurrentColor();

    for(int gen = 0; gen < generations; gen++) {
      yield(); // Prevent watchdog reset
      
      // Process each cell with comprehensive bounds checking
      for(int i = 0; i < NUM_LEDS; i++) {
        uint8_t x, y;
        LedControl::indexToXY(i, x, y);
        
        // Validate coordinates
        if (!LedControl::isValidCoordinate(x, y)) continue;

        int neighbors = 0;
        
        // Check all 8 neighboring cells with bounds checking
        for(int dx = -1; dx <= 1; dx++) {
          for(int dy = -1; dy <= 1; dy++) {
            if(dx == 0 && dy == 0) continue;
            
            // Calculate neighbor position with wrapping
            int nx = (x + dx + MATRIX_WIDTH) % MATRIX_WIDTH;
            int ny = (y + dy + MATRIX_HEIGHT) % MATRIX_HEIGHT;
            
            // Additional validation
            if (nx >= 0 && nx < MATRIX_WIDTH && ny >= 0 && ny < MATRIX_HEIGHT) {
              uint8_t neighborIndex = LedControl::xyToIndex(nx, ny);
              if (neighborIndex < NUM_LEDS && neighborIndex < sizeof(gameOfLifeWorld)) {
                if(gameOfLifeWorld[neighborIndex] == 1) {
                  neighbors++;
                }
              }
            }
          }
        }

        // Apply Conway's rules with bounds checking
        if (i < sizeof(gameOfLifeNextWorld)) {
          if(gameOfLifeWorld[i] == 1 && (neighbors < 2 || neighbors > 3)) { 
            gameOfLifeNextWorld[i] = 0; // Death
          } else if (gameOfLifeWorld[i] == 0 && neighbors == 3) { 
            gameOfLifeNextWorld[i] = 1; // Birth
          } else { 
            gameOfLifeNextWorld[i] = gameOfLifeWorld[i]; // Survival
          }
        }
      }

      // Update world and count living cells with bounds checking
      int liveCells = 0;
      for(int i = 0; i < NUM_LEDS; i++) {
        if (i < sizeof(gameOfLifeWorld) && i < sizeof(gameOfLifeNextWorld)) {
          gameOfLifeWorld[i] = gameOfLifeNextWorld[i];
          
          if (LedControl::isValidIndex(i)) {
            if(gameOfLifeWorld[i] == 1) { 
              leds[i] = color; 
              liveCells++; 
            } else { 
              leds[i] = CRGB::Black; 
            }
          }
        }
      }
      LedControl::updatePanel();

      // Stop if no living cells remain
      if(liveCells == 0) break;
      delay(ANIMATION_DELAY_MEDIUM);
    }
  }

  // Matrix "Digital Rain" effect with bounds checking - FIXED
  void matrixRain(unsigned long duration) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    int head[MATRIX_WIDTH];
    
    // Initialize column heads safely
    for(int i = 0; i < MATRIX_WIDTH && i < sizeof(head)/sizeof(head[0]); i++) { 
      head[i] = -1; 
    }

    while(millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      for(int i = 0; i < MATRIX_WIDTH && i < sizeof(head)/sizeof(head[0]); i++) {
        // Randomly start new drops
        if(head[i] == -1 && random(20) == 0) {
          head[i] = 0;
        }
        
        // Move existing drops down with bounds checking
        if(head[i] >= 0 && head[i] < MATRIX_HEIGHT) {
          if (LedControl::isValidCoordinate(i, head[i])) {
            LedControl::setPixel(i, head[i], LedControl::getCurrentColor());
          }
          head[i]++;
          if(head[i] >= MATRIX_HEIGHT) {
            head[i] = -1;
          }
        } else if (head[i] >= MATRIX_HEIGHT) {
          head[i] = -1;
        }
      }
      
      LedControl::updatePanel();
      
      // Fade effect with bounds checking
      for (int i = 0; i < NUM_LEDS; i++) {
        if (LedControl::isValidIndex(i)) {
          leds[i].fadeToBlackBy(30);
        }
      }
      delay(60);
    }
  }
  
  // Rotating 3D Cube animation with comprehensive bounds checking - FIXED
  namespace Cube3D {
    struct Point3D { float x, y, z; };
    struct Point2D { int x, y; };

    void drawLine(int x1, int y1, int x2, int y2, CRGB color) {
      // Validate endpoints
      if (!LedControl::isValidCoordinate(x1, y1) && !LedControl::isValidCoordinate(x2, y2)) {
        return; // Both points outside bounds
      }
      
      // Clip line to matrix bounds (simple clipping)
      x1 = constrain(x1, 0, MATRIX_WIDTH - 1);
      y1 = constrain(y1, 0, MATRIX_HEIGHT - 1);
      x2 = constrain(x2, 0, MATRIX_WIDTH - 1);
      y2 = constrain(y2, 0, MATRIX_HEIGHT - 1);
      
      int dx = abs(x2 - x1);
      int dy = abs(y2 - y1);
      int sx = (x1 < x2) ? 1 : -1;
      int sy = (y1 < y2) ? 1 : -1;
      int err = dx - dy;
      
      while (true) {
        // Only set pixel if within bounds
        if (LedControl::isValidCoordinate(x1, y1)) {
          LedControl::setPixel(x1, y1, color);
        }
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) { 
          err -= dy; 
          x1 += sx; 
        }
        if (e2 < dx) { 
          err += dx; 
          y1 += sy; 
        }
        
        // Additional bounds check for safety
        if (x1 < 0 || x1 >= MATRIX_WIDTH || y1 < 0 || y1 >= MATRIX_HEIGHT) {
          break;
        }
      }
    }
    
    void render(unsigned long duration) {
      // Validate duration
      if (duration == 0) duration = 1000;
      if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
      
      Point3D v[8] = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
        {-1,-1,1}, {1,-1,1}, {1,1,1}, {-1,1,1}
      };
      
      unsigned long startTime = millis();
      float angle = 0;
      
      while(millis() - startTime < duration) {
        yield(); // Prevent watchdog reset
        
        angle += 0.05;
        if (angle > 2 * PI) angle -= 2 * PI; // Prevent overflow
        
        Point2D p[8];
        
        // Project 3D points to 2D with bounds checking
        for(int i = 0; i < 8; i++) {
          float rz = v[i].z * cos(angle * 0.5) - v[i].x * sin(angle * 0.5);
          float rx = v[i].z * sin(angle * 0.5) + v[i].x * cos(angle * 0.5);
          float ry = v[i].y;

          float rz2 = rz * cos(angle) - ry * sin(angle);
          float ry2 = rz * sin(angle) + ry * cos(angle);
          
          float perspective = 2.0 / (3.0 - rz2);
          
          // Calculate and constrain coordinates
          int projX = (int)((rx * perspective * 3) + 3.5);
          int projY = (int)((ry2 * perspective * 3) + 3.5);
          
          // Ensure coordinates are within reasonable bounds
          p[i].x = constrain(projX, -MATRIX_WIDTH, MATRIX_WIDTH * 2);
          p[i].y = constrain(projY, -MATRIX_HEIGHT, MATRIX_HEIGHT * 2);
        }
        
        LedControl::clearPanel();
        CRGB color = LedControl::getCurrentColor();
        
        // Draw cube edges with bounds-safe line drawing
        for(int i = 0; i < 4; i++) {
          int next = (i + 1) % 4;
          drawLine(p[i].x, p[i].y, p[next].x, p[next].y, color);
          drawLine(p[i+4].x, p[i+4].y, p[next+4].x, p[next+4].y, color);
          drawLine(p[i].x, p[i].y, p[i+4].x, p[i+4].y, color);
        }
        
        LedControl::updatePanel();
        delay(ANIMATION_DELAY_VERY_FAST);
      }
    }
  }

  // Gaming animations with proper bounds checking
  void pacmanAnimation(unsigned long duration) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    
    while(millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      for(int x = -7; x < 9; x++) {
        LedControl::clearPanel();
        
        // Animate Pac-Man with mouth opening/closing
        // Only draw if sprite would be visible
        if (x >= -7 && x <= 8) {
          Shapes::drawSprite(x, 0, 
            (millis() / 200 % 2 == 0) ? Shapes::pacman_open : Shapes::pacman_closed, 
            CRGB::Yellow);
        }
        
        // Ghost following behind (ensure it's visible)
        int ghostX = x - 5;
        if (ghostX >= -7 && ghostX <= 8) {
          Shapes::drawSprite(ghostX, 0, Shapes::ghost, CRGB::Red);
        }
        
        LedControl::updatePanel();
        delay(120);
      }
    }
  }

  void invadersAnimation(unsigned long duration) {
    // Validate duration
    if (duration == 0) duration = 1000;
    if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
    
    unsigned long startTime = millis();
    int inv_x = 1, inv_y = 0, dir = 1;
    
    while(millis() - startTime < duration) {
      yield(); // Prevent watchdog reset
      
      LedControl::clearPanel();
      
      // Draw invader with bounds checking
      if (inv_x >= -7 && inv_x <= 8 && inv_y >= -7 && inv_y <= 8) {
        Shapes::drawSprite(inv_x, inv_y, Shapes::invader, LedControl::getCurrentColor());
      }
      
      LedControl::updatePanel();
      
      inv_x += dir;
      if(inv_x > 2 || inv_x < 0) {
        dir *= -1;
        inv_y++;
        if(inv_y > 2) inv_y = 0;
      }
      delay(300);
    }
  }

  // Existing animations with watchdog protection and bounds checking
  void cylonCol(int repeats = 2, int scanDelay = 80) {
    // Validate parameters
    if (repeats < 1) repeats = 1;
    if (repeats > 20) repeats = 20;
    if (scanDelay < 10) scanDelay = 10;
    if (scanDelay > 500) scanDelay = 500;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (int i = 0; i < repeats; i++) {
      yield(); // Prevent watchdog reset
      
      // Forward scan
      for (uint8_t col = 0; col < MATRIX_WIDTH; col++) { 
        LedControl::clearPanel(); 
        LedControl::safeSetColumn(col, 0xFF, color); 
        LedControl::updatePanel(); 
        delay(scanDelay); 
      }
      
      // Backward scan
      for (int col = MATRIX_WIDTH - 2; col > 0; col--) { 
        if (col >= 0 && col < MATRIX_WIDTH) {
          LedControl::clearPanel(); 
          LedControl::safeSetColumn(col, 0xFF, color); 
          LedControl::updatePanel(); 
          delay(scanDelay);
        }
      }
    }
  }
  
  void cylonRow(int repeats = 2, int scanDelay = 80) {
    // Validate parameters
    if (repeats < 1) repeats = 1;
    if (repeats > 20) repeats = 20;
    if (scanDelay < 10) scanDelay = 10;
    if (scanDelay > 500) scanDelay = 500;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (int i = 0; i < repeats; i++) {
      yield(); // Prevent watchdog reset
      
      // Forward scan
      for (uint8_t row = 0; row < MATRIX_HEIGHT; row++) { 
        LedControl::clearPanel(); 
        LedControl::safeSetRow(row, 0xFF, color); 
        LedControl::updatePanel(); 
        delay(scanDelay); 
      }
      
      // Backward scan
      for (int row = MATRIX_HEIGHT - 2; row > 0; row--) { 
        if (row >= 0 && row < MATRIX_HEIGHT) {
          LedControl::clearPanel(); 
          LedControl::safeSetRow(row, 0xFF, color); 
          LedControl::updatePanel(); 
          delay(scanDelay);
        }
      }
    }
  }
  
  void eyeScan(int repeats = 1, int scanDelay = 70) {
    // Validate parameters
    if (repeats < 1) repeats = 1;
    if (repeats > 20) repeats = 20;
    if (scanDelay < 10) scanDelay = 10;
    if (scanDelay > 500) scanDelay = 500;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (int i = 0; i < repeats; i++) {
      yield(); // Prevent watchdog reset
      
      // Scan rows
      for (uint8_t row = 0; row < MATRIX_HEIGHT; row++) { 
        LedControl::safeSetRow(row, 0xFF, color); 
        LedControl::updatePanel(); 
        delay(scanDelay); 
        LedControl::safeSetRow(row, 0x00); 
      }
      
      LedControl::clearPanel(); 
      delay(scanDelay);
      
      // Scan columns
      for (int col = MATRIX_WIDTH - 1; col >= 0; col--) { 
        if (col >= 0 && col < MATRIX_WIDTH) {
          LedControl::safeSetColumn(col, 0xFF, color); 
          LedControl::updatePanel(); 
          delay(scanDelay); 
          LedControl::safeSetColumn(col, 0x00);
        }
      }
      
      LedControl::clearPanel(); 
      delay(scanDelay);
    }
  }
  
  void oneLoop(int repeats = 2) {
    // Validate repeats
    if (repeats < 1) repeats = 1;
    if (repeats > 20) repeats = 20;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (int j = 0; j < repeats; j++) {
      yield(); // Prevent watchdog reset
      
      for (int i = 0; i < 28; i++) {
        LedControl::clearPanel();
        uint8_t x, y;
        
        // Calculate position around the perimeter with bounds checking
        if (i < 8) { 
          x = i; y = 0; 
        } else if (i < 15) { 
          x = 7; y = i - 7; 
        } else if (i < 22) { 
          x = 7 - (i - 15); y = 7;
        } else { 
          x = 0; y = 7 - (i - 22); 
        }
        
        // Validate and set pixel
        if (LedControl::isValidCoordinate(x, y)) {
          LedControl::setPixel(x, y, color);
        }
        
        LedControl::updatePanel();
        delay(ANIMATION_DELAY_FAST);
      }
    }
  }

  void twoLoop(int repeats = 2) {
    // Validate repeats
    if (repeats < 1) repeats = 1;
    if (repeats > 20) repeats = 20;
    
    CRGB color = LedControl::getCurrentColor();
    
    for (int j = 0; j < repeats; j++) {
      yield(); // Prevent watchdog reset
      
      for (int i = 0; i < 28; i++) {
        LedControl::clearPanel();
        uint8_t x1, y1, x2, y2;
        
        auto path = [&](uint8_t p, uint8_t& x, uint8_t& y){
          if (p < 8) { 
            x = p; y = 0; 
          } else if (p < 15) { 
            x = 7; y = p - 7; 
          } else if (p < 22) { 
            x = 7 - (p - 15); y = 7;
          } else if (p < 28) { 
            x = 0; y = 7 - (p - 22); 
          } else {
            x = 0; y = 0; // Safety fallback
          }
        };
        
        path(i % 28, x1, y1);
        path((i + 14) % 28, x2, y2);
        
        // Validate and set pixels
        if (LedControl::isValidCoordinate(x1, y1)) {
          LedControl::setPixel(x1, y1, color);
        }
        if (LedControl::isValidCoordinate(x2, y2)) {
          LedControl::setPixel(x2, y2, color);
        }
        
        LedControl::updatePanel();
        delay(ANIMATION_DELAY_FAST);
      }
    }
  }
  
  // VU Meter with comprehensive bounds checking - FIXED
  void vuMeter(int loops = 15, int type = 1) {
    // Validate parameters
    if (loops < 1) loops = 1;
    if (loops > 50) loops = 50;
    if (type < 1 || type > 4) type = 1;
    
    CRGB color = LedControl::getCurrentColor();
    int level[MATRIX_WIDTH];
    
    // Initialize random levels safely
    for(int i = 0; i < MATRIX_WIDTH && i < sizeof(level)/sizeof(level[0]); i++) {
      level[i] = random(0, 9);
    }
    
    for (int count = 0; count < loops; count++) {
      yield(); // Prevent watchdog reset
      
      for (int i = 0; i < MATRIX_WIDTH && i < sizeof(level)/sizeof(level[0]); i++) {
        // Ensure level is within valid range
        if (level[i] < 0) level[i] = 0;
        if (level[i] > 8) level[i] = 8;
        
        uint8_t bar = (1 << level[i]) - 1;
        if(type == 3 || type == 4) bar = ~bar;
        
        // Set column or row with bounds checking
        if(type % 2 != 0) {
          if (i < MATRIX_WIDTH) {
            LedControl::safeSetColumn(i, bar, color);
          }
        } else {
          if (i < MATRIX_HEIGHT) {
            LedControl::safeSetRow(i, bar, color);
          }
        }
      }
      
      LedControl::updatePanel();
      delay(150);
      
      // Update levels randomly with bounds checking
      for (int i = 0; i < MATRIX_WIDTH && i < sizeof(level)/sizeof(level[0]); i++) {
        int newLevel = level[i] + random(-2, 3);
        level[i] = constrain(newLevel, 0, 8);
      }
    }
  }
}

// Main dispatcher function for this category with validation
void runAnimationPattern(int patternId) {
  // Validate pattern ID
  if (!((patternId >= 21 && patternId <= 23) ||
        (patternId >= 29 && patternId <= 30) ||
        (patternId >= 52 && patternId <= 55) ||
        (patternId >= 61 && patternId <= 63) ||
        (patternId >= 67 && patternId <= 68))) {
    LedControl::clearPanel();
    return;
  }
  
  unsigned long duration = panelState.alwaysOn ? 15000 : 8000;
  int repeats = panelState.alwaysOn ? 10 : 2;
  
  // Validate parameters
  if (duration > MAX_PATTERN_DURATION) duration = MAX_PATTERN_DURATION;
  if (repeats < 1) repeats = 1;
  if (repeats > 20) repeats = 20;

  switch (patternId) {
    case 21: 
      Animations::cylonCol(repeats); 
      break;
    case 22: 
      Animations::cylonRow(repeats); 
      break;
    case 23: 
      Animations::eyeScan(repeats); 
      break;
    case 29: 
      Animations::twoLoop(repeats); 
      break;
    case 30: 
      Animations::oneLoop(repeats); 
      break;
    case 52: 
      Animations::vuMeter(repeats, 1); 
      break;
    case 53: 
      Animations::vuMeter(repeats, 2); 
      break;
    case 54: 
      Animations::vuMeter(repeats, 3); 
      break;
    case 55: 
      Animations::vuMeter(repeats, 4); 
      break;
    case 61: 
      Animations::gameOfLife(repeats * 5); 
      break;
    case 62: 
      Animations::matrixRain(duration); 
      break;
    case 63: 
      Animations::Cube3D::render(duration); 
      break;
    case 67: 
      Animations::pacmanAnimation(duration); 
      break;
    case 68: 
      Animations::invadersAnimation(duration); 
      break;
    default:
      LedControl::clearPanel();
      break;
  }
  
  if (!panelState.alwaysOn) {
    LedControl::clearPanel();
  }
}

#endif // PATTERNS_ANIMATIONS_H