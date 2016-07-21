#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();

    // setup byte array
    SAMPLES = 3686;
    LINES = 1520;
    SCALE = 4;
    WIDTH = SAMPLES / SCALE;
    HEIGHT = LINES / SCALE;
    position.set(0, 200);

    // TODO replace with a Time Manager class thing
    time = new float;
    *time = 0.0;
    newtime = *time;
    paused = false;

    // setup component manager & components
    CM.setup(SAMPLES, LINES, WIDTH, HEIGHT, position.x, position.y);
    CM.addComponent("N", "north");
    CM.addComponent("E", "east");
    CM.addComponent("U", "up");
    CM.addComponent("MAG", "mag");
    CM.addComponent("HMAG", "hmag");

    CM.addComponent("AMP1N", "sinamp1.north");
    CM.addComponent("AMP1E", "sinamp1.east");
    CM.addComponent("AMP1U", "sinamp1.up");

    CM.addComponent("AMP2N", "sinamp2.north");
    CM.addComponent("AMP2E", "sinamp2.east");
    CM.addComponent("AMP2U", "sinamp2.up");
    CM.addComponent("AMP3N", "sinamp3.north");
    CM.addComponent("AMP3E", "sinamp3.east");
    CM.addComponent("AMP3U", "sinamp3.up");

    CM.addComponent("PHZ1N", "sinphz1.north");
    CM.addComponent("PHZ1E", "sinphz1.east");
    CM.addComponent("PHZ1U", "sinphz1.up");

    CM.addComponent("PHZ2N", "sinphz2.north");
    CM.addComponent("PHZ2E", "sinphz2.east");
    CM.addComponent("PHZ2U", "sinphz2.up");
    CM.addComponent("PHZ3N", "sinphz3.north");
    CM.addComponent("PHZ3E", "sinphz3.east");
    CM.addComponent("PHZ3U", "sinphz3.up");

    CM.setCurrentComponent("MAG");

    // setup worm manager
    WM.setup(CM, "E", "N", position);
    WM.setWormDensity(3.0);
    WM.setWormLifespan(100);
    WM.setWormTailSize(100);
    WM.setWormSize(3);
    WM.setWormSpeed(30);
    WM.setUniform(false);
    WM.setOpaque(false);
    WM.setMode(0);
    WM.setArrowMode(0);
    WM.worms.clear();

    // handle topography
    img_topo.load("topo_test.png");
    img_topo.resize(WIDTH, HEIGHT);
    img_gdop.load("gdop_test.png");
    img_gdop.resize(WIDTH, HEIGHT);

    ofVec2f tl(500+600,750);
    ofVec2f tr(500+1700,500);
    ofVec2f br(500+1900,1175);
    ofVec2f bl(500+1000,1500);
    Date testdate1(2013, 8, 28);
    Date testdate2(2013, 8, 24);
    Date testdate3(2013, 9, 7);
    track1.setup("placeholder.bin", SCALE, 1, 1, position, tl, tr, br, bl);
    track1.addDataPoint(testdate1, 0);
    track1.addDataPoint(testdate2, 100);
    track1.addDataPoint(testdate3, -10);
    track2.setup("placeholder.bin", SCALE, 2, 1, position,
                 ofVec2f(500+800, 750), ofVec2f(500+1600, 400), ofVec2f(500+1800, 500), ofVec2f(500+1000, 1250));
    track2.addDataPoint(testdate1, 0);
    track2.addDataPoint(testdate2, 100);
    track2.addDataPoint(testdate3, -10);
    track3.setup("placeholder.bin", SCALE, 3, -1, position,
                 ofVec2f(500+1100, 800), ofVec2f(500+1500, 750), ofVec2f(500+1700, 1700), ofVec2f(500+1300, 1600));
    track3.addDataPoint(testdate1, 0);
    track3.addDataPoint(testdate2, 100);
    track3.addDataPoint(testdate3, -10);

    tracks.push_back(track1);
    tracks.push_back(track2);
    tracks.push_back(track3);

    active_tracks.resize(tracks.size()+1, 0);

    setupGui();
}

