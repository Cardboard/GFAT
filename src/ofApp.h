#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxDatGui.h"
#include "ofxPlots/ofxPlot.hpp"

#include "modelcomponent.h"
#include "compman.h"
#include "wormman.h"
#include "track.h"
#include "enu.h"

#include "date.h"


using namespace std;

class ofApp : public ofBaseApp{

public:
    enum direction {LEFT, TOP, RIGHT, BOTTOM, TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT};

    void setup();
    void update();
    void draw();

    void setupGui();
    void setupButton(ofRectangle *rect, ofImage *img, ofRectangle *ref, direction dir, bool is_viewport, int padding);
    void setupButtons();
    void setup3dTopo();
    void setCameraPosition();
    void setViewportSizes();
    void drawViewportOutline(const ofRectangle & viewport);
    void drawButton(ofImage *img, ofRectangle *rect, bool colored);
    void drawButtons();


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
    ofVec2f restrictPosition(ofVec2f pos, float obj_w, float obj_h, ofRectangle view);
    void toggleFullscreen(ofRectangle *view);


    ofRectangle vMap;
    ofRectangle v3d;
    ofRectangle vModel;
    ofRectangle vPairspace;
    ofRectangle vHistory;

    float map_zoom;
    ofCamera cam;
    float extrusionAmount;
    float cam_zoom;
    ofVboMesh topo3d;
    ofTexture topo3dtex;

    //

    int SAMPLES; // unscaled width
    int LINES; // unscaled height
    int WIDTH; // scaled width of scalar components
    int HEIGHT; // scaled height of scalar components
    int SCALE;
    ofVec2f position;
    ofVec2f position_3d;
    float rotation_3d;
    ofVec2f selection_pos;

    ofImage img_topo;
    ofImage img_gdop;
    ofVec2f pos_topo;
    float w_topo;
    float h_topo;

    // extra buttons not on a particular ofxDatGui
    bool fullscreen; // true when any view is in fullscreen mode
    bool hide_all; // hides all GUI windows and buttons
    bool is_selection;
    int map_mode; // either 2 (for 2D) or 3 (for 3D)

    // CUSTOM BUTTONS
    // button images
    ofImage img_3d_button;
    ofImage img_2d_button;
    ofImage img_fullscreen;
    // button rectangles
    ofRectangle button_2d;
    ofRectangle button_3d;
    ofRectangle fullscreen_map;
    ofRectangle fullscreen_3d;


    ofImage img_temp_history;
    ofImage img_temp_modelspace;
    ofImage img_temp_pairspace;

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
    ofxDatGui* gTimeGui;
    ofFbo map_buffer;
    ofxDatGuiSlider* timeslider;
    ofxDatGui* gOptions;
    ofxDatGuiFolder* gMapLayers;
    //ofxDatGuiFolder* gMapWorms;
    ofxDatGuiToggle* gMapWorms;
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
