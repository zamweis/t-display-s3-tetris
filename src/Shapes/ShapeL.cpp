#include "ShapeL.h"

ShapeL::ShapeL() : Shape() {
    uint16_t color = TFT_ORANGE;

    if (rotatePos == getROTATEPOSITION1()) {
        setBlock(Block(4, -30, color), 0);
    } else if (rotatePos == getROTATEPOSITION2()) {
        setBlock(Block(5, -30, color), 0);
    } else {
        setBlock(Block(4, -30, color), 0);
    }

    setPoint(0, 0, Point(1, 1));
    setPoint(0, 1, Point(0, 1));
    setPoint(0, 2, Point(0, -1));

    setPoint(1, 0, Point(1, -1));
    setPoint(1, 1, Point(1, 0));
    setPoint(1, 2, Point(-1, 0));

    setPoint(2, 0, Point(-1, -1));
    setPoint(2, 1, Point(0, -1));
    setPoint(2, 2, Point(0, 1));

    setPoint(3, 0, Point(-1, 1));
    setPoint(3, 1, Point(-1, 0));
    setPoint(3, 2, Point(1, 0));
    
    generateShape();
}
