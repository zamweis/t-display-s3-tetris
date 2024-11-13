#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>
#include "TFT_eSPI.h" // Assuming you need this for drawing

class Block {
private:
    int x;
    int y;
    uint16_t color;

public:
    Block(int x = 0, int y = 0, uint16_t color = 0);

    void setCoordinates(int x, int y);
    void setX(int x);
    void setY(int y);
    void setColor(uint16_t color);

    int getX() const;
    int getY() const;
    uint16_t getColor() const;

    void moveLeft();
    void moveRight();
    void moveDown();

    // Marking these methods as const to allow calling on const objects
    void draw(TFT_eSPI &tft, int boxSize, uint16_t color = 0) const;
    void drawWithOffset(TFT_eSPI &tft, int boxSize, int xOffset, int yOffset) const;
    void drawBorderOnly(TFT_eSPI &tft, int boxSize, int offset) const;
    void drawAtLocation(TFT_eSPI &tft, int boxSize) const;

};

#endif // BLOCK_H
