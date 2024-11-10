#include "Block.h"
#include "Shape.h"
#include "Point.h"
#include <iostream>

class ShapeI : public Shape {
public:
    ShapeI() {
        // Initialize rotation position
        rotatePos = ROTATEPOSITION0;
        uint16_t color = 0xFFE0; // Example 16-bit color representation (yellow)

        // Initialize blocks based on rotation position
        if (rotatePos == ROTATEPOSITION0) {
            blockList[0] = Block(5, -1, color);
        } else if (rotatePos == ROTATEPOSITION1) {
            blockList[0] = Block(4, -2, color);
        } else if (rotatePos == ROTATEPOSITION2) {
            blockList[0] = Block(3, -1, color);
        } else {
            blockList[0] = Block(4, -3, color);
        }

        // Setting points based on rotation positions
        setPoint(0, 0, Point(1, 0));
        setPoint(0, 1, Point(-1, 0));
        setPoint(0, 2, Point(-2, 0));

        setPoint(1, 0, Point(0, 1));
        setPoint(1, 1, Point(0, -1));
        setPoint(1, 2, Point(0, -2));

        setPoint(2, 0, Point(-1, 0));
        setPoint(2, 1, Point(1, 0));
        setPoint(2, 2, Point(2, 0));

        setPoint(3, 0, Point(0, -1));
        setPoint(3, 1, Point(0, 1));
        setPoint(3, 2, Point(0, 2));
    }

    void drawNextShape(TFT_eSPI& tft, int boxSize) override {
        // Example drawing logic (modify as per your requirements)
        for (int i = 0; i < 4; ++i) {
            if (rotatePos == ROTATEPOSITION1 || rotatePos == ROTATEPOSITION3) {
                // Drawing logic
                std::cout << "Drawing block with offset (40, 0, 20) at x: " 
                          << blockList[i].getX() << ", y: " << blockList[i].getY() << std::endl;
            } else if (rotatePos == ROTATEPOSITION2) {
                std::cout << "Drawing block with offset (40, 20, 0) at x: " 
                          << blockList[i].getX() << ", y: " << blockList[i].getY() << std::endl;
            } else {
                std::cout << "Drawing block with offset (40, -20, 0) at x: " 
                          << blockList[i].getX() << ", y: " << blockList[i].getY() << std::endl;
            }
        }
    }

    // Implementation of pure virtual functions from Shape
    bool isRotatableAntiClockwise(BlockMap& blockMap) override {
        // Provide appropriate implementation for rotation check
        return true; // Placeholder
    }

    bool isRotatableClockwise(BlockMap& blockMap) override {
        // Provide appropriate implementation for rotation check
        return true; // Placeholder
    }

    void rotateAntiClockwise(BlockMap& blockMap) override {
        // Implement rotation logic
    }

    void rotateClockwise(BlockMap& blockMap) override {
        // Implement rotation logic
    }

    bool isMovableToTheLeft(BlockMap& blockMap) override {
        // Implement movement check logic
        return true; // Placeholder
    }

    bool isMovableToTheRight(BlockMap& blockMap) override {
        // Implement movement check logic
        return true; // Placeholder
    }

    bool isMovableDownwards(BlockMap& blockMap) override {
        // Implement movement check logic
        return true; // Placeholder
    }

    void moveLeft(BlockMap& blockMap) override {
        // Implement left movement logic
    }

    void moveRight(BlockMap& blockMap) override {
        // Implement right movement logic
    }

    void moveDown(BlockMap& blockMap) override {
        // Implement downward movement logic
    }

    void fallDown(BlockMap& blockMap) override {
        // Implement fall down logic
    }
};