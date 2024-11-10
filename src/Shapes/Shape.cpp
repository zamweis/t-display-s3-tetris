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

    // Constants for map dimensions (assumed values, replace with actual if different)
    static constexpr int MAP_WIDTH = 10;
    static constexpr int MAP_HEIGHT = 20;

    Block blockList[NUM_BLOCKS];
    Point positions[NUM_BLOCKS][NUM_POSITIONS];
    int rotatePos = std::rand() % 4; 

protected:
    // Protected Constructor
    Shape() {
        // Randomly select a valid rotation position
        rotatePos = std::rand() % 4; // Generates a number between 0 and 3
        Serial.print("Initialized rotatePos to: ");
        Serial.println(rotatePos); // Debug print to check random value
    }

public:
    // Virtual destructor for base class
    virtual ~Shape() {}

    // Gets the blockList where all the blocks are located in
    Block* getBlockList() {
        return blockList;
    }

    // Gets the list which is used to generate a shape
    Point (*getPositions())[NUM_POSITIONS] {
        return positions;
    }

    // Sets a block into blockList
    void setBlock(const Block& block, int index) {
        if (index >= 0 && index < NUM_BLOCKS) {
            blockList[index] = block;
        }
    }

    // Sets a point into positions
    void setPoint(int x, int y, const Point& point) {
        if (x >= 0 && x < NUM_BLOCKS && y >= 0 && y < NUM_POSITIONS) {
            positions[x][y] = point;
        }
    }

    // Sets a value to rotatePosition
    void setRotatePosition(int rotatePosition) {
        if (rotatePosition >= ROTATEPOSITION0 && rotatePosition <= ROTATEPOSITION3) {
            rotatePos = rotatePosition;
        }
    }

    // Gets the current rotatePos
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

    // Gets the block from blockList at the indexed position
    Block& getBlock(int index) {
        if (index >= 0 && index < NUM_BLOCKS) {
            return blockList[index];
        }
        throw std::out_of_range("Index out of range");
    }

    // Sets the blocks of the new Shape into the blockList
    void generateShape() {
        uint16_t color = blockList[0].getColor();
        for (int i = 1; i < NUM_BLOCKS; ++i) {
            blockList[i] = Block(getXPosition(i - 1), getYPosition(i - 1), color);
        }
    }

    // Gets the horizontal coordinate of the indexed block of blockList
    int getXPosition(int index) const {
        if (index >= 0 && index < NUM_POSITIONS) {
            return blockList[0].getX() + static_cast<int>(positions[rotatePos][index].getX());
        }
        return blockList[0].getX(); // Default fallback
    }

    // Gets the vertical coordinate of the indexed block of blockList
    int getYPosition(int index) const {
        if (index >= 0 && index < NUM_POSITIONS) {
            return blockList[0].getY() + static_cast<int>(positions[rotatePos][index].getY());
        }
        return blockList[0].getY(); // Default fallback
    }

    // Checks if the shape is allowed to rotate anticlockwise
    bool isRotatableAntiClockwise(BlockMap& blockMap) {
        int tmpRotatePosition = (rotatePos == ROTATEPOSITION3) ? ROTATEPOSITION0 : rotatePos + 1;
        return checkRotationValidity(tmpRotatePosition, blockMap);
    }

    // Checks if the shape is allowed to rotate clockwise
    bool isRotatableClockwise(BlockMap& blockMap) {
        int tmpRotatePosition = (rotatePos == ROTATEPOSITION0) ? ROTATEPOSITION3 : rotatePos - 1;
        return checkRotationValidity(tmpRotatePosition, blockMap);
    }

    // Rotates the shape anticlockwise
    void rotateAntiClockwise(BlockMap& blockMap) {
        if (isRotatableAntiClockwise(blockMap)) {
            rotatePos = (rotatePos == ROTATEPOSITION3) ? ROTATEPOSITION0 : rotatePos + 1;
            generateShape();
        }
    }

    // Rotates the shape clockwise
    void rotateClockwise(BlockMap& blockMap) {
        if (isRotatableClockwise(blockMap)) {
            rotatePos = (rotatePos == ROTATEPOSITION0) ? ROTATEPOSITION3 : rotatePos - 1;
            generateShape();
        }
    }

    // Gets the block with the lowest horizontal-coordinate value
    Block getLeftBlock() {
        Block leftBlock = blockList[0];
        for (int i = 1; i < NUM_BLOCKS; ++i) {
            if (leftBlock.getX() > blockList[i].getX()) {
                leftBlock = blockList[i];
            }
        }
        return leftBlock;
    }

    // Checks if all the blocks of the shape can be moved one field to the left
    bool isMovableToTheLeft(BlockMap& blockMap) {
        if (getLeftBlock().getX() == 0) {
            return false;
        }
        for (const auto& block : blockList) {
            if (isInCollisionWithLeftBlock(block, blockMap)) {
                return false;
            }
        }
        return true;
    }

    // Checks if a given block can be moved one field to the left without colliding
    bool isInCollisionWithLeftBlock(const Block& block, BlockMap& blockMap) {
        int x = block.getX() - 1;
        int y = block.getY();
        return !blockMap.isFieldEmpty(x, y);
    }

    // Moves all the blocks of the shape one field to the left
    void moveLeft(BlockMap& blockMap) {
        if (isMovableToTheLeft(blockMap)) {
            for (auto& block : blockList) {
                block.moveLeft();
            }
        }
    }

    // Gets the block with the highest horizontal coordinate value
    Block getRightBlock() {
        Block rightBlock = blockList[0];
        for (int i = 1; i < NUM_BLOCKS; ++i) {
            if (rightBlock.getX() < blockList[i].getX()) {
                rightBlock = blockList[i];
            }
        }
        return rightBlock;
    }

    // Checks if all the blocks of the shape can be moved one field to the right
    bool isMovableToTheRight(BlockMap& blockMap) {
        if (getRightBlock().getX() == MAP_WIDTH - 1) {
            return false;
        }
        for (const auto& block : blockList) {
            if (isInCollisionWithRightBlock(block, blockMap)) {
                return false;
            }
        }
        return true;
    }

    // Checks if a given block can be moved one field to the right without colliding
    bool isInCollisionWithRightBlock(const Block& block, BlockMap& blockMap) {
        int x = block.getX() + 1;
        int y = block.getY();
        return !blockMap.isFieldEmpty(x, y);
    }

    // Moves all the blocks of the shape one field to the right
    void moveRight(BlockMap& blockMap) {
        if (isMovableToTheRight(blockMap)) {
            for (auto& block : blockList) {
                block.moveRight();
            }
        }
    }

    // Gets the block with the highest vertical coordinate value
    Block getLowestBlock() {
        Block lowestBlock = blockList[0];
        for (int i = 1; i < NUM_BLOCKS; ++i) {
            if (lowestBlock.getY() < blockList[i].getY()) {
                lowestBlock = blockList[i];
            }
        }
        return lowestBlock;
    }

    // Checks if all the blocks of the shape can be moved one field downwards
    bool isMovableDownWards(BlockMap& blockMap) {
        if (getLowestBlock().getY() == MAP_HEIGHT - 1) {
            return false;
        }
        for (const auto& block : blockList) {
            if (isInCollisionWithLowerBlock(block, blockMap)) {
                return false;
            }
        }
        return true;
    }

    // Checks if a given block can be moved one field downwards without colliding
    bool isInCollisionWithLowerBlock(const Block& block, BlockMap& blockMap) {
        int x = block.getX();
        int y = block.getY() + 1;
        return !blockMap.isFieldEmpty(x, y);
    }

    // Moves all the blocks of the shape one field downwards
    void moveDown(BlockMap& blockMap) {
        if (isMovableDownWards(blockMap)) {
            for (auto& block : blockList) {
                block.moveDown();
            }
        }
    }

    // Moves the shape downwards as far as possible
    void fallDown(BlockMap& blockMap) {
        while (isMovableDownWards(blockMap)) {
            moveDown(blockMap);
        }
    }

    // Draws all the blocks of the shape
    void drawShape(TFT_eSPI& tft, int boxSize) const {
        for (int i = 0; i < NUM_BLOCKS; ++i) {
            blockList[i].draw(tft, boxSize);
        }
    }

    // Pure virtual function for derived classes to implement drawing next shape
    virtual void drawNextShape(TFT_eSPI& tft, int boxSize) = 0;

    // Draws only the borders of every block of the shape
    void drawShapeBorderOnly(TFT_eSPI& tft, int boxSize, int offset) const {
        for (const auto& block : blockList) {
            block.drawBorderOnly(tft, boxSize, offset);
        }
    }

private:
    // Helper method for rotation validation
    bool checkRotationValidity(int tmpRotatePosition, BlockMap& blockMap) {
        for (int i = 1; i < NUM_BLOCKS; ++i) {
            int x = blockList[0].getX() + static_cast<int>(positions[tmpRotatePosition][i - 1].getX());
            int y = blockList[0].getY() + static_cast<int>(positions[tmpRotatePosition][i - 1].getY());
            if (y >= MAP_HEIGHT || x < 0 || x >= MAP_WIDTH || !blockMap.isFieldEmpty(x, y)) {
                return false;
            }
        }
        return true;
    }
};
