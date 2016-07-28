#include "ofMain.h"
#include "ofApp.h"

#include "modelcomponent.h"
#include "enu.h"

#include "date.h"

#ifndef COMPMAN_H
#define COMPMAN_H

using namespace std;

typedef struct NamedComp {
    string name;
    ModelComponent comp;
} NamedComp;


class CompMan
{
public:
    CompMan();

    void setup(int samples, int lines, int w, int h, int x, int y);
    void setup(int samples, int lines, int w, int h);
    void setPos(ofVec2f pt);
    void addComponent(string name, string filename); // TODO figure out how time is recorded in data / components
    NamedComp getComponent(string name);
    void setCurrentComponent(string name);
    void drawComponent(string name);
    void drawComponent();
    void checkClicks(int x, int y);
    ENU solveEquation(float time);
    ENU solveEquation(ofVec2f point, float time);


    //

    vector<NamedComp> components;
    NamedComp current;

    // position & scale
    ofVec2f pos;
    int width;
    int height;
    int samples;
    int lines;

    // function toggling (0: off, 1: on)
    int secular_enabled;
    int p1_enabled;
    int p2_enabled;
    int p3_enabled;

};



#endif // COMPMAN_H
