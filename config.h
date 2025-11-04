/*
 * Configuration file for Magic Panel ESP32-C3
 * All constants and settings in one place.
 * VERSION 2.6
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <FastLED.h>

// Hardware configuration
#define NUM_LEDS 64
#define LED_PIN 6
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

// Default settings
#define DEFAULT_BRIGHTNESS 60
#define DEFAULT_SPEED 50
#define DEFAULT_I2C_ADDRESS 20
#define PATTERN_UPDATE_INTERVAL 10

// Safety limits
#define MAX_PATTERN_ID 200  // Extended to 200 for Smart Demo at position 200
#define MAX_PATTERN_DURATION 3600000UL  // 1 hour maximum
#define MIN_ANIMATION_SPEED 1
#define MAX_ANIMATION_SPEED 100
#define NUM_PRESET_COLORS 9

// Animation timing constants
#define ANIMATION_DELAY_SLOW 200
#define ANIMATION_DELAY_MEDIUM 100
#define ANIMATION_DELAY_FAST 50
#define ANIMATION_DELAY_VERY_FAST 30
#define TEXT_SCROLL_PAUSE 500
#define DEMO_PATTERN_DURATION 3500
#define DEMO_TEXT_PAUSE 1500
#define TEXT_SCROLL_COMPLETION_ITERATIONS 32  // Enough for longest text

// EEPROM settings
#define EEPROM_SIZE 768  // Increased for checksum and validation
#define EEPROM_MAGIC_ADDR (sizeof(PanelState))
#define EEPROM_MAGIC_VALUE 0x42
#define EEPROM_CHECKSUM_ADDR (EEPROM_MAGIC_ADDR + 1)
#define EEPROM_TEXT_START_ADDR (EEPROM_CHECKSUM_ADDR + 1)
#define TEXT_SLOTS 10
#define MAX_TEXT_LENGTH 48

// Serial communication
#define SERIAL_BAUD 115200
#define MAX_COMMAND_LENGTH 60

// Memory allocation
#define FIRE_EFFECT_BUFFER_SIZE NUM_LEDS
#define TEXT_SCROLL_BUFFER_SIZE 14

// Pattern IDs
enum PatternID {
  // Basic patterns (0-7)
  PATTERN_OFF = 0,
  PATTERN_ON_INDEF = 1,
  PATTERN_ON_2S = 2,
  PATTERN_ON_5S = 3,
  PATTERN_ON_10S = 4,
  PATTERN_TOGGLE = 5,
  PATTERN_ALERT_4S = 6,
  PATTERN_ALERT_10S = 7,
  
  // Trace patterns (8-15)
  PATTERN_TRACE_UP_FILL = 8,
  PATTERN_TRACE_UP_LINE = 9,
  PATTERN_TRACE_DOWN_FILL = 10,
  PATTERN_TRACE_DOWN_LINE = 11,
  PATTERN_TRACE_RIGHT_FILL = 12,
  PATTERN_TRACE_RIGHT_LINE = 13,
  PATTERN_TRACE_LEFT_FILL = 14,
  PATTERN_TRACE_LEFT_LINE = 15,
  
  // Effect patterns (16-19)
  PATTERN_EXPAND_FILL = 16,
  PATTERN_EXPAND_RING = 17,
  PATTERN_COMPRESS_FILL = 18,
  PATTERN_COMPRESS_RING = 19,
  
  // Shape patterns (20-47)
  PATTERN_CROSS = 20,
  PATTERN_CYLON_COL = 21,
  PATTERN_CYLON_ROW = 22,
  PATTERN_EYE_SCAN = 23,
  PATTERN_FADE_OUT_IN = 24,
  PATTERN_FADE_OUT = 25,
  PATTERN_FLASH_ALL = 26,
  PATTERN_FLASH_V = 27,
  PATTERN_FLASH_Q = 28,
  PATTERN_TWO_LOOP = 29,
  PATTERN_ONE_LOOP = 30,
  PATTERN_TEST_FILL = 31,
  PATTERN_TEST_PIXEL = 32,
  PATTERN_AI_LOGO = 33,
  PATTERN_2GWD_LOGO = 34,
  PATTERN_QUAD_TL_TR_BR_BL = 35,
  PATTERN_QUAD_TR_TL_BL_BR = 36,
  PATTERN_QUAD_TR_BR_BL_TL = 37,
  PATTERN_QUAD_TL_BL_BR_TR = 38,
  PATTERN_RANDOM_PIXEL = 39,
  PATTERN_COUNTDOWN_9 = 40,
  PATTERN_COUNTDOWN_3 = 41,
  PATTERN_ALERT_RANDOM_4S = 42,
  PATTERN_ALERT_RANDOM_8S = 43,
  PATTERN_SMILEY = 44,
  PATTERN_SAD = 45,
  PATTERN_HEART = 46,
  PATTERN_CHECKERBOARD = 47,
  
  // Compress patterns (48-51)
  PATTERN_COMPRESS_IN_FILL = 48,
  PATTERN_COMPRESS_IN_CLEAR = 49,
  PATTERN_EXPLODE_OUT_FILL = 50,
  PATTERN_EXPLODE_OUT_CLEAR = 51,
  
  // VU meter patterns (52-55)
  PATTERN_VU_METER_COL_UP = 52,
  PATTERN_VU_METER_ROW_LEFT = 53,
  PATTERN_VU_METER_COL_DOWN = 54,
  PATTERN_VU_METER_ROW_RIGHT = 55,
  
  // Advanced animation patterns (56-68)
  PATTERN_HEART_BEAT = 56,
  PATTERN_RAINBOW_CYCLE = 57,
  PATTERN_FIRE_EFFECT = 58,
  PATTERN_TWINKLE = 59,
  PATTERN_PLASMA = 60,
  PATTERN_GAME_OF_LIFE = 61,
  PATTERN_MATRIX_RAIN = 62,
  PATTERN_ROTATING_CUBE = 63,
  PATTERN_KALEIDOSCOPE = 64,
  PATTERN_RAINDROPS = 65,
  PATTERN_DRIP = 66,
  PATTERN_PACMAN = 67,
  PATTERN_INVADERS = 68,
  
  // Text patterns (80, 97-98)
  PATTERN_BOUNCING_TEXT = 80,
  PATTERN_SCROLL_TEXT_EN = 97,
  PATTERN_SCROLL_TEXT_AU = 98,
  
  // Test patterns (99)
  PATTERN_TEST_ALL = 99,
  
  // R2-D2/Astromech patterns (100-119)
  PATTERN_PSI_SOLID_RED = 100,
  PATTERN_PSI_SOLID_BLUE = 101,
  PATTERN_PSI_SOLID_GREEN = 102,
  PATTERN_PSI_SOLID_YELLOW = 103,
  PATTERN_PSI_SOLID_CYAN = 104,
  PATTERN_PSI_SOLID_MAGENTA = 105,
  PATTERN_PSI_SOLID_WHITE = 106,
  PATTERN_PSI_WIPE_DOWN = 107,
  PATTERN_PSI_WIPE_UP = 108,
  PATTERN_PSI_RANDOM_FLICKER = 109,
  PATTERN_PSI_PULSE = 110,
  PATTERN_PSI_RAINBOW = 111,
  PATTERN_PSI_MARCH_H = 112,
  PATTERN_PSI_MARCH_V = 113,
  PATTERN_PSI_MARCH_D = 114,
  PATTERN_PSI_MARCH_C = 115,
  PATTERN_PSI_MARCH_S = 116,
  PATTERN_R2_COMMUNICATION = 117,
  PATTERN_R2_THINKING = 118,
  PATTERN_R2_ALERT = 119,
  
  // Special patterns
  PATTERN_SMART_DEMO = 200  // Moved to 200 to avoid conflict with PSI patterns
};

// Command types
enum CommandType {
  CMD_NONE,
  CMD_PATTERN,
  CMD_COLOR,
  CMD_BRIGHTNESS,
  CMD_SPEED,
  CMD_MODE,
  CMD_SAVE,
  CMD_LOAD,
  CMD_ALL_ON,
  CMD_ALL_OFF,
  CMD_TEXT_SCROLL,
  CMD_TEXT_SAVE,
  CMD_TEXT_LOAD,
  CMD_TEXT_BOUNCE,
  CMD_SET_FONT,
  CMD_SET_START,
  CMD_HELP,
  CMD_STATUS,
  CMD_LIST_PATTERNS,
  CMD_DEMO,
  CMD_SET_TRANSITION,
  CMD_PLAYLIST_RUN,
  CMD_PLAYLIST_SAVE,
  CMD_PLAYLIST_LOAD
};

// Command structure
struct Command {
  CommandType type;
  int value;
  bool hasRGB;
  uint8_t r, g, b;
  String textData;
  int textSlot;
  unsigned long duration;
};

// Panel state structure
struct PanelState {
  bool alwaysOn;
  uint8_t brightness;
  uint8_t animationSpeed;
  CRGB currentColor;
  bool rainbowMode;
  int lastPattern;
  uint8_t i2cAddress;
  int startPattern;
  bool useAurebesh;
  bool smoothTransitions;
};

// Predefined colors
const CRGB COLORS[] = {
  CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White, CRGB::Yellow,
  CRGB::Cyan, CRGB::Magenta, CRGB::Orange, CRGB::Purple
};

// Validation macros
#define VALIDATE_PATTERN_ID(id) ((id) >= 0 && (id) <= MAX_PATTERN_ID)
#define VALIDATE_BRIGHTNESS(b) ((b) >= 0 && (b) <= 255)
#define VALIDATE_SPEED(s) ((s) >= MIN_ANIMATION_SPEED && (s) <= MAX_ANIMATION_SPEED)
#define VALIDATE_TEXT_SLOT(slot) ((slot) >= 0 && (slot) < TEXT_SLOTS)
#define VALIDATE_COLOR_INDEX(idx) ((idx) >= 0 && (idx) < NUM_PRESET_COLORS)

#endif // CONFIG_H