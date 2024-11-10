#include "Shape.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

Shape::Shape() : rotatePos(std::rand() % 4) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::cout << "Initialized rotatePos to: " << rotatePos << std::endl;
}

Shape::~Shape() {}

Block* Shape::getBlockList() {
    return blockList;
}

Point (*Shape::getPositions())[NUM_POSITIONS] {
    return positions;
}

void Shape::setBlock(const Block& block, int index) {
    if (index >= 0 && index < NUM_BLOCKS) {
        blockList[index] = block;
    }
}

void Shape::setPoint(int x, int y, const Point& point) {
    if (x >= 0 && x < NUM_BLOCKS && y >= 0 && y < NUM_POSITIONS) {
        positions[x][y] = point;
    }
}

void Shape::setRotatePosition(int rotatePosition) {
    if (rotatePosition >= ROTATEPOSITION0 && rotatePosition <= ROTATEPOSITION3) {
        rotatePos = rotatePosition;
    }
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
}

int Shape::getXPosition(int index) const {
    if (index >= 0 && index < NUM_POSITIONS) {
        return blockList[0].getX() + static_cast<int>(positions[rotatePos][index].getX());
    }
    return blockList[0].getX(); // Default fallback
}

int Shape::getYPosition(int index) const {
    if (index >= 0 && index < NUM_POSITIONS) {
        return blockList[0].getY() + static_cast<int>(positions[rotatePos][index].getY());
    }
    return blockList[0].getY(); // Default fallback
}

bool Shape::isRotatableAntiClockwise(BlockMap& blockMap) {
    int tmpRotatePosition = (rotatePos == ROTATEPOSITION3) ? ROTATEPOSITION0 : rotatePos + 1;
    return checkRotationValidity(tmpRotatePosition, blockMap);
}

bool Shape::isRotatableClockwise(BlockMap& blockMap) {
    int tmpRotatePosition = (rotatePos == ROTATEPOSITION0) ? ROTATEPOSITION3 : rotatePos - 1;
    return checkRotationValidity(tmpRotatePosition, blockMap);
}

void Shape::rotateAntiClockwise(BlockMap& blockMap) {
    if (isRotatableAntiClockwise(blockMap)) {
        rotatePos = (rotatePos == ROTATEPOSITION3) ? ROTATEPOSITION0 : rotatePos + 1;
        generateShape();
    }
}

void Shape::rotateClockwise(BlockMap& blockMap) {
    if (isRotatableClockwise(blockMap)) {
        rotatePos = (rotatePos == ROTATEPOSITION0) ? ROTATEPOSITION3 : rotatePos - 1;
        generateShape();
    }
}

Block Shape::getLeftBlock() {
    Block leftBlock = blockList[0];
    for (int i = 1; i < NUM_BLOCKS; ++i) {
        if (leftBlock.getX() > blockList[i].getX()) {
            leftBlock = blockList[i];
        }
    }
    return leftBlock;
}

bool Shape::isMovableToTheLeft(BlockMap& blockMap) {
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

bool Shape::isInCollisionWithLeftBlock(const Block& block, BlockMap& blockMap) {
    int x = block.getX() - 1;
    int y = block.getY();
    return !blockMap.isFieldEmpty(x, y);
}

void Shape::moveLeft(BlockMap& blockMap) {
    if (isMovableToTheLeft(blockMap)) {
        for (auto& block : blockList) {
            block.moveLeft();
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

bool Shape::isInCollisionWithRightBlock(const Block& block, BlockMap& blockMap) {
    int x = block.getX() + 1;
    int y = block.getY();
    return !blockMap.isFieldEmpty(x, y);
}

void Shape::moveRight(BlockMap& blockMap) {
    if (isMovableToTheRight(blockMap)) {
        for (auto& block : blockList) {
            block.moveRight();
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

bool Shape::isMovableDownWards(BlockMap& blockMap) {
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

bool Shape::isInCollisionWithLowerBlock(const Block& block, BlockMap& blockMap) {
    int x = block.getX();
    int y = block.getY() + 1;
    return !blockMap.isFieldEmpty(x, y);
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

void Shape::drawShape(TFT_eSPI& tft, int boxSize) const {
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        blockList[i].draw(tft, boxSize);
    }
}

void Shape::drawShapeBorderOnly(TFT_eSPI& tft, int boxSize, int offset) const {
    for (const auto& block : blockList) {
        block.drawBorderOnly(tft, boxSize, offset);
    }
}

void Shape::eraseShape(TFT_eSPI& tft, int boxSize, uint16_t backgroundColor) const {
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        blockList[i].draw(tft, boxSize, backgroundColor);
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
