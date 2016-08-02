#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"

#include "enuplot.h"
#include "modelcomponent.h"
#include "compman.h"
#include "wormman.h"
#include "track.h"
#include "enu.h"

#include "date.h"

using namespace std;

typedef struct Button {
    ofRectangle rect;
    ofImage img;
    bool hidden = false;
} Button;

class ofApp : public ofBaseApp{

public:
    enum direction {LEFT, TOP, RIGHT, BOTTOM, TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT};

    void setup();
    void update();
    void draw();

    void setupGui();
    void setupPlots();
    void setupButton(Button *btn, string filename, ofRectangle *ref, direction dir, bool is_viewport, int padding);
    void setupButtons();
    void setup3dTopo();
    void setCameraPosition();
    void setViewportSizes();
    void drawViewportOutline(const ofRectangle & viewport);
    void drawPlots();
    void drawButton(Button *btn, bool colored, bool visible);
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
    bool isButtonClicked(ofVec2f mpos, Button *btn);
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
    ofVec2f selection2_pos;

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
    int mouse_mode; // 0: worm drawing mode, 1: selection mode

    // CUSTOM BUTTONS
    Button btn_2d;
    Button btn_3d;
    Button btn_fullscreen_map;
    Button btn_fullscreen_model;

    Button btn_model;
    Button btn_model_vel;
    Button btn_model_p1;
    Button btn_model_p2;
    Button btn_model_p3;

    Button btn_settings;
    Button btn_settings_worm;
    Button btn_settings_selection;
    Button btn_settings_size_small;
    Button btn_settings_size_med;
    Button btn_settings_size_large;
    Button btn_settings_density_low;
    Button btn_settings_density_med;
    Button btn_settings_density_high;

    Button btn_worms_lag;
    Button btn_worms_eul;
    Button btn_worms_lag_worms;
    Button btn_worms_lag_lines;
    Button btn_worms_lag_dots;
    Button btn_worms_eul_worms;
    Button btn_worms_eul_lines;
    Button btn_worms_eul_dots;
    Button btn_worms_eul_disp;
    Button btn_worms_up;

    Button btn_layers;
    Button btn_layers_gdop;
    Button btn_layers_heightmap;
    Button btn_layers_contour;

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
    bool show_contour;

    // PLOTS N THINGS
    ENUPlot plotENU;

};
