/*
 * Command Processor
 * Handles both JawaLite and simple serial commands
 * VERSION 2.6 - Enhanced input validation, bounds checking, and error handling
 */

#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include "config.h"

class CommandProcessor {
public:
  Command parseCommand(String input) {
    Command cmd;
    initializeCommand(cmd);
    
    input.trim();
    if (input.length() == 0 || input.length() > MAX_COMMAND_LENGTH) {
      return cmd; // Return empty command for invalid input
    }
    
    // Basic commands
    if (input == "HELP" || input == "?") { 
      cmd.type = CMD_HELP; 
      return cmd; 
    }
    if (input.startsWith("HELP ")) { 
      cmd.type = CMD_HELP; 
      String topic = input.substring(5);
      topic.trim();
      if (topic.length() <= MAX_TEXT_LENGTH) {
        cmd.textData = topic;
      }
      return cmd; 
    }
    if (input == "STATUS") { 
      cmd.type = CMD_STATUS; 
      return cmd; 
    }
    if (input == "LIST") { 
      cmd.type = CMD_LIST_PATTERNS; 
      return cmd; 
    }
    if (input == "SAVE") { 
      cmd.type = CMD_SAVE; 
      return cmd; 
    }
    if (input == "LOAD") { 
      cmd.type = CMD_LOAD; 
      return cmd; 
    }
    if (input == "ON") { 
      cmd.type = CMD_ALL_ON; 
      return cmd; 
    }
    if (input == "OFF") { 
      cmd.type = CMD_ALL_OFF; 
      return cmd; 
    }
    if (input == "DEMO") { 
      cmd.type = CMD_DEMO; 
      return cmd; 
    }

    // Text commands with validation
    if (input.startsWith("TEXT_BOUNCE:")) { 
      String text = input.substring(12);
      text.trim();
      if (text.length() > 0 && text.length() <= MAX_TEXT_LENGTH) {
        cmd.type = CMD_TEXT_BOUNCE; 
        cmd.textData = text;
      }
      return cmd; 
    }
    if (input.startsWith("TEXT:")) { 
      String text = input.substring(5);
      text.trim();
      if (text.length() > 0 && text.length() <= MAX_TEXT_LENGTH) {
        cmd.type = CMD_TEXT_SCROLL; 
        cmd.textData = text;
      }
      return cmd; 
    }
    if (input.startsWith("TEXTSAVE")) {
      cmd.type = CMD_TEXT_SAVE;
      int colonPos = input.indexOf(':');
      if (colonPos > 8 && colonPos < input.length() - 1) { 
        String slotStr = input.substring(8, colonPos);
        int slot = parseInteger(slotStr);
        
        // Validate slot number
        if (VALIDATE_TEXT_SLOT(slot)) {
          cmd.textSlot = slot;
          String text = input.substring(colonPos + 1);
          text.trim();
          if (text.length() > 0 && text.length() <= MAX_TEXT_LENGTH) {
            cmd.textData = text;
          } else {
            cmd.type = CMD_NONE; // Invalid text
          }
        } else {
          cmd.type = CMD_NONE; // Invalid slot
        }
      } else {
        cmd.type = CMD_NONE; // Invalid format
      }
      return cmd;
    }
    if (input.startsWith("TEXTLOAD")) { 
      String slotStr = input.substring(8);
      slotStr.trim();
      int slot = parseInteger(slotStr);
      
      // Validate slot number
      if (VALIDATE_TEXT_SLOT(slot)) {
        cmd.type = CMD_TEXT_LOAD; 
        cmd.textSlot = slot;
      }
      return cmd; 
    }
    
    // Configuration commands with validation
    if (input.startsWith("FONT")) { 
      String valueStr = input.substring(4);
      valueStr.trim();
      int value = parseInteger(valueStr);
      if (value == 0 || value == 1) {
        cmd.type = CMD_SET_FONT; 
        cmd.value = value;
      }
      return cmd; 
    }
    if (input.startsWith("START")) { 
      String valueStr = input.substring(5);
      valueStr.trim();
      int value = parseInteger(valueStr);
      if (VALIDATE_PATTERN_ID(value)) {
        cmd.type = CMD_SET_START; 
        cmd.value = value;
      }
      return cmd; 
    }
    if (input.startsWith("TRANSITION")) { 
      String valueStr = input.substring(10);
      valueStr.trim();
      int value = parseInteger(valueStr);
      if (value == 0 || value == 1) {
        cmd.type = CMD_SET_TRANSITION; 
        cmd.value = value;
      }
      return cmd; 
    }
    if (input.startsWith("PLAYLIST_RUN:")) { 
      String playlist = input.substring(13);
      playlist.trim();
      if (playlist.length() > 0 && playlist.length() <= MAX_COMMAND_LENGTH) {
        cmd.type = CMD_PLAYLIST_RUN; 
        cmd.textData = playlist;
      }
      return cmd; 
    }
    
    // Single character commands with enhanced validation
    if (input.length() >= 1) {
      char firstChar = input.charAt(0);
      String value = input.substring(1);
      value.trim();
      
switch (firstChar) {
        case 'T':
        case 'S': {
          // Enhanced logic for T<pattern>[:<duration>][:C<color>]
          String tempValue = value;
          cmd.type = CMD_PATTERN;
          
          // The first part is always the pattern ID
          int firstColon = tempValue.indexOf(':');
          String patternStr = (firstColon == -1) ? tempValue : tempValue.substring(0, firstColon);
          int patternId = parseInteger(patternStr);

          if (!VALIDATE_PATTERN_ID(patternId)) {
            cmd.type = CMD_NONE; // Invalid pattern ID
            break;
          }
          cmd.value = patternId;

          // Process the remaining parts (arguments)
          if (firstColon != -1) {
            String remainingArgs = tempValue.substring(firstColon + 1);
            
            // To split arguments if there are multiple (e.g., T57:30:C4)
            while (remainingArgs.length() > 0) {
              int nextColon = remainingArgs.indexOf(':');
              String currentArg = (nextColon == -1) ? remainingArgs : remainingArgs.substring(0, nextColon);
              currentArg.trim();

              // Check if it's a color command
              if (currentArg.startsWith("C")) {
                String colorStr = currentArg.substring(1);
                int colorIndex = parseInteger(colorStr);
                if (colorIndex >= 0 && colorIndex <= 9) {
                  cmd.hasRGB = true; // Signal that a color should be set
                  if (colorIndex == 9) {
                    // Special case for rainbow, we mark it with R=G=B=255
                    cmd.r = 255; cmd.g = 255; cmd.b = 255;
                  } else if (VALIDATE_COLOR_INDEX(colorIndex)) {
                    // Read the color from the global COLORS array (from config.h)
                    CRGB color = COLORS[colorIndex];
                    cmd.r = color.r; cmd.g = color.g; cmd.b = color.b;
                  }
                }
              }
              // Check if it's a duration (just a number)
              else {
                int durationSeconds = parseInteger(currentArg);
                if (durationSeconds > 0 && durationSeconds <= 3600) {
                  cmd.duration = (unsigned long)durationSeconds * 1000UL;
                }
              }

              if (nextColon == -1) {
                break; // No more arguments
              }
              remainingArgs = remainingArgs.substring(nextColon + 1);
            }
          }
          break;
        }
        case 'A': 
          cmd.type = CMD_ALL_ON; 
          break;
        case 'D': 
          cmd.type = CMD_ALL_OFF; 
          break;
        case 'P': {
          int mode = parseInteger(value);
          if (mode == 0 || mode == 1) {
            cmd.type = CMD_MODE; 
            cmd.value = mode;
          }
          break;
        }
        case 'C': {
          cmd.type = CMD_COLOR; 
          if (value.indexOf(',') > 0) { 
            if (parseRGB(value, cmd)) {
              cmd.hasRGB = true;
            } else {
              cmd.type = CMD_NONE; // Invalid RGB format
            }
          } else { 
            int colorIndex = parseInteger(value);
            // Validate color index (0-9, where 9 is rainbow)
            if (colorIndex >= 0 && colorIndex <= 9) {
              cmd.value = colorIndex;
            } else {
              cmd.type = CMD_NONE; // Invalid color index
            }
          } 
          break;
        }
        case 'B': {
          int brightness = parseInteger(value);
          if (VALIDATE_BRIGHTNESS(brightness)) {
            cmd.type = CMD_BRIGHTNESS; 
            cmd.value = brightness;
          }
          break;
        }
        case 'V': {
          int speed = parseInteger(value);
          if (VALIDATE_SPEED(speed)) {
            cmd.type = CMD_SPEED; 
            cmd.value = speed;
          }
          break;
        }
      }
    }
    
    // Legacy speed command
    if (cmd.type == CMD_NONE && input.startsWith("SP")) { 
      String valueStr = input.substring(2);
      valueStr.trim();
      int speed = parseInteger(valueStr);
      if (VALIDATE_SPEED(speed)) {
        cmd.type = CMD_SPEED; 
        cmd.value = speed;
      }
    }
    
    return cmd;
  }
  
private:
  void initializeCommand(Command &cmd) {
    cmd.type = CMD_NONE;
    cmd.hasRGB = false;
    cmd.textData = "";
    cmd.textSlot = 0;
    cmd.duration = 0;
    cmd.value = 0;
    cmd.r = 0; 
    cmd.g = 0; 
    cmd.b = 0;
  }
  
