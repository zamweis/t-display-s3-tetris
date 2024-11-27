#ifndef SHAPEZ_H
#define SHAPEZ_H

#include "Shape.h"
#include "TFT_eSPI.h"

class ShapeZ : public Shape {
public:
    ShapeZ();
    ShapeType getType() const override {
        return ShapeType::Z;
    }
};

#endif // SHAPEZ_H
