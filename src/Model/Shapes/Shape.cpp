#include "Shape.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include "Config.h"

Shape::Shape() : rotatePos(random(0, ROTATEPOSITION3)) {}

Shape::~Shape() {}

Block* Shape::getBlockList()  {
    return blockList;
}

Point (*Shape::getPositions() )[NUM_POSITIONS] {
    return positions;
}

void Shape::setBlock(const Block& block, int index) {
    if (index >= 0 && index < NUM_BLOCKS) {
        blockList[index] = block;
    } else {
        throw std::out_of_range("setBlock: Index " + std::to_string(index) + 
                                " is out of range. Valid range is 0 to " + 
                                std::to_string(NUM_BLOCKS - 1) + ".");
    }
}

void Shape::setPoint(int x, int y, const Point& point) {
   positions[x][y] = point;
}

void Shape::setRotatePosition(int rotatePosition) {
    rotatePos = rotatePosition;
}

int Shape::getRotatePosition() const {
    return rotatePos;
}

int Shape::getROTATEPOSITION0() { return ROTATEPOSITION0; }
int Shape::getROTATEPOSITION1() { return ROTATEPOSITION1; }
int Shape::getROTATEPOSITION2() { return ROTATEPOSITION2; }
int Shape::getROTATEPOSITION3() { return ROTATEPOSITION3; }

Block& Shape::getBlock(int index) {
    if (index >= 0 && index < NUM_BLOCKS) {
        return blockList[index];
    }
    throw std::out_of_range("Index out of range");
}

void Shape::generateShape() {
    uint16_t color = blockList[0].getColor();
    for (int i = 1; i < NUM_BLOCKS; ++i) {
        blockList[i] = Block(getXPosition(i - 1), getYPosition(i - 1), color);
    }
    /*
    Serial.printf("Shape: Rotation %d, Punkte:\n", rotatePos);
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        Serial.printf("Punkt %d: X=%d, Y=%d\n", i, positions[rotatePos][i].getX(), positions[rotatePos][i].getY());
    }
    */
}

int Shape::getXPosition(int index) const {
    if (index >= 0 && index < NUM_POSITIONS) {
        return blockList[0].getX() + static_cast<int>(positions[rotatePos][index].getX());
    }
    // Throw exception if the index is out of bounds
    throw std::out_of_range("getXPosition: Index " + std::to_string(index) + 
                            " is out of range. Valid range is 0 to " + 
                            std::to_string(NUM_POSITIONS - 1) + ".");
}

int Shape::getYPosition(int index) const {
    if (index >= 0 && index < NUM_POSITIONS) {
        return blockList[0].getY() + static_cast<int>(positions[rotatePos][index].getY());
    }
    // Throw exception if the index is out of bounds
    throw std::out_of_range("getYPosition: Index " + std::to_string(index) + 
                            " is out of range. Valid range is 0 to " + 
                            std::to_string(NUM_POSITIONS - 1) + ".");
}

bool Shape::canMoveToPosition(int x, int y, const BlockMap& blockMap) const {
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        int blockX = x + positions[rotatePos][i].getX(); // Berechne Ziel-X-Position
        int blockY = y + positions[rotatePos][i].getY(); // Berechne Ziel-Y-Position

        // Prüfe, ob die X-Position außerhalb des Spielfelds liegt
        if (blockX < 0 || blockX >= BlockMap::MAP_WIDTH) {
            return false; // X-Position ist außerhalb des Spielfelds
        }

        // Prüfe, ob die Y-Position unterhalb der Grenze liegt
        if (blockY >= BlockMap::MAP_HEIGHT) {
            return false; // Y-Position ist unterhalb der Spielfeldgrenze
        }

        // Prüfe, ob das Feld besetzt ist
        if (!blockMap.isFieldEmpty(blockX, blockY)) {
            return false; // Feld ist besetzt
        }
    }

    return true; // Alle Prüfungen bestanden
}

bool Shape::canRotateToPosition(int tmpRotatePosition, const BlockMap& blockMap) const {
    for (int i = 1; i < NUM_BLOCKS; ++i) {
        int x = blockList[0].getX() + static_cast<int>(positions[tmpRotatePosition][i - 1].getX());
        int y = blockList[0].getY() + static_cast<int>(positions[tmpRotatePosition][i - 1].getY());
        if (y >= MAP_HEIGHT || x < 0 || x >= MAP_WIDTH || !blockMap.isFieldEmpty(x, y)) {
            return false; // Ungültige Position
        }
    }
    return true;
}

void Shape::rotateToPosition(int targetRotatePosition, BlockMap& blockMap) {
    if (canRotateToPosition(targetRotatePosition, blockMap)) {
        rotatePos = targetRotatePosition;
        generateShape();
    }
}

void Shape::rotate(BlockMap& blockMap, bool clockwise) {
    int nextRotatePos = clockwise
                        ? (rotatePos == ROTATEPOSITION3 ? ROTATEPOSITION0 : rotatePos + 1)
                        : (rotatePos == ROTATEPOSITION0 ? ROTATEPOSITION3 : rotatePos - 1);
    if (canRotateToPosition(nextRotatePos, blockMap)) {
        rotatePos = nextRotatePos;
        generateShape();
    }
}

void Shape::rotateClockwise(BlockMap& blockMap) {
    rotate(blockMap, true);
}

void Shape::rotateAntiClockwise(BlockMap& blockMap) {
    rotate(blockMap, false);
}

Block Shape::getLeftBlock() {
    Block leftBlock = blockList[0];
    for (int i = 1; i < NUM_BLOCKS; ++i) {
        if (leftBlock.getX() > blockList[i].getX()) {
            leftBlock = blockList[i];
        }
    }
    return std::move(leftBlock);
}

