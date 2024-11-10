#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int x;
    int y;
    char *color; // Simplified color representation as a string
} Block;

typedef struct {
    Block blocks[4];
    int rotatePosition;
} ShapeI;

// Function prototypes
void initShapeI(ShapeI *shape);
void setPoint(Point *point, int x, int y);
void drawNextShape(ShapeI *shape);

// Constants for rotation positions
const int ROTATE_POSITION_0 = 0;
const int ROTATE_POSITION_1 = 1;
const int ROTATE_POSITION_2 = 2;
const int ROTATE_POSITION_3 = 3;

void initShapeI(ShapeI *shape) {
    shape->rotatePosition = ROTATE_POSITION_0; // Initialize rotation position
    char *color = "255,238,0"; // Simplified color as a string

    if (shape->rotatePosition == ROTATE_POSITION_0) {
        shape->blocks[0] = (Block){5, -1, color};
    } else if (shape->rotatePosition == ROTATE_POSITION_1) {
        shape->blocks[0] = (Block){4, -2, color};
    } else if (shape->rotatePosition == ROTATE_POSITION_2) {
        shape->blocks[0] = (Block){3, -1, color};
    } else {
        shape->blocks[0] = (Block){4, -3, color};
    }

    // Setting points based on rotation
    Point points[4][3];
    // Rotate 0
    setPoint(&points[0][0], 1, 0);
    setPoint(&points[0][1], -1, 0);
    setPoint(&points[0][2], -2, 0);
    // Rotate 1
    setPoint(&points[1][0], 0, 1);
    setPoint(&points[1][1], 0, -1);
    setPoint(&points[1][2], 0, -2);
    // Rotate 2
    setPoint(&points[2][0], -1, 0);
    setPoint(&points[2][1], 1, 0);
    setPoint(&points[2][2], 2, 0);
    // Rotate 3
    setPoint(&points[3][0], 0, -1);
    setPoint(&points[3][1], 0, 1);
    setPoint(&points[3][2], 0, 2);
}

void setPoint(Point *point, int x, int y) {
    point->x = x;
    point->y = y;
}

void drawNextShape(ShapeI *shape) {
    int rotatePos = shape->rotatePosition;
    for (int i = 0; i < 4; i++) {
        if (rotatePos == ROTATE_POSITION_1 || rotatePos == ROTATE_POSITION_3) {
            // Replace with your drawing logic; for example:
            printf("Drawing block with offset (40, 0, 20) at x: %d, y: %d\n", shape->blocks[i].x, shape->blocks[i].y);
        } else if (rotatePos == ROTATE_POSITION_2) {
            printf("Drawing block with offset (40, 20, 0) at x: %d, y: %d\n", shape->blocks[i].x, shape->blocks[i].y);
        } else {
            printf("Drawing block with offset (40, -20, 0) at x: %d, y: %d\n", shape->blocks[i].x, shape->blocks[i].y);
        }
    }
}

int main() {
    ShapeI shape;
    initShapeI(&shape);
    drawNextShape(&shape);

    return 0;
}
