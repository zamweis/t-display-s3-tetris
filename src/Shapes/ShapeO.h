#ifndef SHAPEO_H
#define SHAPEO_H

#include "Shape.h"
#include "TFT_eSPI.h"

/**
 * @brief ShapeO class representing an O-shaped (square) tetromino.
 * Inherits from the base Shape class.
 */
class ShapeO : public Shape {
public:
    /**
     * @brief Constructor for ShapeO.
     * Initializes the O-shaped tetromino.
     */
    ShapeO();
};

#endif // SHAPEO_H
