#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>
#include "TFT_eSPI.h" // Assuming this is a library for your display

class Block {
private:
    int x;
    int y;
    uint16_t color;

public:
    // Constructor with default parameters
    Block(int x = 0, int y = 0, uint16_t color = 0);

    // Setters
    void setCoordinates(int x, int y);
    void setX(int x);
    void setY(int y);
    void setColor(uint16_t color);
    
    // Getters
    int getX() const;
    int getY() const;
    uint16_t getColor() const;

    // Movement methods
    void moveLeft();
    void moveRight();
    void moveDown();

    // Drawing methods
    void draw(TFT_eSPI &tft, int boxSize);
    void drawWithOffset(TFT_eSPI &tft, int boxSize, int xOffset, int yOffset);
    void drawBorderOnly(TFT_eSPI &tft, int boxSize, int offset);
};

#endif // BLOCK_H
