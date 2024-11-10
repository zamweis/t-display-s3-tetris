#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For seeding rand() with time()
#include "Block.h"
#include "BlockMap.h"
#include "TFT_eSPI.h"
#include "Point.h" // Assuming you have a `Point` class

class Shape {
private:
    // Constants for rotation positions
    static constexpr int ROTATEPOSITION0 = 0;
    static constexpr int ROTATEPOSITION1 = 1;
    static constexpr int ROTATEPOSITION2 = 2;
    static constexpr int ROTATEPOSITION3 = 3;

    // Constants for block and position array sizes
    static constexpr int NUM_BLOCKS = 4;
    static constexpr int NUM_POSITIONS = 3;

    Block blockList[NUM_BLOCKS];
    Point positions[NUM_BLOCKS][NUM_POSITIONS];
    int rotatePos;

public:
    // Constructor
    Shape() : rotatePos(std::rand() % NUM_BLOCKS) { // Initialize rotation position randomly
        // Seed the random number generator if not already done elsewhere
        static bool isSeeded = false;
        if (!isSeeded) {
            std::srand(std::time(nullptr));
            isSeeded = true;
        }
    }

    // Virtual destructor for base class
    virtual ~Shape() {}

    // Accessor methods
    Block* getBlockList() {
        return blockList;
    }

    Point (*getPositions())[NUM_POSITIONS] {
        return positions;
    }

    void setBlock(const Block& block, int index) {
        if (index >= 0 && index < NUM_BLOCKS) {
            blockList[index] = block;
        }
    }

    void setPoint(int x, int y, const Point& point) {
        if (x >= 0 && x < NUM_BLOCKS && y >= 0 && y < NUM_POSITIONS) {
            positions[x][y] = point;
        }
    }

    void setRotatePosition(int rotatePosition) {
        if (rotatePosition >= ROTATEPOSITION0 && rotatePosition <= ROTATEPOSITION3) {
            rotatePos = rotatePosition;
        }
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
        if (index >= 0 && index < NUM_BLOCKS) {
            return blockList[index];
        }
        throw std::out_of_range("Index out of range");
    }

    void generateShape() {
        uint16_t color = blockList[0].getColor();
        for (int i = 1; i < NUM_BLOCKS; ++i) {
            blockList[i] = Block(getXPosition(i - 1), getYPosition(i - 1), color);
        }
    }

    int getXPosition(int index) const {
        if (index >= 0 && index < NUM_POSITIONS) {
            return blockList[0].getX() + static_cast<int>(positions[rotatePos][index].getX());
        }
        return blockList[0].getX(); // Default fallback
    }

    int getYPosition(int index) const {
        if (index >= 0 && index < NUM_POSITIONS) {
            return blockList[0].getY() + static_cast<int>(positions[rotatePos][index].getY());
        }
        return blockList[0].getY(); // Default fallback
    }

    // Pure virtual functions for rotation and movement checks (to be implemented in derived classes)
    virtual bool isRotatableAntiClockwise(BlockMap& blockMap) = 0;
    virtual bool isRotatableClockwise(BlockMap& blockMap) = 0;
    virtual void rotateAntiClockwise(BlockMap& blockMap) = 0;
    virtual void rotateClockwise(BlockMap& blockMap) = 0;

    Block getLeftBlock() const {
        Block leftBlock = blockList[0];
        for (int i = 1; i < NUM_BLOCKS; ++i) {
            if (leftBlock.getX() > blockList[i].getX()) {
                leftBlock = blockList[i];
            }
        }
        return leftBlock;
    }

    // Movement methods (to be implemented in derived classes)
    virtual bool isMovableToTheLeft(BlockMap& blockMap) = 0;
    virtual bool isMovableToTheRight(BlockMap& blockMap) = 0;
    virtual bool isMovableDownwards(BlockMap& blockMap) = 0;

    virtual void moveLeft(BlockMap& blockMap) = 0;
    virtual void moveRight(BlockMap& blockMap) = 0;
    virtual void moveDown(BlockMap& blockMap) = 0;
    virtual void fallDown(BlockMap& blockMap) = 0;

    // Drawing methods
    void drawShape(TFT_eSPI& tft, int boxSize) const {
        for (int i = 0; i < NUM_BLOCKS; ++i) {
            blockList[i].draw(tft, boxSize);
        }
    }

    virtual void drawNextShape(TFT_eSPI& tft, int boxSize) = 0; // Pure virtual function

    void drawShapeBorderOnly(TFT_eSPI& tft, int boxSize, int offset) const {
        for (int i = 0; i < NUM_BLOCKS; ++i) {
            blockList[i].drawBorderOnly(tft, boxSize, offset);
        }
    }
};
