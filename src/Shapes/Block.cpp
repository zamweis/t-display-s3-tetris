#include "Block.h"

// Constructor
Block::Block(int x, int y, uint16_t color) : x(x), y(y), color(color) {}

void Block::setCoordinates(int x, int y) {
    this->x = x;
    this->y = y;
}

void Block::setX(int x) {
    this->x = x;
}

void Block::setY(int y) {
    this->y = y;
}

void Block::setColor(uint16_t color) {
    this->color = color;
}

int Block::getX() const {
    return x;
}

int Block::getY() const {
    return y;
}

uint16_t Block::getColor() const {
    return color;
}

void Block::moveLeft() {
    x -= 1;
}

void Block::moveRight() {
    x += 1;
}

void Block::moveDown() {
    y += 1;
}

void Block::draw(TFT_eSPI &tft, int boxSize) const {
    // Calculate padding for centering within the grid cell
    int padding = (boxSize - (boxSize - 4)) / 2; // Adjust the padding based on boxSize
    tft.fillRect(x * boxSize + padding, y * boxSize + padding, boxSize - 3, boxSize - 3, color);
}

void Block::drawWithOffset(TFT_eSPI &tft, int boxSize, int xOffset, int yOffset) const {
    int padding = (boxSize - (boxSize - 4)) / 2; // Centering adjustment
    tft.fillRect((x - 2) * boxSize + xOffset + padding, (y + 4) * boxSize + yOffset + padding, boxSize - 3, boxSize - 3, color);
}

void Block::drawBorderOnly(TFT_eSPI &tft, int boxSize, int offset) const {
    int padding = (boxSize - (boxSize - 4)) / 2; // Centering adjustment
    tft.drawRect(x * boxSize + padding, (y + offset) * boxSize + padding, boxSize - 3, boxSize - 3, color);
}
