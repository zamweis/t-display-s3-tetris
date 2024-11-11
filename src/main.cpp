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

uint16_t backgroundColor;
TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display
Shape* shape = nullptr;
BlockMap blockMap;

// Game variables
unsigned long lastMoveDownTime = 0;
int score = 0;
int level = 1;
int linesCleared = 0;
const int initialSpeed = 1000; // Initial move-down delay in ms

// Function to calculate the speed of shape movement based on the level
int getMoveDownSpeed() {
    return initialSpeed / (1 + level / 5); // Adjust speed as level increases
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
        score += pointsPerLine[clearedLines - 1] * level;
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

void loop() {
    if (shape) {
        if (shape->isMovableDownWards(blockMap)) {
            shape->eraseShape(tft, BOX_SIZE, backgroundColor); // Erase old position
            shape->moveDown(blockMap); // Move shape down
            shape->drawShape(tft, BOX_SIZE); // Draw new position
        } else {
            Serial.println("Shape cannot move down. Adding to block map.");

            // Add null pointer and bounds check
            if (shape->getBlockList() != nullptr) {
                blockMap.addBlocks(shape->getBlockList(), 4); // Ensure size matches number of blocks
            } else {
                Serial.println("Error: Block list is null.");
            }
            delete shape; // Free memory
            shape = nullptr; // Reset pointer
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

    delay(50); // Small delay to avoid excessive CPU usage
}
