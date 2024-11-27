#include "ShapeJ.h"

ShapeJ::ShapeJ() : Shape() {
    uint16_t color = TFT_BLUE;
    numRotations = 4;
    rotatePos = random(0, numRotations);
    // Initialisierung Mainblock
    blockList[0] = Block(4, 0, color);

    // Coordinates of the blocks relative to the mainblock
    // Rotation 0
    setPoint(0, 0, Point(0, -1));
    setPoint(0, 1, Point(0, 1));
    setPoint(0, 2, Point(-1, 1));

    // Rotation 1
    setPoint(1, 0, Point(1, 0));
    setPoint(1, 1, Point(-1, 0));
    setPoint(1, 2, Point(-1, -1));

    // Rotation 2
    setPoint(2, 0, Point(0, 1));
    setPoint(2, 1, Point(0, -1));
    setPoint(2, 2, Point(1, -1));

    // Rotation 3
    setPoint(3, 0, Point(-1, 0));
    setPoint(3, 1, Point(1, 0));
    setPoint(3, 2, Point(1, 1));

    generateShape();
    //Serial.println("ShapeJ: created");
}
