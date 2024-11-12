#include "Game/BlockMap.h"
#include "Model/Shapes/Shape.h" // Include the full definition of Shape
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
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        return map[x][y] == nullptr;
    } 
    return true;
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
void BlockMap::clearLine(int lineIndex, TFT_eSPI& tft, int boxSize, uint16_t backgroundColor) {
    if (lineIndex >= 0 && lineIndex < MAP_HEIGHT) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map[x][lineIndex] != nullptr) {
                map[x][lineIndex]->draw(tft, boxSize, backgroundColor); // Clear block graphics
                delete map[x][lineIndex]; // Free memory
                map[x][lineIndex] = nullptr; // Remove block
            }
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

bool BlockMap::isLineEmpty(int lineIndex) const {
    if (lineIndex >= 0 && lineIndex < MAP_HEIGHT) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (!isFieldEmpty(x, lineIndex)) {
                return false; // A block is found, so the line is not empty
            }
        }
        return true; // No blocks found, so the line is empty
    }
    return true; // Return true for out-of-bounds line index
}

// Moves a block down by one field
void BlockMap::moveBlockDown(int x, int y) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAX_LINE_INDEX && map[x][y] != nullptr) {
        map[x][y + 1] = map[x][y];
        map[x][y] = nullptr;
        map[x][y + 1]->setY(y + 1);
    }
}

// Moves an entire line down by a specified number of lines
void BlockMap::moveLineDown(int lineIndex, TFT_eSPI& tft, int amountOfLines, int boxSize, uint16_t backgroundColor) {
    if (lineIndex >= 0 && lineIndex + amountOfLines < MAP_HEIGHT) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (!isFieldEmpty(x, lineIndex)) {
                map[x][lineIndex]->draw(tft, boxSize, backgroundColor); // Clear previous position
                map[x][lineIndex + amountOfLines] = map[x][lineIndex]; // Move block
                map[x][lineIndex] = nullptr; // Clear old position
                map[x][lineIndex + amountOfLines]->setY(lineIndex + amountOfLines);
                map[x][lineIndex + amountOfLines]->draw(tft, boxSize); // Redraw at new position
            }
        }
    }
}

// Clears all full lines and moves all lines above down by the number of cleared lines
int BlockMap::clearAndMoveAllFullLines(TFT_eSPI& tft, int boxSize, uint16_t backgroundColor) {
    int totalClearedLines = 0;

    // Traverse from bottom to top to avoid index shifting issues
    for (int y = MAP_HEIGHT - 1; y >= 0; --y) {
        if (isLineFull(y)) {
            clearLine(y, tft, boxSize, backgroundColor);
            ++totalClearedLines;
        } else if (totalClearedLines > 0) {
            moveLineDown(y, tft, totalClearedLines, boxSize, backgroundColor);
        }
    }
    return totalClearedLines;
}

void BlockMap::drawAllBlocks(TFT_eSPI& tft, int boxSize) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        if (isLineEmpty(y)) {
            continue; // Skip drawing this line if it is empty
        }
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map[x][y] != nullptr) {
                map[x][y]->draw(tft, boxSize); // Draw the block if present
            }
        }
    }
}

bool BlockMap::checkGameOver() const {
    for (int x = 0; x < MAP_WIDTH; ++x) {
        if (map[x][0] != nullptr) { // If there's a block in the top row
            return true;
        }
    }
    return false;
}
