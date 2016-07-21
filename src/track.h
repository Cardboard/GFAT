#include "component.h"

#include "date.h"
#include "point.h"

#ifndef TRACK_H
#define TRACK_H

typedef struct Data {
    Date date;   // image acquisition date
    //ENU enu;     // LOS displacement components
    float bperp; // b-perp for this track & date combination

    Data(Date d, float b) {
        date = d;
        bperp = b;
    }
} Data;

typedef struct Pair {
    Date firstdate;
    Date seconddate;
    Data firstpt;
    Data secondpt;
} Pair;

class Track
{
public:
    Track();

    void setup(string file, int scale, int trackno, int orbit, ofVec2f pos, ofVec2f topleft, ofVec2f topright, ofVec2f botright, ofVec2f botleft);
    void setPos(ofVec2f pt);
    bool isClickInTrack(float x, float y); // returns true if a click occured within the track's bounds
    // TODO do we have ENU LOS data for each track?
    void addDataPoint(Date date, float bperp);
    Data getDataFromTime(Date date);
    Data getDataFromInt(int n); // get the nth datum
    void createPair(Data d1, Data d2); // link d1 and d2, making them into a pair
    vector<Pair> getAllPairs();
    void draw();

    //

    int trackno;
    int orbit;
    int beam;
    int acquisitions;
    int igrams;
    int scale; // used for drawing the rectangles on the screen
    ofVec2f pos; // local position/offset; used for drawing
    ofVec2f tl;
    ofVec2f tr;
    ofVec2f br;
    ofVec2f bl;
    ofPolyline shape;

    vector<Data> data;
    vector<Pair> pairs;
};

#endif // TRACK_H
