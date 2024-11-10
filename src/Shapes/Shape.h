#ifndef SHAPE_H
#define SHAPE_H

#include "Block.h"
#include "BlockMap.h"
#include "Point.h"

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
    Shape();
    virtual ~Shape();

    Block* getBlockList();
    Point (*getPositions())[3];
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
    int getXPosition(int index);
    int getYPosition(int index);

    virtual bool isRotatableAntiClockwise(BlockMap& blockMap) = 0;
    virtual bool isRotatableClockwise(BlockMap& blockMap) = 0;
    virtual void rotateAntiClockwise(BlockMap& blockMap) = 0;
    virtual void rotateClockwise(BlockMap& blockMap) = 0;

    Block getLeftBlock();
    virtual bool isMovableToTheLeft(BlockMap& blockMap) = 0;
    virtual bool isMovableToTheRight(BlockMap& blockMap) = 0;
    virtual bool isMovableDownwards(BlockMap& blockMap) = 0;

    virtual void moveLeft(BlockMap& blockMap) = 0;
    virtual void moveRight(BlockMap& blockMap) = 0;
    virtual void moveDown(BlockMap& blockMap) = 0;
    virtual void fallDown(BlockMap& blockMap) = 0;

    void drawShape(TFT_eSPI& tft, int boxSize);
    virtual void drawNextShape(TFT_eSPI& tft, int boxSize) = 0;
    void drawShapeBorderOnly(TFT_eSPI& tft, int boxSize, int offset);
};

#endif // SHAPE_H
