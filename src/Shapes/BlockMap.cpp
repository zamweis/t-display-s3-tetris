#include "BlockMap.h"
#include "Shape.h" // Include the full definition of Shape
#include <iostream>


// Constructor
BlockMap::BlockMap() {
    for (int x = 0; x < MAP_WIDTH; ++x) {
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            map[x][y] = nullptr;
        }
    }
}

// Destructor
BlockMap::~BlockMap() {
    for (int x = 0; x < MAP_WIDTH; ++x) {
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            if (map[x][y] != nullptr) {
                delete map[x][y];
                map[x][y] = nullptr;
            }
        }
    }
}

void BlockMap::addBlock(Block* block) {
    if (block != nullptr && block->getX() >= 0 && block->getX() < MAP_WIDTH &&
        block->getY() >= 0 && block->getY() < MAP_HEIGHT) {
        // Free existing block memory (if any)
        if (map[block->getX()][block->getY()] != nullptr) {
            delete map[block->getX()][block->getY()]; // Prevent memory leaks
        }
        // Assign new block pointer
        map[block->getX()][block->getY()] = block;
    } else {
        delete block; // Clean up the block if it's out of bounds
    }
}

void BlockMap::addBlocks(Block blockList[], int size) {
    for (int i = 0; i < size; ++i) {
        // Creating a copy using `new` is fine, but you must ensure ownership and proper cleanup
        addBlock(new Block(blockList[i])); // Copy constructor should create a deep copy
    }
}

bool BlockMap::isFieldEmpty(int x, int y) const {
    if(y < 0) {
        return true;
    }
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        return map[x][y] == nullptr;
    } 
    return false;
}

// Gets the block at given coordinates
Block* BlockMap::getBlock(int x, int y) const {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        return map[x][y];
    }
    return nullptr;
}

// Removes a specific block from blockMap
void BlockMap::removeBlock(Block* block) {
    if (block != nullptr && block->getX() >= 0 && block->getX() < MAP_WIDTH &&
        block->getY() >= 0 && block->getY() < MAP_HEIGHT) {
        delete map[block->getX()][block->getY()];
        map[block->getX()][block->getY()] = nullptr;
    }
}

// Removes a block from blockMap by its coordinates
void BlockMap::removeBlock(int x, int y) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        delete map[x][y];
        map[x][y] = nullptr;
    }
}

// Clears all blocks in a given line
void BlockMap::clearLine(int lineIndex) {
    if (lineIndex >= 0 && lineIndex < MAP_HEIGHT) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            removeBlock(x, lineIndex);
        }
    }
}

// Checks if a line is full
bool BlockMap::isLineFull(int lineIndex) const {
    if (lineIndex >= 0 && lineIndex < MAP_HEIGHT) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
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
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAX_LINE_INDEX && map[x][y] != nullptr) {
        map[x][y + 1] = map[x][y];
        map[x][y] = nullptr;
        map[x][y + 1]->setY(y + 1);
    }
}

// Checks if a block is movable downwards
bool BlockMap::isBlockMovableDownwards(int x, int y) const {
    if (y + 1 < MAP_HEIGHT && x >= 0 && x < MAP_WIDTH) {
        return map[x][y + 1] == nullptr;
    }
    return false;
}

// Moves an entire line down by one
void BlockMap::moveLineDown(int lineIndex) {
    if (lineIndex >= 0 && lineIndex < MAX_LINE_INDEX) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (!isFieldEmpty(x, lineIndex)) {
                moveBlockDown(x, lineIndex);
            }
        }
    }
}

// Gets the index of the first line that isn't empty starting from a lineIndex
int BlockMap::getFirstNotEmptyLine(int lineIndex) const {
    if (lineIndex < 0 || lineIndex >= MAP_HEIGHT) {
        return -1;
    }
    for (int y = lineIndex; y >= 0; --y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
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
