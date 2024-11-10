#include "ShapeJ.h"
#include "Block.h"
#include "Point.h"
#include <iostream>

ShapeJ::ShapeJ() {
    uint16_t color = TFT_BLUE; // Example 16-bit color (adjust as needed)

    // Initialize blocks based on rotation position
    if (rotatePos == ROTATEPOSITION3) {
        blockList[0] = Block(4, -1, color);
    } else {
        blockList[0] = Block(4, -2, color);
    }

    // Setting points for rotations
    setPoint(0, 0, Point(0, -1));
    setPoint(0, 1, Point(0, 1));
    setPoint(0, 2, Point(-1, 1));

    setPoint(1, 0, Point(-1, 0));
    setPoint(1, 1, Point(1, 0));
    setPoint(1, 2, Point(1, 1));

    setPoint(2, 0, Point(0, 1));
    setPoint(2, 1, Point(0, -1));
    setPoint(2, 2, Point(1, -1));

    setPoint(3, 0, Point(1, 0));
    setPoint(3, 1, Point(-1, 0));
    setPoint(3, 2, Point(-1, -1));
}

void ShapeJ::drawNextShape(TFT_eSPI& tft, int boxSize) {
    for (int i = 0; i < 4; ++i) {
        std::cout << "Drawing block at x: " << blockList[i].getX()
                  << ", y: " << blockList[i].getY() << std::endl;
        // Replace with your drawing logic using tft if necessary
    }
}
