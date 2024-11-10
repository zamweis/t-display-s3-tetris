#include "Block.h"
#include "Shape.h"
#include "Point.h"
#include <iostream>

class ShapeO : public Shape {
public:
    ShapeO() {
        rotatePos = ROTATEPOSITION0;
        uint16_t color = 0xC80000; // Example 16-bit color (adjust as needed)

        // Initialize blocks
        blockList[0] = Block(4, -1, color);

        // Setting points (since ShapeO typically doesn't rotate)
        for (int i = 0; i < 4; ++i) {
            setPoint(i, 0, Point(1, 0));
            setPoint(i, 1, Point(0, -1));
            setPoint(i, 2, Point(1, -1));
        }
    }

    void drawNextShape(TFT_eSPI& tft, int boxSize) override {
        // Example drawing logic
        for (int i = 0; i < 4; ++i) {
            std::cout << "Drawing block at x: " << blockList[i].getX()
                      << ", y: " << blockList[i].getY() << std::endl;
        }
    }
};
