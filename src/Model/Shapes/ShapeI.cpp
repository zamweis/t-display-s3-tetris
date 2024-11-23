#include "ShapeI.h"
#include <iostream>

// Constructor implementation for ShapeI
ShapeI::ShapeI() : Shape() {
    uint16_t color = TFT_WHITE;

    // Setze die Blockpositionen für jede Rotation
    // Ausgangsrotation (horizontal)
    blockList[0] = Block(5, 0, color);
    blockList[1] = Block(5, -1, color);
    blockList[2] = Block(5, -2, color);
    blockList[3] = Block(5, -3, color);

    // Definiere Rotationspunkte für jede mögliche Rotation
    // Rotation 0: horizontal (standard)
    setPoint(0, 0, Point(1, 0));
    setPoint(0, 1, Point(-1, 0));
    setPoint(0, 2, Point(-2, 0));

    // Rotation 1: vertikal
    setPoint(1, 0, Point(0, 1));
    setPoint(1, 1, Point(0, -1));
    setPoint(1, 2, Point(0, -2));

    // Rotation 2: horizontal (identisch zu Rotation 0)
    setPoint(2, 0, Point(1, 0));
    setPoint(2, 1, Point(-1, 0));
    setPoint(2, 2, Point(-2, 0));

    // Rotation 3: vertikal (identisch zu Rotation 1)
    setPoint(3, 0, Point(0, 1));
    setPoint(3, 1, Point(0, -1));
    setPoint(3, 2, Point(0, -2));

    generateShape(); // Erstelle die Shape
    Serial.println("ShapeI: created");
}
