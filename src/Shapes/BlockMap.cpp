#include "BlockMap.h"
#include "Shape.h" // Include the full definition of Shape
#include <iostream>

// Constructor
BlockMap::BlockMap() {
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 22; ++y) {
            map[x][y] = nullptr;
        }
    }
}

// Destructor
BlockMap::~BlockMap() {
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 22; ++y) {
            delete map[x][y];
            map[x][y] = nullptr;
        }
    }
}

// Adds a given block to blockMap at the corresponding position
void BlockMap::addBlock(Block* block) {
    if (block != nullptr && block->getX() >= 0 && block->getX() < 10 &&
        block->getY() >= 0 && block->getY() < 22) {
        if (map[block->getX()][block->getY()] != nullptr) {
            delete map[block->getX()][block->getY()]; // Clean up if block already exists
        }
        map[block->getX()][block->getY()] = block;
    }
}

// Adds all the blocks of a shape into the blockMap
void BlockMap::addBlocks(Block blockList[], int size) {
    for (int i = 0; i < size; ++i) {
        addBlock(new Block(blockList[i])); // Copy the block into the map
    }
}

// Checks if a field is empty
bool BlockMap::isFieldEmpty(int x, int y) const {
    if (x >= 0 && x < 10 && y >= 0 && y < 22) {
        return map[x][y] == nullptr;
    }
    return false;
}

// Gets the block at given coordinates
Block* BlockMap::getBlock(int x, int y) const {
    if (x >= 0 && x < 10 && y >= 0 && y < 22) {
        return map[x][y];
    }
    return nullptr;
}

// Removes a specific block from blockMap
void BlockMap::removeBlock(Block* block) {
    if (block != nullptr && block->getX() >= 0 && block->getX() < 10 &&
        block->getY() >= 0 && block->getY() < 22) {
        delete map[block->getX()][block->getY()];
        map[block->getX()][block->getY()] = nullptr;
    }
}

// Removes a block from blockMap by its coordinates
void BlockMap::removeBlock(int x, int y) {
    if (x >= 0 && x < 10 && y >= 0 && y < 22) {
        delete map[x][y];
        map[x][y] = nullptr;
    }
}

// Clears all blocks in a given line
void BlockMap::clearLine(int lineIndex) {
    if (lineIndex >= 0 && lineIndex < 22) {
        for (int x = 0; x < 10; ++x) {
            removeBlock(x, lineIndex);
        }
    }
}

// Checks if a line is full
bool BlockMap::isLineFull(int lineIndex) const {
    if (lineIndex >= 0 && lineIndex < 22) {
        for (int x = 0; x < 10; ++x) {
            if (isFieldEmpty(x, lineIndex)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

// Clears all full lines in a range covered by a shape
int BlockMap::clearFullLines(Shape& activeShape) {
    int numberOfLines = 0;
    int startLine = activeShape.getBlock(0).getY();
    int endLine = activeShape.getBlock(3).getY(); // Assuming blocks are sorted by Y
    for (int i = 0; i <= startLine - endLine; ++i) {
        if (isLineFull(startLine - i)) {
            clearLine(startLine - i);
            ++numberOfLines;
        }
    }
    return numberOfLines;
}

// Moves a block down by one field
void BlockMap::moveBlockDown(int x, int y) {
    if (x >= 0 && x < 10 && y >= 0 && y < 21 && map[x][y] != nullptr) {
        map[x][y + 1] = map[x][y];
        map[x][y] = nullptr;
        map[x][y + 1]->setY(y + 1);
    }
}

// Checks if a block is movable downwards
bool BlockMap::isBlockMovableDownwards(int x, int y) const {
    if (y + 1 < 22 && x >= 0 && x < 10) {
        return map[x][y + 1] == nullptr;
    }
    return false;
}

// Moves an entire line down by one
void BlockMap::moveLineDown(int lineIndex) {
    if (lineIndex >= 0 && lineIndex < 21) {
        for (int x = 0; x < 10; ++x) {
            if (!isFieldEmpty(x, lineIndex)) {
                moveBlockDown(x, lineIndex);
            }
        }
    }
}

// Gets the index of the first line that isn't empty starting from a lineIndex
int BlockMap::getFirstNotEmptyLine(int lineIndex) const {
    if (lineIndex < 0 || lineIndex >= 22) {
        return -1;
    }
    for (int y = lineIndex; y >= 0; --y) {
        for (int x = 0; x < 10; ++x) {
            if (!isFieldEmpty(x, y)) {
                return y;
            }
        }
    }
    return -1;
}

// Moves all not empty lines down as far as possible
void BlockMap::moveAllNotEmptyLinesDown(Shape& activeShape) {
    int startLine = activeShape.getBlock(0).getY();
    for (int y = startLine; y > 0; --y) {
        int firstNotEmptyLine = getFirstNotEmptyLine(y);
        if (firstNotEmptyLine != -1) {
            moveLineDown(firstNotEmptyLine);
        }
    }
}
