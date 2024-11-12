#include "Config.h"

// Screen dimensions
const int SCREEN_WIDTH = 170;
const int SCREEN_HEIGHT = 320;
const int BOX_SIZE = 17;

// Background color
const uint16_t backgroundColor = ((30 >> 3) << 11) | ((30 >> 2) << 5) | (30 >> 3);

// Button states
ButtonState leftButtonState = IDLE;
ButtonState rightButtonState = IDLE;
unsigned long buttonPressStart = 0;

// Game-related timing constants
const unsigned long initialMoveDelay = 120;
const unsigned long rotationHoldThreshold = 300;
const unsigned long rotationDebounceInterval = 250;