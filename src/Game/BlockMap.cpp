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
        if (map[block->getX()][block->getY()] != nullptr) {
            // Avoid double deletion by not deleting the old block pointer twice
            delete map[block->getX()][block->getY()]; 
        }
        map[block->getX()][block->getY()] = block; // Assign new block
    } else {
        delete block; // Clean up the block if out of bounds
    }
}

void BlockMap::addBlocks(Block blockList[], int size) {
    for (int i = 0; i < size; ++i) {
        int x = blockList[i].getX();
        int y = blockList[i].getY();
        if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
            addBlock(new Block(blockList[i])); // Only add block if within bounds
        } else {
            Serial.printf("Warning: Attempted to add block out of bounds at (%d, %d)\n", x, y);
        }
    }
}

bool BlockMap::isFieldEmpty(int x, int y) const {
    return (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) ? (map[x][y] == nullptr) : true;
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

int BlockMap::calculateHeight() const {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map[x][y] != nullptr) {
                return MAP_HEIGHT - y; // Return height from the top
            }
        }
    }
    return 0;
}

std::vector<int> BlockMap::calculateColumnHeights() const {
    std::vector<int> columnHeights(MAP_WIDTH, 0);
    for (int x = 0; x < MAP_WIDTH; ++x) {
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            if (map[x][y] != nullptr) {
                columnHeights[x] = MAP_HEIGHT - y;
                break;
            }
        }
    }
    return columnHeights;
}

int BlockMap::calculateBumpiness() const {
    std::vector<int> columnHeights = calculateColumnHeights();
    int bumpiness = 0;
    for (int x = 0; x < MAP_WIDTH - 1; ++x) {
        bumpiness += std::abs(columnHeights[x] - columnHeights[x + 1]);
    }
    return bumpiness;
}

int BlockMap::countHoles() const {
    int holeCount = 0;
    for (int x = 0; x < MAP_WIDTH; ++x) {
        bool blockEncountered = false;
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            if (map[x][y] != nullptr) {
                blockEncountered = true;
            } else if (blockEncountered) {
                ++holeCount; // Count holes only after encountering a block in the column
            }
        }
    }
    return holeCount;
}

int BlockMap::getHighestColumn() const {
    int maxHeight = 0;
    for (int x = 0; x < MAP_WIDTH; ++x) {
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            if (map[x][y] != nullptr) {
                maxHeight = std::max(maxHeight, MAP_HEIGHT - y);
                break;
            }
        }
    }
    return maxHeight;
}

int BlockMap::countClearedLines() const {
    int clearedLines = 0;
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        if (isLineFull(y)) {
            ++clearedLines;
        }
    }
    return clearedLines;
}