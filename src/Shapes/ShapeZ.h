#ifndef SHAPEZ_H
#define SHAPEZ_H

#include "Shape.h"
#include "TFT_eSPI.h"

class ShapeZ : public Shape {
public:
    ShapeZ() {
        // Initialize blocks and points specific to ShapeZ
        blockList[0] = Block(4, -2, 0xF800); // Example color
        // Set other blocks and rotation points as needed
    }

    void drawNextShape(TFT_eSPI& tft, int boxSize) override {
        // Custom drawing logic for ShapeZ
        for (int i = 0; i < 4; ++i) {
            tft.drawRect(blockList[i].getX() * boxSize, blockList[i].getY() * boxSize, boxSize, boxSize, 0xFFFF);
        }
    }
};

#endif // SHAPEZ_H
