#include "ShapeT.h"

ShapeT::ShapeT() : Shape() {
    uint16_t color = TFT_MAGENTA;
    
    if (getRotatePosition() == getROTATEPOSITION1()) {
        blockList[0] = Block(4, -30, color);
    } else {
        blockList[0] = Block(4, -30, color);
    }

    setPoint(0, 0, Point(0, -1));
    setPoint(0, 1, Point(1, 0));
    setPoint(0, 2, Point(0, 1));

    setPoint(1, 0, Point(-1, 0));
    setPoint(1, 1, Point(0, -1));
    setPoint(1, 2, Point(1, 0));

    setPoint(2, 0, Point(0, 1));
    setPoint(2, 1, Point(-1, 0));
    setPoint(2, 2, Point(0, -1));

    setPoint(3, 0, Point(1, 0));
    setPoint(3, 1, Point(0, 1));
    setPoint(3, 2, Point(-1, 0));
    
    generateShape();
}