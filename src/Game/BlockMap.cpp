#include "Game/BlockMap.h"
#include "Model/Shapes/Shape.h" // Include the full definition of Shape
#include <iostream>
#include <map>

// Constructor
BlockMap::BlockMap() {
    std::fill(map, map + MAP_WIDTH * MAP_HEIGHT, nullptr);
}

// Destructor
BlockMap::~BlockMap() {
    for (Block* block : map) {
        delete block;
    }
}

// Copy Constructor
BlockMap::BlockMap(const BlockMap& other) {
    for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        map[i] = other.map[i] ? new Block(*other.map[i]) : nullptr;
    }
}

// Assignment Operator
BlockMap& BlockMap::operator=(const BlockMap& other) {
    if (this == &other) return *this;

    // Clean up existing data
    for (Block* block : map) {
        delete block;
    }

    // Copy data
    for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        map[i] = other.map[i] ? new Block(*other.map[i]) : nullptr;
    }

    return *this;
}

void BlockMap::addBlock(Block* block) {
    if (block && block->getX() >= 0 && block->getX() < MAP_WIDTH &&
        block->getY() >= 0 && block->getY() < MAP_HEIGHT) {
        int index = block->getY() * MAP_WIDTH + block->getX();
        delete map[index];
        map[index] = block;
    } else {
        delete block;
    }
}

void BlockMap::addBlocks(Block blockList[], int size) {
    for (int i = 0; i < size; ++i) {
        addBlock(new Block(blockList[i]));
    }
}

bool BlockMap::isFieldEmpty(int x, int y) const {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        return map[y * MAP_WIDTH + x] == nullptr;
    }
    return true;
}

// Gets the block at given coordinates
Block* BlockMap::getBlock(int x, int y) const {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        return map[y * MAP_WIDTH + x];
    }
    return nullptr;
}

// Removes a specific block from blockMap
void BlockMap::removeBlock(Block* block) {
    if (block && block->getX() >= 0 && block->getX() < MAP_WIDTH &&
        block->getY() >= 0 && block->getY() < MAP_HEIGHT) {
        int index = block->getY() * MAP_WIDTH + block->getX();
        delete map[index];
        map[index] = nullptr;
    }
}

// Removes a block from blockMap by its coordinates
void BlockMap::removeBlock(int x, int y) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        int index = y * MAP_WIDTH + x;
        delete map[index];
        map[index] = nullptr;
    }
}

// Clears all blocks in a given line
void BlockMap::clearLine(int lineIndex, TFT_eSPI& tft, uint16_t backgroundColor) {
    if (lineIndex >= 0 && lineIndex < MAP_HEIGHT) {
        int startIdx = lineIndex * MAP_WIDTH;
        for (int x = 0; x < MAP_WIDTH; ++x) {
            map[startIdx + x]->draw(tft, backgroundColor); // Clear block graphics
            delete map[startIdx + x];
            map[startIdx + x] = nullptr; // Remove block
        }
    }
}

void BlockMap::clearLine(int lineIndex) {
    if (lineIndex >= 0 && lineIndex < MAP_HEIGHT) {
        int startIdx = lineIndex * MAP_WIDTH;
        for (int x = 0; x < MAP_WIDTH; ++x) {
            delete map[startIdx + x];
            map[startIdx + x] = nullptr;
        }
    }
}

// Checks if a line is full
bool BlockMap::isLineFull(int lineIndex) const {
    if (lineIndex >= 0 && lineIndex < MAP_HEIGHT) {
        int startIdx = lineIndex * MAP_WIDTH;
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (!map[startIdx + x]) return false;
        }
        return true;
    }
    return false;
}

bool BlockMap::isLineEmpty(int lineIndex) const {
    if (lineIndex >= 0 && lineIndex < MAP_HEIGHT) {
        int startIdx = lineIndex * MAP_WIDTH;
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map[startIdx + x]) return false;
        }
        return true;
    }
    return true;
}

// Moves a block down by one field
void BlockMap::moveBlockDown(int x, int y) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y + 1 < MAP_HEIGHT) {
        int currentIdx = y * MAP_WIDTH + x;
        int targetIdx = currentIdx + MAP_WIDTH;

        if (map[currentIdx]) {
            map[targetIdx] = map[currentIdx];
            map[currentIdx] = nullptr;
            map[targetIdx]->setY(y + 1);
        }
    }
}

