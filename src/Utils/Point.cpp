#include <stdio.h>
#include <stdlib.h>

// Define a struct for the Point
typedef struct {
    int x;
    int y;
} Point;

// Function to create a new Point
Point* createPoint(int x, int y) {
    Point* newPoint = (Point*)malloc(sizeof(Point));
    if (newPoint != NULL) {
        newPoint->x = x;
        newPoint->y = y;
    }
    return newPoint;
}

// Function to get the x-coordinate
int getX(Point* point) {
    return point->x;
}

// Function to set the x-coordinate
void setX(Point* point, int x) {
    point->x = x;
}

// Function to get the y-coordinate
int getY(Point* point) {
    return point->y;
}

// Function to set the y-coordinate
void setY(Point* point, int y) {
    point->y = y;
}

// Function to print the Point
void printPoint(Point* point) {
    printf("Point{x=%d, y=%d}\n", point->x, point->y);
}

// Function to free the Point memory
void freePoint(Point* point) {
    free(point);
}

int main() {
    // Create a new Point
    Point* p = createPoint(3, 4);
    printPoint(p);

    // Set new values
    setX(p, 10);
    setY(p, 20);
    printPoint(p);

    // Free memory
    freePoint(p);
    return 0;
}
