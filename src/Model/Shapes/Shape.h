#ifndef SHAPE_H
#define SHAPE_H

#include "Model/Block.h"
#include "Game/BlockMap.h"
#include "Model/Point.h"
#include "TFT_eSPI.h"
#include <stdexcept>
#include <utility>

/**
 * @brief Base class representing a Tetris shape and its associated operations.
 */
class Shape {
public:
    // Constants for rotation positions
    static constexpr int ROTATEPOSITION0 = 0;
    static constexpr int ROTATEPOSITION1 = 1;
    static constexpr int ROTATEPOSITION2 = 2;
    static constexpr int ROTATEPOSITION3 = 3;

    static constexpr int NUM_BLOCKS = 4;    ///< Number of blocks in a shape
    static constexpr int NUM_POSITIONS = 3; ///< Number of rotation positions

    // Constructor and Destructor
    Shape();
    virtual ~Shape();

    // Block Management
    Block* getBlockList() const;
    Block& getBlock(int index);
    void setBlock(const Block& block, int index);
    int getXPosition(int index) const;
    int getYPosition(int index) const;
    Block getLeftBlock();
    Block getRightBlock();
    Block getHighestBlock();
    Block getLowestBlock();
    Block getMaintBlock();
    void generateShape();

    // Position Management
    Point (*getPositions())[NUM_POSITIONS];
    void setPoint(int x, int y, const Point& point);
    std::pair<int, int> getHorizontalBounds(const BlockMap& blockMap) const;
    void setPosition(int x, int y);
    int getWidth();

    // Rotation Management
    void setRotatePosition(int rotatePosition);
    int getRotatePosition() const;
    void rotate(BlockMap& blockMap, bool clockwise);
    bool canRotateToPosition(int targetRotation, const BlockMap& blockMap, bool checkClockwise) const;
    bool rotateToPosition(int targetRotation, BlockMap& blockMap);
    bool isRotatableAntiClockwise(const BlockMap& blockMap) const;
    bool isRotatableClockwise(const BlockMap& blockMap)const ;
    bool rotateAntiClockwise(const BlockMap& blockMap);
    bool rotateClockwise(const BlockMap& blockMap);
    bool checkRotationValidity(int tmpRotatePosition, const BlockMap& blockMap) const;
    
    // Movement Management
    bool isMovableToTheLeft(BlockMap& blockMap);
    bool isMovableToTheRight(BlockMap& blockMap);
    bool isMovableDownWards(BlockMap& blockMap);
    void moveLeft(BlockMap& blockMap);
    void moveRight(BlockMap& blockMap);
    void moveDown(BlockMap& blockMap);
    void fallDown(BlockMap& blockMap);
    void moveToLowestBlockkAtMinusOne();
    bool canMoveToPosition(int x, int y, const BlockMap& blockMap) const;

    // Collision Detection
    bool isInCollisionWithLeftBlock(const Block& block, BlockMap& blockMap);
    bool isInCollisionWithRightBlock(const Block& block, BlockMap& blockMap);
    bool isInCollisionWithLowerBlock(const Block& block, BlockMap& blockMap);

    // Drawing and Erasing
    void drawShape(TFT_eSPI& tft) const;
    void drawShapeBorderOnly(TFT_eSPI& tft, int offset) const;
    void eraseShape(TFT_eSPI& tft, uint16_t backgroundColor) const;

    // Static Accessors for Rotation Positions
    static int getROTATEPOSITION0();
    static int getROTATEPOSITION1();
    static int getROTATEPOSITION2();
    static int getROTATEPOSITION3();

protected:
    // Member Variables
    Block blockList[NUM_BLOCKS]; ///< List of blocks comprising the shape
    Point positions[NUM_BLOCKS][NUM_POSITIONS]; ///< Rotation offsets
    int rotatePos; ///< Current rotation position
};

#endif // SHAPE_H
