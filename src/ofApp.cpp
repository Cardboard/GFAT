#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();

    // setup byte array
    SAMPLES = 1007;
    LINES = 1741;
    SCALE = 1;
    WIDTH = SAMPLES / SCALE;
    HEIGHT = LINES / SCALE;
    position.set(0, 0);
    map_zoom = 1.f; // starting zoom for the 2D map
    map_mode = 2; // start in 2D mode (change to 3 to start in 3D mode)
    fullscreen = false; // start in fullscreen mode because we toggle fullscreen at the program's start

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
    WM.setup(&CM, "E", "N", position);
    WM.wormPreset("worms");

    // handle topography
    img_topo.load("rutford_stretched.png");
    img_topo.resize(WIDTH, HEIGHT);
    img_gdop.load("gdop_test.png");
    img_gdop.resize(WIDTH, HEIGHT);

    // 2d & 3d toggle buttons
    img_3d_button.load("3dview_button.png");
    button_3d.width = img_3d_button.getWidth();
    button_3d.height = img_3d_button.getHeight();
    img_2d_button.load("2dview_button.png");
    button_2d.width = img_2d_button.getWidth();
    button_2d.height = img_2d_button.getHeight();
    img_fullscreen.load("fullscreen_button.png");
    fullscreen_map.width = img_fullscreen.getWidth();
    fullscreen_map.height = img_fullscreen.getHeight();
    fullscreen_3d.width = img_fullscreen.getWidth();
    fullscreen_3d.height = img_fullscreen.getHeight();


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
    toggleFullscreen(&v3d);

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

    // mouse-related stuff
    float mx = ofGetMouseX();
    float my = ofGetMouseY();
    // 3D topography rotation
    if (isPointInRect(ofVec2f(mx, my), v3d.position, v3d.width, v3d.height)) {
        // point the camera at the 3d topography
        setCameraPosition();
    }

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

    // place the 2D & 3D toggle buttons in the map view
    button_2d.x = vMap.x + vMap.width - button_3d.width - button_2d.width;
    button_3d.x = vMap.x + vMap.width - button_3d.width;
    button_2d.y = vMap.y;
    button_3d.y = vMap.y;
    // place the fullscreen button for the map
    fullscreen_map.x = vMap.x + vMap.width - fullscreen_map.width;
    fullscreen_map.y = vMap.y + vMap.height - fullscreen_map.height;
    // place the fullscreen button for the 3d map
    fullscreen_3d.x = v3d.x + v3d.width - fullscreen_3d.width;
    fullscreen_3d.y = v3d.y + v3d.height - fullscreen_3d.height;

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
    gui->addBreak()->setHeight(10.0f);

    // worms -- gui elements
    gMapWorms = gui->addToggle("Worm Options", false);
    gui->addBreak()->setHeight(10.0f);

    // tracks -- gui elements
    gMapTracks = gui->addFolder("Tracks", ofColor::greenYellow);
    gMapTracks->collapse();
    gMapTracks->addButton("   Clear Selection");
    gMapTracks->addButton("   Select All Available Tracks");
    ofxDatGuiMatrix* tracks = gMapTracks->addMatrix("   Available Tracks", 56, true);
    tracks->setRadioMode(false);
    gui->addBreak()->setHeight(10.0f);

    // model -- gui elements
    gMapModel = gui->addFolder("Model", ofColor::fuchsia);
    gMapModel->collapse();
    gMapModel->addToggle("   Secular Velocity", true);
    gMapModel->addToggle("   Period 1", true);
    gMapModel->addToggle("   Period 2", true);
    gMapModel->addToggle("   Period 3", true);
    gMapModel->addToggle("   F(x)", false);

    // worms -- gui elements
    gOptions = new ofxDatGui(ofxDatGuiAnchor::BOTTOM_LEFT);
    gOptions->setVisible(false);
    gOptions->setAutoDraw(false);
    gOptions->addHeader(":: Drag Me To Reposition ::");
    // eulerian - lagrangian toggle
    ofxDatGuiToggle *euler_toggle = gOptions->addToggle("LaGrangian", false);
    // -- worm presets
    ofxDatGuiFolder *gWormPresets = gOptions->addFolder("Presets", ofColor::thistle);
    gWormPresets->collapse();
    gWormPresets->addButton("   Worms");
    gWormPresets->addButton("   Lines");
    gWormPresets->addButton("   Arrows");
    gWormPresets->addButton("   Dots");
    // -- individual worm options
    gOptions->addBreak()->setHeight(10.0f);
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

    // tracks -- events
    gMapTracks->onButtonEvent([&](ofxDatGuiButtonEvent e) {
    });
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
    // model -- events
    gMapModel->onToggleEvent([&](ofxDatGuiToggleEvent e) {
        if (e.target->is("   Secular Velocity")) {
            CM.secular_enabled = e.checked == true ? 1 : 0;
            // if we're in eulerian mode we need to refresh the worms
            // so that the worms' starting ENU values get reset
            if (WM.mode == 1) {
                WM.refreshWorms(true);
            }
        } else if (e.target->is("   Period 1")) {
            CM.p1_enabled = e.checked == true ? 1 : 0;
        } else if (e.target->is("   Period 2")) {
            CM.p2_enabled = e.checked == true ? 1 : 0;
        } else if (e.target->is("   Period 3")) {
            CM.p3_enabled = e.checked == true ? 1 : 0;
        }
    });
    gOptions->getToggle("   Arrows")->setVisible(false);

    // worms -- events
    euler_toggle->onToggleEvent([&](ofxDatGuiToggleEvent e) {
        if (e.checked == false) {
            //  disable certain eulerian-only options
            gOptions->getToggle("   Arrows")->setVisible(false);
            WM.setMode(0);
            WM.wormPreset("worms");
            // turn off euler arrows if they're turned on,
            // since this vis doesn't work with lagrangian motion
            if (WM.arrowmode == 1) {
                WM.setArrowMode(0);
            }
            WM.refreshWorms(true);
            // update the label to reflect the toggle's current state
            e.target->setLabel("LaGrangian");
        } else {
            // re-enable certain eulerian-only options
            gOptions->getToggle("   Arrows")->setVisible(true);
            WM.setMode(1);
            WM.wormPreset("worms");
            WM.refreshWorms(true);
            // update the label to reflect the toggle's current state
            e.target->setLabel("Eulerian");
        }
    });
    gWormPresets->onButtonEvent([&](ofxDatGuiButtonEvent e) {
        if (e.target->is("   Worms")) {
            WM.wormPreset("worms");
        } else if (e.target->is("   Lines")) {
            WM.wormPreset("lines");
        } else if (e.target->is("   Arrows")) {
            WM.wormPreset("arrows");
        } else if (e.target->is("   Dots")) {
            WM.wormPreset("dots");
        }
        // TODO update slider values to reflect the preset
    });
    gMapWorms->onToggleEvent([&](ofxDatGuiToggleEvent e) {
        gOptions->setPosition(ofxDatGuiAnchor::BOTTOM_LEFT);
        gOptions->setVisible(e.checked); // TODO move elsewhere
    });
    // worm options -- events
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
    for (int y = 0; y < h; y+=25) {
        for (int x = 0; x < w; x+=25) {
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
    setCameraPosition();
}

void ofApp::setCameraPosition(){
    ofVec2f mpos = restrictPosition(ofVec2f(ofGetMouseX(), ofGetMouseY()), 0, 0, v3d); // only move the 3d screen when we mouse over it
    float rotateAmount = ofMap(mpos.x, v3d.x, v3d.x + v3d.width, 0, 360);
    ofVec3f camDirection(0, 1, 1);
    ofVec3f centre(946/2.f, 1558.f * 3/4.f, 255/2.f);
    ofVec3f camDirectionRotated = camDirection.getRotated(rotateAmount, ofVec3f(0, 0, 1));
    ofVec3f camPosition = centre + camDirectionRotated * cam_zoom;

    cam.setPosition(camPosition);
    cam.lookAt(centre);
    cam.roll(rotateAmount);
}

//--------------------------------------------------------------
void ofApp::draw(){
    // ========= 2D WORMPLOT ========
    drawViewportOutline(vMap);
    ofPushView();
    ofViewport(vMap);
    ofSetupScreen();

    // <------
    // everything drawn between map_buffer.begin() and map_buffer.end() will be
    // drawn onto either the 2D view or on the 3D mesh, depending on which mode we're in.
    // the gui and buttons are thus drawn after the buffer drawing
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

    //CM.current.comp.overlay.draw(0, 0); // mask which slightly blocks where we don't have data

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
    map_buffer.draw(position.x, position.y, WIDTH * map_zoom, HEIGHT * map_zoom);
    topo3dtex = map_buffer.getTexture();

    // <------

    // draw guis
    gui->draw();
    gOptions->draw();
    // draw buttons
    //img_3d_button.draw(button_3d.position);
    //img_2d_button.draw(button_2d.position);
    img_fullscreen.draw(fullscreen_map);

    ofPopView(); // done drawing to 2d wormplit viewport

    // ========= 3D WORMPLOT ========

    drawViewportOutline(v3d);
    ofPushView();
    ofViewport(v3d);
    ofSetupScreen();
    //ofEnableDepthTest();
        cam.begin(v3d);
            //map_buffer.getTexture().bind();
            //topo3d.draw();
            //map_buffer.getTexture().unbind();
            // draw vertices (probably for testing)

            ofSetColor(ofColor::gray);
            //topo3d.drawWireframe();
            glPointSize(2);
            ofSetColor(ofColor::yellow);
            topo3d.drawVertices();

        cam.end();
        // more vertices drawing code
        int n = topo3d.getNumVertices();
        float nearestDistance = 0;
        ofVec2f nearestVertex;
        int nearestIndex = 0;
        ofVec2f mouse (mouseX - v3d.x, mouseY - v3d.y);
        for (int i=0; i<n; i++) {
            ofVec3f cur = cam.worldToScreen(topo3d.getVertex(i));
            float distance = cur.distance(mouse);
            if (i==0 || distance < nearestDistance) {
                nearestDistance = distance;
                nearestVertex = cur;
                nearestIndex = i;
            }
        }
        ofSetColor(ofColor::aquamarine);
        ofDrawLine(nearestVertex, mouse);

        ofNoFill;
        ofSetColor(ofColor::magenta);
        ofDrawCircle(nearestVertex, 4);
        ofVec2f offset(10, -10);
        ofDrawBitmapStringHighlight(ofToString(nearestIndex), mouse + offset);
        ofSetColor(ofColor::white);

    ofPopView();
    // TODO draw this inside the viewport drawing? (only visible when in fullscreen mode for v3d?...)
    img_fullscreen.draw(fullscreen_3d);

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

        position = restrictPosition(position, WIDTH * map_zoom, HEIGHT * map_zoom, vMap);
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
        WM.createWorm((x - position.x) / map_zoom, (y - position.y) / map_zoom);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    // left clicks
    if (button == 0) {
        if (show_worms)
            WM.createWorm((x - position.x) / map_zoom, (y - position.y) / map_zoom);

        // get new set of active tracks depending on the mouse click location
        vector<Track>::iterator it, end;

        // 2d/3d toggle buttons
        if (isPointInRect(ofVec2f(x, y), button_2d.position, button_2d.width, button_2d.height)) {
            map_mode = 2;
        } else if (isPointInRect(ofVec2f(x, y), button_3d.position, button_3d.width, button_3d.height)) {
            map_mode = 3;
        }

        // fullscreen toggle buttons
        if (isPointInRect(ofVec2f(x, y), fullscreen_map.position, fullscreen_map.width, fullscreen_map.height)) {
            toggleFullscreen(&vMap);
        } else if (isPointInRect(ofVec2f(x, y), fullscreen_3d.position, fullscreen_3d.width, fullscreen_3d.height)) {
            toggleFullscreen(&v3d);
        }

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
    float mx = ofGetMouseX();
    float my = ofGetMouseY();
    // 3D topography rotation
    if (isPointInRect(ofVec2f(mx, my), v3d.position, v3d.width, v3d.height)) {
        cam_zoom -= scrollY * 5.f;
    } else if (isPointInRect(ofVec2f(mx, my), vMap.position, vMap.width, vMap.height)) {
        map_zoom += scrollY * 0.01f;
        map_zoom = max(0.5f, min(map_zoom, 2.f)); // clamp map_zoom
    }
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

void ofApp::toggleFullscreen(ofRectangle *view){
    if (fullscreen == true) {
        setViewportSizes();
    } else {
        float w = ofGetWindowWidth();
        float h = ofGetWindowHeight();
        float timeslider_height = gTimeGui->getSlider("Time")->getHeight();

        // set all viewport sizes to zero before modifying the chosen viewport's properties
        // 2D/3D map
        vMap.x = 0;
        vMap.y = 0;
        vMap.width = 0;
        vMap.height = 0;
        v3d.x = 0;
        v3d.y = 0;
        v3d.width = 0;
        v3d.height = 0;

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

        // set the chosen viewport to fullscreen
        view->x = 0;
        view->y = 0;
        view->width = w;
        view->height = h - timeslider_height;

        // place the 2D & 3D toggle buttons in the map view
        button_2d.x = vMap.x + vMap.width - button_3d.width - button_2d.width;
        button_3d.x = vMap.x + vMap.width - button_3d.width;
        button_2d.y = vMap.y;
        button_3d.y = vMap.y;
        // place the fullscreen button for the map
        fullscreen_map.x = vMap.x + vMap.width - fullscreen_map.width;
        fullscreen_map.y = vMap.y + vMap.height - fullscreen_map.height;
        // place the fullscreen button for the 3d map
        fullscreen_3d.x = v3d.x + v3d.width - fullscreen_3d.width;
        fullscreen_3d.y = v3d.y + v3d.height - fullscreen_3d.height;

        // keep the guis in their respective viewports
        ofVec2f restricted_options_pos = restrictPosition(gOptions->getPosition(),\
                                                          gOptions->getWidth(), gOptions->getHeight(), vMap);
        gOptions->setPosition(restricted_options_pos.x, restricted_options_pos.y);
    }
    fullscreen = !fullscreen;

}
