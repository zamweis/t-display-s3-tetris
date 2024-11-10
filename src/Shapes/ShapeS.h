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

    /**
     * @brief Draws the next shape preview.
     * @param tft Reference to the TFT_eSPI display object.
     * @param boxSize Size of each box for rendering the shape.
     */
    void drawNextShape(TFT_eSPI& tft, int boxSize) override;
};

#endif // SHAPES_H