bool Shape::isMovableToTheLeft(BlockMap& blockMap) {
    if (getLeftBlock().getX() == 0) {
        return false;
    }
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        if (!isInCollisionWithLeftBlock(getBlock(i), blockMap)) {
            return true;
        }
    }
    return false;
}

bool Shape::isInCollisionWithLeftBlock(const Block& block, BlockMap& blockMap) {
    bool result = false;
    int x = block.getX() - 1;
    int y = block.getY();
    if (blockMap.isFieldEmpty(x, y) == false) {
        result = true;
    }
    return result;
}

void Shape::moveLeft(BlockMap& blockMap) {
    if (isMovableToTheLeft(blockMap)) {
        for (int i = 0; i < NUM_BLOCKS; ++i) {
            blockList[i].moveLeft();
        }
    }
}

Block Shape::getRightBlock() {
    Block rightBlock = blockList[0];
    for (int i = 1; i < NUM_BLOCKS; ++i) {
        if (rightBlock.getX() < blockList[i].getX()) {
            rightBlock = blockList[i];
        }
    }
    return rightBlock;
}

bool Shape::isMovableToTheRight(BlockMap& blockMap) {
    bool result = true;
    if (getRightBlock().getX() == MAP_WIDTH-1) {
        result = false;
    } else {
        for (int i = 0; i < NUM_BLOCKS; ++i) {
            if (isInCollisionWithRightBlock(getBlock(i), blockMap)) {
                return false;
            }
        }
    }
    return result;
}

bool Shape::isInCollisionWithRightBlock(const Block& block, BlockMap& blockMap) {
    bool result = false;
    int x = block.getX() + 1;
    int y = block.getY();
    if (blockMap.isFieldEmpty(x, y) == false) {
        result = true;
    }
    return result;
}

void Shape::moveRight(BlockMap& blockMap) {
    if (isMovableToTheRight(blockMap)) {
        for (int i = 0; i < NUM_BLOCKS; ++i) {
            blockList[i].moveRight();
        }
    }
}

Block Shape::getLowestBlock() {
    Block lowestBlock = blockList[0];
    for (int i = 1; i < NUM_BLOCKS; ++i) {
        if (lowestBlock.getY() < blockList[i].getY()) {
            lowestBlock = blockList[i];
        }
    }
    return lowestBlock;
}

Block Shape::getHighestBlock() {
    Block highestBlock = blockList[0];
    for (int i = 1; i < NUM_BLOCKS; ++i) {
        if (highestBlock.getY() > blockList[i].getY()) {
            highestBlock = blockList[i];
        }
    }
    return highestBlock;
}

bool Shape::isMovableDownWards(BlockMap& blockMap) {
    bool result = true;
    if (getLowestBlock().getY() >= MAP_HEIGHT-1) {
        result = false;
    } else {
        for (int i = 0; i <= 3 && result == true; i++) {
            result = !isInCollisionWithLowerBlock(getBlock(i), blockMap);
        }
    }
    return result;
}

bool Shape::isInCollisionWithLowerBlock(const Block& block, BlockMap& blockMap) {
    bool result = false;
    int x = block.getX();
    int y = block.getY() + 1;
    if (blockMap.isFieldEmpty(x, y) == false) {
        result = true;
    }
    return result;
}

void Shape::moveDown(BlockMap& blockMap) {
    if (isMovableDownWards(blockMap)) {
        for (auto& block : blockList) {
            block.moveDown();
        }
    }
}

void Shape::fallDown(BlockMap& blockMap) {
    while (isMovableDownWards(blockMap)) {
        moveDown(blockMap);
    }
}

void Shape::drawShape(TFT_eSPI& tft) const {
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        blockList[i].draw(tft);

    }
}

void Shape::drawShapeBorderOnly(TFT_eSPI& tft, int offset) const {
    for (const auto& block : blockList) {
        block.drawBorderOnly(tft, offset);
    }
}

void Shape::eraseShape(TFT_eSPI& tft, uint16_t backgroundColor) const {
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        blockList[i].draw(tft, backgroundColor);
    }
}

bool Shape::checkRotationValidity(int tmpRotatePosition, BlockMap& blockMap) {
    for (int i = 1; i < NUM_BLOCKS; ++i) {
        int x = blockList[0].getX() + static_cast<int>(positions[tmpRotatePosition][i - 1].getX());
        int y = blockList[0].getY() + static_cast<int>(positions[tmpRotatePosition][i - 1].getY());
        if (y >= MAP_HEIGHT || x < 0 || x >= MAP_WIDTH || !blockMap.isFieldEmpty(x, y)) {
            return false;
        }
    }
    return true;
}

void Shape::moveToLowestBlockkAtMinusOne() {
    Block lowerstBlock = getLowestBlock(); // Assuming this function finds the block with the highest y value in the shape

    int yOffset = lowerstBlock.getY() - (-1); // Calculate offset to move the highest block to -1

    for (auto& block : blockList) {
        block.setY(block.getY() - yOffset); // Adjust each block's y position by the computed offset
    }
}

// In Shape.cpp or relevant source file
int Shape::getWidth() {
   std::set<int> uniqueXCoordinates;

    // Collect unique x-coordinates of all blocks
    for (const Block& block : blockList) {
        uniqueXCoordinates.insert(block.getX());
    }

    // The width is the number of unique x-coordinates
    return uniqueXCoordinates.size();
}

void Shape::setPosition(int x, int y) {
    // Update the position of the first block to the new coordinates
    blockList[0].setCoordinates(x, y);

    generateShape();   
}
