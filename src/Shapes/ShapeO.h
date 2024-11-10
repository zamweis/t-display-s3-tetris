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

    /**
     * @brief Draws the next shape preview.
     * @param tft Reference to the TFT_eSPI display object.
     * @param boxSize Size of each box for rendering the shape.
     */
    void drawNextShape(TFT_eSPI& tft, int boxSize) override;
};

#endif // SHAPEO_H