//--------------------------------------------------------------
void ofApp::update(){
    // update calculated components

    // update current time

    // update the current component

    // update the worms components
    if (!paused) {
        *time += 1.0;
        if (*time > 24.0 * 45) *time = 0.0;
    }

    if (show_worms)
        WM.updateWorms(&CM, *time);
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    // GUI
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
    gui->addFRM(1.0f);
    ofxDatGuiSlider* timeslider = gui->addSlider("Time", 0, 24*45);
    timeslider->bind(*time);
    // layers -- gui elements
    gMapLayers = gui->addFolder("Layers", ofColor::aliceBlue);
    gMapLayers->collapse();
    gMapLayers->addToggle("   Tracks", false);
    gMapLayers->addToggle("   LOS Displacement", false);
    gMapLayers->addToggle("   Azimuth Displacement", false);
    show_worms = true;
    gMapLayers->addToggle("   Worms", show_worms);
    gMapLayers->addToggle("   Horizontal Model Disp", false);
    gMapLayers->addToggle("   Model Disp", false);
    gMapLayers->addToggle("   GDOP", false);
    show_heightmap = true;
    gMapLayers->addToggle("   Heightmap", show_heightmap);
    // layers -- events
    gMapLayers->onToggleEvent([&](ofxDatGuiToggleEvent e) {
        if (e.target->is("Track")) {
            show_tracks = e.checked;
        } else if (e.target->is("   LOS Displacement")) {
            show_losdisp = e.checked;
        } else if (e.target->is("   Azimuth Displacement")) {
            show_azidisp = e.checked;
        } else if (e.target->is("   Worms")) {
            show_worms = e.checked;
        } else if (e.target->is("   Horizontal Model Disp")) {
            show_modelhdisp = e.checked;
        } else if (e.target->is("   Model Disp")) {
            show_modeldisp = e.checked;
        } else if (e.target->is("   GDOP")) {
            show_gdop = e.checked;
            if (e.checked) {
                WM.setColorMode(1);
            } else {
                WM.setColorMode(0);
            }
        } else if (e.target->is("   Heightmap")) {
            show_heightmap = e.checked;
        }
    });
    gui->addBreak()->setHeight(10.0f);
    // worms -- gui elements
    gMapWorms = gui->addFolder("Worm Options", ofColor::orangeRed);
    gMapWorms->collapse();
    gMapWorms->addButton("   Default");
    gMapWorms->addButton("   Streamlines");
    gMapWorms->addButton("   Arrows");
    gMapWorms->addButton("   Circles");
    gMapWorms->addToggle("   Show future while paused", false);
    gui->addBreak()->setHeight(10.0f);

    // worms -- events
    gMapWorms->onButtonEvent([&](ofxDatGuiButtonEvent e) {
        if (e.target->is("   Default")) {
            WM.setWormLifespan(100);
            WM.setWormTailSize(100);
            WM.setWormSize(3);
            WM.setWormSpeed(30);
            WM.setOpaque(false);
            WM.setArrowMode(0);
            WM.setWormDensity(3.0);
        } else if (e.target->is("   Streamlines")) {
            WM.setWormLifespan(500);
            WM.setWormTailSize(500);
            WM.setWormSize(1);
            WM.setWormSpeed(15);
            WM.setOpaque(true);
            WM.setArrowMode(0);
            WM.setWormDensity(5.0);
        } else if (e.target->is("   Arrows")) {
            WM.setWormLifespan(300);
            WM.setWormTailSize(50);
            WM.setWormSize(2);
            WM.setWormSpeed(30);
            WM.setOpaque(false);
            WM.setArrowMode(1);
            WM.setWormDensity(3.0);
        } else if (e.target->is("   Circles")) {
            WM.setWormLifespan(100);
            WM.setWormTailSize(2);
            WM.setWormSize(5);
            WM.setWormSpeed(30);
            WM.setOpaque(false);
            WM.setArrowMode(0);
            WM.setWormDensity(0.1);
        } else if (e.target->is("   Show future while paused")) {
            // TODO implement these kind of worms
        }
    });

    // tracks -- gui elements
    gMapTracks = gui->addFolder("Tracks", ofColor::greenYellow);
    gMapTracks->collapse();
    gMapTracks->addButton("   Clear Selection");
    gMapTracks->addButton("   Select All Available Tracks");
    ofxDatGuiMatrix* tracks = gMapTracks->addMatrix("   Available Tracks", 56, true);
    tracks->setRadioMode(false);
    gui->addBreak()->setHeight(10.0f);
    // tracks -- events
    gMapTracks->onButtonEvent([&](ofxDatGuiButtonEvent e) {
    });

    // model -- gui elements
    gMapModel = gui->addFolder("Model", ofColor::fuchsia);
    gMapModel->collapse();
    gMapModel->addToggle("   Secular Velocity", true);
    gMapModel->addToggle("   Period 1", true);
    gMapModel->addToggle("   Period 2", true);
    gMapModel->addToggle("   Period 3", true);
    gMapModel->addToggle("   F(x)", false);
    // model -- events
    gMapModel->onToggleEvent([&](ofxDatGuiToggleEvent e) {
        if (e.target->is("   Secular Velocity")) {
            if (e.checked == true) {
                WM.setMode(0);
                WM.refreshWorms(true);
            } else {
                WM.setMode(1);
                WM.refreshWorms(true);
            }
        }
    });

    // setup separate options gui for adjusting worms
    gOptions = new ofxDatGui(ofxDatGuiAnchor::BOTTOM_LEFT);
    gOptions->addHeader(":: Drag Me To Reposition ::");
    ofxDatGuiSlider* wormsize = gOptions->addSlider("   Size", 1, 10);
    wormsize->setValue(WM.worm_size);
    ofxDatGuiSlider* wormdensity = gOptions->addSlider("   Density", 0.0f, 1.0f);
    wormdensity->setValue(WM.worm_density);
    // weird behavior occurs when tail length is 1 for some reason
    ofxDatGuiSlider* wormlength = gOptions->addSlider("   Tail Length", 2, 500);
    wormlength->setValue(WM.worm_tailsize);
    ofxDatGuiSlider* wormlifespan = gOptions->addSlider("   Lifespan", 50, 500);
    wormlifespan->setValue(WM.worm_lifespan);
    ofxDatGuiToggle* wormopaque = gOptions->addToggle("   Opaque", false);
    wormopaque->setChecked(WM.worm_opaque);
    gOptions->onSliderEvent([&](ofxDatGuiSliderEvent e) {
        if (e.target->is("   Size")) {
            WM.setWormSize(e.value);
            WM.refreshWorms(false);
        } else if (e.target->is("   Density")) {
            WM.setWormDensity(e.value);
            WM.refreshWorms(true);
        } else if (e.target->is("   Tail Length")) {
            WM.setWormTailSize(e.value);
            WM.refreshWorms(true);
        } else if (e.target->is("   Lifespan")) {
            WM.setWormLifespan(e.value);
            WM.refreshWorms(true);
        }
    });
    gOptions->onToggleEvent([&](ofxDatGuiToggleEvent e) {
        if (e.target->is("   Opaque")) {
            WM.setOpaque(e.checked);
            WM.refreshWorms(false);
        }
    });
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (show_heightmap)
        // TODO delete magic no.
        img_topo.draw(position.x, position.y);

    if (show_gdop)
        // TODO delete magic no.
        img_gdop.draw(position.x, position.y);

    CM.current.comp.overlay.draw(position.x, position.y); // mask which slightly blocks where we don't have data

    if (show_worms)
        WM.drawWorms();


    if (show_tracks) {
        vector<Track>::iterator it, end;
        active_tracks.clear();
        for (it = tracks.begin(), end = tracks.end(); it != end; it++) {
            if (active_tracks[it->trackno] == 1)
                it->setPos(position);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    paused = !paused;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    ofVec2f pt;
    pt.set(x, y);
    // drag the map with the right mouse button
    if (button == 2) {
        if (isPointInRect(pt, position, WIDTH, HEIGHT)) {
            float m_dx = ofGetMouseX() - ofGetPreviousMouseX();
            float m_dy = ofGetMouseY() - ofGetPreviousMouseY();
            position.x += m_dx;
            position.y += m_dy;
            CM.setPos(position);
            WM.pos.set(position);
        }

        // move tracks
        vector<Track>::iterator it, end;
        for (it = tracks.begin(), end = tracks.end(); it != end; it++) {
            // we only care about updating the position of tracks relevant to the current selection
            if (active_tracks[it->trackno] == 1)
                it->setPos(position);
        }
    // drag to create a line or whatever of worms
    } else if (button == 0) {
        WM.createWorm(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (show_worms)
        WM.createWorm(x, y);
    CM.checkClicks(x, y);

    // get new set of active tracks depending on the mouse click location
    vector<Track>::iterator it, end;

    for (it = tracks.begin(), end = tracks.end(); it != end; it++) {
        if (it->isClickInTrack(x, y) == true) {// && gui_tracks.getToggle("track " + to_string(it->trackno))) {
            active_tracks[it->trackno] = 1;
        } else {
            active_tracks[it->trackno] = 0;
        }
    }

}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
    cout << scrollX << endl;
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
ofVec2f ofApp::posToScaledPos(ofVec2f pt){
    return ofVec2f(pt.x / SCALE, pt.y / SCALE);
}

//--------------------------------------------------------------
ofVec2f ofApp::scaledPosToPos(ofVec2f pt){
    return ofVec2f(pt.x * SCALE, pt.y * SCALE);

}

//--------------------------------------------------------------
bool ofApp::isPointInRect(ofVec2f checkpt, ofVec2f pt, float w, float h){
    if (checkpt.x > pt.x && checkpt.x < pt.x + w && checkpt.y > pt.y && checkpt.y < pt.y + h) {
        return true;
    }
    return false;
}

//------------------------GUI-LISTENERS-------------------------
void ofApp::timeChanged(int &t){
    //newtime = t;
    //time = t;

    // TODO wth is the below code for anymore
    if (CM.current.comp.click_i_x != -1 && CM.current.comp.click_i_y != -1) {
    }
    //CM.setCurrentComponent(t);
    //WM.updateComponents(CM);
}

//--------------------------------------------------------------
void ofApp::changedEuler(bool &b){
    if (b == true) {
        WM.setMode(1);
        WM.refreshWorms(true);
    } else {
        WM.setMode(0);
        WM.worms.clear();
    }
}

void ofApp::changedOpaque(bool &b){
    WM.setOpaque(b);
}

//--------------------------------------------------------------
void ofApp::changedWormLength(int &length){
    WM.setWormTailSize(length);
}

//--------------------------------------------------------------
void ofApp::changedWormLifespan(int &lifespan){
    WM.setWormLifespan(lifespan);
}

//--------------------------------------------------------------
void ofApp::changedWormDensity(float &density){
    WM.setWormDensity(density);
}

//--------------------------------------------------------------
void ofApp::changedWormSize(int &size){
    WM.setWormSize(size);
}

//--------------------------------------------------------------
void ofApp::changedWormSpeed(int &speed){
    WM.setWormSpeed(speed);
}

//--------------------------------------------------------------
void ofApp::changedEulerArrows(bool &b){
    if (b == true) {
        WM.setArrowMode(1);
    } else {
        WM.setArrowMode(0);
    }
}

//--------------------------------------------------------------
void ofApp::changedUniformPlacement(bool &b){
    WM.setUniform(b);
}
