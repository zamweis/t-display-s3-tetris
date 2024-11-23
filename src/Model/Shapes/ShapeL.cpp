#include "ShapeL.h"

ShapeL::ShapeL() : Shape() {
    uint16_t color = TFT_ORANGE;

    // Initialisierung der Blöcke
    blockList[0] = Block(4, 0, color);

    // Rotation 0
    setPoint(0, 0, Point(0, -1));  // Above
    setPoint(0, 1, Point(0, 1));   // Below
    setPoint(0, 2, Point(1, 1));   // Right and below

    // Rotation 1
    setPoint(1, 0, Point(-1, 0));   // Left
    setPoint(1, 1, Point(1, 0));    // Right
    setPoint(1, 2, Point(1, -1));   // Right and above

    // Rotation 2
    setPoint(2, 0, Point(0, 1));    // Below
    setPoint(2, 1, Point(0, -1));   // Above
    setPoint(2, 2, Point(-1, -1));  // Left and above

    // Rotation 3
    setPoint(3, 0, Point(1, 0));    // Right
    setPoint(3, 1, Point(-1, 0));   // Left
    setPoint(3, 2, Point(-1, 1));   // Left and below


    generateShape();
    Serial.println("ShapeL: created");
}
