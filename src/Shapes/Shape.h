#ifndef SHAPE_H
#define SHAPE_H

#include "Block.h"
#include "BlockMap.h"
#include "Point.h"
#include "TFT_eSPI.h"
#include <stdexcept>

class Shape {
private:

protected:
    Shape();

public:
    ~Shape();

    static constexpr int ROTATEPOSITION0 = 0;
    static constexpr int ROTATEPOSITION1 = 1;
    static constexpr int ROTATEPOSITION2 = 2;
    static constexpr int ROTATEPOSITION3 = 3;
    static constexpr int NUM_BLOCKS = 4;
    static constexpr int NUM_POSITIONS = 3;
    static constexpr int MAP_WIDTH = 10;
    static constexpr int MAP_HEIGHT = 19;

    Block blockList[NUM_BLOCKS];
    Point positions[NUM_BLOCKS][NUM_POSITIONS];
    int rotatePos;
    Block* getBlockList();
    Point (*getPositions())[NUM_POSITIONS];
    void setBlock(const Block& block, int index);
    void setPoint(int x, int y, const Point& point);
    void setRotatePosition(int rotatePosition);
    int getRotatePosition() const;
    static int getROTATEPOSITION0();
    static int getROTATEPOSITION1();
    static int getROTATEPOSITION2();
    static int getROTATEPOSITION3();
    Block& getBlock(int index);
    void generateShape();
    int getXPosition(int index) const;
    int getYPosition(int index) const;
    bool isRotatableAntiClockwise(BlockMap& blockMap);
    bool isRotatableClockwise(BlockMap& blockMap);
    void rotateAntiClockwise(BlockMap& blockMap);
    void rotateClockwise(BlockMap& blockMap);
    Block getLeftBlock();
    bool isMovableToTheLeft(BlockMap& blockMap);
    bool isInCollisionWithLeftBlock(const Block& block, BlockMap& blockMap);
    void moveLeft(BlockMap& blockMap);
    Block getRightBlock();
    Block getHighestBlock();
    bool isMovableToTheRight(BlockMap& blockMap);
    bool isInCollisionWithRightBlock(const Block& block, BlockMap& blockMap);
    void moveRight(BlockMap& blockMap);
    Block getLowestBlock();
    bool isMovableDownWards(BlockMap& blockMap);
    bool isInCollisionWithLowerBlock(const Block& block, BlockMap& blockMap);
    void moveDown(BlockMap& blockMap);
    void fallDown(BlockMap& blockMap);
    void drawShape(TFT_eSPI& tft, int boxSize) const;
    void drawShapeBorderOnly(TFT_eSPI& tft, int boxSize, int offset) const;
    void eraseShape(TFT_eSPI& tft, int boxSize, uint16_t backgroundColor) const;
    void moveToLowestBlockkAtMinusOne();

private:
    bool checkRotationValidity(int tmpRotatePosition, BlockMap& blockMap);
};

#endif // SHAPE_H
