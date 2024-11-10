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

    /**
     * @brief Draws the next shape preview.
     * @param tft Reference to the TFT_eSPI display object.
     * @param boxSize Size of each box for rendering the shape.
     */
    void drawNextShape(TFT_eSPI& tft, int boxSize) override;
};

#endif // SHAPEI_H
