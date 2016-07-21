#ifndef POINT_H
#define POINT_H

typedef struct Point {
    float x;
    float y;

    Point(float initx, float inity) {
        x = initx;
        y = inity;
    }
} Point;

#endif // POINT_H
