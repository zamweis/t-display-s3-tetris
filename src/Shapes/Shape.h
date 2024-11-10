#ifndef SHAPE_H
#define SHAPE_H

#include "Block.h"
#include "BlockMap.h"
#include "Point.h"
#include "TFT_eSPI.h"
#include <stdexcept>

class Shape {
protected:
    static const int ROTATEPOSITION0 = 0;
    static const int ROTATEPOSITION1 = 1;
    static const int ROTATEPOSITION2 = 2;
    static const int ROTATEPOSITION3 = 3;

    Block blockList[4];
    Point positions[4][3];
    int rotatePos;

public:
    // Constructor
    Shape() : rotatePos(ROTATEPOSITION0) {}

    // Virtual destructor for polymorphic behavior
    virtual ~Shape() {}

    // Common methods available to derived classes
    Block* getBlockList() {
        return blockList;
    }

    Point (*getPositions())[3] {
        return positions;
    }

    void setBlock(const Block& block, int index) {
        if (index >= 0 && index < 4) {
            blockList[index] = block;
        }
    }

    void setPoint(int x, int y, const Point& point) {
        if (x >= 0 && x < 4 && y >= 0 && y < 3) {
            positions[x][y] = point;
        }
    }

    void setRotatePosition(int rotatePosition) {
        rotatePos = rotatePosition;
    }

    int getRotatePosition() const {
        return rotatePos;
    }

    static int getROTATEPOSITION0() { return ROTATEPOSITION0; }
    static int getROTATEPOSITION1() { return ROTATEPOSITION1; }
    static int getROTATEPOSITION2() { return ROTATEPOSITION2; }
    static int getROTATEPOSITION3() { return ROTATEPOSITION3; }

    Block& getBlock(int index) {
        if (index >= 0 && index < 4) {
            return blockList[index];
        }
        throw std::out_of_range("Index out of range");
    }

    void generateShape() {
        for (int i = 1; i <= 3; i++) {
            uint16_t color = blockList[0].getColor();
            blockList[i] = Block(getXPosition(i - 1), getYPosition(i - 1), color);
        }
    }

    int getXPosition(int index) const {
        if (index >= 0 && index < 3) {
            return blockList[0].getX() + static_cast<int>(positions[rotatePos][index].getX());
        }
        return blockList[0].getX();
    }

    int getYPosition(int index) const {
        if (index >= 0 && index < 3) {
            return blockList[0].getY() + static_cast<int>(positions[rotatePos][index].getY());
        }
        return blockList[0].getY();
    }

    // Virtual functions for derived classes to implement specific behavior
    virtual bool isRotatableAntiClockwise(BlockMap& blockMap) { return true; } // Default implementation
    virtual bool isRotatableClockwise(BlockMap& blockMap) { return true; } // Default implementation
    virtual void rotateAntiClockwise(BlockMap& blockMap) {}
    virtual void rotateClockwise(BlockMap& blockMap) {}

    virtual bool isMovableToTheLeft(BlockMap& blockMap) { return true; } // Default implementation
    virtual bool isMovableToTheRight(BlockMap& blockMap) { return true; } // Default implementation
    virtual bool isMovableDownwards(BlockMap& blockMap) { return true; } // Default implementation

    virtual void moveLeft(BlockMap& blockMap) {}
    virtual void moveRight(BlockMap& blockMap) {}
    virtual void moveDown(BlockMap& blockMap) {}
    virtual void fallDown(BlockMap& blockMap) {}

    Block getLeftBlock() const {
        Block leftBlock = blockList[0];
        for (int i = 1; i <= 3; i++) {
            if (leftBlock.getX() > blockList[i].getX()) {
                leftBlock = blockList[i];
            }
        }
        return leftBlock;
    }

    // Drawing methods
    void drawShape(TFT_eSPI& tft, int boxSize) {
        for (int i = 0; i < 4; i++) {
            blockList[i].draw(tft, boxSize);
        }
    }

    virtual void drawNextShape(TFT_eSPI& tft, int boxSize) = 0; // Pure virtual function for custom drawing

    void drawShapeBorderOnly(TFT_eSPI& tft, int boxSize, int offset) {
        for (int i = 0; i < 4; i++) {
            blockList[i].drawBorderOnly(tft, boxSize, offset);
        }
    }

    void eraseShape(TFT_eSPI& tft, int boxSize, uint16_t backgroundColor) {
        for (int i = 0; i < 4; i++) {
            blockList[i].draw(tft, boxSize, backgroundColor);
        }
    }
};

#endif // SHAPE_H
