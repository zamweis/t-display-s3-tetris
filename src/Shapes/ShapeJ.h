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

    /**
     * @brief Draws the next shape preview.
     * @param tft Reference to the TFT_eSPI display object.
     * @param boxSize Size of each box for rendering the shape.
     */
    void drawNextShape(TFT_eSPI& tft, int boxSize) override;
};

#endif // SHAPEJ_H
