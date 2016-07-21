#include "ofMain.h"

#ifndef WORM_H
#define WORM_H

#include "enu.h"

typedef struct RGB {
    float red;
    float green;
    float blue;

    RGB(float r, float g, float b) {
        red = r;
        green = g;
        blue = b;
    }
    RGB() {
        red = 0.0;
        green = 0.0;
        blue = 0.0;
    }
} RGB;


class Worm
{
public:
    Worm();

    void setup(int mode, int lifespan, float x, float y, float size);

    void draw(ofVec2f offset); // uses worm's current age to determine transparency and/or color
    void drawArrow(ofVec2f offset); // draw eularian arrows instead of worms
    bool update(int mode, float speed, float dt);
    void setENU(ENU enu);
    RGB hslToRgb(float h,float s,float l);
    float hue2rgb(float v1, float v2, float vH);

    //

    int age;
    int mode;
    int lifespan; // when age > lifespan, the worm gets deleted
    float x;
    float y;
    float startx;
    float starty;
    float size;
    double scaled_size;
    bool opaque;
    bool colormode;

    float alpha;

    ENU startenu;
    ENU enu;
    RGB color;

    // living worms become more opaque over time, dying worms become more transparent
    int status; // 1 if living -1 if dying
};

#endif // WORM_H
