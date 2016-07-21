#include "component.h"

#ifndef ENU_H
#define ENU_H

typedef struct ENU {
    float east;
    float north;
    float up;

    ENU(float e, float n, float u) {
        east = e;
        north = n;
        up = u;
    }
    ENU() {
        east = 9999.0;
        north = 9999.0;
        up = 9999.0;
    }
} ENU;

#endif // ENU_H
