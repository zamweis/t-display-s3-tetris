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

const unsigned long initialMoveDelay = 120; // Initial delay for single move
const unsigned long continuousMoveInterval = 0; // Interval for continuous movement

int score = 0;
int level = 1;
int linesCleared = 0;

bool leftButtonPressed = false;
bool rightButtonPressed = false;
bool leftButtonHeld = false;
bool rightButtonHeld = false;

// Table for speeds at different levels (values in milliseconds)
const int levelSpeeds[] = {1000, 800, 700, 600, 500, 400, 300, 200, 100, 80}; // Minimum speed set to 80ms

// Function to calculate the speed of shape movement based on the level
int getMoveDownSpeed() {
    return (level < 10) ? levelSpeeds[level - 1] : 80; // Cap minimum speed to 80ms
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

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing TFT display...");

    tft.init();
    tft.setRotation(0);
    backgroundColor = tft.color565(30, 30, 30);

    Serial.println("Drawing grid...");
    drawGrid();

    srand(millis()); // Seed random generator

    // Configure buttons
    pinMode(BUTTON_LEFT, INPUT_PULLUP);
    pinMode(BUTTON_RIGHT, INPUT_PULLUP);

    // Create the first shape
    shape = createRandomShape();
    if (shape) {
        shape->moveToLowestBlockkAtMinusOne();
        shape->drawShape(tft, BOX_SIZE);
    } else {
        Serial.println("Failed to create initial shape.");
    }

    lastMoveDownTime = millis(); // Initialize move-down timing
}

void handleButtonMovement(unsigned long currentTime) {
    // Handle left button
    if (digitalRead(BUTTON_LEFT) == LOW) {
        if (!leftButtonHeld) {
            lastMoveLeftTime = currentTime; // Initial press, set timing
            leftButtonHeld = true;
            if (shape && shape->isMovableToTheLeft(blockMap)) {
                shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                shape->moveLeft(blockMap);
                shape->drawShape(tft, BOX_SIZE);
            }
        } else if (currentTime - lastMoveLeftTime >= initialMoveDelay) {
            // Continuous movement after initial delay
            if (currentTime - lastMoveLeftTime >= continuousMoveInterval) {
                lastMoveLeftTime = currentTime; // Reset timing for continuous movement
                if (shape && shape->isMovableToTheLeft(blockMap)) {
                    shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                    shape->moveLeft(blockMap);
                    shape->drawShape(tft, BOX_SIZE);
                }
            }
        }
    } else {
        leftButtonHeld = false; // Reset button state when released
    }

    // Handle right button
    if (digitalRead(BUTTON_RIGHT) == LOW) {
        if (!rightButtonHeld) {
            lastMoveRightTime = currentTime; // Initial press, set timing
            rightButtonHeld = true;
            if (shape && shape->isMovableToTheRight(blockMap)) {
                shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                shape->moveRight(blockMap);
                shape->drawShape(tft, BOX_SIZE);
            }
        } else if (currentTime - lastMoveRightTime >= initialMoveDelay) {
            // Continuous movement after initial delay
            if (currentTime - lastMoveRightTime >= continuousMoveInterval) {
                lastMoveRightTime = currentTime; // Reset timing for continuous movement
                if (shape && shape->isMovableToTheRight(blockMap)) {
                    shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                    shape->moveRight(blockMap);
                    shape->drawShape(tft, BOX_SIZE);
                }
            }
        }
    } else {
        rightButtonHeld = false; // Reset button state when released
    }
}

void loop() {
    unsigned long currentTime = millis();
    
    // Handle button inputs with refined logic
    handleButtonMovement(currentTime);

    // Handle shape movement downwards based on timing
    if (shape) {
        if (currentTime - lastMoveDownTime >= getMoveDownSpeed()) {
            lastMoveDownTime = currentTime; // Reset timing for next move
            if (shape->isMovableDownWards(blockMap)) {
                shape->eraseShape(tft, BOX_SIZE, backgroundColor); // Erase old position
                shape->moveDown(blockMap); // Move shape down
                shape->drawShape(tft, BOX_SIZE); // Draw new position
            } else {
                Serial.println("Shape cannot move down. Adding to block map.");

                // Add blocks to the block map
                if (shape->getBlockList() != nullptr) {
                    blockMap.addBlocks(shape->getBlockList(), 4); // Ensure size matches number of blocks
                } else {
                    Serial.println("Error: Block list is null.");
                }

                // Clear full lines and update score/level
                int clearedLines = blockMap.clearFullLines(*shape, tft, BOX_SIZE, backgroundColor); // Clear lines occupied by the shape
                if (clearedLines > 0) {
                    updateScoreAndLevel(clearedLines); // Update score and level based on cleared lines
                    blockMap.moveAllNotEmptyLinesDown(tft, clearedLines, BOX_SIZE, backgroundColor); // Move lines down if necessary
                }

                shape->eraseShape(tft, BOX_SIZE, backgroundColor); // Erase old position
                delete shape; // Free memory
                shape = nullptr; // Reset pointer
            }
        }
    } else {
        Serial.println("Creating a new shape.");
        shape = createRandomShape();
        if (shape) {
            shape->moveToLowestBlockkAtMinusOne();
            shape->drawShape(tft, BOX_SIZE);
        } else {
            Serial.println("Failed to create new shape.");
        }
    }

    // Draw all blocks in the block map after updating
    blockMap.drawAllBlocks(tft, BOX_SIZE);
}