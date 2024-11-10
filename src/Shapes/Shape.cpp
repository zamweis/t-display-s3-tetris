#include <vector>
#include <cstdlib> // For rand() and srand()
#include "Block.h"
#include "TFT_eSPI.h"
#include "Utils/Utils.h" // Assuming `Utils` is a header file with random utility functions
#include "Utils/Point.h" // Assuming you have a `Point` class

class Shape {
private:
    static const int ROTATEPOSITION0 = 0;
    static const int ROTATEPOSITION1 = 1;
    static const int ROTATEPOSITION2 = 2;
    static const int ROTATEPOSITION3 = 3;

    Block blockList[4];
    Point positions[4][3];
    int rotatePos;

public:
    // Constructor
    Shape() : rotatePos(Utils::rand(0, 3)) {
        // Additional initialization if necessary
    }

    // Virtual destructor for base class
    virtual ~Shape() {}

    // Accessor methods
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

    static int getROTATEPOSITION0() {
        return ROTATEPOSITION0;
    }

    static int getROTATEPOSITION1() {
        return ROTATEPOSITION1;
    }

    static int getROTATEPOSITION2() {
        return ROTATEPOSITION2;
    }

    static int getROTATEPOSITION3() {
        return ROTATEPOSITION3;
    }

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

    int getXPosition(int index) {
        if (index >= 0 && index < 3) {
            return blockList[0].getX() + static_cast<int>(positions[rotatePos][index].getX());
        }
        return blockList[0].getX(); // Default fallback
    }

    int getYPosition(int index) {
        if (index >= 0 && index < 3) {
            return blockList[0].getY() + static_cast<int>(positions[rotatePos][index].getY());
        }
        return blockList[0].getY(); // Default fallback
    }

    // Pure virtual functions for rotation and movement checks (assuming you plan to implement these)
    virtual bool isRotatableAntiClockwise(BlockMap& blockMap) = 0;
    virtual bool isRotatableClockwise(BlockMap& blockMap) = 0;
    virtual void rotateAntiClockwise(BlockMap& blockMap) = 0;
    virtual void rotateClockwise(BlockMap& blockMap) = 0;

    Block getLeftBlock() {
        Block leftBlock = blockList[0];
        for (int i = 1; i <= 3; i++) {
            if (leftBlock.getX() > blockList[i].getX()) {
                leftBlock = blockList[i];
            }
        }
        return leftBlock;
    }

    // Movement methods (assuming BlockMap is defined somewhere)
    virtual bool isMovableToTheLeft(BlockMap& blockMap) = 0;
    virtual bool isMovableToTheRight(BlockMap& blockMap) = 0;
    virtual bool isMovableDownwards(BlockMap& blockMap) = 0;

    virtual void moveLeft(BlockMap& blockMap) = 0;
    virtual void moveRight(BlockMap& blockMap) = 0;
    virtual void moveDown(BlockMap& blockMap) = 0;
    virtual void fallDown(BlockMap& blockMap) = 0;

    // Drawing methods
    void drawShape(TFT_eSPI& tft, int boxSize) {
        for (int i = 0; i < 4; i++) {
            blockList[i].draw(tft, boxSize);
        }
    }

    virtual void drawNextShape(TFT_eSPI& tft, int boxSize) = 0; // Pure virtual function

    void drawShapeBorderOnly(TFT_eSPI& tft, int boxSize, int offset) {
        for (int i = 0; i < 4; i++) {
            blockList[i].drawBorderOnly(tft, boxSize, offset);
        }
    }
};
