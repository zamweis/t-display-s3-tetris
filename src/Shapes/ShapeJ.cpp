#include "ShapeJ.h"
#include <iostream>

ShapeJ::ShapeJ() : Shape() {
    uint16_t color = TFT_BLUE; // Example 16-bit color (adjust as needed)

    // Initialize blocks based on rotation position
    if (rotatePos == ROTATEPOSITION3) {
        blockList[0] = Block(5, -30, color);
    } else {
        blockList[0] = Block(5, -30, color);
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

    generateShape();
}
