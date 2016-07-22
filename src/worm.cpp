#include "worm.h"

Worm::Worm() {}

void Worm::setup(int mode, int lifespan, float x, float y, float size)
{
    this->lifespan = lifespan;
    this->x = x;
    this->y = y;
    this->startx = x;
    this->starty = y;
    this->size = size;
    this->age = 0;
    this->status = 5;
    this->alpha = 0.0;
    this->opaque = false;
    this->mode = mode;      // 0 => lagrangian, 1 => eulerian
    this->colormode = 0; // 0 => color, 1 => b&w
}

void Worm::drawArrow(ofVec2f offset)
{
    // don't let eulerian arrows die
    if (mode == 1 && alpha == 255.0)
        age = 255;

    ofSetColor(color.red, color.green, color.blue, min((float)age, alpha));

    float length = sqrt((startx - x) * (startx - x) + (starty - y) * (starty - y));
    //float scaled_circle =  (length / ((10.0 - 0.0) / (3.0 - 1.0))) + 0.0;
    float xlength = -5 * (startx-x)/length;
    float ylength = -5 * (starty-y)/length;

    ofSetLineWidth(1);
    ofDrawLine(startx + offset.x, starty + offset.y, x+xlength + offset.x, y+ylength + offset.y);

    ofDrawCircle(startx + offset.x, starty + offset.y, size);

    float lineangle = atan2(ylength, xlength);
    float angle = PI/4;
    float h = abs(4/cos(angle));
    float angle1 = lineangle + PI + angle;
    float topx = (x+xlength + offset.x) + cos(angle1) * h;
    float topy = (y+ylength + offset.y) + sin(angle1) * h;
    float angle2 = lineangle + PI - angle;
    float botx = (x+xlength + offset.x) + cos(angle2) * h;
    float boty = (y+ylength + offset.y) + sin(angle2) * h;

    // left part of arrow
    ofDrawLine(x+xlength + offset.x, y+ylength + offset.y, topx, topy);
    // right part of arrow
    ofDrawLine(x+xlength + offset.x, y+ylength + offset.y, botx, boty);

    ofSetColor(255,255,255);
}

void Worm::draw(ofVec2f offset)
{
    // make the worms look like chain-chomps on leashes perhaps?
    /*
    if (mode == 1 && alpha == 255.0) {
        age = 255.0; // keeps worms from dying
        ofSetLineWidth(1);
        ofSetColor(30, 30, 30, 250);
        ofDrawLine(startx + offset.x, starty + offset.y, x + offset.x, y + offset.y);
    }
    */

    if (x == startx && y == starty && startenu.east == 9999.0 && startenu.north == 9999.0) {
        ofSetColor(color.red, color.green, color.blue, 0);// , 0.0);
    } else {
        if (opaque) {
            ofSetColor(color.red, color.green, color.blue, 255);
        } else {
            /*if (mode == 1) {
                float hmag = sqrt(enu.east*enu.east + enu.north*enu.north);
                float hmagstart = sqrt(startenu.east*startenu.east + startenu.north*startenu.north);
                float diff = hmagstart - hmag;
                float scaledcolor = min(255.0, (abs(diff) / (0.6 / (230.0 - 25.0))) + 25.0);
                if (hmagstart > hmag) {
                    ofSetColor(scaledcolor, 0, 0, 255);
                } else if (hmagstart < hmag) {
                    ofSetColor(0, 0, scaledcolor, 255);
                } else {
                    ofSetColor(40, 40, 40, 255);
                }
            } else {*/
                ofSetColor(color.red, color.green, color.blue, min((float)age, alpha));
            //}
        }
    }

    ofDrawCircle(x + offset.x, y + offset.y, scaled_size);
    ofSetColor(255,255,255);
}

RGB Worm::hslToRgb(float h, float s, float l){
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    if (s == 0)
    {
        r = g = b = (unsigned char)(l * 255);
    }
    else
    {
        float v1, v2;
        float hue = (float)h / 360;

        v2 = (l < 0.5) ? (l * (1 + s)) : ((l + s) - (l * s));
        v1 = 2 * l - v2;

        r = (float)(255 * hue2rgb(v1, v2, hue + (1.0f / 3)));
        g = (float)(255 * hue2rgb(v1, v2, hue));
        b = (float)(255 * hue2rgb(v1, v2, hue - (1.0f / 3)));
    }

    return RGB(r, g, b);
}

float Worm::hue2rgb(float v1,float v2, float vH){
    if (vH < 0)
            vH += 1;
        if (vH > 1)
            vH -= 1;
        if ((6 * vH) < 1)
            return (v1 + (v2 - v1) * 6 * vH);
        if ((2 * vH) < 1)
            return v2;
        if ((3 * vH) < 2)
            return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);
        return v1;
}

void Worm::setENU(ENU e)
{
    // set starte and startn once
    if (startenu.east == 9999.0 && startenu.north == 9999.0 && startenu.up == 9999.0) {
        startenu.east = e.east;
        startenu.north = e.north;
        startenu.up = e.up;
    }
    enu = e;
    // set color based on horizontal magnitude
    float hmag = sqrt(e.north * e.north + e.east * e.east);
    if (colormode == 0) {
        float rangedhmag =  (hmag / ((1.6 - 0.0) / (360.0 - 260.0))) + 260.0;
        color = hslToRgb(rangedhmag, 1.0, 0.6);
    } else if (colormode == 1) {
        float rangedhmag = min(1.0, 2 * (hmag / ((1.6 - 0.0) / (1.0 - 0.0))) + 0.0);
        color = hslToRgb(0.0, 0.0, rangedhmag);
    }
}

bool Worm::update(int mode, float speed, float dt)
{
    if (mode == 0) { // lagrangian movement
        x += dt * speed * enu.east * 1.6; // 1.6 = magic number
        y -= dt * speed * enu.north * .3;  // .4 = magic number
    } else {
        // TODO move the worms a lot less, but implement scaling to be able to see motion
        x = startx + (-startenu.east + enu.east) * speed * 1.6; //+= dt * enu.east * 1.6; // 1.6 = magic number
        y = starty + (-startenu.north + enu.north) * speed * 0.3; //-= dt * enu.north * .4;  // .4 = magic number
    }

    if (status > 0 || lifespan < 0) {
        age += status;
    }

    if (age > 300 && status > 0) {
        age = 300;
        status = -status;
    }

    if (status < 0) {
        lifespan -= 1;
    }

    if (age < 0 && mode == 0) {
        return true;
    }

    return false;
}
