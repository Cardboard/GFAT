#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();

    // setup byte array
    SAMPLES = 1007;
    LINES = 1741;
    SCALE = 1;
    WIDTH = SAMPLES / SCALE;
    HEIGHT = LINES / SCALE;
    position.set(0, 0);
    map_zoom = 0.f; // starting zoom for the 2D map

    // TODO replace with a Time Manager class thing
    time = new float;
    *time = 0.0; // starting time
    newtime = *time;
    paused = false;

    // setup component manager & components
    CM.setup(SAMPLES, LINES, WIDTH, HEIGHT, position.x, position.y);
    CM.addComponent("N", "components/north.utm");
    CM.addComponent("E", "components/east.utm");
    CM.addComponent("U", "components/up.utm");
    CM.addComponent("MAG", "components/mag.utm");
    CM.addComponent("HMAG", "components/hmag.utm");

    CM.addComponent("AMP1N", "components/sinamp1.north.utm");
    CM.addComponent("AMP1E", "components/sinamp1.east.utm");
    CM.addComponent("AMP1U", "components/sinamp1.up.utm");

    CM.addComponent("AMP2N", "components/sinamp2.north.utm");
    CM.addComponent("AMP2E", "components/sinamp2.east.utm");
    CM.addComponent("AMP2U", "components/sinamp2.up.utm");
    CM.addComponent("AMP3N", "components/sinamp3.north.utm");
    CM.addComponent("AMP3E", "components/sinamp3.east.utm");
    CM.addComponent("AMP3U", "components/sinamp3.up.utm");

    CM.addComponent("PHZ1N", "components/sinphz1.north.utm");
    CM.addComponent("PHZ1E", "components/sinphz1.east.utm");
    CM.addComponent("PHZ1U", "components/sinphz1.up.utm");

    CM.addComponent("PHZ2N", "components/sinphz2.north.utm");
    CM.addComponent("PHZ2E", "components/sinphz2.east.utm");
    CM.addComponent("PHZ2U", "components/sinphz2.up.utm");
    CM.addComponent("PHZ3N", "components/sinphz3.north.utm");
    CM.addComponent("PHZ3E", "components/sinphz3.east.utm");
    CM.addComponent("PHZ3U", "components/sinphz3.up.utm");

    // TODO what is this for; is it necessary
    CM.setCurrentComponent("MAG");

    // setup worm manager & set default vis
    // TODO WM.wormPreset("worms");
    WM.setup(&CM, "E", "N", position);
    WM.setWormDensity(0.5);
    WM.setWormLifespan(50);
    WM.setWormTailSize(100);
    WM.setWormSize(3);
    WM.setWormSpeed(30);
    WM.setUniform(false);
    WM.setOpaque(false);
    WM.setMode(0);
    WM.setArrowMode(0);
    //WM.worms.clear();

    // handle topography
    img_topo.load("rutford_stretched.png");
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

    img_temp_history.load("selection_history-01.png");
    img_temp_modelspace.load("model_space-01.png");
    img_temp_pairspace.load("data_space-01.png");
}

//--------------------------------------------------------------
void ofApp::update(){
    // update the guis
    gui->update();
    gOptions->update();
    // keep the guis in their respective viewports
    ofVec2f restricted_options_pos = restrictPosition(gOptions->getPosition(),\
                                                      gOptions->getWidth(), gOptions->getHeight(), vMap);
    gOptions->setPosition(restricted_options_pos.x, restricted_options_pos.y);

    // update calculated components

    // update current time

    // update the current component

    // update the worms components
    if (!paused) {
        *time += 1.0;
        if (*time > 24.0 * 45) *time = 0.0;
    }

    if (show_worms)
        WM.updateWorms(*time);

    // point the camera at the 3d topography
    ofVec2f mpos = restrictPosition(ofVec2f(ofGetMouseX(), ofGetMouseY()), 0, 0, v3d); // only move the 3d screen when we mouse over it
    float rotateAmount = ofMap(mpos.x, v3d.x, v3d.x + v3d.width, 0, 180);
    ofVec3f camDirection(0, 1, 1);
    ofVec3f centre(946/2.f, 1558/2.f, 255/2.f);
    ofVec3f camDirectionRotated = camDirection.getRotated(-rotateAmount, ofVec3f(0, 0, -1));
    ofVec3f camPosition = centre + camDirectionRotated * cam_zoom;

    cam.setPosition(camPosition);
    cam.lookAt(centre);
    cam.roll(rotateAmount);

}

