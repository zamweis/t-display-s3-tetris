#ifndef SHAPEL_H
#define SHAPEL_H

#include "Shape.h"
#include "TFT_eSPI.h"

/**
 * @brief ShapeL class representing an L-shaped tetromino.
 * Inherits from the base Shape class.
 */
class ShapeL : public Shape {
public:
    /**
     * @brief Constructor for ShapeL.
     * Initializes the L-shaped tetromino.
     */
    ShapeL();
    ShapeType getType() const override {
        return ShapeType::L;
    }
};

#endif // SHAPEL_H
