#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxDatGui.h"
#include "ofxPlots/ofxPlot.hpp"

#include "component.h"
#include "compman.h"
#include "wormman.h"
#include "track.h"
#include "enu.h"

#include "point.h"
#include "date.h"


using namespace std;

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void setupGui();
    void setup3dTopo();
    void setViewportSizes();
    void drawViewportOutline(const ofRectangle & viewport);


    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseScrolled(int x, int y, float scrollX, float scrollY);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    //
    ofVec2f posToScaledPos(ofVec2f pt);
    ofVec2f scaledPosToPos(ofVec2f pt);
    bool isPointInRect(ofVec2f checkpt, ofVec2f pt, float w, float h);

    ofRectangle vMap;
    ofRectangle v3d;
    ofRectangle vModel;
    ofRectangle vPairspace;
    ofRectangle vHistory;

    ofCamera cam;
    float extrusionAmount;
    ofVboMesh topo3d;
    ofTexture topo3dtex;

    //

    int SAMPLES; // unscaled width
    int LINES; // unscaled height
    int WIDTH; // scaled width of scalar components
    int HEIGHT; // scaled height of scalar components
    int SCALE;
    ofVec2f position;

    ofImage img_topo;
    ofImage img_gdop;
    ofVec2f pos_topo;
    float w_topo;
    float h_topo;

    CompMan CM;
    WormMan WM;
    ENU enu;

    // TODO delete when time manager takes over
    float* time;
    float newtime;
    bool paused;
    string COMPONENT;

    // TODO delete when TrackMan is invented
    vector<Track> tracks;        // holds all tracks, including activated tracks
    vector<int> active_tracks; // holds all tracks activated by the most recent click
    Track track1;
    Track track2;
    Track track3;

    // GUI
    ofxDatGui* gui;
    ofxDatGuiSlider* timeslider;
    ofxDatGui* gOptions;
    ofxDatGuiFolder* gMapLayers;
    ofxDatGuiFolder* gMapWorms;
    ofxDatGuiFolder* gMapTracks;
    ofxDatGuiFolder* gMapModel;
    bool show_tracks;
    bool show_losdisp;
    bool show_azidisp;
    bool show_worms;
    bool show_modeldisp;
    bool show_modelhdisp;
    bool show_gdop;
    bool show_heightmap;

    // PLOTS N THINGS

};
