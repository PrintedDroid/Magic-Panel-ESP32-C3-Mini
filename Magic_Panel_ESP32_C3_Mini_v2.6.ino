/*
 * Magic Panel ESP32-C3 Mini
 * by Printed-Droid.com
 * VERSION 2.6
 *
 * CHANGES IN V2.6:
 * - Implemented chained command syntax for patterns.
 * - Color and duration can now be set in a single command.
 * - Example: T57:C1 starts the Rainbow Cycle with Green color.
 * - Example: T62:30:C4 runs Matrix Rain for 30 seconds with Cyan color.
 *
 * CHANGES IN V2.5:
 * - Added R2-D2/Astromech-specific patterns (100-119)
 * - Added PSI (Processor State Indicator) animations
 * - Moved Smart Demo to pattern ID 200
 *
 * CHANGES IN V2.4:
 * - Fixed memory fragmentation issues with String operations
 * - Added comprehensive bounds checking for all array access
 * - Fixed buffer overflow vulnerabilities in text functions
 * - Improved serial communication synchronization
 * - Added EEPROM data validation on load
 * - Optimized string processing to reduce heap usage
 * - Added error recovery mechanisms
 * - Fixed timing consistency across all patterns
 * - Added mutex-like protection for shared resources
 * - Improved watchdog protection with proper yield placement
 *
 * CHANGES IN V2.3:
 * - Fixed memory management issues (static allocation for fire effect)
 * - Fixed serial buffer overflow protection with proper null termination
 * - Fixed text scrolling completion in demo mode
 * - Fixed duration handling consistency across all patterns
 * - Unified all code comments and documentation to English
 * - Added proper error handling and bounds checking
 * - Implemented watchdog protection with yield() calls
 * - Added timing constants for consistent animations
 *
 * CHANGES IN V2.2:
 * - Added T<pattern>:<seconds> syntax for custom duration (e.g., T57:30 for 30 seconds)
 * - Duration applies only to the specific pattern execution
 * - Works with all time-based and animation patterns
 *
 * CHANGES IN V2.1:
 * - Fixed serial command processing for external controllers via RX/TX pins
 *
 * COMMAND REFERENCE:
 * Pattern Control:
 *   T<n> or S<n>         - Run pattern number n (0-200)
 *   T<n>:<seconds>       - Run pattern n for specified seconds (e.g., T57:30)
 *   DEMO                 - Run curated demo show (same as T200)
 *   
 * Display Control:
 *   A or ON              - All LEDs on
 *   D or OFF             - All LEDs off
 *   C<n>                 - Set color (0-8, 9=rainbow)
 *   C<r>,<g>,<b>         - Set RGB color directly
 *   B<n>                 - Set brightness (0-255)
 *   V<n> or SP<n>        - Set animation speed (1-100)
 *   P<n>                 - Set mode (0=timed, 1=always on)
 *   
 * Text Commands:
 *   TEXT:<string>        - Scroll text
 *   TEXT_BOUNCE:<string> - Bouncing text animation
 *   TEXTSAVE<n>:<string> - Save text to slot n (0-9)
 *   TEXTLOAD<n>          - Load and scroll text from slot n
 *   FONT<n>              - Set font (0=standard, 1=aurebesh)
 *   
 * System Commands:
 *   SAVE                 - Save settings to EEPROM
 *   LOAD                 - Load settings from EEPROM
 *   STATUS               - Show current settings
 *   LIST                 - List all patterns
 *   HELP                 - Show help
 *   START<n>             - Set startup pattern
 *   TRANSITION<n>        - Enable/disable smooth transitions (0/1)
 *
 * PATTERN RANGES:
 *   0-99:    Original patterns
 *   100-119: R2-D2/Astromech patterns
 *   200:     Smart Demo
 *
 * BOARD & PINS:
 * - Board: Lolin C3 Mini
 * - LED Pin: GPIO 6
 * - I2C Address: 20 (configurable)
 */

#include <FastLED.h>
#include <Wire.h>
#include <EEPROM.h>

// Include all configuration and library headers
#include "config.h"
#include "led_control.h"
#include "command_processor.h"
#include "patterns_basic.h"
#include "patterns_trace.h"
#include "patterns_effects.h"
#include "patterns_shapes.h"
#include "patterns_animations.h"
#include "patterns_text.h"
#include "patterns_astromech.h"  // R2-D2 specific patterns

// Global variables
CRGB leds[NUM_LEDS];
PanelState panelState;
CommandProcessor cmdProcessor;

// Command processing state - prevent concurrent access
volatile bool processingCommand = false;

// Fixed buffer for robust Serial1 communication with proper bounds checking
char serial1Buffer[MAX_COMMAND_LENGTH + 1];
uint8_t serial1BufferPos = 0;

// Fixed buffer for Serial communication
char serialBuffer[MAX_COMMAND_LENGTH + 1];
uint8_t serialBufferPos = 0;

// Timing variables
unsigned long lastUpdate = 0;
unsigned long patternEndTime = 0;
bool patternActive = false;

