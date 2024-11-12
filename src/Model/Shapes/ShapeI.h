#ifndef SHAPEI_H
#define SHAPEI_H

#include "Shape.h"
#include "TFT_eSPI.h"

/**
 * @brief ShapeI class representing an I-shaped (straight line) tetromino.
 * Inherits from the base Shape class.
 */
class ShapeI : public Shape {
public:
    /**
     * @brief Constructor for ShapeI.
     * Initializes the I-shaped tetromino.
     */
    ShapeI();
};

#endif // SHAPEI_H
