#include "Block.h"
#include "Shape.h"
#include "Point.h"
#include "TFT_eSPI.h" // Include your display library if needed

class ShapeL : public Shape {
public:
    ShapeL() {
        // Set initial rotation position
        rotatePos = ROTATEPOSITION0;
        uint16_t color = 0x00B5B5; // Example color in 16-bit format (adjust as needed)

        // Initialize blocks based on rotation position
        if (rotatePos == getROTATEPOSITION1()) {
            setBlock(Block(4, -1, color), 0);
        } else if (rotatePos == getROTATEPOSITION2()) {
            setBlock(Block(5, -2, color), 0);
        } else {
            setBlock(Block(4, -2, color), 0);
        }

        // Setting points for each rotation state
        // Rotate position 0
        setPoint(0, 0, Point(1, 1));
        setPoint(0, 1, Point(0, 1));
        setPoint(0, 2, Point(0, -1));

        // Rotate position 1
        setPoint(1, 0, Point(1, -1));
        setPoint(1, 1, Point(1, 0));
        setPoint(1, 2, Point(-1, 0));

        // Rotate position 2
        setPoint(2, 0, Point(-1, -1));
        setPoint(2, 1, Point(0, -1));
        setPoint(2, 2, Point(0, 1));

        // Rotate position 3
        setPoint(3, 0, Point(-1, 1));
        setPoint(3, 1, Point(-1, 0));
        setPoint(3, 2, Point(1, 0));
    }

    void drawNextShape(TFT_eSPI& tft, int boxSize) override {
        int rotatePos = getRotatePosition();
        for (int i = 0; i <= 3; ++i) {
            if (rotatePos == 1 || rotatePos == 3) {
                blockList[i].drawWithOffset(tft, boxSize, 0, -20);
            } else {
                blockList[i].drawWithOffset(tft, boxSize, -20, 0);
            }
        }
    }
};
