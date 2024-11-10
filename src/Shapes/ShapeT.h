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

    /**
     * @brief Draws the next shape preview.
     * @param tft Reference to the TFT_eSPI display object.
     * @param boxSize Size of each box for rendering the shape.
     */
    void drawNextShape(TFT_eSPI& tft, int boxSize) override;
};

#endif // SHAPET_H
