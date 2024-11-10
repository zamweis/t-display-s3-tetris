#include "ShapeI.h"
#include "Block.h"
#include "Point.h"
#include <iostream>

// Constructor implementation for ShapeI
ShapeI::ShapeI() : Shape() {
    // Initialize rotation position
    uint16_t color = TFT_WHITE; // Example 16-bit color representation (yellow)

    // Initialize blocks based on rotation position
    if (rotatePos == ROTATEPOSITION0) {
        blockList[0] = Block(5, -1, color);
    } else if (rotatePos == ROTATEPOSITION1) {
        blockList[0] = Block(4, -2, color);
    } else if (rotatePos == ROTATEPOSITION2) {
        blockList[0] = Block(3, -1, color);
    } else {
        blockList[0] = Block(4, -3, color);
    }

    // Setting points based on rotation positions
    setPoint(0, 0, Point(1, 0));
    setPoint(0, 1, Point(-1, 0));
    setPoint(0, 2, Point(-2, 0));

    setPoint(1, 0, Point(0, 1));
    setPoint(1, 1, Point(0, -1));
    setPoint(1, 2, Point(0, -2));

    setPoint(2, 0, Point(-1, 0));
    setPoint(2, 1, Point(1, 0));
    setPoint(2, 2, Point(2, 0));

    setPoint(3, 0, Point(0, -1));
    setPoint(3, 1, Point(0, 1));
    setPoint(3, 2, Point(0, 2));

    generateShape();
}

// Implementation of drawNextShape
void ShapeI::drawNextShape(TFT_eSPI& tft, int boxSize) {
    for (int i = 0; i < 4; ++i) {
        if (rotatePos == ROTATEPOSITION1 || rotatePos == ROTATEPOSITION3) {
            std::cout << "Drawing block with offset (40, 0, 20) at x: " 
                      << blockList[i].getX() << ", y: " << blockList[i].getY() << std::endl;
        } else if (rotatePos == ROTATEPOSITION2) {
            std::cout << "Drawing block with offset (40, 20, 0) at x: " 
                      << blockList[i].getX() << ", y: " << blockList[i].getY() << std::endl;
        } else {
            std::cout << "Drawing block with offset (40, -20, 0) at x: " 
                      << blockList[i].getX() << ", y: " << blockList[i].getY() << std::endl;
        }
    }
}
