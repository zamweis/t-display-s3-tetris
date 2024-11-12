#include "ShapeI.h"
#include <iostream>

// Constructor implementation for ShapeI
ShapeI::ShapeI() : Shape() {
    // Initialize rotation position
    uint16_t color = TFT_WHITE; // Example 16-bit color representation (yellow)

    // Initialize blocks based on rotation position
    if (rotatePos == ROTATEPOSITION1 || rotatePos == ROTATEPOSITION3) {
        blockList[0] = Block(5, -30, color);
    } else {
        blockList[0] = Block(5, -30, color);
    }

    // Setting points based on rotation positions
    setPoint(0, 0, Point(1, 0));
    setPoint(0, 1, Point(-1, 0));
    setPoint(0, 2, Point(-2, 0));

    setPoint(1, 0, Point(0, 1));
    setPoint(1, 1, Point(0, -1));
    setPoint(1, 2, Point(0, -2));

    setPoint(2, 0, Point(1, 0));
    setPoint(2, 1, Point(-1, 0));
    setPoint(2, 2, Point(-2, 0));

    setPoint(3, 0, Point(0, 1));
    setPoint(3, 1, Point(0, -1));
    setPoint(3, 2, Point(0, -2));

    generateShape();
}