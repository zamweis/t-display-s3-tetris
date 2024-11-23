#include "Block.h"
#include "Config.h"

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

void Block::draw(TFT_eSPI &tft,  uint16_t colorOverride) const {
    // Calculate padding for centering within the grid cell
    int padding = 2; // Adjust the padding based on BOX_SIZE
    uint16_t drawColor = (colorOverride != 0) ? colorOverride : color;

    if (y >= 0) {
        tft.fillRect(x * BOX_SIZE + padding, y * BOX_SIZE + padding, BOX_SIZE - 3, BOX_SIZE - 3, drawColor);
    } else {
       // Serial.println("Block is outside visible area, ignoring");
    }
}

void Block::drawWithOffset(TFT_eSPI &tft, int xOffset, int yOffset) const {
    int padding = (BOX_SIZE - (BOX_SIZE - 4)) / 2; // Centering adjustment
    tft.fillRect((x - 2) * BOX_SIZE + xOffset + padding, (y + 4) * BOX_SIZE + yOffset + padding, BOX_SIZE - 3, BOX_SIZE - 3, color);
}

void Block::drawBorderOnly(TFT_eSPI &tft, int offset) const {
    int padding = (BOX_SIZE - (BOX_SIZE - 4)) / 2; // Centering adjustment
    tft.drawRect(x * BOX_SIZE + padding, (y + offset) * BOX_SIZE + padding, BOX_SIZE - 3, BOX_SIZE - 3, color);
}
