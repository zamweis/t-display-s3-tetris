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

// Function to create a shape based on a random index
Shape* createRandomShape() {
    Shape* newShape = nullptr;
    int index = rand() % 7;  // Generate a random index between 0 and 6
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

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing TFT display...");

    tft.init();
    tft.setRotation(0);
    backgroundColor = tft.color565(30, 30, 30);

    Serial.println("Drawing grid...");
    drawGrid();

    srand(static_cast<unsigned int>(time(nullptr))); // Seed random generator

    // Create the first shape
    shape = createRandomShape();
    if (shape) {
        shape->drawShape(tft, BOX_SIZE);
    } else {
        Serial.println("Failed to create initial shape.");
    }
}

void loop() {
    if (shape) {
        // Check if the shape can move down
        if (shape->isMovableDownWards(blockMap)) {
            Serial.println("Shape is movable downwards. Erasing and redrawing.");
            shape->eraseShape(tft, BOX_SIZE, backgroundColor); // Erase old position
            shape->moveDown(blockMap); // Move shape down
            shape->drawShape(tft, BOX_SIZE); // Draw new position
        } else {
            Serial.println("Shape cannot move down. Adding to block map.");
            // Shape can't move down; add it to the block map
            blockMap.addBlocks(shape->getBlockList(), 4); // Ensure size matches number of blocks
            delete shape; // Free memory
            shape = nullptr; // Reset pointer
        }
    } else {
        Serial.println("Creating a new shape.");
        // Create a new shape when there is no active shape
        shape = createRandomShape();
        if (shape) {
            shape->drawShape(tft, BOX_SIZE); // Draw the new shape
        } else {
            Serial.println("Failed to create new shape.");
        }
    }

    delay(500); // Adjust the speed of shape movement as needed
}
