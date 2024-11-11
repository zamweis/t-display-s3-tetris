#include "ShapeO.h"
#include "Block.h"
#include "Point.h"

ShapeO::ShapeO() : Shape() {
    uint16_t color = TFT_YELLOW;

    blockList[0] = Block(4, -30, color);

    for (int i = 0; i < 4; ++i) {
        setPoint(i, 0, Point(1, 0));
        setPoint(i, 1, Point(0, -1));
        setPoint(i, 2, Point(1, -1));
    }
    
    generateShape();
}