//--------------------------------------------------------------
void ofApp::setViewportSizes(){
    float w = ofGetWindowWidth();
    float h = ofGetWindowHeight();
    float timeslider_height = gTimeGui->getSlider("Time")->getHeight();

    // 2D/3D map
    vMap.x = 0;
    vMap.y = 0;
    vMap.width = w * 1/2;
    vMap.height = h - timeslider_height;
    v3d.x = w * 1/2;
    v3d.y = 0;
    v3d.width = w * 1/2;
    v3d.height = h - timeslider_height;

    // pairspace
    vPairspace.x = 0;
    vPairspace.y = 0;
    vPairspace.width = 0;
    vPairspace.height = 0;
    // selection history
    vHistory.x = 0;
    vHistory.y = 0;
    vHistory.width = 0;
    vHistory.height = 0;
    // modelspace
    vModel.x = 0;
    vModel.y = 0;
    vModel.width = 0;
    vModel.height = 0;


    // debugging 3d viewport
    /*
    vMap.x = 0;
    vMap.y = 0;
    vMap.width = 0;
    vMap.height = 0;
    v3d.x = 0;
    v3d.y = 0;
    v3d.width = w;
    v3d.height = h;
    */

    // keep the guis in their respective viewports
    ofVec2f restricted_options_pos = restrictPosition(gOptions->getPosition(),\
                                                      gOptions->getWidth(), gOptions->getHeight(), vMap);
    gOptions->setPosition(restricted_options_pos.x, restricted_options_pos.y);
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    // GUI
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
    gui->setAutoDraw(false);
    gui->addFRM(1.0f);
    gTimeGui = new ofxDatGui(ofxDatGuiAnchor::BOTTOM_LEFT);
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
    gMapWorms = gui->addToggle("Worm Options", false);
    gMapWorms->onToggleEvent([&](ofxDatGuiToggleEvent e) {
        gOptions->setVisible(e.checked); // TODO move elsewhere
    });

    /*
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
            WM.setWormSize(2);
            WM.setWormSpeed(60);
            WM.setOpaque(false);
            WM.setArrowMode(0);
            WM.setWormDensity(1.0);
        } else if (e.target->is("   Show future while paused")) {
            // TODO implement these kind of worms
        }
    });
    */
    gui->addBreak()->setHeight(10.0f);

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
    gOptions->setVisible(false);
    gOptions->setAutoDraw(false);
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

//--------------------------------------------------------------
void ofApp::setup3dTopo(){
    cam_zoom = 1000.0;
    extrusionAmount = 1.0;
    ofImage heightmap;
    heightmap.load("rutford_stretched.png");
    // texture
    map_buffer.allocate(WIDTH, HEIGHT, GL_RGB);
    ofImage topo3dteximg;
    topo3dteximg.load("rutford_stretched_tex.png");
    topo3dtex = topo3dteximg.getTexture();
    //topo3dtex.setTextureWrap(GL_REPEAT, GL_REPEAT);
    float h = heightmap.getHeight();
    float w = heightmap.getWidth();
    float px_height, px_color;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            px_height = max(0.0f, heightmap.getColor(x, y).getBrightness());
            px_color = max(100.f, 255.f - 8 * px_height);
            topo3d.addVertex(ofPoint(x, y, px_height * extrusionAmount));
            //topo3d.addColor(ofColor(255.f));
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

    // <------
    map_buffer.begin();

    if (show_heightmap) {
        img_topo.draw(0, 0);
    } else {
        ofSetColor(225);
        ofDrawRectangle(vMap.x, vMap.y, WIDTH, HEIGHT);
        ofSetColor(255, 255, 255);
    }

    if (show_gdop)
        // TODO delete magic no.
        img_gdop.draw(0, 0);

    CM.current.comp.overlay.draw(0, 0); // mask which slightly blocks where we don't have data

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
    map_buffer.end();
    map_buffer.draw(position.x, position.y);
    topo3dtex = map_buffer.getTexture();

    // <------

    gui->draw();
    gOptions->draw();

    ofPopView(); // done drawing to 2d wormplit viewport

    // ========= 3D WORMPLOT ========

    drawViewportOutline(v3d);
    ofPushView();
    ofViewport(v3d);
    ofSetupScreen();
    //ofEnableDepthTest();
    cam.begin(v3d);
        map_buffer.getTexture().bind();
        topo3d.draw();
        map_buffer.getTexture().unbind();
    cam.end();
    ofPopView();

    // ========= PAIRSPACE ========
    drawViewportOutline(vPairspace);
    ofPushView();
    ofViewport(vPairspace);
    ofSetupScreen();
    // do any drawing for the pairspace below
    img_temp_pairspace.draw(0, 0, vPairspace.getWidth(), vPairspace.getHeight());
    ofPopView();
    // ========= SELECTION HISTORY ========
    drawViewportOutline(vHistory);
    ofPushView();
    ofViewport(vHistory);
    ofSetupScreen();
    // do any drawing for the pairspace below
    img_temp_history.draw(0, 0, vHistory.getWidth(), vHistory.getHeight());

    ofPopView();
    // ========= MODELSPACE ========
    drawViewportOutline(vModel);
    ofPushView();
    ofViewport(vModel);
    ofSetupScreen();
    // do any drawing for the pairspace below
    img_temp_modelspace.draw(0, 0, vModel.getWidth(), vModel.getHeight());

    ofPopView();
}

void ofApp::drawViewportOutline(const ofRectangle & viewport){
    ofPushStyle();
    ofFill();
    ofSetColor(0);
    ofSetLineWidth(0);
    ofDrawRectangle(viewport);
    ofNoFill();
    ofSetColor(40);
    ofSetLineWidth(3.0f);
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

        position = restrictPosition(position, WIDTH, HEIGHT, vMap);
        //CM.setPos(position);
        //WM.pos.set(position);

        // move tracks
        vector<Track>::iterator it, end;
        for (it = tracks.begin(), end = tracks.end(); it != end; it++) {
            // we only care about updating the position of tracks relevant to the current selection
            if (active_tracks[it->trackno] == 1)
                it->setPos(position);
        }
    // drag to create a line or whatever of worms
    } else if (button == 0 && isPointInRect(pt, ofVec2f(0, 0), vMap.width, vMap.height)) {
        WM.createWorm(x - position.x, y - position.y);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (button == 0) {
        if (show_worms)
            WM.createWorm(x - position.x, y - position.y);
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
    cam_zoom -= scrollY * 5.f;
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

    // make sure the map is within the newly-resized viewport
    position = restrictPosition(position, WIDTH, HEIGHT, vMap);
    //CM.setPos(position);
    //WM.pos.set(position);
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

ofVec2f ofApp::restrictPosition(ofVec2f pos, float obj_w, float obj_h, ofRectangle view){
    // restrain the map to the viewport's edges
    if (obj_w > view.width) {
        if (pos.x > view.x) pos.x = view.x;
        if (pos.x + obj_w < view.x + view.width) pos.x = view.width - obj_w;
    } else {
        // map is smaller than the viewport so keep it within the bounds now
        if (pos.x < view.x) pos.x = view.x;
        if (pos.x + obj_w > view.x + view.width) pos.x = view.width - obj_w;
    }
    if (obj_h > view.height) {
        if (pos.y > view.y) pos.y = view.y;
        if (pos.y + obj_h < view.y + view.height) pos.y = view.height - obj_h;
    } else {
        // map is smaller than the viewport so keep it within the bounds now
        if (pos.y < view.y) pos.y = view.y;
        if (pos.y + obj_h > view.y + view.height) pos.y = view.height - obj_h;
    }
    return pos;
}
