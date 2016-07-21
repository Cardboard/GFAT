#include "ofMain.h"
#include "ofApp.h"

#include "component.h"
#include "enu.h"

#include "point.h"
#include "date.h"

#ifndef COMPMAN_H
#define COMPMAN_H

using namespace std;

typedef struct NamedComp {
    string name;
    Component comp;
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

};



#endif // COMPMAN_H
