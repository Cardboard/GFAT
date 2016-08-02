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
    if (mode == 1 && alpha == 255.0) {
        age = 255;
    }

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
    if (alpha == 255.0) {
        if (mode == 1) {
            age = 255.0; // keeps worms from dying
        }
        ofSetLineWidth(1);
        ofSetColor(30, 30, 30, 250);
        ofDrawLine(startx + offset.x, starty + offset.y, x + offset.x, y + offset.y);
    }
    */

    if (include_up == 1) {
        float up_scale = max(1.f, 10.f * abs(enu.up));
        // don't draw worms that haven't moved at all yet
        if (x == startx && y == starty && startenu.east == 9999.0 && startenu.north == 9999.0) {
            ofSetColor(0, 0, 0, 0);
        } else {
            // draw outline
            /*
            ofSetColor(255);
            ofNoFill();
            ofDrawCircle(x, y, up_scale * size);
            ofFill();
            */
            if (opaque) {
                ofSetColor(color.red, color.green, color.blue, 255);
            } else {
                ofSetColor(color.red, color.green, color.blue, min((float)age, alpha));
            }

        }
        ofDrawCircle(x, y, up_scale * size);
    } else {
        // don't draw worms that haven't moved at all yet
        if (x == startx && y == starty && startenu.east == 9999.0 && startenu.north == 9999.0) {
            ofSetColor(0, 0, 0, 0);
        } else {
            // draw outline
            /*
            ofSetColor(255);
            ofNoFill();
            ofDrawCircle(x, y, up_scale * size);//scaled_size);
            ofFill();
            */
            if (opaque) {
                ofSetColor(color.red, color.green, color.blue, 255);
            } else {
                ofSetColor(color.red, color.green, color.blue, min((float)age, alpha));
            }
        }
        ofDrawCircle(x, y, scaled_size);
    }

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

    if (include_up == 1) {
        // set color based on total magnitude
        float mag = sqrt(e.up * e.up + e.north * e.north + e.east * e.east);
        if (colormode == 0) {
            if (mode == 1 && opaque == true) {
                float magstart = sqrt(startenu.up * startenu.up + startenu.east*startenu.east + startenu.north*startenu.north);
                float diff = magstart - mag;
                float scaledcolor = min(255.0, (abs(diff) / (0.6 / (230.0 - 25.0))) + 25.0);
                if (magstart > mag) {
                    color = RGB(scaledcolor, 0, 0);
                } else if (magstart < mag) {
                    color = RGB(0, 0, scaledcolor);
                } else {
                    color = RGB(40, 40, 40);
                }
            } else {
                float rangedH =  (mag / ((1.6 - 0.0) / (420.0 - 320.0))) + 320.0;
                if (rangedH > 360) rangedH -= 360;
                float rangedS =  (mag / ((1.6 - 0.0) / (1.0 - 0.62))) + 0.62;
                float rangedL =  (mag / ((1.6 - 0.0) / (0.86 - 0.29))) + 0.29;

                color = hslToRgb(rangedH, rangedS, rangedL);
            }
        } else if (colormode == 1) {
            float rangedhmag = min(1.0, 2 * (mag / ((1.6 - 0.0) / (1.0 - 0.0))) + 0.0);
            color = hslToRgb(0.0, 0.0, rangedhmag);
        }
    } else {
        // set color based on horizontal magnitude
        float hmag = sqrt(e.north * e.north + e.east * e.east);
        if (colormode == 0) {
            if (mode == 1 && opaque == true) {
                float hmagstart = sqrt(startenu.east*startenu.east + startenu.north*startenu.north);
                float diff = hmagstart - hmag;
                float scaledcolor = min(255.0, (abs(diff) / (0.6 / (230.0 - 25.0))) + 25.0);
                if (hmagstart > hmag) {
                    color = RGB(scaledcolor, 0, 0);
                } else if (hmagstart < hmag) {
                    color = RGB(0, 0, scaledcolor);
                } else {
                    color = RGB(40, 40, 40);
                }
            } else {
                float rangedH =  (hmag / ((1.6 - 0.0) / (420.0 - 320.0))) + 320.0;
                if (rangedH > 360) rangedH -= 360;
                float rangedS =  (hmag / ((1.6 - 0.0) / (1.0 - 0.62))) + 0.62;
                float rangedL =  (hmag / ((1.6 - 0.0) / (0.86 - 0.29))) + 0.29;

                color = hslToRgb(rangedH, rangedS, rangedL);
            }
        } else if (colormode == 1) {
            float rangedhmag = min(1.0, 2 * (hmag / ((1.6 - 0.0) / (1.0 - 0.0))) + 0.0);
            color = hslToRgb(0.0, 0.0, rangedhmag);
        }
    }
}

bool Worm::update(int mode, float speed, float dt)
{
    if (mode == 0) { // lagrangian movement
        x += dt * speed * enu.east; // 1.6 = magic number
        y -= dt * speed * enu.north;  // .4 = magic number
    } else {
        // TODO move the worms a lot less, but implement scaling to be able to see motion
        if (!opaque) {
            x = startx + (-startenu.east + enu.east) * speed; //+= dt * enu.east * 1.6; // 1.6 = magic number
            y = starty + (-startenu.north + enu.north) * speed; //-= dt * enu.north * .4;  // .4 = magic number
        } else {
            x = startx;
            y = starty;
        }
    }

    if (status > 0 || (lifespan < 0 && mode == 0)) {
        age += status;
    }

    if (age > 300 && status > 0) {
        age = 300;
        status = -abs(status);
    }

    if (status < 0) {
        lifespan -= 1;
    }

    if (age < 0 && mode == 0) {
        return true;
    }

    return false;
}
