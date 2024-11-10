#include "Point.h"
#include <iostream> // for std::cout

// Constructor
Point::Point(float x, float y) : x(x), y(y) {}

// Getters
float Point::getX() const {
    return x;
}

float Point::getY() const {
    return y;
}

// Setters
void Point::setX(float x) {
    this->x = x;
}

void Point::setY(float y) {
    this->y = y;
}

// Print method
void Point::printPoint() const {
    std::cout << "Point{x=" << x << ", y=" << y << "}" << std::endl;
}
