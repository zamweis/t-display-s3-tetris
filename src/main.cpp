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
#define BOX_SIZE 10

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display

// Function to create a shape based on an index
Shape* createShapeByIndex(int index, int x, int y) {
    Shape* shape = nullptr;
    uint16_t color;

    switch (index) {
        case 0: shape = new ShapeI(); color = TFT_WHITE; break;
        case 1: shape = new ShapeJ(); color = TFT_BLUE; break;
        case 2: shape = new ShapeL(); color = TFT_ORANGE; break;
        case 3: shape = new ShapeO(); color = TFT_YELLOW; break;
        case 4: shape = new ShapeS(); color = TFT_GREEN; break;
        case 5: shape = new ShapeT(); color = TFT_MAGENTA; break;
        case 6: shape = new ShapeZ(); color = TFT_RED; break;
        default: return nullptr; // Fallback in case of an unexpected index
    }

    // Set the initial block for the shape using the color specific to the shape
    if (shape) {
        shape->setBlock(Block(x, y, color), 0); // Set the initial block with the specified color
    }

    return shape;
}

void setup() {
    tft.init();
    tft.setRotation(0);  // Set display to portrait mode
    tft.fillScreen(TFT_BLACK);
    
    // Define dark color (e.g., dark gray)
    uint16_t darkColor = tft.color565(50, 50, 50);  // RGB value for dark gray

    // Calculate rectangle dimensions and positions with a 10-pixel border
    int rectX = 10;
    int rectY = 10;
    int rectWidth = SCREEN_WIDTH - 20;
    int rectHeight = SCREEN_HEIGHT - 20;

    // Draw filled rectangle
    tft.fillRect(rectX, rectY, rectWidth, rectHeight, darkColor);

    // Seed random generator
    srand(static_cast<unsigned int>(time(nullptr)));
}

void loop() {
}
