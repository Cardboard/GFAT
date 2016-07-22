#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();

    // setup byte array
    SAMPLES = 3686;
    LINES = 1520;
    SCALE = 2;
    WIDTH = SAMPLES / SCALE;
    HEIGHT = LINES / SCALE;
    position.set(0, 0);

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

    // setup worm manager & set default vis
    WM.setup(CM, "E", "N", position);
    WM.setWormDensity(0.5);
    WM.setWormLifespan(0);
    WM.setWormTailSize(100);
    WM.setWormSize(3);
    WM.setWormSpeed(30);
    WM.setUniform(false);
    WM.setOpaque(false);
    WM.setMode(0);
    WM.setArrowMode(0);
    //WM.worms.clear();

    // handle topography
    img_topo.load("shaded_height.png");
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
    setup3dTopo();

    // temporary(?) viewport stuff
    setViewportSizes();
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

    // point the camera at the 3d topography
    float rotateAmount = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 360);
    ofVec3f camDirection(0, 1, 1);
    ofVec3f centre(946/2.f, 1558/2.f, 255/2.f);
    ofVec3f camDirectionRotated = camDirection.getRotated(rotateAmount, ofVec3f(0, 0, 1));
    ofVec3f camPosition = centre + camDirectionRotated * 1000.0;

    cam.setPosition(camPosition);
    cam.lookAt(centre);
    cam.roll(rotateAmount);

}

void ofApp::setViewportSizes(){
    float w = ofGetWindowWidth();
    float h = ofGetWindowHeight();
    /*
    vMap.x = 0;
    vMap.y = 0;
    vMap.width = w * 2/3;
    vMap.height = h / 2;
    v3d.x = w * 2/3;
    v3d.y = 0;
    v3d.width = w * 1/3;
    v3d.height = h / 2;
    */
    // debugging 3d viewport
    vMap.x = 0;
    vMap.y = 0;
    vMap.width = 0;
    vMap.height = 0;
    v3d.x = 0;
    v3d.y = 0;
    v3d.width = w;
    v3d.height = h;
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    // GUI
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
    gui->addFRM(1.0f);
    ofxDatGui* gTimeGui = new ofxDatGui(ofxDatGuiAnchor::BOTTOM_LEFT);
    timeslider = gTimeGui->addSlider("Time", 0, 24*45);
    timeslider->bind(*time);
    timeslider->setWidth(ofGetWindowWidth(), 0.05);
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
    gOptions = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
    gOptions->addHeader(":: Drag Me To Reposition ::");
    ofxDatGuiSlider* wormsize = gOptions->addSlider("   Size", 1, 10);
    wormsize->setPrecision(0);
    wormsize->setValue(WM.worm_size);
    ofxDatGuiSlider* wormdensity = gOptions->addSlider("   Density", 0.0f, 1.0f);
    wormdensity->setValue(WM.worm_density);
    // weird behavior occurs when tail length is 1 for some reason
    ofxDatGuiSlider* wormlength = gOptions->addSlider("   Tail Length", 2, 500);
    wormlength->setPrecision(0);
    wormlength->setValue(WM.worm_tailsize);
    ofxDatGuiSlider* wormlifespan = gOptions->addSlider("   Lifespan", 50, 500);
    wormlifespan->setPrecision(0);
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

void ofApp::setup3dTopo(){
    extrusionAmount = 1.0;
    ofImage heightmap;
    heightmap.load("rutford_surface.png");
    // texture
    ofImage topo3dteximg;
    topo3dteximg.load("rutford_tex.png");
    topo3dtex = topo3dteximg.getTexture();
    //topo3dtex.setTextureWrap(GL_REPEAT, GL_REPEAT);
    float h = heightmap.getHeight();
    float w = heightmap.getWidth();
    float px_height, px_color;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            px_height = max(0.0f, heightmap.getColor(x, y).getBrightness());
            px_color = min(255.f, 4 * px_height);
            topo3d.addVertex(ofPoint(x, y, px_height * extrusionAmount));
            topo3d.addColor(ofColor(px_color));
            topo3d.addTexCoord(ofVec2f(x, y));
        }
    }
    for (int y = 0; y < h-1; y++){
        for (int x=0; x < w-1; x++){
            topo3d.addIndex(x+y*w);               // 0
            topo3d.addIndex((x+1)+y*w);           // 1
            topo3d.addIndex(x+(y+1)*w);           // 10

            topo3d.addIndex((x+1)+y*w);           // 1
            topo3d.addIndex((x+1)+(y+1)*w);       // 11
            topo3d.addIndex(x+(y+1)*w);           // 10
        }
    }

    cam.setScale(1, -1, 1);
}

//--------------------------------------------------------------
void ofApp::draw(){
    // ========= 2D WORMPLOT ========
    drawViewportOutline(vMap);
    ofPushView();
    ofViewport(vMap);
    ofSetupScreen();

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
    ofPopView(); // done drawing to 2d wormplit viewport

    // ========= 3D WORMPLOT ========
    ofEnableDepthTest();
    cam.begin(v3d);
        topo3dtex.bind();
        topo3d.draw();
        topo3dtex.unbind();
    cam.end();
}

void ofApp::drawViewportOutline(const ofRectangle & viewport){
    ofPushStyle();
    ofFill();
    ofSetColor(30);
    ofSetLineWidth(0);
    ofDrawRectangle(viewport);
    ofNoFill();
    ofSetColor(20);
    ofSetLineWidth(1.0f);
    ofDrawRectangle(viewport);
    ofPopStyle();
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
    if (button == 2 && isPointInRect(pt, vMap.position, vMap.width, vMap.height)) {
        float m_dx = ofGetMouseX() - ofGetPreviousMouseX();
        float m_dy = ofGetMouseY() - ofGetPreviousMouseY();
        position.x += m_dx;
        position.y += m_dy;
        // restrain the map to the viewport's edges
        if (WIDTH > vMap.width) {
            if (position.x > vMap.x) position.x = vMap.x;
            if (position.x + WIDTH < vMap.width) position.x = vMap.width - WIDTH;
        } else {
            // map is smaller than the viewport so keep it within the bounds now
            if (position.x < vMap.x) position.x = vMap.x;
            if (position.x + WIDTH > vMap.width) position.x = vMap.width - WIDTH;
        }
        if (HEIGHT > vMap.height) {
            if (position.y > vMap.y) position.y = vMap.y;
            if (position.y + HEIGHT < vMap.height) position.y = vMap.height - HEIGHT;
        } else {
            // map is smaller than the viewport so keep it within the bounds now
            if (position.y < vMap.y) position.y = vMap.y;
            if (position.y + HEIGHT > vMap.height) position.y = vMap.height - HEIGHT;
        }
        CM.setPos(position);
        WM.pos.set(position);

        // move tracks
        vector<Track>::iterator it, end;
        for (it = tracks.begin(), end = tracks.end(); it != end; it++) {
            // we only care about updating the position of tracks relevant to the current selection
            if (active_tracks[it->trackno] == 1)
                it->setPos(position);
        }
    // drag to create a line or whatever of worms
    } else if (button == 0 && isPointInRect(pt, vMap.position, vMap.width, vMap.height)) {
        WM.createWorm(x - vMap.x, y - vMap.y);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (button == 0) {
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

}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
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
    // resize the global timeline
    timeslider->setWidth(ofGetWindowWidth(), 0.05);
    // resize all of the viewports
    setViewportSizes();
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
