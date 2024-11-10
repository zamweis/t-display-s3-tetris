#include <iostream>
#include <string>
#include <TFT_eSPI.h> // Include the TFT_eSPI library for ESP32-S3

class Block
{
private:
    int x;
    int y;
    uint16_t color; // Use 16-bit color values for TFT displays

public:
    // Constructor
    Block(int x, int y, uint16_t color) : x(x), y(y), color(color) {}

    // Set coordinates
    void setCoordinates(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    // Setters
    void setX(int x)
    {
        this->x = x;
    }

    void setY(int y)
    {
        this->y = y;
    }

    void setColor(uint16_t color)
    {
        this->color = color;
    }

    // Getters
    int getX() const
    {
        return x;
    }

    int getY() const
    {
        return y;
    }

    uint16_t getColor() const
    {
        return color;
    }

    // Movement methods
    void moveLeft()
    {
        x -= 1;
    }

    void moveRight()
    {
        x += 1;
    }

    void moveDown()
    {
        y += 1;
    }

    // Draw methods
    // This assumes you have a `TFT_eSPI` instance named `tft`.
    void draw(TFT_eSPI &tft, int boxSize)
    {
        tft.fillRect(x * boxSize + 2, y * boxSize + 2, boxSize - 5, boxSize - 5, color);
        tft.drawRect(x * boxSize + 2, y * boxSize + 2, boxSize - 5, boxSize - 5, TFT_WHITE); // Optional: Draw a border
    }

    void drawWithOffset(TFT_eSPI &tft, int boxSize, int xOffset, int yOffset)
    {
        tft.fillRect((x - 2) * boxSize + xOffset, (y + 4) * boxSize + yOffset, boxSize - 5, boxSize - 5, color);
        tft.drawRect((x - 2) * boxSize + xOffset, (y + 4) * boxSize + yOffset, boxSize - 5, boxSize - 5, TFT_WHITE); // Optional: Draw a border
    }

    void drawBorderOnly(TFT_eSPI &tft, int boxSize, int offset)
    {
        tft.drawRect(x * boxSize + 2, (y + offset) * boxSize + 2, boxSize - 5, boxSize - 5, color);
    }
};