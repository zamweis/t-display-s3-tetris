#ifndef POINT_H
#define POINT_H

class Point {
private:
    float x;
    float y;

public:
    Point(float x = 0, float y = 0);

    float getX() const;
    float getY() const;
    void setX(float x);
    void setY(float y);

    void printPoint() const;
};

#endif // POINT_H
