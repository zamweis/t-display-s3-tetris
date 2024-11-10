#include "ShapeL.h"
#include "Block.h"
#include "Point.h"

ShapeL::ShapeL() {
    rotatePos = ROTATEPOSITION0;
    uint16_t color = 0x00B5B5;

    if (rotatePos == getROTATEPOSITION1()) {
        setBlock(Block(4, -1, color), 0);
    } else if (rotatePos == getROTATEPOSITION2()) {
        setBlock(Block(5, -2, color), 0);
    } else {
        setBlock(Block(4, -2, color), 0);
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
}

void ShapeL::drawNextShape(TFT_eSPI& tft, int boxSize) {
    for (int i = 0; i < 4; ++i) {
        blockList[i].drawWithOffset(tft, boxSize, 0, -20);
    }
}