int BlockMap::clearAndMoveAllFullLines(TFT_eSPI& tft, const Shape& shape, uint16_t backgroundColor) {
    int totalClearedLines = 0;

    // Define the range of lines to check based on the lowest and highest blocks of the shape
    int startLine = shape.getLowestBlock().getY();    // Lowest block's row (highest Y value)
    int endLine = shape.getHighestBlock().getY();     // Highest block's row (lowest Y value)

    // Debug: Log start and end lines
    //Serial.printf("clearAndMoveAllFullLines(startLine=%d, endLine=%d)\n", startLine, endLine);

    // Track lines that are full and the number of cleared lines below them
    std::map<int, int> lineToClearedBelow;

    // First pass: Identify full lines and track the cleared lines below each
    int clearedBelow = 0;
    for (int y = startLine; y >= 0; --y) {
        if (isLineEmpty(y)) {
            // If the current line is empty, no need to check further lines above
            //Serial.printf("Line %d is empty. Stopping further checks.\n", y);
            break;
        }

        if (isLineFull(y)) {
            clearLine(y, tft, backgroundColor);
            ++clearedBelow;
            ++totalClearedLines;

            // Debug: Log cleared line
            //Serial.printf("Cleared line %d. Total cleared lines: %d\n", y, totalClearedLines);
        } else {
            lineToClearedBelow[y] = clearedBelow;
        }
    }

    if (totalClearedLines == 0) {
        return 0; // No lines cleared, return early
    }

    //Serial.printf("totalClearedLines: %d\n", totalClearedLines);

    // Second pass: Move lines based on the tracked cleared lines below
    for (auto it = lineToClearedBelow.rbegin(); it != lineToClearedBelow.rend(); ++it) {
        int y = it->first;
        int moveBy = it->second;

        if (moveBy > 0 && !isLineEmpty(y)) {
            int targetIndex = y + moveBy;
            if (targetIndex < MAP_HEIGHT) {
                moveLineDown(y, tft, moveBy, backgroundColor);
                //Serial.printf("Moved line %d down by %d rows to %d.\n", y, moveBy, targetIndex);
            }
        }
    }

    return totalClearedLines;
}

int BlockMap::clearAndMoveAllFullLines2(const Shape& shape) {
    int totalClearedLines = 0;

    // Define the range of lines to check based on the lowest and highest blocks of the shape
    int startLine = shape.getLowestBlock().getY();    // Lowest block's row (highest Y value)
    int endLine = shape.getHighestBlock().getY();     // Highest block's row (lowest Y value)

    // Debug: Log start and end lines
    //Serial.printf("clearAndMoveAllFullLines(startLine=%d, endLine=%d)\n", startLine, endLine);

    // Track lines that are full and the number of cleared lines below them
    std::map<int, int> lineToClearedBelow;

    // First pass: Identify full lines and track the cleared lines below each
    int clearedBelow = 0;
    for (int y = startLine; y >= 0; --y) {
        if (isLineEmpty(y)) {
            // If the current line is empty, no need to check further lines above
            //Serial.printf("Line %d is empty. Stopping further checks.\n", y);
            break;
        }

        if (isLineFull(y)) {
            clearLine(y);
            ++clearedBelow;
            ++totalClearedLines;

            // Debug: Log cleared line
            //Serial.printf("Cleared line %d. Total cleared lines: %d\n", y, totalClearedLines);
        } else {
            lineToClearedBelow[y] = clearedBelow;
        }
    }

    if (totalClearedLines == 0) {
        return 0; // No lines cleared, return early
    }

    //Serial.printf("totalClearedLines: %d\n", totalClearedLines);

    // Second pass: Move lines based on the tracked cleared lines below
    for (auto it = lineToClearedBelow.rbegin(); it != lineToClearedBelow.rend(); ++it) {
        int y = it->first;
        int moveBy = it->second;

        if (moveBy > 0 && !isLineEmpty(y)) {
            int targetIndex = y + moveBy;
            if (targetIndex < MAP_HEIGHT) {
                moveLineDown(y, moveBy);
                //Serial.printf("Moved line %d down by %d rows to %d.\n", y, moveBy, targetIndex);
            }
        }
    }

    return totalClearedLines;
}

