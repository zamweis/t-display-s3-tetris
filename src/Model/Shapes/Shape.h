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
    enum class ShapeType {
        I,
        O,
        T,
        S,
        Z,
        J,
        L,
        Unknown
    };
    // Constants for rotation positions
    static constexpr int ROTATEPOSITION0 = 0;
    static constexpr int ROTATEPOSITION1 = 1;
    static constexpr int ROTATEPOSITION2 = 2;
    static constexpr int ROTATEPOSITION3 = 3;

    static constexpr int NUM_BLOCKS = 4;    // Number of blocks in a shape
    static constexpr int NUM_POSITIONS = 3; // Number of rotation positions

    // Constructor and Destructor
    Shape();
    virtual ~Shape();

    // Block Management
    Block* getBlockList() const;
    Block& getBlock(int index);
    void setBlock(const Block& block, int index);
    int getXPosition(int index) const;
    int getYPosition(int index) const;
    Block getLeftBlock() const;
    Block getRightBlock() const;
    Block getHighestBlock() const;
    Block getLowestBlock() const;
    Block getMaintBlock();
    void generateShape();

    // Position Management
    Point (*getPositions())[NUM_POSITIONS];
    void setPoint(int x, int y, const Point& point);
    std::pair<int, int> getHorizontalBounds(const BlockMap& blockMap) const;
    void setPosition(int x, int y);
    int getWidth();

    // Rotation Management
    int getNumRotations() const { return numRotations; }
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
    bool isMovableToTheLeft(const BlockMap& blockMap);
    bool isMovableToTheRight(const BlockMap& blockMap);
    bool isMovableDownWards(const BlockMap& blockMap);
    bool canMoveToPosition(int x, int y, const BlockMap& blockMap) const;
    void moveLeft();
    void moveRight();
    void moveDown();
    void fallDown(const BlockMap& blockMap);
    void moveToLowestBlockkAtMinusOne();

    // Collision Detection
    bool isInCollisionWithLeftBlock(const Block& block, const BlockMap& blockMap);
    bool isInCollisionWithRightBlock(const Block& block, const BlockMap& blockMap);
    bool isInCollisionWithLowerBlock(const Block& block, const BlockMap& blockMap);

    // Drawing and Erasing
    void drawShape(TFT_eSPI& tft) const;    void drawShapeBorderOnly(TFT_eSPI& tft, int offset) const;
    void eraseShape(TFT_eSPI& tft, uint16_t backgroundColor) const;

    // Static Accessors for Rotation Positions
    static int getROTATEPOSITION0();
    static int getROTATEPOSITION1();
    static int getROTATEPOSITION2();
    static int getROTATEPOSITION3();

    virtual ShapeType getType() const {
        return ShapeType::Unknown; // Default implementation for the base class
    }

protected:
    // Member Variables
    Block blockList[NUM_BLOCKS]; // List of blocks comprising the shape
    Point positions[NUM_BLOCKS][NUM_POSITIONS]; // Rotation offsets
    int rotatePos; // Current rotation position
    int numRotations; // Number of rotations for the shape
};

#endif // SHAPE_H
