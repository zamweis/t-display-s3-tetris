#ifndef SHAPET_H
#define SHAPET_H

#include "Shape.h"
#include "TFT_eSPI.h"

/**
 * @brief ShapeT class representing a T-shaped tetromino.
 * Inherits from the base Shape class.
 */
class ShapeT : public Shape {
public:
    /**
     * @brief Constructor for ShapeT.
     * Initializes the T-shaped tetromino.
     */
    ShapeT();
};

#endif // SHAPET_H
