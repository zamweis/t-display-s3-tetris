#include <stdio.h>
#include <string.h>

// Struct representing a Point
typedef struct {
    int x;
    int y;
} Point;

// Struct representing a Block with position and color
typedef struct {
    int x;
    int y;
    char color[20]; // Using a string to represent color for simplicity
} Block;

// Struct representing the ShapeZ with blocks and rotation points
typedef struct {
    Block blocks[4]; // Array to store blocks of the shape
    int rotatePosition;
    Point points[4][3]; // 4 rotation positions, each with 3 points
} ShapeZ;

// Constants representing rotation positions
enum { ROTATE_POSITION_0, ROTATE_POSITION_1, ROTATE_POSITION_2, ROTATE_POSITION_3 };

// Function prototypes
void initShapeZ(ShapeZ *shape);
void setPoint(Point *point, int x, int y);
void drawNextShape(const ShapeZ *shape);
void setBlock(Block *block, int x, int y, const char *color);

void initShapeZ(ShapeZ *shape) {
    shape->rotatePosition = ROTATE_POSITION_0; // Initial rotation position
    const char color[] = "0,14,173"; // Simplified color representation

    // Initial block setup (redundant logic simplified)
    setBlock(&shape->blocks[0], 4, -2, color);

    // Setting points for each rotation state
    // Rotate position 0
    setPoint(&shape->points[0][0], 1, -1);
    setPoint(&shape->points[0][1], 1, 0);
    setPoint(&shape->points[0][2], 0, 1);
    // Rotate position 1
    setPoint(&shape->points[1][0], -1, -1);
    setPoint(&shape->points[1][1], 0, -1);
    setPoint(&shape->points[1][2], 1, 0);
    // Rotate position 2
    setPoint(&shape->points[2][0], -1, 1);
    setPoint(&shape->points[2][1], -1, 0);
    setPoint(&shape->points[2][2], 0, -1);
    // Rotate position 3
    setPoint(&shape->points[3][0], 1, 1);
    setPoint(&shape->points[3][1], 0, 1);
    setPoint(&shape->points[3][2], -1, 0);
}

void setPoint(Point *point, int x, int y) {
    point->x = x;
    point->y = y;
}

void setBlock(Block *block, int x, int y, const char *color) {
    block->x = x;
    block->y = y;
    strncpy(block->color, color, sizeof(block->color) - 1);
    block->color[sizeof(block->color) - 1] = '\0';
}

void drawNextShape(const ShapeZ *shape) {
    // Placeholder drawing logic (to be replaced with actual graphics logic)
    int rotatePos = shape->rotatePosition;
    for (int i = 0; i < 4; i++) {
        if (rotatePos == ROTATE_POSITION_0) {
            printf("Drawing block at x: %d, y: %d with color: %s at offset (40, -20, 0)\n",
                   shape->blocks[i].x, shape->blocks[i].y, shape->blocks[i].color);
        } else if (rotatePos == ROTATE_POSITION_1) {
            printf("Drawing block at x: %d, y: %d with color: %s at offset (40, 0, 20)\n",
                   shape->blocks[i].x, shape->blocks[i].y, shape->blocks[i].color);
        } else if (rotatePos == ROTATE_POSITION_2) {
            printf("Drawing block at x: %d, y: %d with color: %s at offset (40, 20, 0)\n",
                   shape->blocks[i].x, shape->blocks[i].y, shape->blocks[i].color);
        } else {
            printf("Drawing block at x: %d, y: %d with color: %s at offset (40, 0, -20)\n",
                   shape->blocks[i].x, shape->blocks[i].y, shape->blocks[i].color);
        }
    }
}

int main() {
    ShapeZ shape;
    initShapeZ(&shape);
    drawNextShape(&shape);

    return 0;
}
