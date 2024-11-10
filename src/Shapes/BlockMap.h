#ifndef BLOCKMAP_H
#define BLOCKMAP_H

#include "Block.h"

// Forward declaration of Shape
class Shape;

class BlockMap {
private:
    static constexpr int MAP_WIDTH = 10;
    static constexpr int MAP_HEIGHT = 19;
    static constexpr int MAX_LINE_INDEX = MAP_HEIGHT - 1; // Define MAX_LINE_INDEX here
    Block* map[MAP_WIDTH][MAP_HEIGHT]; // Corrected dimensions

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
    int clearFullLines(Shape& activeShape);
    bool isLineEmpty(int lineIndex) const;
    void moveBlockDown(int x, int y);
    bool isBlockMovableDownwards(int x, int y) const;
    void moveLineDown(int lineIndex);
    int getFirstNotEmptyLine(int lineIndex) const;
    void moveAllNotEmptyLinesDown(Shape& activeShape);
};

#endif // BLOCKMAP_H
