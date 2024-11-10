#ifndef SHAPES_H
#define SHAPES_H

#include "Shape.h"
#include "TFT_eSPI.h"

/**
 * @brief ShapeS class representing an S-shaped tetromino.
 * Inherits from the base Shape class.
 */
class ShapeS : public Shape {
public:
    /**
     * @brief Constructor for ShapeS.
     * Initializes the S-shaped tetromino.
     */
    ShapeS();
};

#endif // SHAPES_H
