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

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display

// Function to create a shape based on an index
Shape* createShapeByIndex(int index, int x, int y) {
    Shape* shape = nullptr;
    uint16_t color;

    // Debug: Print shape creation index
    Serial.print("Creating shape with index: ");
    Serial.println(index);

    switch (index) {
        case 0: shape = new ShapeI(); break;
        case 1: shape = new ShapeJ(); break;
        case 2: shape = new ShapeL(); break;
        case 3: shape = new ShapeO(); break;
        case 4: shape = new ShapeS(); break;
        case 5: shape = new ShapeT(); break;
        case 6: shape = new ShapeZ(); break;
        default:
            Serial.println("Invalid shape index!");
            return nullptr; // Fallback in case of an unexpected index
    }
    
    if (shape) {
        // Debug: Confirm shape creation
        Serial.println("Shape created successfully.");
        Serial.print("Shape rotation: ");
        Serial.println(shape->getRotatePosition());

        // Print block information
        Serial.println("Block list for the shape:");
        for (int i = 0; i < 4; ++i) {
            try {
                Block& block = shape->getBlock(i);
                Serial.print("Block ");
                Serial.print(i);
                Serial.print(" - X: ");
                Serial.print(block.getX());
                Serial.print(", Y: ");
                Serial.print(block.getY());
                Serial.print(", Color: ");
                Serial.println(block.getColor());
            } catch (const std::out_of_range& e) {
                Serial.print("Error accessing block ");
                Serial.print(i);
                Serial.println(": Index out of range.");
            }
        }
    }

    return shape;
}

void drawGrid(TFT_eSPI& tft) {
    // Background
    tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, tft.color565(30, 30, 30)); // Dark background color

    // Columns
    tft.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_BLACK); // Outer border
    for (int col = 0; col <= SCREEN_WIDTH / BOX_SIZE; ++col) {
        int x = col * BOX_SIZE;
        tft.drawLine(x, 0, x, SCREEN_HEIGHT, TFT_BLACK);
    }

    // Rows
    for (int row = 0; row <= SCREEN_HEIGHT / BOX_SIZE; ++row) {
        int y = row * BOX_SIZE;
        tft.drawLine(0, y, SCREEN_WIDTH, y, TFT_BLACK);
    }

    // Debug: Print completion message for grid drawing
    Serial.println("Grid drawn successfully.");
}

void setup() {
    Serial.begin(115200); // Initialize serial communication for debugging
    Serial.println("Initializing TFT display...");

    tft.init();
    tft.setRotation(0);  // Set display to portrait mode
    
    Serial.println("Drawing grid...");
    drawGrid(tft);
    
    // Seed random generator
    srand(static_cast<unsigned int>(time(nullptr)));
    Serial.println("Random generator seeded.");
}

void loop() {
    drawGrid(tft);
    // Calculate center position for the shapes
    int centerX = (SCREEN_WIDTH / 2) / BOX_SIZE;
    int centerY = (SCREEN_HEIGHT / 2) / BOX_SIZE;

    // Debug: Print center positions
    //Serial.print("Center positions - X: ");
    //Serial.print(centerX);
    //Serial.print(", Y: ");
   // Serial.println(centerY);

    // Loop through and draw each shape, one at a time
    for (int i = 0; i < 7; ++i) {
        // Create and draw the shape
        Shape* shape = createShapeByIndex(i, centerX, centerY);
        if (shape) {
            //Serial.print("Drawing shape with index: ");
            //Serial.println(i);

            shape->drawShape(tft, BOX_SIZE);

            // Debug: Indicate completion of shape drawing
           //Serial.println("Shape drawn. Waiting...");

            delay(1000); // Wait for 1 second before drawing the next shape

            // Debug: Deleting shape
            //Serial.println("Deleting shape...");
            delete shape;
            //Serial.println("Shape deleted.");
        } else {
            Serial.println("Failed to create shape.");
        }
    }
}