void BlockMap::moveLineDown(int lineIndex, int rowsToMove) {
    if (lineIndex >= 0 && lineIndex < MAP_HEIGHT && rowsToMove > 0) {
        int startIdx = lineIndex * MAP_WIDTH;
        int targetStartIdx = startIdx + rowsToMove * MAP_WIDTH;

        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (targetStartIdx + x < MAP_WIDTH * MAP_HEIGHT) {
                delete map[targetStartIdx + x];
                map[targetStartIdx + x] = map[startIdx + x];
                map[startIdx + x] = nullptr;

                if (map[targetStartIdx + x]) {
                    map[targetStartIdx + x]->setY((targetStartIdx + x) / MAP_WIDTH);
                }
            }
        }
    }
}

void BlockMap::moveLineDown(int lineIndex, TFT_eSPI& tft, int rowsToMove, uint16_t backgroundColor) {
    if (lineIndex < 0 || lineIndex >= MAP_HEIGHT || rowsToMove <= 0) {
        Serial.println("Invalid parameters. Exiting moveLineDown.");
        return;
    }

    int targetIndex = lineIndex + rowsToMove;
    if (targetIndex >= MAP_HEIGHT) {
        Serial.printf("Target index (%d) out of bounds. Skipping.\n", targetIndex);
        return;
    }

    int startIdx = lineIndex * MAP_WIDTH;
    int targetStartIdx = targetIndex * MAP_WIDTH;

    for (int x = 0; x < MAP_WIDTH; ++x) {
        int currentIdx = startIdx + x;
        int targetIdx = targetStartIdx + x;

        if (map[currentIdx] != nullptr) {
            // Clear the block's previous graphical position
            map[currentIdx]->draw(tft, backgroundColor);

            // Move block to the target index
            map[targetIdx] = map[currentIdx];
            map[currentIdx] = nullptr; // Clear the original position

            // Update the block's position and redraw it
            map[targetIdx]->setY(targetIndex);
            map[targetIdx]->draw(tft);
        }
    }
    Serial.printf("Line %d moved down by %d rows to %d.\n", lineIndex, rowsToMove, targetIndex);
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
        int startIdx = y * MAP_WIDTH;

        // Check if the line is empty
        bool lineIsEmpty = true;
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map[startIdx + x] != nullptr) {
                lineIsEmpty = false;
                break;
            }
        }
        if (lineIsEmpty) {
            continue; // Skip drawing this line if it is empty
        }

        // Draw all blocks in the line
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Block* block = map[startIdx + x];
            if (block != nullptr) {
                block->draw(tft); // Draw the block if present
            }
        }
    }
}


bool BlockMap::checkGameOver() const {
    for (int x = 0; x < MAP_WIDTH; ++x) {
        if (map[x] != nullptr) { // Top row corresponds to indices 0 to MAP_WIDTH - 1
            return true; // Block found in the top row
        }
    }
    return false; // No blocks in the top row
}

int BlockMap::getColumnHeight(int x) const {
    if (x < 0 || x >= MAP_WIDTH) {
        return 0; // Out-of-bounds column
    }

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        int index = y * MAP_WIDTH + x;
        if (map[index] != nullptr) {
            return MAP_HEIGHT - y; // Height is measured from the bottom
        }
    }
    return 0; // Column is empty
}

int BlockMap::getTotalHoles() const {
    int totalHoles = 0;
    for (int x = 0; x < MAP_WIDTH; ++x) {
        bool blockFound = false;
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            int index = y * MAP_WIDTH + x; // Calculate the 1D index
            if (map[index] != nullptr) {
                blockFound = true; // Start counting holes after the first block
            } else if (blockFound) {
                totalHoles++; // Count holes only after encountering a block
            }
        }
    }
    return totalHoles;
}

int BlockMap::getBumpiness() const {
    int bumpiness = 0;
    int prevHeight = getColumnHeight(0);
    for (int x = 1; x < MAP_WIDTH; ++x) {
        int currHeight = getColumnHeight(x);
        bumpiness += abs(currHeight - prevHeight);
        prevHeight = currHeight;
    }
    return bumpiness;
}

void BlockMap::printBlockMap() const {
    Serial.println("BlockMap State:");
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        Serial.print(y < 10 ? " " : ""); // Align single-digit rows
        Serial.print(y);
        Serial.print(" | ");
        int rowStartIndex = y * MAP_WIDTH; // Start index for the row
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Serial.print(map[rowStartIndex + x] != nullptr ? "#" : ".");
        }
        Serial.println();
    }
    Serial.println("    --------------------"); // Divider
    Serial.println("     0 1 2 3 4 5 6 7 8 9"); // Column labels
}