// Forward declarations
void runStartupAnimation();
void runPattern(int patternId, unsigned long duration = 0);
void runAllPatterns();
void runSmartDemo();
String getPatternName(int patternId);
void showHelp(String topic);
void listPatterns();
void showStatus();
void saveSettings();
void loadSettings();
bool validateSettings(PanelState* state);
void confirmCommand();
void processSerialCommand();
void processExternalCommand();
void executeCommand(Command cmd);
void setPatternEndTime(unsigned long duration);
void receiveEvent(int numBytes);
void saveTextToSlot(int slot, const char* text);
String loadTextFromSlot(int slot);
void safeStringCopy(char* dest, const char* src, size_t maxLen);

void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) { ; }

  // Initialize the second serial port (Hardware UART)
  // Baudrate: 9600 (as defined in config.h)
  // RX Pin: GPIO 20
  // TX Pin: GPIO 21
  Serial1.begin(SERIAL_BAUD, SERIAL_8N1, 20, 21);

  Serial.println(F("\n========================================"));
  Serial.println(F("  Magic Panel ESP32-C3 v2.6"));
  Serial.println(F("========================================"));
  Serial.println(F("Type 'HELP' for commands or 'DEMO' for a show."));
  
  EEPROM.begin(EEPROM_SIZE);
  loadSettings();

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(panelState.brightness);
  
  Wire.begin(panelState.i2cAddress);
  Wire.onReceive(receiveEvent);
  
  // Initialize buffers
  memset(serialBuffer, 0, sizeof(serialBuffer));
  memset(serial1Buffer, 0, sizeof(serial1Buffer));
  
  LedControl::clearPanel();
  runStartupAnimation();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Process commands only if not already processing (simple mutex)
  if (!processingCommand) {
    // Listen for commands from the built-in USB Serial
    if (Serial.available()) { 
      processSerialCommand(); 
    }
    
    // Listen for commands from the external controller via RX/TX pins
    if (Serial1.available()) { 
      processExternalCommand(); 
    }
  }

  // Pattern timing management
  if (currentTime - lastUpdate > PATTERN_UPDATE_INTERVAL) {
    lastUpdate = currentTime;

    if (patternActive && currentTime >= patternEndTime) {
      if(panelState.smoothTransitions) {
        LedControl::fadeOutEffect();
      } else {
        LedControl::clearPanel();
      }
      patternActive = false;
    }
    
    if (panelState.alwaysOn && panelState.lastPattern > 4 && !patternActive && panelState.lastPattern != PATTERN_SMART_DEMO) {
      runPattern(panelState.lastPattern);
    }
  }
  
  yield(); // Prevent watchdog reset in main loop
}

void runPattern(int patternId, unsigned long duration) {
  // Validate pattern ID
  if (patternId < 0 || patternId > MAX_PATTERN_ID) {
    Serial.println(F("Invalid pattern ID"));
    return;
  }
  
  if (panelState.lastPattern != patternId && panelState.smoothTransitions) {
    LedControl::fadeOutEffect(2);
  } else {
    LedControl::clearPanel();
  }
  
  if (patternId == panelState.lastPattern && !panelState.alwaysOn && patternActive) { 
    return;
  }

  if (patternId == PATTERN_SMART_DEMO) {
    runSmartDemo();
    return;
  }
  
  patternActive = false;
  panelState.lastPattern = patternId;
  
  // Set custom duration if provided
  if (duration > 0) {
    setPatternEndTime(duration);
  }
  
  // Dispatch to appropriate pattern category
  switch (patternId) {
    case 0 ... 7: 
      runBasicPattern(patternId); 
      break;
    case 8 ... 15: 
    case 48 ... 51: 
      runTracePattern(patternId); 
      break;
    case 16 ... 19: 
    case 24 ... 28: 
    case 57 ... 60: 
    case 64 ... 66: 
      runEffectPattern(patternId);
      break;
    case 20: 
    case 31 ... 47: 
    case 56:
      runShapePattern(patternId); 
      break;
    case 21 ... 23: 
    case 29 ... 30: 
    case 52 ... 55: 
    case 61 ... 63: 
    case 67 ... 68: 
      runAnimationPattern(patternId);
      break;
    case 80: 
    case 97 ... 98: 
      runTextPattern(patternId); 
      break;
    case 99: 
      runAllPatterns(); 
      break;
    case 100 ... 119:  // R2-D2/Astromech patterns
      runAstromechPattern(patternId);
      break;
    case 200:  // Smart Demo
      runSmartDemo();
      break;
    default:
      Serial.print(F("Unknown pattern ID: "));
      Serial.println(patternId);
      break;
  }
}

void processSerialCommand() {
  if (processingCommand) return; // Prevent concurrent processing
  processingCommand = true;
  
  while (Serial.available() && serialBufferPos < MAX_COMMAND_LENGTH) {
    char c = Serial.read();
    
    if (c == '\r' || c == '\n') {
      if (serialBufferPos > 0) {
        serialBuffer[serialBufferPos] = '\0'; // Null terminate
        
        // Convert to uppercase in-place
        for (uint8_t i = 0; i < serialBufferPos; i++) {
          if (serialBuffer[i] >= 'a' && serialBuffer[i] <= 'z') {
            serialBuffer[i] -= 32;
          }
        }
        
        String commandStr(serialBuffer);
        Command cmd = cmdProcessor.parseCommand(commandStr);
        executeCommand(cmd);
        
        // Reset buffer
        serialBufferPos = 0;
        memset(serialBuffer, 0, sizeof(serialBuffer));
      }
    } else {
      serialBuffer[serialBufferPos++] = c;
      
      // Buffer overflow protection
      if (serialBufferPos >= MAX_COMMAND_LENGTH) {
        Serial.println(F("Command too long, discarded."));
        serialBufferPos = 0;
        memset(serialBuffer, 0, sizeof(serialBuffer));
      }
    }
  }
  
  processingCommand = false;
}

