#include "ofMain.h"

#include "compman.h"
#include "enu.h"

#ifndef ENUPLOT_H
#define ENUPLOT_H

typedef struct DataPt {
    float xval;
    float yval;

    DataPt(float x, float y) {
        xval = x;
        yval = y;
    }
} DataPt;


class ENUPlot
{
public:
    ENUPlot();
    ENUPlot(CompMan *cm, ofVec2f *pt, ofVec2f *pt2, float *time, string label_plot, string label_x, string label_y);
    void setRects(ofRectangle rect_plot);

    void clearData();
    void clearData2();
    // (re)compute data using a specified step and range of x-values, with
    // xscale & yscale determining the distance between ticks on the horizontal & vertical axes. if
    // xscale or yscale are zero, scales will be calculated automatically
    void refreshData(float step, float xstart, float xend, float xscale, float yscale);
    void refreshData2(float step, float xstart, float xend, float xscale, float yscale);
    void draw();
    void drawPlot(ofRectangle *rect, vector<DataPt> *data, vector<DataPt> *data2, float x_min, float x_max, float y_min, float y_max, float yscale, string label);

    //

    CompMan *cm;
    ofVec2f *pt;
    ofVec2f *pt2;
    ofTrueTypeFont font;
    ofRectangle rect_plot; // rectangle dictating the size of the entire plot (including axes, padding, labels, etc)
    ofRectangle rect_data_e;
    ofRectangle rect_data_n;
    ofRectangle rect_data_u;
    //ofRectangle rect_data; // rectangle containing only the actual data (bounded by x & y axes)
    vector<DataPt> data_e;
    vector<DataPt> data_n;
    vector<DataPt> data_u;
    vector<DataPt> data_e2;
    vector<DataPt> data_n2;
    vector<DataPt> data_u2;
    string label_plot;
    string label_x;
    string label_y;
    float xscale;
    float yscale_e, yscale_n, yscale_u; // used to determine where to draw ticks
    float x_min_e, x_max_e;
    float y_min_e, y_max_e;
    float x_min_n, x_max_n;
    float y_min_n, y_max_n;
    float x_min_u, x_max_u;
    float y_min_u, y_max_u;
    bool empty; // if empty == true we don't have any data so we won't attempt to draw data :)
    bool empty2;

    float *time;

    ofColor cBackground;
    ofColor cLabels;
    ofColor cNumbers;
    ofColor cTertiary;
    ofColor cData;
    ofColor cData2;
    ofColor cAxes;
};

#endif // ENUPLOT_H
