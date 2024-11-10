#ifndef SHAPEJ_H
#define SHAPEJ_H

#include "Shape.h"
#include "TFT_eSPI.h"

/**
 * @brief ShapeJ class representing a J-shaped tetromino.
 * Inherits from the base Shape class.
 */
class ShapeJ : public Shape {
public:
    /**
     * @brief Constructor for ShapeJ.
     * Initializes the J-shaped tetromino.
     */
    ShapeJ();
};

#endif // SHAPEJ_H
