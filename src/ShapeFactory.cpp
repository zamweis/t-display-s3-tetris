#include "ShapeFactory.h"
#include "Shapes/ShapeI.h"
#include "Shapes/ShapeJ.h"
#include "Shapes/ShapeL.h"
#include "Shapes/ShapeO.h"
#include "Shapes/ShapeS.h"
#include "Shapes/ShapeT.h"
#include "Shapes/ShapeZ.h"
#include <cstdlib> // for random number generation if needed

Shape* ShapeFactory::createRandomShape() {
    int index = rand() % 7; // Or use another RNG function if needed
    switch (index) {
        case 0: return new ShapeI();
        case 1: return new ShapeJ();
        case 2: return new ShapeL();
        case 3: return new ShapeO();
        case 4: return new ShapeS();
        case 5: return new ShapeT();
        case 6: return new ShapeZ();
        default: return nullptr;
    }
}