void processExternalCommand() {
  if (processingCommand) return; // Prevent concurrent processing
  processingCommand = true;
  
  while (Serial1.available() && serial1BufferPos < MAX_COMMAND_LENGTH) {
    char c = Serial1.read();
    
    if (c == '\r' || c == '\n') {
      if (serial1BufferPos > 0) {
        serial1Buffer[serial1BufferPos] = '\0'; // Proper null termination
        
        // Convert to uppercase in-place
        for (uint8_t i = 0; i < serial1BufferPos; i++) {
          if (serial1Buffer[i] >= 'a' && serial1Buffer[i] <= 'z') {
            serial1Buffer[i] -= 32;
          }
        }
        
        String commandStr(serial1Buffer);
        Command cmd = cmdProcessor.parseCommand(commandStr);
        executeCommand(cmd);
        
        // Reset buffer
        serial1BufferPos = 0;
        memset(serial1Buffer, 0, sizeof(serial1Buffer));
      }
    } else {
      serial1Buffer[serial1BufferPos++] = c;
      
      // Buffer overflow protection
      if (serial1BufferPos >= MAX_COMMAND_LENGTH) {
        serial1BufferPos = 0;
        memset(serial1Buffer, 0, sizeof(serial1Buffer));
      }
    }
  }
  
  processingCommand = false;
}

