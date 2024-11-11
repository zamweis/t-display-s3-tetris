#include <Arduino.h>
#include "TFT_eSPI.h"
#include "Shapes/ShapeI.h"
#include "Shapes/ShapeJ.h"
#include "Shapes/ShapeL.h"
#include "Shapes/ShapeO.h"
#include "Shapes/ShapeS.h"
#include "Shapes/ShapeT.h"
#include "Shapes/ShapeZ.h"

#define SCREEN_WIDTH 170
#define SCREEN_HEIGHT 320
#define BOX_SIZE 17

#define BACKLIGHT_PIN 38 // Use the TFT_BL pin specified in your configuration

#define BUTTON_LEFT 0 // BOOT button pin (GPIO0)
#define BUTTON_RIGHT 14 // IO14 button pin

uint16_t backgroundColor;
TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display
Shape* shape = nullptr;
BlockMap blockMap;

// Game variables
unsigned long lastMoveDownTime = 0;
unsigned long lastMoveLeftTime = 0;
unsigned long lastMoveRightTime = 0;

enum ButtonState { IDLE, PRESSED, SHORT_PRESS, LONG_PRESS };
ButtonState leftButtonState = IDLE;
ButtonState rightButtonState = IDLE;
unsigned long buttonPressStart = 0;

const unsigned long initialMoveDelay = 120; // Initial delay for single move

const unsigned long rotationHoldThreshold = 300; // Time threshold to differentiate between move and rotate (in milliseconds)
const unsigned long rotationDebounceInterval = 250; // Debounce interval for rotation when holding down (in milliseconds)

unsigned long lastRotateLeftTime = 0;
unsigned long lastRotateRightTime = 0;

int score = 0;
int level = 9;
int linesCleared = 0;

bool leftButtonPressed = false;
bool rightButtonPressed = false;
bool leftButtonHeld = false;
bool rightButtonHeld = false;

// Expanded table for speeds at different levels (values in milliseconds)
const int levelSpeeds[] = { 300, 250, 200, 150, 100, 80, 70, 60, 50, 45, 40, 35, 30, 25, 20, 15};

// Function to calculate the speed of shape movement based on the level
int getMoveDownSpeed() {
    return (level < sizeof(levelSpeeds) / sizeof(levelSpeeds[0])) ? levelSpeeds[level - 1] : levelSpeeds[sizeof(levelSpeeds) / sizeof(levelSpeeds[0]) - 1];
}

// Function to create a shape based on a random index
Shape* createRandomShape() {
    Shape* newShape = nullptr;

    // Generate a random number
    int index = rand() % 7;
    switch (index) {
        case 0: newShape = new ShapeI(); break;
        case 1: newShape = new ShapeJ(); break;
        case 2: newShape = new ShapeL(); break;
        case 3: newShape = new ShapeO(); break;
        case 4: newShape = new ShapeS(); break;
        case 5: newShape = new ShapeT(); break;
        case 6: newShape = new ShapeZ(); break;
        default: break;
    }

    if (!newShape) {
        Serial.println("Failed to create shape. Returning nullptr.");
    }
    return newShape;
}

void drawGrid() {
    tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backgroundColor); // Background color
    tft.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_BLACK); // Outer border

    for (int col = 0; col <= SCREEN_WIDTH / BOX_SIZE; ++col) {
        tft.drawLine(col * BOX_SIZE, 0, col * BOX_SIZE, SCREEN_HEIGHT, TFT_BLACK);
    }
    for (int row = 0; row <= SCREEN_HEIGHT / BOX_SIZE; ++row) {
        tft.drawLine(0, row * BOX_SIZE, SCREEN_WIDTH, row * BOX_SIZE, TFT_BLACK);
    }

    Serial.println("Grid drawn successfully.");
}

// Function to update the score and level
void updateScoreAndLevel(int clearedLines) {
    const int pointsPerLine[4] = {40, 100, 300, 1200}; // Scoring based on number of lines cleared at once
    if (clearedLines > 0 && clearedLines <= 4) {
        score += pointsPerLine[clearedLines - 1] * (level + 1); // Use (level + 1) multiplier for classic Tetris scoring
        linesCleared += clearedLines;

        // Increase the level for every 10 lines cleared
        level = 1 + linesCleared / 10;
        Serial.print("Score: ");
        Serial.println(score);
        Serial.print("Level: ");
        Serial.println(level);
    }
}

void resetGame() {
    // Reset game variables
    score = 0;
    level = 1;
    linesCleared = 0;

    // Clear the block map
    delete shape;
    shape = nullptr;
    blockMap = BlockMap(); // Reinitialize block map

    // Redraw grid and start new game
    drawGrid();
    shape = createRandomShape();
    if (shape) {
        shape->moveToLowestBlockkAtMinusOne();
        shape->drawShape(tft, BOX_SIZE);
    }
}

