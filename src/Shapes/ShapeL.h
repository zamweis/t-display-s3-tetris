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

    /**
     * @brief Draws the next shape preview.
     * @param tft Reference to the TFT_eSPI display object.
     * @param boxSize Size of each box for rendering the shape.
     */
    void drawNextShape(TFT_eSPI& tft, int boxSize) override;
};

#endif // SHAPEL_H