void executeCommand(Command cmd) {
  if (cmd.type == CMD_NONE) {
    Serial.println(F("Unknown command. Type HELP for a list of commands."));
    return;
  }
  
  switch (cmd.type) {
    case CMD_PATTERN: 
      if (cmd.value >= 0 && cmd.value <= MAX_PATTERN_ID) {
        
        // NEW PART: Set the color if it was included in the command
        if (cmd.hasRGB) {
          // Special case for rainbow
          if (cmd.r == 255 && cmd.g == 255 && cmd.b == 255) {
            panelState.rainbowMode = true;
            Serial.println(F("Color set to Rainbow (via chained command)"));
          } else {
            panelState.rainbowMode = false;
            panelState.currentColor = CRGB(cmd.r, cmd.g, cmd.b);
            Serial.print(F("Color set to RGB("));
            Serial.print(cmd.r); Serial.print(F(","));
            Serial.print(cmd.g); Serial.print(F(","));
            Serial.print(cmd.b); Serial.println(F(") (via chained command)"));
          }
        }
        
        if (cmd.duration > 0) {
          Serial.print(F("Running pattern "));
          Serial.print(cmd.value);
          Serial.print(F(" for "));
          Serial.print(cmd.duration / 1000);
          Serial.println(F(" seconds"));
        }
        runPattern(cmd.value, cmd.duration);
      } else {
        Serial.println(F("Invalid pattern number"));
      }
      break;
      
    case CMD_DEMO: 
      runSmartDemo(); 
      break;
      
    case CMD_COLOR:
      if (cmd.hasRGB) {
        panelState.currentColor = CRGB(cmd.r, cmd.g, cmd.b);
        panelState.rainbowMode = false;
        Serial.print(F("Color set to RGB("));
        Serial.print(cmd.r); Serial.print(F(","));
        Serial.print(cmd.g); Serial.print(F(","));
        Serial.print(cmd.b); Serial.println(F(")"));
      } else {
        panelState.rainbowMode = (cmd.value == 9);
        if (!panelState.rainbowMode && cmd.value >= 0 && cmd.value < NUM_PRESET_COLORS) {
          panelState.currentColor = COLORS[cmd.value];
          Serial.print(F("Color set to preset "));
          Serial.println(cmd.value);
        } else if (panelState.rainbowMode) {
          Serial.println(F("Rainbow mode enabled"));
        }
      }
      break;
      
    case CMD_BRIGHTNESS:
      panelState.brightness = constrain(cmd.value, 0, 255);
      FastLED.setBrightness(panelState.brightness);
      Serial.print(F("Brightness set to "));
      Serial.println(panelState.brightness);
      break;
      
    case CMD_SPEED: 
      panelState.animationSpeed = constrain(cmd.value, 1, 100); 
      Serial.print(F("Animation speed set to "));
      Serial.println(panelState.animationSpeed);
      break;
      
    case CMD_MODE: 
      panelState.alwaysOn = (cmd.value == 1); 
      Serial.print(F("Mode set to: "));
      Serial.println(panelState.alwaysOn ? F("Always On") : F("Timed"));
      break;
      
    case CMD_SAVE: 
      saveSettings(); 
      break;
      
    case CMD_LOAD: 
      loadSettings(); 
      break;
      
    case CMD_ALL_ON: 
      LedControl::allOn(LedControl::getCurrentColor()); 
      Serial.println(F("All LEDs on"));
      break;
      
    case CMD_ALL_OFF: 
      LedControl::clearPanel(); 
      Serial.println(F("All LEDs off"));
      break;
      
    case CMD_TEXT_SCROLL: 
      if (cmd.textData.length() > 0 && cmd.textData.length() <= MAX_TEXT_LENGTH) {
        Text::clearBuffer();
        Text::scrollCustomText(cmd.textData, panelState.useAurebesh ? 2 : 1); 
        LedControl::clearPanel();
      } else {
        Serial.println(F("Invalid text length"));
      }
      break;
      
    case CMD_TEXT_BOUNCE: 
      if (cmd.textData.length() > 0 && cmd.textData.length() <= MAX_TEXT_LENGTH) {
        Text::bouncingText(cmd.textData, panelState.useAurebesh ? 2 : 1);
      } else {
        Serial.println(F("Invalid text length"));
      }
      break;
      
    case CMD_TEXT_SAVE: 
      if (cmd.textSlot >= 0 && cmd.textSlot < TEXT_SLOTS) {
        if (cmd.textData.length() > 0 && cmd.textData.length() <= MAX_TEXT_LENGTH) {
          saveTextToSlot(cmd.textSlot, cmd.textData.c_str());
        } else {
          Serial.println(F("Text too long"));
        }
      } else {
        Serial.println(F("Invalid text slot (0-9)"));
      }
      break;
      
    case CMD_TEXT_LOAD: {
        if (cmd.textSlot >= 0 && cmd.textSlot < TEXT_SLOTS) {
          String loadedText = loadTextFromSlot(cmd.textSlot);
          if (loadedText.length() > 0) {
            Text::clearBuffer();
            Text::scrollCustomText(loadedText, panelState.useAurebesh ? 2 : 1); 
            LedControl::clearPanel();
          } else { 
            Serial.print(F("No text in slot ")); 
            Serial.println(cmd.textSlot);
          }
        } else {
          Serial.println(F("Invalid text slot (0-9)"));
        }
      }
      break;
      
    case CMD_SET_FONT: 
      panelState.useAurebesh = (cmd.value == 1);
      Serial.print(F("Font set to: ")); 
      Serial.println(panelState.useAurebesh ? "Aurebesh" : "Standard"); 
      break;
      
    case CMD_SET_START: 
      panelState.startPattern = constrain(cmd.value, 0, MAX_PATTERN_ID);
      Serial.print(F("Startup pattern set to: ")); 
      Serial.println(panelState.startPattern); 
      break;
      
    case CMD_HELP: 
      showHelp(cmd.textData); 
      break;
      
    case CMD_STATUS: 
      showStatus(); 
      break;
      
    case CMD_LIST_PATTERNS: 
      listPatterns(); 
      break;
      
    case CMD_SET_TRANSITION:
      panelState.smoothTransitions = (cmd.value == 1);
      Serial.print(F("Smooth Transitions set to: "));
      Serial.println(panelState.smoothTransitions ? "ON" : "OFF");
      break;
      
    case CMD_PLAYLIST_RUN: {
        Serial.println(F("Running playlist..."));
        // Use char array instead of String manipulation
        char playlist[MAX_COMMAND_LENGTH];
        safeStringCopy(playlist, cmd.textData.c_str(), MAX_COMMAND_LENGTH);
        
        char* token = strtok(playlist, ",");
        while (token != NULL) {
          int patternId = atoi(token);
          if (patternId >= 0 && patternId <= MAX_PATTERN_ID) {
            Serial.print(F(" > Running pattern: ")); 
            Serial.println(patternId); 
            runPattern(patternId);
            delay(100); // Small delay between patterns
          }
          token = strtok(NULL, ",");
          yield(); // Prevent watchdog
        }
        
        Serial.println(F("Playlist finished."));
      }
      break;
      
    case CMD_PLAYLIST_SAVE: 
      Serial.println(F("Playlist SAVE is not implemented due to EEPROM limits."));
      break;
      
    case CMD_PLAYLIST_LOAD: 
      Serial.println(F("Playlist LOAD is not implemented due to EEPROM limits.")); 
      break;
  }
}

void receiveEvent(int numBytes) {
  if (processingCommand) return; // Prevent concurrent processing
  processingCommand = true;
  
  char i2cBuffer[MAX_COMMAND_LENGTH + 1];
  uint8_t bufferPos = 0;
  
  while (Wire.available() && bufferPos < MAX_COMMAND_LENGTH) { 
    i2cBuffer[bufferPos++] = (char)Wire.read(); 
  }
  i2cBuffer[bufferPos] = '\0';
  
  if (bufferPos > 0) {
    // Convert to uppercase in-place
    for (uint8_t i = 0; i < bufferPos; i++) {
      if (i2cBuffer[i] >= 'a' && i2cBuffer[i] <= 'z') {
        i2cBuffer[i] -= 32;
      }
    }
    
    String commandStr(i2cBuffer);
    executeCommand(cmdProcessor.parseCommand(commandStr));
  }
  
  processingCommand = false;
}

void saveSettings() {
  // Validate settings before saving
  if (!validateSettings(&panelState)) {
    Serial.println(F("Invalid settings detected, using defaults"));
    // Reset to defaults
    panelState.brightness = DEFAULT_BRIGHTNESS;
    panelState.animationSpeed = DEFAULT_SPEED;
    panelState.alwaysOn = true;
    panelState.currentColor = CRGB::Red;
    panelState.rainbowMode = false;
    panelState.i2cAddress = DEFAULT_I2C_ADDRESS;
    panelState.startPattern = 0;
    panelState.useAurebesh = false;
    panelState.smoothTransitions = false;
    panelState.lastPattern = 0;
  }
  
  EEPROM.put(0, panelState);
  EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
  
  // Add checksum for data integrity
  uint8_t checksum = 0;
  uint8_t* data = (uint8_t*)&panelState;
  for (size_t i = 0; i < sizeof(PanelState); i++) {
    checksum ^= data[i];
  }
  EEPROM.write(EEPROM_CHECKSUM_ADDR, checksum);
  
  EEPROM.commit();
  Serial.println(F("Settings saved to EEPROM."));
}