void drawScreen(bool isGameOver) {
    tft.fillScreen(TFT_BLACK); // Black background for a classic Tetris look

    // Set up text properties
    tft.setTextColor(isGameOver ? TFT_RED : TFT_GREEN, TFT_BLACK);
    tft.setTextSize(3);

    // Display appropriate message based on the screen type
    if (isGameOver) {
        tft.setCursor(5, 50);
        tft.println("GAME OVER");
    } else {
        tft.setCursor(25, 50);
        tft.println("WELCOME");
    }

    // Draw a blocky border like Tetris pieces
    int blockSize = BOX_SIZE;
    uint16_t colors[] = {TFT_BLUE, TFT_GREEN, TFT_YELLOW, TFT_RED}; // Array of colors for blocks
    int numColors = sizeof(colors) / sizeof(colors[0]); // Calculate the number of colors

    // Top border
    for (int i = 0; i < SCREEN_WIDTH; i += blockSize) {
        Block borderBlock(i / blockSize, 0, colors[i / blockSize % numColors]); // Create a block with x, y, and color
        borderBlock.draw(tft, blockSize); // Use the block's draw method
    }

    // Bottom border
    for (int i = 0; i < SCREEN_WIDTH; i += blockSize) {
        Block borderBlock(i / blockSize, (SCREEN_HEIGHT) / blockSize, colors[i / blockSize % numColors]);
        borderBlock.draw(tft, blockSize);
    }

    // Left border
    for (int j = 0; j < SCREEN_HEIGHT; j += blockSize) {
        Block borderBlock(0, j / blockSize, colors[j / blockSize % numColors]);
        borderBlock.draw(tft, blockSize);
    }

    // Right border
    for (int j = 0; j < SCREEN_HEIGHT; j += blockSize) {
        Block borderBlock((SCREEN_WIDTH - blockSize) / blockSize, j / blockSize, colors[j / blockSize % numColors]);
        borderBlock.draw(tft, blockSize);
    }

    // Display appropriate instructions or score based on the screen type
    tft.setTextSize(2); // Smaller text for instructions/score
    tft.setTextColor(TFT_WHITE);

    if (isGameOver) {
        tft.setCursor(30, 120);
        tft.print("Score: ");
        tft.println(score);
        tft.setCursor(0, 150);
        tft.println("Press a button");
        tft.setCursor(27, 170);
        tft.println("to restart");
    } else {
        tft.setCursor(0, 120);
        tft.println("Press a button");
        tft.setCursor(38, 150);
        tft.println("to start");
    }
}

void setup() {
    Serial.begin(115200);
    tft.init();
    pinMode(BACKLIGHT_PIN, OUTPUT);
    analogWrite(BACKLIGHT_PIN, 100);
    tft.setRotation(0);
    backgroundColor = tft.color565(30, 30, 30);
    drawScreen(false);
    while (digitalRead(BUTTON_LEFT) == HIGH && digitalRead(BUTTON_RIGHT) == HIGH) {}
    resetGame();
}


void handleButtonState(ButtonState &state, int buttonPin, unsigned long currentTime, void (Shape::*moveFunc)(BlockMap &), void (Shape::*rotateFunc)(BlockMap &)) {
    switch (state) {
        case IDLE:
            if (digitalRead(buttonPin) == LOW) {
                state = PRESSED;
                buttonPressStart = currentTime;
            }
            break;
        case PRESSED:
            if (digitalRead(buttonPin) == HIGH) {
                if (currentTime - buttonPressStart < rotationHoldThreshold) {
                    state = SHORT_PRESS;
                } else {
                    state = IDLE; // Long press but button released early
                }
            } else if (currentTime - buttonPressStart >= rotationHoldThreshold) {
                state = LONG_PRESS;
            }
            break;
        case SHORT_PRESS:
            if (shape) {
                shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                (shape->*moveFunc)(blockMap);
                shape->drawShape(tft, BOX_SIZE);
            }
            state = IDLE;
            break;
        case LONG_PRESS:
            if (digitalRead(buttonPin) == HIGH) {
                state = IDLE;
            } else {
                // Handle continuous rotation with debounce
                static unsigned long lastRotateTime = 0;
                if (currentTime - lastRotateTime >= rotationDebounceInterval) {
                    if (shape) {
                        shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                        (shape->*rotateFunc)(blockMap);
                        shape->drawShape(tft, BOX_SIZE);
                        lastRotateTime = currentTime;
                    }
                }
            }
            break;
    }
}


void loop() {
    unsigned long currentTime = millis();
    if (blockMap.checkGameOver()) {
        drawScreen(true);
        while (true) {
            if (digitalRead(BUTTON_LEFT) == LOW || digitalRead(BUTTON_RIGHT) == LOW) {
                resetGame();
                break;
            }
        }
        return;
    }

    handleButtonState(leftButtonState, BUTTON_LEFT, currentTime, &Shape::moveLeft, &Shape::rotateAntiClockwise);
    handleButtonState(rightButtonState, BUTTON_RIGHT, currentTime, &Shape::moveRight, &Shape::rotateClockwise);

    if (shape) {
        if (currentTime - lastMoveDownTime >= getMoveDownSpeed()) {
            lastMoveDownTime = currentTime;
            if (shape->isMovableDownWards(blockMap)) {
                shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                shape->moveDown(blockMap);
                shape->drawShape(tft, BOX_SIZE);
            } else {
                if (shape->getBlockList() != nullptr) {
                    blockMap.addBlocks(shape->getBlockList(), 4);
                }
                int clearedLines = blockMap.clearAndMoveAllFullLines(tft, BOX_SIZE, backgroundColor); 
                if (clearedLines > 0) {
                    updateScoreAndLevel(clearedLines);
                }
                shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                blockMap.drawAllBlocks(tft, BOX_SIZE);
                delete shape;
                shape = nullptr;
            }
        }
    } else {
        shape = createRandomShape();
        if (shape) {
            shape->moveToLowestBlockkAtMinusOne();
            shape->drawShape(tft, BOX_SIZE);
        }
    }
    blockMap.drawAllBlocks(tft, BOX_SIZE);
}