#include "ShapeT.h"
#include "Block.h"
#include "Point.h"

ShapeT::ShapeT() : Shape() {
    uint16_t color = TFT_MAGENTA;

    blockList[0] = Block(4, 1, color);

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
