#include <Arduino.h>
#include "TFT_eSPI.h"
#include "Shapes/ShapeI.h"
#include "Shapes/ShapeJ.h"
#include "Shapes/ShapeL.h"
#include "Shapes/ShapeO.h"
#include "Shapes/ShapeS.h"
#include "Shapes/ShapeT.h"
#include "Shapes/ShapeZ.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135
#define BOX_SIZE 10

void setup() {
    TFT_eSPI tft = TFT_eSPI(); // Initialize your display instance
    tft.init();
    tft.setRotation(1); // Adjust rotation as needed for your display

    // Clear screen
    tft.fillScreen(TFT_BLACK);

    // Calculate center position for the shapes
    int centerX = SCREEN_WIDTH / 2 / BOX_SIZE;
    int centerY = SCREEN_HEIGHT / 2 / BOX_SIZE;

    // Create shapes
    ShapeI shapeI;
    ShapeJ shapeJ;
    ShapeL shapeL;
    ShapeO shapeO;
    ShapeS shapeS;
    ShapeT shapeT;
    ShapeZ shapeZ;

    // Set starting positions
    shapeI.setBlock(Block(centerX, centerY, TFT_WHITE), 0);
    shapeJ.setBlock(Block(centerX, centerY, TFT_BLUE), 0);
    shapeL.setBlock(Block(centerX, centerY, TFT_ORANGE), 0);
    shapeO.setBlock(Block(centerX, centerY, TFT_YELLOW), 0);
    shapeS.setBlock(Block(centerX, centerY, TFT_GREEN), 0);
    shapeT.setBlock(Block(centerX, centerY, TFT_MAGENTA), 0);
    shapeZ.setBlock(Block(centerX, centerY, TFT_RED), 0);

    // Draw shapes
    shapeI.drawNextShape(tft, BOX_SIZE);
    shapeJ.drawNextShape(tft, BOX_SIZE);
    shapeL.drawNextShape(tft, BOX_SIZE);
    shapeO.drawNextShape(tft, BOX_SIZE);
    shapeS.drawNextShape(tft, BOX_SIZE);
    shapeT.drawNextShape(tft, BOX_SIZE);
    shapeZ.drawNextShape(tft, BOX_SIZE);
}

void loop() {
    // Add any update or loop logic here if necessary
}
