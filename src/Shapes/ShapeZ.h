#ifndef SHAPEZ_H
#define SHAPEZ_H

#include "Shape.h"
#include "TFT_eSPI.h"

class ShapeZ : public Shape {
public:
    ShapeZ();
    void drawNextShape(TFT_eSPI& tft, int boxSize) override;
};

#endif // SHAPEZ_H
