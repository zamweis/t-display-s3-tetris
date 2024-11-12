#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>

// Screen dimensions
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int BOX_SIZE;

// Background color
extern const uint16_t backgroundColor;

// Button states
enum ButtonState { IDLE, PRESSED, SHORT_PRESS, LONG_PRESS };
extern ButtonState leftButtonState;
extern ButtonState rightButtonState;
extern unsigned long buttonPressStart;

// Button pins
extern const int BACKLIGHT_PIN;
extern const int BUTTON_LEFT;
extern const int BUTTON_RIGHT;

// Game-related timing constants
extern const unsigned long initialMoveDelay;
extern const unsigned long rotationHoldThreshold;
extern const unsigned long rotationDebounceInterval;

// Other configuration constants
constexpr int MAX_NAME_LENGTH = 6; // Changed from `extern const int` to `constexpr`
constexpr int maxHighScores = 15;

// Level speed table (values in milliseconds)
constexpr  int levelSpeeds[] = {500, 450, 400, 350, 300, 250, 200, 190, 180, 170, 160, 150, 140, 130, 120, 110, 100, 90, 80, 60, 50};

#endif // CONFIG_H
