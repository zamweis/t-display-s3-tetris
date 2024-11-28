#ifndef BLOCKMAP_H
#define BLOCKMAP_H
#include <vector>
#include "Model/Block.h"
#include "Model/Block.h"
#include <array> 
// Forward declaration of Shape
class Shape;

class BlockMap {
public:
    BlockMap();
    ~BlockMap();
    BlockMap(const BlockMap& other); // Copy constructor
    BlockMap& operator=(const BlockMap& other); // Assignment operator

    static constexpr int MAP_WIDTH = 10;
    static constexpr int MAP_HEIGHT = 19;

    Block* map[MAP_WIDTH * MAP_HEIGHT] = {nullptr}; // Single contiguous array

    // Block management
    void addBlock(Block* block);
    void addBlocks(Block blockList[], int size);
    void removeBlock(Block* block);
    void removeBlock(int x, int y);
    bool isFieldEmpty(int x, int y) const;
    Block* getBlock(int x, int y) const;
    bool checkGameOver() const;

    // Line operations
    void clearLine(int lineIndex, TFT_eSPI& tft, uint16_t backgroundColor);
    void clearLine(int lineIndex);
    bool isLineFull(int lineIndex) const;
    bool isLineEmpty(int lineIndex) const;
    void moveLineDown(int lineIndex, TFT_eSPI& tft, int amountOfLines, uint16_t backgroundColor);
    void moveLineDown(int lineIndex, int amountOfLines);
    int clearAndMoveAllFullLines(TFT_eSPI& tft, const Shape& shape, uint16_t backgroundColor);
    int clearAndMoveAllFullLines2(const Shape& shape);
    
    // AI-specific utility methods
    int getColumnHeight(int x) const; // Returns the height of a specified column
    int getTotalHoles() const; // Counts the number of holes in the block map
    int getBumpiness() const; // Calculates the bumpiness of the board
    int countClearedLines() const; // Returns the number of cleared lines (if applicable)
    int getAmoutOfFullLines() const;

    // Movement and block positioning
    void moveBlockDown(int x, int y);
    bool isBlockMovableDownwards(int x, int y) const;
    void drawAllBlocks(TFT_eSPI& tft);

    void printBlockMap() const;

private:
    // Helper methods for internal operations
    int getFirstNotEmptyLine(int lineIndex) const;
    void moveAllNotEmptyLinesDown(TFT_eSPI& tft, int clearedLines, uint16_t backgroundColor);
};

#endif // BLOCKMAP_H
