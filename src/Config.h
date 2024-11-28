#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>

// Screen dimensions
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int BOX_SIZE;

static constexpr int MAP_WIDTH = 10;
static constexpr int MAP_HEIGHT = 19;

// Background color
extern const uint16_t backgroundColor;

// Button states
enum ButtonState { IDLE, PRESSED, SHORT_PRESS, LONG_PRESS };
extern ButtonState leftButtonState;
extern ButtonState rightButtonState;
extern unsigned long buttonPressStart;

// Button pins
constexpr int BACKLIGHT_PIN = 38;
constexpr int BUTTON_LEFT = 0;
constexpr int BUTTON_RIGHT = 14;

// Game-related timing constants
extern const unsigned long initialMoveDelay;
extern const unsigned long rotationHoldThreshold;
extern const unsigned long rotationDebounceInterval;

// Other configuration constants
constexpr int MAX_NAME_LENGTH = 6; // Ensure this is constexpr
constexpr int maxHighScores = 15;  // Ensure this is constexpr

// Level speed table (values in milliseconds)
constexpr int levelSpeeds[] = {250};

// Weight ranges
constexpr float heightWeightMin = -10.0f, heightWeightMax = 0.0f;   // Wide range to explore extremes
constexpr float holeWeightMin = -12.0f, holeWeightMax = 0.0f;      // Significant exploration around -5.80
constexpr float bumpinessWeightMin = -10.0f, bumpinessWeightMax = 0.0f; // Wide range around -4.51
constexpr float lineClearWeightMin = -15.0f, lineClearWeightMax = 0.0f; // Extremely broad to allow novel solutions
constexpr float maxHeightWeightMin = -2.0f, maxHeightWeightMax = 4.0f;  // Larger range around 0.12

#endif // CONFIG_H
