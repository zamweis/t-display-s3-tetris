#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include "Model/Shapes/Shape.h"

class ShapeFactory {
public:
    static Shape* createRandomShape(); // Declaration only
};

#endif // SHAPE_FACTORY_H