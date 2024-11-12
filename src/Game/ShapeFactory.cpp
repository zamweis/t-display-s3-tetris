#include "ShapeFactory.h"
#include "Model/Shapes/ShapeI.h"
#include "Model/Shapes/ShapeJ.h"
#include "Model/Shapes/ShapeL.h"
#include "Model/Shapes/ShapeO.h"
#include "Model/Shapes/ShapeS.h"
#include "Model/Shapes/ShapeT.h"
#include "Model/Shapes/ShapeZ.h"
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
