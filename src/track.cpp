#include "track.h"

Track::Track()
{

}

void Track::setup(string file, int scale, int trackno, int orbit, ofVec2f pos, ofVec2f topleft, ofVec2f topright, ofVec2f botright, ofVec2f botleft) {
    this->scale = scale;
    this->trackno = trackno;
    this->orbit = orbit;
    this->pos = pos;
    this->tl = topleft;
    this->tr = topright;
    this->br = botright;
    this->bl = botleft;

    vector<ofPoint> verts;
    verts.push_back(ofPoint(tl.x/scale + pos.x, tl.y/scale + pos.y));
    verts.push_back(ofPoint(tr.x/scale + pos.x, tr.y/scale + pos.y));
    verts.push_back(ofPoint(br.x/scale + pos.x, br.y/scale + pos.y));
    verts.push_back(ofPoint(bl.x/scale + pos.x, bl.y/scale + pos.y));
    shape.addVertices(verts);
    shape.close();
}

void Track::setPos(ofVec2f pt){
    shape.clear();
    vector<ofPoint> verts;
    verts.push_back(ofPoint(tl.x/scale + pt.x, tl.y/scale + pt.y));
    verts.push_back(ofPoint(tr.x/scale + pt.x, tr.y/scale + pt.y));
    verts.push_back(ofPoint(br.x/scale + pt.x, br.y/scale + pt.y));
    verts.push_back(ofPoint(bl.x/scale + pt.x, bl.y/scale + pt.y));
    shape.addVertices(verts);
    shape.close();
}

bool Track::isClickInTrack(float x, float y) { // returns true if a click occured within the track's bounds
    return shape.inside(x, y);
}

void Track::draw() {
    shape.draw();
}

void Track::addDataPoint(Date date, float bperp) {
    data.push_back(Data(date, bperp));
}

Data Track::getDataFromTime(Date date) {

}

Data Track::getDataFromInt(int n) { // get the nth datum

}

void Track::createPair(Data d1, Data d2) { // link d1 and d2, making them into a pair

}
vector<Pair> Track::getAllPairs() {

}
