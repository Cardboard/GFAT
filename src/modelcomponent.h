#include "ofMain.h"

#include <stdint.h>
#include <iostream>
#include <vector>
#include <fstream>

#ifndef Component_H
#define Component_H

typedef unsigned char uchar;

class ModelComponent
{
public:
    ModelComponent();
    ~ModelComponent();

    void setInfo(int samples, int lines);
    void setInfo(string name, int samples, int lines);
    void loadAndConvert(string filename); // load bytes from a file and convert them into a 2D array
    void setupImage(); // set all of the images pixels based on the values in 'array'
    void setPos(float x, float y);
    void setSize(int w); // set the size of the image to be drawn based on a given width
    void setSize(int w, int h); // set the size of the image to be drawn
    void checkClicks(int x, int y); // returns true if the image was clicked and the value is non-nan
    void highlightPixel(); // highlight a clicked pixel
    float getValue(float x, float y);
    float getValue(ofVec2f point); // get the float value of the pixel at the global position (x, y)
    void circlePixels();

    //

    // binary data & image
    string name;
    int samples; // image width
    int lines; // image height
    int time; // the time associated with the component
    float min;
    float max;
    vector<vector<float>> array;

    // position & scale
    ofVec2f pos;
    int width;
    int height;
    float scale;

    // etc
    float threshold; // used in determining what is considered "zero" for coloring purposes
    ofVec2f click_pos; // global x,y of last valid (on image & non-nane) mouse click
    int click_i_x; // array x-index of last valid mouse click
    int click_i_y; // array y-index of last valid mouse click

    ofImage img;
    ofImage overlay;
};

#endif // Component_H
