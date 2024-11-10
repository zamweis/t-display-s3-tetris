#ifndef BLOCKMAP_H
#define BLOCKMAP_H

#include "Block.h"

// Forward declaration of Shape
class Shape;

class BlockMap {
private:
    Block* map[10][10];

public:
    BlockMap();
    ~BlockMap();

    void addBlock(Block* block);
    void addBlocks(Block blockList[], int size);
    bool isFieldEmpty(int x, int y) const;
    Block* getBlock(int x, int y) const;
    void removeBlock(Block* block);
    void removeBlock(int x, int y);
    void clearLine(int lineIndex);
    bool isLineFull(int lineIndex) const;
    int clearFullLines(Shape& activeShape); // No error since Shape is forward-declared
    bool isLineEmpty(int lineIndex) const;
    void moveBlockDown(int x, int y);
    bool isBlockMovableDownwards(int x, int y) const;
    void moveLineDown(int lineIndex);
    int getFirstNotEmptyLine(int lineIndex) const;
    void moveAllNotEmptyLinesDown(Shape& activeShape); // No error since Shape is forward-declared
};

#endif // BLOCKMAP_H
