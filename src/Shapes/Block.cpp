#include "Block.h"

// Constructor
Block::Block(int x, int y, uint16_t color) : x(x), y(y), color(color) {}

// Set coordinates
void Block::setCoordinates(int x, int y) {
    this->x = x;
    this->y = y;
}

// Setters
void Block::setX(int x) {
    this->x = x;
}

void Block::setY(int y) {
    this->y = y;
}

void Block::setColor(uint16_t color) {
    this->color = color;
}

// Getters
int Block::getX() const {
    return x;
}

int Block::getY() const {
    return y;
}

uint16_t Block::getColor() const {
    return color;
}

// Movement methods
void Block::moveLeft() {
    x -= 1;
}

void Block::moveRight() {
    x += 1;
}

void Block::moveDown() {
    y += 1;
}

// Drawing methods
void Block::draw(TFT_eSPI &tft, int boxSize) {
    tft.fillRect(x * boxSize + 2, y * boxSize + 2, boxSize - 5, boxSize - 5, color);
    tft.drawRect(x * boxSize + 2, y * boxSize + 2, boxSize - 5, boxSize - 5, TFT_WHITE); // Optional: Draw a border
}

void Block::drawWithOffset(TFT_eSPI &tft, int boxSize, int xOffset, int yOffset) {
    tft.fillRect((x - 2) * boxSize + xOffset, (y + 4) * boxSize + yOffset, boxSize - 5, boxSize - 5, color);
    tft.drawRect((x - 2) * boxSize + xOffset, (y + 4) * boxSize + yOffset, boxSize - 5, boxSize - 5, TFT_WHITE); // Optional: Draw a border
}

void Block::drawBorderOnly(TFT_eSPI &tft, int boxSize, int offset) {
    tft.drawRect(x * boxSize + 2, (y + offset) * boxSize + 2, boxSize - 5, boxSize - 5, color);
}