  int parseInteger(String str) {
    str.trim();
    if (str.length() == 0) return -1;
    
    // Check if string contains only digits (and optional minus sign)
    bool isNegative = false;
    int startIdx = 0;
    
    if (str.charAt(0) == '-') {
      isNegative = true;
      startIdx = 1;
    }
    
    for (unsigned int i = startIdx; i < str.length(); i++) {
      if (!isDigit(str.charAt(i))) {
        return -1; // Invalid integer
      }
    }
    
    long result = str.toInt();
    
    // Check for overflow
    if (result > INT_MAX || result < INT_MIN) {
      return -1;
    }
    
    return (int)result;
  }
  
  bool parseRGB(String value, Command &cmd) {
    value.trim();
    
    int comma1 = value.indexOf(',');
    if (comma1 <= 0) return false;
    
    int comma2 = value.indexOf(',', comma1 + 1);
    if (comma2 <= comma1) return false;
    
    // Extract substrings
    String rStr = value.substring(0, comma1);
    String gStr = value.substring(comma1 + 1, comma2);
    String bStr = value.substring(comma2 + 1);
    
    // Parse and validate each component
    int r = parseInteger(rStr);
    int g = parseInteger(gStr);
    int b = parseInteger(bStr);
    
    // Validate RGB values (0-255)
    if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
      cmd.r = (uint8_t)r;
      cmd.g = (uint8_t)g;
      cmd.b = (uint8_t)b;
      return true;
    }
    
    return false; // Invalid RGB values
  }
};

#endif // COMMAND_PROCESSOR_H