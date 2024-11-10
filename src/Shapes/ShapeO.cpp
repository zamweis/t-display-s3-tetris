#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct for representing a Point
typedef struct {
    int x;
    int y;
} Point;

// Struct for representing a Block with color as a string (for simplicity)
typedef struct {
    int x;
    int y;
    char *color; // Color represented as a string
} Block;

// ShapeO struct to hold blocks and rotation points
typedef struct {
    Block blocks[4];
    int rotatePosition;
    Point points[4][3]; // Points for each rotation state
} ShapeO;

// Function prototypes
void initShapeO(ShapeO *shape);
void setPoint(Point *point, int x, int y);
void drawNextShape(const ShapeO *shape);

// Constants for rotation positions
const int ROTATE_POSITION_0 = 0;
const int ROTATE_POSITION_1 = 1;
const int ROTATE_POSITION_2 = 2;
const int ROTATE_POSITION_3 = 3;

void initShapeO(ShapeO *shape) {
    shape->rotatePosition = ROTATE_POSITION_0; // Initial rotation position
    char *color = "200,0,0"; // Simplified color representation as a string

    // Initial block placement
    shape->blocks[0] = (Block){4, -1, color};

    // Setting points for each rotation state
    // Rotate position 0
    setPoint(&shape->points[0][0], 1, 0);
    setPoint(&shape->points[0][1], 0, -1);
    setPoint(&shape->points[0][2], 1, -1);
    // Rotate position 1
    setPoint(&shape->points[1][0], 1, 0);
    setPoint(&shape->points[1][1], 0, -1);
    setPoint(&shape->points[1][2], 1, -1);
    // Rotate position 2
    setPoint(&shape->points[2][0], 1, 0);
    setPoint(&shape->points[2][1], 0, -1);
    setPoint(&shape->points[2][2], 1, -1);
    // Rotate position 3
    setPoint(&shape->points[3][0], 1, 0);
    setPoint(&shape->points[3][1], 0, -1);
    setPoint(&shape->points[3][2], 1, -1);
}

void setPoint(Point *point, int x, int y) {
    point->x = x;
    point->y = y;
}

void drawNextShape(const ShapeO *shape) {
    // Placeholder for drawing logic (replace with your graphics library functions if applicable)
    for (int i = 0; i < 4; i++) {
        printf("Drawing block at x: %d, y: %d with color: %s\n", shape->blocks[i].x, shape->blocks[i].y, shape->blocks[i].color);
        // Add your drawing logic here if using a graphics library
        // For example: drawBlock(shape->blocks[i], 40, -20, -20);
    }
}

int main() {
    ShapeO shape;
    initShapeO(&shape);
    drawNextShape(&shape);

    return 0;
}