void loadSettings() {
  // Read magic byte to check if EEPROM has been initialized
  if (EEPROM.read(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC_VALUE) {
    EEPROM.get(0, panelState);
    
    // Validate checksum
    uint8_t storedChecksum = EEPROM.read(EEPROM_CHECKSUM_ADDR);
    uint8_t checksum = 0;
    uint8_t* data = (uint8_t*)&panelState;
    for (size_t i = 0; i < sizeof(PanelState); i++) {
      checksum ^= data[i];
    }
    
    if (storedChecksum != checksum || !validateSettings(&panelState)) {
      Serial.println(F("Checksum mismatch or invalid settings. Loading defaults."));
      goto load_defaults;
    }
    
    FastLED.setBrightness(panelState.brightness);
    Serial.println(F("Settings loaded from EEPROM."));
  } else {
load_defaults:
    // EEPROM not initialized or corrupted, load defaults and save them
    Serial.println(F("No valid settings found. Loading defaults."));
    panelState.brightness = DEFAULT_BRIGHTNESS;
    panelState.animationSpeed = DEFAULT_SPEED;
    panelState.alwaysOn = true;
    panelState.currentColor = CRGB::Red;
    panelState.rainbowMode = false;
    panelState.i2cAddress = DEFAULT_I2C_ADDRESS;
    panelState.startPattern = 0;
    panelState.useAurebesh = false;
    panelState.smoothTransitions = false;
    panelState.lastPattern = 0;
    
    saveSettings();
  }
}

bool validateSettings(PanelState* state) {
  if (!state) return false;
  
  // Validate brightness
  if (state->brightness > 255) return false;
  
  // Validate animation speed
  if (state->animationSpeed < 1 || state->animationSpeed > 100) return false;
  
  // Validate I2C address
  if (state->i2cAddress < 1 || state->i2cAddress > 127) return false;
  
  // Validate pattern IDs
  if (state->startPattern < 0 || state->startPattern > MAX_PATTERN_ID) return false;
  if (state->lastPattern < 0 || state->lastPattern > MAX_PATTERN_ID) return false;
  
  // Validate color (basic check)
  if (state->currentColor.r > 255 || state->currentColor.g > 255 || state->currentColor.b > 255) return false;
  
  return true;
}

void saveTextToSlot(int slot, const char* text) {
  if (slot < 0 || slot >= TEXT_SLOTS || !text) {
    Serial.println(F("Invalid text slot or null text"));
    return;
  }
  
  int baseAddr = EEPROM_TEXT_START_ADDR + (slot * (MAX_TEXT_LENGTH + 1));
  size_t textLen = strlen(text);
  
  // Truncate text if too long
  if (textLen > MAX_TEXT_LENGTH) {
    textLen = MAX_TEXT_LENGTH;
  }
  
  // Write length first, then text data
  EEPROM.write(baseAddr, textLen);
  for (size_t i = 0; i < textLen; i++) {
    EEPROM.write(baseAddr + 1 + i, text[i]);
  }
  EEPROM.commit();
  
  Serial.print(F("Text saved to slot ")); 
  Serial.println(slot);
}

String loadTextFromSlot(int slot) {
  if (slot < 0 || slot >= TEXT_SLOTS) return "";
  
  int baseAddr = EEPROM_TEXT_START_ADDR + (slot * (MAX_TEXT_LENGTH + 1));
  int textLen = EEPROM.read(baseAddr);
  
  // Validate text length
  if (textLen == 0 || textLen == 255 || textLen > MAX_TEXT_LENGTH) return "";
  
  String text = "";
  text.reserve(textLen + 1); // Pre-allocate space for efficiency
  
  for (int i = 0; i < textLen; i++) {
    char c = EEPROM.read(baseAddr + 1 + i);
    // Validate character is printable
    if (c >= 32 && c <= 126) {
      text += c;
    }
  }
  return text;
}

void safeStringCopy(char* dest, const char* src, size_t maxLen) {
  if (!dest || !src || maxLen == 0) return;
  
  size_t i;
  for (i = 0; i < maxLen - 1 && src[i] != '\0'; i++) {
    dest[i] = src[i];
  }
  dest[i] = '\0';
}

void confirmCommand() { 
  // Visual feedback for command confirmation
  LedControl::setPixel(0, 0, CRGB::Green);
  LedControl::updatePanel();
  delay(100);
  LedControl::setPixel(0, 0, CRGB::Black);
  LedControl::updatePanel();
}

void runStartupAnimation() {
  LedControl::allOn(CRGB::Red); 
  delay(300);
  LedControl::allOn(CRGB::Green); 
  delay(300);
  LedControl::allOn(CRGB::Blue); 
  delay(300);
  LedControl::clearPanel();
  
  Shapes::showDigit(2); 
  delay(500);
  Shapes::showDigit(6); 
  delay(500); // Version 2.6
  LedControl::clearPanel();
  
  if (panelState.startPattern >= 0 && panelState.startPattern <= MAX_PATTERN_ID) {
    delay(250); 
    runPattern(panelState.startPattern);
  }
}

void setPatternEndTime(unsigned long duration) {
  if (duration > MAX_PATTERN_DURATION) {
    duration = MAX_PATTERN_DURATION; // Cap at maximum duration
  }
  patternEndTime = millis() + duration;
  patternActive = true;
}

String getPatternName(int patternId) {
  switch (patternId) {
    case 0: return F("Off");
    case 1: return F("On Indefinite");
    case 2: return F("On 2s");
    case 3: return F("On 5s");
    case 4: return F("On 10s");
    case 5: return F("Toggle");
    case 6: return F("Alert 4s");
    case 7: return F("Alert 10s");
    case 8: return F("Trace Up Fill");
    case 9: return F("Trace Up Line");
    case 10: return F("Trace Down Fill");
    case 11: return F("Trace Down Line");
    case 12: return F("Trace Right Fill");
    case 13: return F("Trace Right Line");
    case 14: return F("Trace Left Fill");
    case 15: return F("Trace Left Line");
    case 16: return F("Expand Fill");
    case 17: return F("Expand Ring");
    case 18: return F("Compress Fill");
    case 19: return F("Compress Ring");
    case 20: return F("Cross");
    case 21: return F("Cylon Column");
    case 22: return F("Cylon Row");
    case 23: return F("Eye Scan");
    case 24: return F("Fade Out/In");
    case 25: return F("Fade Out");
    case 26: return F("Flash All");
    case 27: return F("Flash Vertical");
    case 28: return F("Flash Quadrants");
    case 29: return F("Two Loop");
    case 30: return F("One Loop");
    case 31: return F("Test Fill");
    case 32: return F("Test Pixel");
    case 33: return F("AI Logo");
    case 34: return F("2GWD Logo");
    case 35: return F("Quadrant TL->TR->BR->BL");
    case 36: return F("Quadrant TR->TL->BL->BR");
    case 37: return F("Quadrant TR->BR->BL->TL");
    case 38: return F("Quadrant TL->BL->BR->TR");
    case 39: return F("Random Pixel");
    case 40: return F("Countdown 9-0");
    case 41: return F("Countdown 3-0");
    case 42: return F("Alert Random 4s");
    case 43: return F("Alert Random 8s");
    case 44: return F("Smiley Face");
    case 45: return F("Sad Face");
    case 46: return F("Heart");
    case 47: return F("Checkerboard");
    case 48: return F("Compress In Fill");
    case 49: return F("Compress In Clear");
    case 50: return F("Explode Out Fill");
    case 51: return F("Explode Out Clear");
    case 52: return F("VU Meter Columns Up");
    case 53: return F("VU Meter Rows Left");
    case 54: return F("VU Meter Columns Down");
    case 55: return F("VU Meter Rows Right");
    case 56: return F("Animated Heart");
    case 57: return F("Rainbow Cycle");
    case 58: return F("Fire Effect");
    case 59: return F("Twinkle");
    case 60: return F("Plasma");
    case 61: return F("Game of Life");
    case 62: return F("Matrix Rain");
    case 63: return F("3D Cube");
    case 64: return F("Kaleidoscope");
    case 65: return F("Raindrops");
    case 66: return F("Drip Effect");
    case 67: return F("Pac-Man");
    case 68: return F("Space Invaders");
    case 80: return F("Bouncing Text");
    case 97: return F("Scroll Text (EN)");
    case 98: return F("Scroll Text (AU)");
    case 99: return F("Test All Patterns");
    // R2-D2/Astromech patterns
    case 100: return F("PSI Solid Red");
    case 101: return F("PSI Solid Blue");
    case 102: return F("PSI Solid Green");
    case 103: return F("PSI Solid Yellow");
    case 104: return F("PSI Solid Cyan");
    case 105: return F("PSI Solid Magenta");
    case 106: return F("PSI Solid White");
    case 107: return F("PSI Color Wipe Down");
    case 108: return F("PSI Color Wipe Up");
    case 109: return F("PSI Random Flicker");
    case 110: return F("PSI Pulse");
    case 111: return F("PSI Rainbow");
    case 112: return F("PSI March Horizontal");
    case 113: return F("PSI March Vertical");
    case 114: return F("PSI March Diagonal");
    case 115: return F("PSI March Checkerboard");
    case 116: return F("PSI March Spiral");
    case 117: return F("R2-D2 Communication");
    case 118: return F("R2-D2 Thinking");
    case 119: return F("R2-D2 Alert");
    case 200: return F("Smart Demo");
    default: return F("Unknown Pattern");
  }
}

void runAllPatterns() {
  Serial.println(F("Running all patterns (full test)..."));
  
  // Test basic patterns (0-68)
  for (int i = 0; i <= 68; i++) {
    yield(); // Prevent watchdog reset
    Serial.print(F("Pattern "));
    Serial.print(i); 
    Serial.print(F(" - ")); 
    Serial.println(getPatternName(i));
    runPattern(i);
    delay(2000);
  }
  
  // Test text pattern
  Serial.print(F("Pattern 80 - ")); 
  Serial.println(getPatternName(80));
  runPattern(80); 
  delay(2000);
  
  // Test text scroll patterns
  for (int i = 97; i <= 98; i++) {
    yield();
    Serial.print(F("Pattern "));
    Serial.print(i); 
    Serial.print(F(" - ")); 
    Serial.println(getPatternName(i));
    runPattern(i);
    delay(2000);
  }
  
  // Test all patterns test
  Serial.print(F("Pattern 99 - ")); 
  Serial.println(getPatternName(99));
  // Note: Pattern 99 itself runs all patterns, so we skip it here to avoid recursion
  
  // Test R2-D2/Astromech patterns
  Serial.println(F("Testing R2-D2/Astromech patterns..."));
  for (int i = 100; i <= 119; i++) {
    yield();
    Serial.print(F("Pattern "));
    Serial.print(i); 
    Serial.print(F(" - ")); 
    Serial.println(getPatternName(i));
    runPattern(i);
    delay(2000);
  }
  
  // Test Smart Demo
  Serial.print(F("Pattern 200 - ")); 
  Serial.println(getPatternName(200));
  // Note: Smart Demo is long, so we just mention it
  
  Serial.println(F("Test complete"));
}

// Smart demo with proper text completion
void runSmartDemo() {
  patternActive = false; 
  Serial.println(F("Starting Smart Demo..."));
  
  // 1. Text Scroll in Aurebesh (Red)
  panelState.currentColor = CRGB::Red;
  panelState.rainbowMode = false;
  FastLED.setBrightness(panelState.brightness);
  Text::clearBuffer();
  Text::scrollCustomText("PRINTED-DROID", 2);
  // Text now properly completes before continuing
  
  // 2. Text Scroll in English (Rainbow)
  Text::clearBuffer();
  Text::scrollRainbowText("PRINTED-DROID", 1);
  // Added proper pause after rainbow text completion
  delay(DEMO_TEXT_PAUSE);
  
  // 3. Curated list of patterns (including some R2-D2 patterns)
  const int patterns_to_show[] = {
    1, 5, 6, 8, 9, 12, 13, 16, 17, 20, 21, 23, 24, 28, 29, 30, 33, 
    35, 40, 44, 46, 47, 52, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 80,
    109, 110, 111, 117, 118  // Add some R2-D2 patterns to the demo
  };
  int num_patterns = sizeof(patterns_to_show) / sizeof(patterns_to_show[0]);
  
  Serial.println(F("Now showing curated patterns..."));
  for (int i = 0; i < num_patterns; i++) {
    yield(); // Prevent watchdog reset
    int patternId = patterns_to_show[i];
    Serial.print(F("Showing Pattern: ")); 
    Serial.print(patternId); 
    Serial.print(F(" - ")); 
    Serial.println(getPatternName(patternId));
    runPattern(patternId);
    delay(DEMO_PATTERN_DURATION);
  }
  
  runPattern(0);
  Serial.println(F("Smart Demo finished."));
}

void showHelp(String topic) {
  // Convert topic to uppercase for comparison
  char topicUpper[32];
  safeStringCopy(topicUpper, topic.c_str(), 32);
  for (size_t i = 0; i < strlen(topicUpper); i++) {
    if (topicUpper[i] >= 'a' && topicUpper[i] <= 'z') {
      topicUpper[i] -= 32;
    }
  }
  
  if (strlen(topicUpper) == 0 || strcmp(topicUpper, "COMMANDS") == 0) {
    Serial.println(F("\n=== Magic Panel Help Menu ==="));
    Serial.println(F("Type 'HELP FULL' for complete documentation."));
    Serial.println(F("Type 'LIST' for a detailed list of all patterns."));
    Serial.println(F("Type 'DEMO' for a curated show of patterns."));
    Serial.println(F("Type 'STATUS' to see current settings."));
    Serial.println(F("\n--- Quick Command Reference ---"));
    Serial.println(F("Pattern: T<n> or T<n>:<seconds>"));
    Serial.println(F("Example: T57:30 (Rainbow for 30 seconds)"));
    Serial.println(F("Example: T109 (PSI Random Flicker)"));
    Serial.println(F("Color: C<0-9> or C<r>,<g>,<b>"));
    Serial.println(F("Brightness: B<0-255>"));
    Serial.println(F("Speed: V<1-100>"));
    Serial.println(F("\n--- Pattern Ranges ---"));
    Serial.println(F("0-99:    Original patterns"));
    Serial.println(F("100-119: R2-D2/Astromech patterns"));
    Serial.println(F("200:     Smart Demo"));
    return;
  }

  if (strcmp(topicUpper, "FULL") == 0) {
    Serial.println(F("\n=== Documentation: Magic Panel for ESP32-C3 Mini (v2.6) ==="));
    Serial.println(F("\n-- FAQ --"));
    Serial.println(F("Q1: Nothing on Serial Monitor?"));
    Serial.println(F("A: Check baud rate is 9600. Ensure the correct COM port is selected."));
    Serial.println(F("Q2: Patterns look wrong/mirrored?"));
    Serial.println(F("A: The code expects a 'Progressive/Scanline' layout. Check your matrix wiring and the 'xyToIndex' function in led_control.h."));
    Serial.println(F("Q3: How to change defaults permanently?"));
    Serial.println(F("A: Set your desired values with commands (e.g., B 150), then use the 'SAVE' command to store them in EEPROM."));
    Serial.println(F("Q4: Memory issues or crashes?"));
    Serial.println(F("A: This has been fixed in v2.4 with improved memory management and bounds checking."));
    Serial.println(F("Q5: What are PSI patterns?"));
    Serial.println(F("A: PSI (Processor State Indicator) patterns are R2-D2 specific animations (100-119)."));
  }
}

void listPatterns() {
  Serial.println(F("\n--- Complete Pattern List ---"));
  
  Serial.println(F("\n=== Basic Patterns (0-7) ==="));
  for(int i = 0; i <= 7; i++) {
    Serial.print(i);
    Serial.print(F("\t - "));
    Serial.println(getPatternName(i));
  }
  
  Serial.println(F("\n=== Trace Patterns (8-15) ==="));
  for(int i = 8; i <= 15; i++) {
    Serial.print(i);
    Serial.print(F("\t - "));
    Serial.println(getPatternName(i));
  }
  
  Serial.println(F("\n=== Effect Patterns (16-28) ==="));
  for(int i = 16; i <= 28; i++) {
    Serial.print(i);
    Serial.print(F("\t - "));
    Serial.println(getPatternName(i));
  }
  
  Serial.println(F("\n=== Animation Patterns (29-30) ==="));
  for(int i = 29; i <= 30; i++) {
    Serial.print(i);
    Serial.print(F("\t - "));
    Serial.println(getPatternName(i));
  }
  
  Serial.println(F("\n=== Shape Patterns (31-47) ==="));
  for(int i = 31; i <= 47; i++) {
    Serial.print(i);
    Serial.print(F("\t - "));
    Serial.println(getPatternName(i));
  }
  
  Serial.println(F("\n=== Compress Patterns (48-51) ==="));
  for(int i = 48; i <= 51; i++) {
    Serial.print(i);
    Serial.print(F("\t - "));
    Serial.println(getPatternName(i));
  }
  
  Serial.println(F("\n=== VU Meter Patterns (52-55) ==="));
  for(int i = 52; i <= 55; i++) {
    Serial.print(i);
    Serial.print(F("\t - "));
    Serial.println(getPatternName(i));
  }
  
  Serial.println(F("\n=== Advanced Animations (56-68) ==="));
  for(int i = 56; i <= 68; i++) {
    Serial.print(i);
    Serial.print(F("\t - "));
    Serial.println(getPatternName(i));
  }
  
  Serial.println(F("\n=== Text Patterns ==="));
  Serial.print(F("80\t - "));
  Serial.println(getPatternName(80));
  for(int i = 97; i <= 98; i++) {
    Serial.print(i);
    Serial.print(F("\t - "));
    Serial.println(getPatternName(i));
  }
  
  Serial.println(F("\n=== Test Pattern ==="));
  Serial.print(F("99\t - "));
  Serial.println(getPatternName(99));
  
  Serial.println(F("\n=== R2-D2/Astromech Patterns (100-119) ==="));
  for(int i = 100; i <= 119; i++) {
    Serial.print(i);
    Serial.print(F("\t - "));
    Serial.println(getPatternName(i));
  }
  
  Serial.println(F("\n=== Special Pattern ==="));
  Serial.print(F("200\t - "));
  Serial.println(getPatternName(200));
}

void showStatus() {
  Serial.println(F("\n=== CURRENT STATUS ==="));
  Serial.print(F("Brightness: ")); 
  Serial.print(panelState.brightness);
  Serial.print(F(" (")); 
  Serial.print((panelState.brightness * 100) / 255); 
  Serial.println(F("%)"));
  
  Serial.print(F("Color: "));
  if (panelState.rainbowMode) { 
    Serial.println(F("Rainbow Mode"));
  } else { 
    Serial.print(F("RGB(")); 
    Serial.print(panelState.currentColor.r); 
    Serial.print(F(",")); 
    Serial.print(panelState.currentColor.g); 
    Serial.print(F(",")); 
    Serial.print(panelState.currentColor.b); 
    Serial.println(F(")")); 
  }
  
  Serial.print(F("Mode: "));
  Serial.println(panelState.alwaysOn ? F("Always On") : F("Timed"));
  Serial.print(F("Speed: ")); 
  Serial.println(panelState.animationSpeed);
  Serial.print(F("Font: ")); 
  Serial.println(panelState.useAurebesh ? "Aurebesh" : "Standard");
  Serial.print(F("Startup Pattern: ")); 
  Serial.print(panelState.startPattern);
  Serial.print(F(" (")); 
  Serial.print(getPatternName(panelState.startPattern)); 
  Serial.println(F(")"));
  Serial.print(F("I2C Address: ")); 
  Serial.println(panelState.i2cAddress);
  Serial.print(F("Smooth Transitions: ")); 
  Serial.println(panelState.smoothTransitions ? "ON" : "OFF");
  Serial.print(F("Last Pattern: ")); 
  Serial.println(panelState.lastPattern);
  
  Serial.println(F("\nStored Texts:"));
  for (int i = 0; i < TEXT_SLOTS; i++) {
    String text = loadTextFromSlot(i);
    if (text.length() > 0) { 
      Serial.print(F("  Slot ")); 
      Serial.print(i); 
      Serial.print(F(": \"")); 
      Serial.print(text); 
      Serial.println(F("\"")); 
    }
  }
}