#include "ShapeZ.h"
#include "Block.h"
#include "Point.h"

ShapeZ::ShapeZ() {
    rotatePos = ROTATEPOSITION0;
    uint16_t color = TFT_RED;

    blockList[0] = Block(4, -2, color);

    setPoint(0, 0, Point(1, -1));
    setPoint(0, 1, Point(1, 0));
    setPoint(0, 2, Point(0, 1));

    setPoint(1, 0, Point(-1, -1));
    setPoint(1, 1, Point(0, -1));
    setPoint(1, 2, Point(1, 0));

    setPoint(2, 0, Point(-1, 1));
    setPoint(2, 1, Point(-1, 0));
    setPoint(2, 2, Point(0, -1));

    setPoint(3, 0, Point(1, 1));
    setPoint(3, 1, Point(0, 1));
    setPoint(3, 2, Point(-1, 0));
}

void ShapeZ::drawNextShape(TFT_eSPI& tft, int boxSize) {
    for (int i = 0; i < 4; ++i) {
        blockList[i].draw(tft, boxSize);
    }
}
