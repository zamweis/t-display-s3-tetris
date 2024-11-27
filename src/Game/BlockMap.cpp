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
            delete map[x][y];
            map[x][y] = nullptr;
        }
    }
}

// Copy Constructor
BlockMap::BlockMap(const BlockMap& other) {
    for (int x = 0; x < MAP_WIDTH; ++x) {
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            map[x][y] = other.map[x][y] ? new Block(*other.map[x][y]) : nullptr;
        }
    }
}

// Assignment Operator
BlockMap& BlockMap::operator=(const BlockMap& other) {
    if (this != &other) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            for (int y = 0; y < MAP_HEIGHT; ++y) {
                delete map[x][y];
                map[x][y] = other.map[x][y] ? new Block(*other.map[x][y]) : nullptr;
            }
        }
    }
    return *this;
}

void BlockMap::addBlock(Block* block) {
    if (!block) return;
    int x = block->getX(), y = block->getY();
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        delete map[x][y];
        map[x][y] = block;
    } else {
        delete block; // Out-of-bounds cleanup
    }
}

void BlockMap::addBlocks(Block blockList[], int size) {
    for (int i = 0; i < size; ++i) {
        Block* newBlock = new Block(blockList[i]);
        addBlock(newBlock);
    }
}

bool BlockMap::isFieldEmpty(int x, int y) const {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        return map[x][y] == nullptr;
    } 
    // Fields outside the grid are seen as empty because this methode should only be used to check for collisions with other blocks
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
void BlockMap::clearLine(int lineIndex, TFT_eSPI& tft, uint16_t backgroundColor) {
    if (lineIndex >= 0 && lineIndex < MAP_HEIGHT) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map[x][lineIndex] != nullptr) {
                map[x][lineIndex]->draw(tft, backgroundColor); // Clear block graphics
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
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y + 1 < MAP_HEIGHT && map[x][y] != nullptr) {
        map[x][y + 1] = map[x][y];
        map[x][y] = nullptr;
        map[x][y + 1]->setY(y + 1);
    }
}

// Moves an entire line down by a specified number of lines
void BlockMap::moveLineDown(int lineIndex, TFT_eSPI& tft, int amountOfLines, uint16_t backgroundColor) {
    if (lineIndex >= 0 && lineIndex + amountOfLines < MAP_HEIGHT) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (!isFieldEmpty(x, lineIndex)) {
                map[x][lineIndex]->draw(tft, backgroundColor); // Clear previous position
                map[x][lineIndex + amountOfLines] = map[x][lineIndex]; // Move block
                map[x][lineIndex] = nullptr; // Clear old position
                map[x][lineIndex + amountOfLines]->setY(lineIndex + amountOfLines);
                map[x][lineIndex + amountOfLines]->draw(tft); // Redraw at new position
            }
        }
    }
}

int BlockMap::clearAndMoveAllFullLines(TFT_eSPI& tft, uint16_t backgroundColor) {
    int totalClearedLines = 0;
    for (int y = MAP_HEIGHT - 1; y >= 0; --y) {
        if (isLineFull(y)) {
            clearLine(y, tft, backgroundColor);
            ++totalClearedLines;
        } else if (totalClearedLines > 0) {
            moveLineDown(y, tft, totalClearedLines, backgroundColor);
        }
    }
    return totalClearedLines;
}

// Clears all full lines and moves all lines above down by the number of cleared lines
int BlockMap::getAmoutOfFullLines() const {
    int totalClearedLines = 0;

    // Traverse from bottom to top to avoid index shifting issues
    for (int y = MAP_HEIGHT - 1; y >= 0; --y) {
        if (isLineFull(y)) {
            ++totalClearedLines;
        }
    }
    return totalClearedLines;
}


void BlockMap::drawAllBlocks(TFT_eSPI& tft) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map[x][y]) {
                map[x][y]->draw(tft);
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

int BlockMap::getTotalHoles() const {
    int totalHoles = 0;
    for (int x = 0; x < MAP_WIDTH; ++x) {
        bool blockFound = false;
        for (int y = MAP_HEIGHT - 1; y >= 0; --y) {
            if (map[x][y]) {
                blockFound = true;
            } else if (blockFound) {
                ++totalHoles;
            }
        }
    }
    return totalHoles;
}

int BlockMap::getColumnHeight(int x) const {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        if (map[x][y] != nullptr) {
            return MAP_HEIGHT - y;
        }
    }
    return 0; // Column is empty
}

int BlockMap::getBumpiness() const {
    int bumpiness = 0;
    std::vector<int> heights(MAP_WIDTH);
    for (int x = 0; x < MAP_WIDTH; ++x) {
        heights[x] = getColumnHeight(x);
    }
    for (int x = 1; x < MAP_WIDTH; ++x) {
        bumpiness += std::abs(heights[x] - heights[x - 1]);
    }
    return bumpiness;
}

void BlockMap::printBlockMap() const {
    Serial.println("BlockMap State:");
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        Serial.print(y < 10 ? " " : ""); // Align single-digit rows
        Serial.print(y);
        Serial.print(" | ");
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Serial.print(map[x][y] != nullptr ? "#" : ".");
        }
        Serial.println();
    }
    Serial.println("    --------------------"); // Divider
    Serial.println("     0 1 2 3 4 5 6 7 8 9"); // Column labels
}
