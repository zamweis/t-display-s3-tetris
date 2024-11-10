#include "Block.h"
#include "Shape.h"
#include "Point.h"
#include <iostream>

class ShapeT : public Shape {
public:
    ShapeT() {
        rotatePos = ROTATEPOSITION0;
        uint16_t color = 0xFFAA00; // Example 16-bit color (adjust as needed)

        // Initial block setup based on rotation position
        blockList[0] = Block(4, -2, color);

        // Setting points for each rotation state
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
    }

    void drawNextShape(TFT_eSPI& tft, int boxSize) override {
        for (int i = 0; i < 4; ++i) {
            std::cout << "Drawing block at x: " << blockList[i].getX()
                      << ", y: " << blockList[i].getY() << std::endl;
        }
    }
};
