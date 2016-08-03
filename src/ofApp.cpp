#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(false);
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();

    // setup byte array
    SAMPLES = 1027;
    LINES = 1820;
    SCALE = 1;
    WIDTH = SAMPLES / SCALE;
    HEIGHT = LINES / SCALE;
    map_zoom = 0.5f; // starting zoom for the 2D map
    position.set(0, 0);
    map_mode = 2; // start in 2D mode (change to 3 to start in 3D mode)
    mouse_mode = 0; // start in "worm drawing" mode (1 => selection mode)
    fullscreen = false;
    hide_all = false;
    show_tracks = false;
    is_selection = false;
    is_selection2 = false;
    show_gdop = 2; // 0: gdop, 1: obscov, *: none
    show_topo = 0; // 0: surface, 1: bed, 2: thickness, *: none
    show_contour = false;
    selection_pos.set(-1,-1);
    selection2_pos.set(-1,-1);

    font.load("fonts/AkzidenzGrotesk-LightSC.otf", 8);

    // TODO replace with a Time Manager class thing
    time = new float;
    *time = 0.0; // starting time
    newtime = *time;
    paused = false;
    timescale = 50.f;

    // setup component manager & components
    CM.setup(SAMPLES, LINES, WIDTH, HEIGHT, 0, 0);
    CM.addComponent("N", "data/model/north.utm");
    CM.addComponent("E", "data/model/east.utm");
    CM.addComponent("U", "data/model/up.utm");
    CM.addComponent("MAG", "data/model/mag.utm");
    CM.addComponent("HMAG", "data/model/hmag.utm");

    CM.addComponent("AMP1N", "data/model/sinamp1.north.utm");
    CM.addComponent("AMP1E", "data/model/sinamp1.east.utm");
    CM.addComponent("AMP1U", "data/model/sinamp1.up.utm");

    CM.addComponent("AMP2N", "data/model/sinamp2.north.utm");
    CM.addComponent("AMP2E", "data/model/sinamp2.east.utm");
    CM.addComponent("AMP2U", "data/model/sinamp2.up.utm");
    CM.addComponent("AMP3N", "data/model/sinamp3.north.utm");
    CM.addComponent("AMP3E", "data/model/sinamp3.east.utm");
    CM.addComponent("AMP3U", "data/model/sinamp3.up.utm");

    CM.addComponent("PHZ1N", "data/model/sinphz1.north.utm");
    CM.addComponent("PHZ1E", "data/model/sinphz1.east.utm");
    CM.addComponent("PHZ1U", "data/model/sinphz1.up.utm");

    CM.addComponent("PHZ2N", "data/model/sinphz2.north.utm");
    CM.addComponent("PHZ2E", "data/model/sinphz2.east.utm");
    CM.addComponent("PHZ2U", "data/model/sinphz2.up.utm");
    CM.addComponent("PHZ3N", "data/model/sinphz3.north.utm");
    CM.addComponent("PHZ3E", "data/model/sinphz3.east.utm");
    CM.addComponent("PHZ3U", "data/model/sinphz3.up.utm");

    // TODO what is this for; is it necessary
    CM.setCurrentComponent("MAG");

    // setup worm manager & set default vis
    WM.setup(&CM, "E", "N", position);
    WM.wormPreset("worms");

    // handle topography
    img_topo_bed.load("bedmap2_bed_rutford.png");
    img_topo_surface.load("bedmap2_surface_rutford.png");
    img_topo_thickness.load("bedmap2_thickness_rutford.png");
    //img_topo.resize(WIDTH, HEIGHT);
    img_gdop.load("gdop.png");
    img_obscov.load("obscov.dop.png");

    //img_gdop.resize(WIDTH, HEIGHT);

    Date testdate1(2013, 8, 28);
    Date testdate2(2013, 8, 24);
    Date testdate3(2013, 9, 7);
    track1.setup("placeholder.bin", SCALE, 1, 1, position, \
                 ofVec2f(500, 500), ofVec2f(750, 520), ofVec2f(800, 700), ofVec2f(600, 520));
    track1.addDataPoint(testdate1, 0);
    track1.addDataPoint(testdate2, 100);
    track1.addDataPoint(testdate3, -10);
    track2.setup("placeholder.bin", SCALE, 2, 1, position, \
                 ofVec2f(200, 200), ofVec2f(200, 400), ofVec2f(300, 500), ofVec2f(500, 700));
    track2.addDataPoint(testdate1, 0);
    track2.addDataPoint(testdate2, 100);
    track2.addDataPoint(testdate3, -10);
    track3.setup("placeholder.bin", SCALE, 3, -1, position, \
                 ofVec2f(250, 150), ofVec2f(150, 400), ofVec2f(200, 700), ofVec2f(150, 600));
    track3.addDataPoint(testdate1, 0);
    track3.addDataPoint(testdate2, 100);
    track3.addDataPoint(testdate3, -10);

    tracks.push_back(track1);
    tracks.push_back(track2);
    tracks.push_back(track3);

    active_tracks.resize(tracks.size()+1, 0);

    setup3dTopo();

    // temporary(?) viewport stuff
    setupGui();
    setViewportSizes();

    img_temp_history.load("selection_history-01.png");
    img_temp_pairspace.load("data_space-01.png");

    // setup plots
    plotENU = ENUPlot(&CM, &selection_pos, &selection2_pos, time, "ENU", "Time", "Disp");

    ofSetFrameRate(30);
}

//--------------------------------------------------------------
void ofApp::update(){
    // update calculated components

    // update current time

    // update the current component

    // update the worms components
    if (!paused) {
        *time = ofGetElapsedTimef() * timescale;
        if (*time > 500.f) {
            *time = 0.0;
            ofResetElapsedTimeCounter();
        }
    }

    if (show_worms)
        WM.updateWorms(*time);

    // mouse-related stuff
    float mx = ofGetMouseX();
    float my = ofGetMouseY();
    // 3D topography rotation
    if (isPointInRect(ofVec2f(mx, my), vMap.position, vMap.width, vMap.height)) {
        // point the camera at the 3d topography
        setCameraPosition();
    }

}

//--------------------------------------------------------------
void ofApp::setViewportSizes(){
    float w = ofGetWindowWidth();
    float h = ofGetWindowHeight();
    float timeslider_height = gTimeGui->getSlider("Time")->getHeight();

    // constant view sizes regardless of whether there's a selection or not
    // 2D/3D map
    vMap.x = 0;
    vMap.y = 0;
    vMap.width = w * 1/2;
    vMap.height = h - timeslider_height;

    // modelspace
    vModel.x = w * 1/2;
    vModel.y = 0;
    vModel.width = w * 1/2;
    vModel.height = h * 2/3;
    // pairspace
    vPairspace.x = w * 1/2;
    vPairspace.y = h * 2/3;
    vPairspace.width = w * 1/2;
    vPairspace.height = h * 1/3 - timeslider_height;
    // selection history
    vHistory.x = 0;
    vHistory.y = 0;
    vHistory.width = 0;
    vHistory.height = 0;

    /*
    if (!is_selection) {
        // 2D/3D map
        vMap.x = 0;
        vMap.y = 0;
        vMap.width = w * 3/4;
        vMap.height = h - timeslider_height;

        // pairspace
        vPairspace.x = 0;
        vPairspace.y = 0;
        vPairspace.width = 0;
        vPairspace.height = 0;
        // selection history
        vHistory.x = w * 3/4;
        vHistory.y = 0;
        vHistory.width = w * 1/4;
        vHistory.height = h - timeslider_height;
        // modelspace
        vModel.x = 0;
        vModel.y = 0;
        vModel.width = 0;
        vModel.height = 0;

    // draw multiple views once a selection has been made
    } else {
        // 2D/3D map
        vMap.x = 0;
        vMap.y = 0;
        vMap.width = w * 1/3;
        vMap.height = h - timeslider_height;

        // pairspace
        vPairspace.x = w * 1/3;
        vPairspace.y = h * 1/2;
        vPairspace.width = w * 2/3;
        vPairspace.height = h * 1/2;
        // selection history
        vHistory.x = 0;
        vHistory.y = 0;
        vHistory.width = 0;
        vHistory.height = 0;
        // modelspace
        vModel.x = w * 1/3;
        vModel.y = 0;
        vModel.width = w * 2/3;
        vModel.height = h * 1/2;
    }
    */

    // we need to update the button positions now that the viewports have changed sizes
    setupButtons();
    // update the plots only if there's an existing selection
    //if (is_selection)
        setupPlots();

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
    timeslider = gTimeGui->addSlider("Time", 0, 500.f);
    timeslider->bind(*time);
    timeslider->setWidth(ofGetWindowWidth(), 0.05);
    timeslider->setStripeVisible(false);
    gTimeGui->setTheme(new ofxDatGuiThemeCharcoal());

    // layers -- gui elements
    gMapLayers = gui->addFolder("Layers", ofColor::aliceBlue);
    gMapLayers->collapse();
    gMapLayers->addToggle("   Tracks", true);
    gMapLayers->addToggle("   LOS Displacement", false);
    gMapLayers->addToggle("   Azimuth Displacement", false);
    show_worms = true;
    gMapLayers->addToggle("   Worms", show_worms);
    gMapLayers->addToggle("   Horizontal Model Disp", false);
    gMapLayers->addToggle("   Model Disp", false);
    gMapLayers->addToggle("   GDOP", false);
    show_topo = 0; // 0: surface, 1: bed, 2: thickness
    gMapLayers->addToggle("   Heightmap", show_topo);
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
    gMapModel->addToggle("   V(t)", true);
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
void ofApp::setupPlots(){
    plotENU.setRects(ofRectangle(0, 0, vModel.width, vModel.height));
}

//--------------------------------------------------------------
void ofApp::setupButtons(){
    // fullscreen toggle buttons
    btn_fullscreen_map.is_image = true;
    btn_fullscreen_model.is_image = true;

    setupButton(&btn_fullscreen_map, "icons/fullscreen_button.png", &vMap, BOTTOM_RIGHT, true, -5, -5);
    setupButton(&btn_fullscreen_model, "icons/fullscreen_button.png", &vModel, BOTTOM_RIGHT, true, -5, -5);

    // 2d/3d toggle buttons
    btn_3d.is_image = true;
    btn_2d.is_image = true;
    setupButton(&btn_3d, "icons/3dview_button.png", &btn_fullscreen_map.rect, LEFT, false, -3, -2);
    setupButton(&btn_2d, "icons/2dview_button.png", &btn_3d.rect, LEFT, false, 0, 0);

    // model buttons
    setupButton(&btn_model, "icons/model.png", &vMap, TOP_LEFT, true, 20, 20);
    btn_model.is_label = true;
    btn_model.is_image = true;
    setupButton(&btn_model_vel, "v(t)", &btn_model.rect, RIGHT, false, 6, 2);
    setupButton(&btn_model_p1, "P1", &btn_model_vel.rect, RIGHT, false, 6, 0);
    setupButton(&btn_model_p2, "P2", &btn_model_p1.rect, RIGHT, false, 6, 0);
    setupButton(&btn_model_p3, "P3", &btn_model_p2.rect, RIGHT, false, 6, 0);

    // layer buttons
    setupButton(&btn_layers, "icons/layers.png", &btn_model.rect, BOTTOM, false, 0, 3);
    btn_layers.is_label = true;
    btn_layers.is_image = true;
    //setupButton(&btn_layers_none, "None", &btn_layers.rect, RIGHT, false, 6, 2);
    setupButton(&btn_layers_surface, "Surface", &btn_layers.rect, RIGHT, false, 6, 0);
    setupButton(&btn_layers_divider1, "/", &btn_layers_surface.rect, RIGHT, false, 1, 0);
    setupButton(&btn_layers_bed, "Bed", &btn_layers_divider1.rect, RIGHT, false, 1, 0);
    setupButton(&btn_layers_divider2, "/", &btn_layers_bed.rect, RIGHT, false, 1, 0);
    setupButton(&btn_layers_thickness, "Thickness", &btn_layers_divider2.rect, RIGHT, false, 1, 0);
    setupButton(&btn_layers_gdop, "GDOP", &btn_layers_thickness.rect, RIGHT, false, 6, 0);
    setupButton(&btn_layers_divider3, "/", &btn_layers_gdop.rect, RIGHT, false, 1, 0);
    setupButton(&btn_layers_obscov, "OBSCOV", &btn_layers_divider3.rect, RIGHT, false, 1, 0);

    // mode buttons
    setupButton(&btn_mode, "icons/mode.png", &btn_layers.rect, BOTTOM, false, 0, 3);
    btn_mode.is_label = true;
    btn_mode.is_image = true;
    setupButton(&btn_mode_drawing, "Draw", &btn_mode.rect, RIGHT, false, 6, 2);
    setupButton(&btn_mode_selection, "Select", &btn_mode_drawing.rect, RIGHT, false, 6, 0);

    // flow buttons
    setupButton(&btn_flow, "icons/worms.png", &btn_mode.rect, BOTTOM, false, 0, 3);
    btn_flow.is_label = true;
    btn_flow.is_image = true;
    setupButton(&btn_flow_worms, "Worms", &btn_flow.rect, RIGHT, false, 6, 2);
    setupButton(&btn_flow_lines, "Lines", &btn_flow_worms.rect, RIGHT, false, 6, 0);
    setupButton(&btn_flow_dots, "Dots", &btn_flow_lines.rect, RIGHT, false, 6, 0);
    setupButton(&btn_flow_disp, "Disp", &btn_flow_dots.rect, RIGHT, false, 6, 0);

    // options buttons
    setupButton(&btn_options, "icons/settings.png", &btn_flow.rect, BOTTOM, false, 0, 3);
    btn_options.is_label = true;
    btn_options.is_image = true;
    setupButton(&btn_options_lag, "LaGrangian", &btn_options.rect, RIGHT, false, 6, 2);
    setupButton(&btn_options_eul, "Eulerian", &btn_options.rect, RIGHT, false, 6, 2);
    setupButton(&btn_options_small, "Small", &btn_options_lag.rect, RIGHT, false, 12, 0);
    setupButton(&btn_options_medium, "Medium", &btn_options_lag.rect, RIGHT, false, 6, 0);
    setupButton(&btn_options_large, "Large", &btn_options_lag.rect, RIGHT, false, 12, 0);
    setupButton(&btn_options_density_low, "Sparse", &btn_options_medium.rect, RIGHT, false, 6, 0);
    setupButton(&btn_options_density_med, "Dense", &btn_options_medium.rect, RIGHT, false, 6, 0);
    setupButton(&btn_options_density_high, "Packed", &btn_options_medium.rect, RIGHT, false, 6, 0);
    setupButton(&btn_options_EN, "EN", &btn_options_density_high.rect, RIGHT, false, 8, 0);
    setupButton(&btn_options_ENU, "ENU", &btn_options_density_high.rect, RIGHT, false, 8, 0);
}

//--------------------------------------------------------------
void ofApp::setupButton(Button *btn, string text, ofRectangle *ref, direction dir, bool is_viewport, int padding_x, int padding_y){

    // set the button's width and height based on the image's width and height if it is an image-based button
    if (btn->is_image) {
        if (!btn->img.isAllocated())
            btn->img.load(text);
        btn->rect.width = btn->img.getWidth();
        btn->rect.height = btn->img.getHeight();
    // otherwise, set the width and height based on the text's width & height
    } else {
        btn->text = text;
        btn->rect.width = font.stringWidth(text);
        btn->rect.height = font.stringHeight("!");
    }
    // place the button according to the given direction, relative to a viewport
    if (is_viewport) {
        switch(dir) {
            // anchor the button to one of the view's sides or corners
            case LEFT:
                btn->rect.x = ref->x + padding_x;
                btn->rect.y = ref->y + ref->height + padding_y;
                break;
            case TOP_LEFT:
                btn->rect.x = ref->x + padding_x;
                btn->rect.y = ref->y + padding_y;
                break;
            case TOP:
                btn->rect.x = ref->x + ref->width/2 - btn->rect.width/2 + padding_x;
                btn->rect.y = ref->y + padding_y;
                break;
            case TOP_RIGHT:
                btn->rect.x = ref->x + ref->width - btn->rect.width + padding_x;
                btn->rect.y = ref->y + padding_y;
                break;
            case RIGHT:
                btn->rect.x = ref->x + ref->width - btn->rect.width + padding_x;
                btn->rect.y = ref->y + ref->height/2 + btn->rect.height/2 + padding_y;
                break;
            case BOTTOM_RIGHT:
                btn->rect.x = ref->x + ref->width - btn->rect.width + padding_x;
                btn->rect.y = ref->y + ref->height - btn->rect.height + padding_y;
                break;
            case BOTTOM:
                btn->rect.x = ref->x + ref->width/2 - btn->rect.width/2 + padding_x;
                btn->rect.y = ref->y + ref->height - btn->rect.height + padding_y;
                break;
            case BOTTOM_LEFT:
                btn->rect.x = ref->x + padding_x;
                btn->rect.y = ref->y + ref->height - btn->rect.height + padding_y;
                break;
        }
    // place the button according to the given direction, relative to another button (or rectangle)
    } else {
        switch(dir) {
            // unlike with the viewport, we place the button in the specified direction of the reference rectangle,
            // rather than in a viewport's corner
            case LEFT:
                btn->rect.x = ref->x - btn->rect.width + padding_x;
                btn->rect.y = ref->y + padding_y;
                break;
            case TOP_LEFT:
                btn->rect.x = ref->x - btn->rect.width + padding_x;
                btn->rect.y = ref->y - btn->rect.height + padding_y;
                break;
            case TOP:
                btn->rect.x = ref->x + padding_x;
                btn->rect.y = ref->y - btn->rect.height + padding_y;
                break;
            case TOP_RIGHT:
                btn->rect.x = ref->x + ref->width + padding_x;
                btn->rect.y = ref->y - btn->rect.height + padding_y;
                break;
            case RIGHT:
                btn->rect.x = ref->x + ref->width + padding_x;
                btn->rect.y = ref->y + padding_y;
                break;
            case BOTTOM_RIGHT:
                btn->rect.x = ref->x + btn->rect.width + padding_x;
                btn->rect.y = ref->y + btn->rect.height + padding_y;
                break;
            case BOTTOM:
                btn->rect.x = ref->x + padding_x;
                btn->rect.y = ref->y + ref->height + padding_y;
                break;
            case BOTTOM_LEFT:
                btn->rect.x = ref->x - btn->rect.width + padding_x;
                btn->rect.y = ref->y + ref->height + padding_y;
                break;
        }
    }
    // we neet to round the pixel locations because drawing at a float (x, y) causes the image
    // to be distorted
    btn->rect.x = (int) btn->rect.x;
    btn->rect.y = (int) btn->rect.y;
}

//--------------------------------------------------------------
void ofApp::setup3dTopo(){
    position_3d = ofVec2f(946/2.f, 1558.f/2.f); // starting point is the center of the mesh
    rotation_3d = 0.f;
    cam_zoom = 1000.0;
    extrusionAmount = 0.2;
    ofImage heightmap;
    heightmap.load("bedmap2_surface_rutford.png");
    // texture
    map_buffer.allocate(WIDTH, HEIGHT, GL_RGB);
    //ofImage topo3dteximg;
    //topo3dteximg.load("rutford_stretched_tex.png");
    //topo3dtex = topo3dteximg.getTexture();
    //topo3dtex.setTextureWrap(GL_REPEAT, GL_REPEAT);
    float h = heightmap.getHeight();
    float w = heightmap.getWidth();
    float px_height;//, px_color;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            px_height = max(0.0f, heightmap.getColor(x, y).getBrightness());
            //px_color = 255.f - min(255.f, px_height);
            topo3d.addVertex(ofPoint(x, y, px_height * extrusionAmount));
            //topo3d.addColor(ofColor(px_color));
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

//--------------------------------------------------------------
void ofApp::setCameraPosition(){
    ofVec3f camDirection(0, 1, 1);
    ofVec3f centre(position_3d.x, position_3d.y, 255/2.f);
    ofVec3f camDirectionRotated = camDirection.getRotated(rotation_3d, ofVec3f(0, 0, 1));
    ofVec3f camPosition = centre + camDirectionRotated * cam_zoom;

    cam.setPosition(camPosition);
    cam.lookAt(centre);
    cam.roll(rotation_3d);
}

//--------------------------------------------------------------
void ofApp::draw(){
    // ========= DRAW WORMS TO A BUFFER (for the MAP VIEWPORT) ========
    drawViewportOutline(vMap);
    ofPushView();
    ofViewport(vMap);
    ofSetupScreen();

    // <------
    // everything drawn between map_buffer.begin() and map_buffer.end() will be
    // drawn onto either the 2D view or on the 3D mesh, depending on which mode we're in.
    // the gui and buttons are thus drawn after the buffer drawing
    map_buffer.begin();

    if (show_topo == 0) {
        img_topo_surface.draw(0, 0);
    } else if (show_topo == 1) {
        img_topo_bed.draw(0, 0);
    } else if (show_topo == 2) {
        img_topo_thickness.draw(0, 0);
    } else {
        ofSetColor(225);
        ofDrawRectangle(vMap.x, vMap.y, WIDTH, HEIGHT);
        ofSetColor(255, 255, 255);
    }

    if (show_gdop == 0) {
        ofSetColor(255, 255, 255, 150);
        img_gdop.draw(0, 0);
        ofSetColor(255);
    } else if (show_gdop == 1) {
        ofSetColor(255, 255, 255, 150);
        img_obscov.draw(0, 0);
        ofSetColor(255);
    }

    //CM.current.comp.overlay.draw(0, 0); // mask which slightly blocks where we don't have data

    if (show_worms)
        WM.drawWorms();

    if (show_tracks) {
        vector<Track>::iterator it, end;
        for (it = tracks.begin(), end = tracks.end(); it != end; it++) {
            if (active_tracks[it->trackno] == 1) {
                ofSetColor(ofColor::yellow);
            } else {
                ofSetColor(ofColor::blueSteel);
            }
            it->draw();
        }
    }
    if (selection_pos.x != -1 && selection_pos.y != -1) {
        ofNoFill();
        ofSetColor(ofColor::lightCoral);
        ofDrawCircle(selection_pos, 15);
        ofFill();
        ofDrawCircle(selection_pos, 5);
    }
    if (selection2_pos.x != -1 && selection2_pos.y != -1) {
        ofNoFill();
        ofSetColor(ofColor::aliceBlue);
        ofDrawCircle(selection2_pos, 15);
        ofFill();
        ofDrawCircle(selection2_pos, 5);
    }

    ofSetColor(ofColor::white);

    map_buffer.end();

    topo3dtex = map_buffer.getTexture();

    // ========= 2D WORMPLOT ========
    if (map_mode == 2) {
        map_buffer.draw(position.x, position.y, WIDTH * map_zoom, HEIGHT * map_zoom);
    // ========= 3D WORMPLOT ========
    } else if (map_mode == 3) {
        cam.begin(vMap);

            map_buffer.getTexture().bind();
            topo3d.draw();
            map_buffer.getTexture().unbind();
        cam.end();
    }
    ofPopView(); // done drawing to 2d wormplit viewport
    // ========= PAIRSPACE ========
    drawViewportOutline(vPairspace);
    ofPushView();
    ofViewport(vPairspace);
    ofSetupScreen();
    // do any drawing for the pairspace below
    //img_temp_pairspace.draw(0, 0, vPairspace.getWidth(), vPairspace.getHeight());
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
    drawPlots();
    //img_temp_modelspace.draw(0, 0, vModel.getWidth(), vModel.getHeight());

    ofPopView();

    // draw all GUI elements
    if (!hide_all) {
        // draw buttons
        drawButtons();
    }
}

//--------------------------------------------------------------
void ofApp::drawPlots(){
    //if (is_selection)
        plotENU.draw();
}

//--------------------------------------------------------------
void ofApp::drawButton(Button *btn, bool colored, bool visible){
    if (visible) {
        btn->hidden = false;

        // set color
        if (colored) {
            ofSetColor(ofColor::lightCoral);
        //} else if (btn->is_label){
            //ofSetColor(ofColor::magenta);
        } else {
            ofSetColor(ofColor(255, 255, 255, 150));
        }

        // draw the image/text
        if (btn->is_image)
            btn->img.draw(btn->rect.position);
        else
            font.drawString(btn->text, btn->rect.position.x, btn->rect.position.y + font.stringHeight("!"));
    } else {
        btn->hidden = true;
    }
}

//--------------------------------------------------------------
void ofApp::drawButtons() {
    // draw all buttons, and color them as "on" or "off" depending on if the boolean expression is true or false
    drawButton(&btn_2d, map_mode == 2, true);
    drawButton(&btn_3d, map_mode == 3, true);
    drawButton(&btn_fullscreen_map, fullscreen == true, true);
    drawButton(&btn_fullscreen_model, fullscreen == true, true);

    drawButton(&btn_model, false, true);
    drawButton(&btn_model_vel, CM.vel_enabled == true, true);
    drawButton(&btn_model_p1, CM.p1_enabled == true, true);
    drawButton(&btn_model_p2, CM.p2_enabled, true);
    drawButton(&btn_model_p3, CM.p3_enabled, true);


    drawButton(&btn_layers, false, true);
    //drawButton(&btn_layers_none, show_topo >= 3, true);
    drawButton(&btn_layers_surface, show_topo == 0, true);
    drawButton(&btn_layers_divider1, false, true);
    drawButton(&btn_layers_bed, show_topo == 1, true);
    drawButton(&btn_layers_divider2, false, true);
    drawButton(&btn_layers_thickness, show_topo == 2, true);
    drawButton(&btn_layers_gdop, show_gdop == 0, true);
    drawButton(&btn_layers_divider3, false, true);
    drawButton(&btn_layers_obscov, show_gdop == 1, true);

    drawButton(&btn_mode, false, true);
    drawButton(&btn_mode_drawing, mouse_mode == 0, true);
    drawButton(&btn_mode_selection, mouse_mode == 1, true);

    drawButton(&btn_flow, false, true);
    drawButton(&btn_flow_worms, WM.preset == "worms", true);
    drawButton(&btn_flow_lines, WM.preset == "lines", true);
    drawButton(&btn_flow_dots, WM.preset == "dots", true);
    drawButton(&btn_flow_disp, WM.preset == "disp", WM.mode == 1);

    drawButton(&btn_options, false, true);
    drawButton(&btn_options_lag, false, WM.mode == 0);
    drawButton(&btn_options_eul, false, WM.mode == 1);
    drawButton(&btn_options_small, false, WM.worm_size == 2);
    drawButton(&btn_options_medium, false, WM.worm_size == 5);
    drawButton(&btn_options_large, false, WM.worm_size == 10);
    drawButton(&btn_options_density_low, false, WM.density_preset == 0);
    drawButton(&btn_options_density_med, false, WM.density_preset == 1);
    drawButton(&btn_options_density_high, false, WM.density_preset == 2);
    drawButton(&btn_options_EN, false, WM.include_up == 0);
    drawButton(&btn_options_ENU, false, WM.include_up == 1);
}

//--------------------------------------------------------------
void ofApp::drawViewportOutline(const ofRectangle & viewport){
    ofPushStyle();
    ofFill();
    ofSetColor(51);
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
    //cout << key << endl;
    if (key == 104) {
        hide_all = !hide_all;
    } else if (key == 32) {
        paused = !paused;
    }
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
    float m_dx = ofGetMouseX() - ofGetPreviousMouseX();
    float m_dy = ofGetMouseY() - ofGetPreviousMouseY();
    // drag the maps with the right mouse button
    if (button == 2) {
        if (isPointInRect(pt, vMap.position, vMap.width, vMap.height)) {
            if (map_mode == 2) {
            position.x += m_dx;
            position.y += m_dy;

            position = restrictPosition(position, WIDTH * map_zoom, HEIGHT * map_zoom, vMap);
            //CM.setPos(position);
            //WM.pos.set(position);

            // move tracks
            /*
            vector<Track>::iterator it, end;
            for (it = tracks.begin(), end = tracks.end(); it != end; it++) {
                // we only care about updating the position of tracks relevant to the current selection
                if (active_tracks[it->trackno] == 1)
                    it->setPos(position);
            }
            */
            } else if (map_mode == 3) {
                // panning must be done relative to the mesh's orientation

                float m_angle = atan2(m_dy, m_dx);
                float radians = m_angle + (PI/180 * rotation_3d);
                float speed = sqrt(m_dx*m_dx + m_dy*m_dy);
                position_3d.x -= speed * cos(radians);
                position_3d.y -= speed * sin(radians);
                // 946x1558 is the size of the topography image, so here we're restricting panning to between the
                // mesh's four corners
                position_3d = restrictPosition(position_3d, 1, 1, ofRectangle(0, 0, 946, 1558));
            }
        }
    // left mouse button
    } else if (button == 0) {
        if (map_mode == 3 && isPointInRect(pt, ofVec2f(0, 0), vMap.width, vMap.height)) {
            rotation_3d += m_dx;
            if (rotation_3d > 360) rotation_3d = 0.f;
            else if (rotation_3d < 0) rotation_3d = 360.f;
        // drag to create a line or whatever of worms
        } else if (mouse_mode == 0 && map_mode == 2 && isPointInRect(pt, ofVec2f(0, 0), vMap.width, vMap.height)) {
            WM.createWorm((x - position.x) / map_zoom, (y - position.y) / map_zoom);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    // left clicks
    if (button == 0) {
        bool reset_selection = true; // if we don't click a gui button or somewhere on the map, this remains true

        // get new set of active tracks depending on the mouse click location
        vector<Track>::iterator it, end;

        // ======== BUTTONS =========
        // 2D/3D BUTTONS
        if (isButtonClicked(ofVec2f(x, y), &btn_2d)) {
            map_mode = 2;
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_3d)) {
            map_mode = 3;
            reset_selection = false;
        // FULLSCREEN BUTTONS
        } else if (isButtonClicked(ofVec2f(x, y), &btn_fullscreen_map)) {
            toggleFullscreen(&vMap);
            // shift the 2D map over to take advantage of the newly freed up real-estate
            if (fullscreen == true) {
                position.x = ofGetWidth()/2 - WIDTH/2 * map_zoom;
            // or shift it back because of the loss of real estate
            } else {
                position.x = vMap.width/2 - WIDTH/2 * map_zoom;
            }
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_fullscreen_model)) {
            toggleFullscreen(&vModel);
            setupPlots();
            reset_selection = false;
        // MODE BUTTONS
        } else if (isButtonClicked(ofVec2f(x, y), &btn_mode_drawing)) {
            mouse_mode = 0;
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_mode_selection)) {
            mouse_mode = 1;
            reset_selection = false;
        // LAYER BUTTONS
        } else if (isButtonClicked(ofVec2f(x, y), &btn_layers_gdop)) {
            show_gdop = show_gdop == 0 ? 2 : 0;
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_layers_obscov)) {
            show_gdop = show_gdop == 1 ? 2 : 1;
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_layers_bed)) {
            show_topo = show_topo == 1 ? 3 : 1;
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_layers_thickness)) {
            show_topo = show_topo == 2 ? 3 : 2;
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_layers_none)) {
            show_topo = 3;
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_layers_surface)) {
            show_topo = show_topo == 0 ? 3 : 0;
            reset_selection = false;
        // WORM PRESET BUTTONS
        } else if (WM.include_up == 0 && isButtonClicked(ofVec2f(x, y), &btn_options_EN)){
            WM.include_up = WM.include_up == 1 ? 0 : 1;
            WM.refreshWorms(true);
        } else if (WM.include_up == 1 && isButtonClicked(ofVec2f(x, y), &btn_options_ENU)){
            WM.include_up = WM.include_up == 0 ? 1 : 0;
            WM.refreshWorms(true);
        } else if (WM.mode == 1 && isButtonClicked(ofVec2f(x, y), &btn_options_eul)) {
            WM.setMode(0);
            if (WM.preset == "disp" && WM.mode == 0) WM.preset = "worms";
            WM.wormPreset(WM.preset);
            WM.refreshWorms(true);
            reset_selection = false;
        } else if (WM.mode == 0 && isButtonClicked(ofVec2f(x, y), &btn_options_lag)) {
            WM.setMode(1);
            WM.wormPreset(WM.preset);
            WM.refreshWorms(true);
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_flow_worms)) {
            if (WM.preset != "worms") {
                WM.preset = "worms";
                WM.wormPreset(WM.preset);
                WM.refreshWorms(true);
            }
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_flow_lines)) {
            if (WM.preset != "lines") {
                WM.preset = "lines";
                WM.wormPreset(WM.preset);
                WM.refreshWorms(true);
            }
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_flow_dots)) {
            if (WM.preset != "dots") {
                WM.preset = "dots";
                WM.wormPreset(WM.preset);
                WM.refreshWorms(true);
            }
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_flow_disp)) {
            if (WM.preset != "disp") {
                WM.preset = "disp";
                WM.wormPreset(WM.preset);
                WM.refreshWorms(true);
            }
            reset_selection = false;
        // MODEL BUTTONS
        } else if (isButtonClicked(ofVec2f(x, y), &btn_model_vel)) {
            CM.vel_enabled = CM.vel_enabled == 1 ? 0 : 1;
            plotENU.refreshData(0.1f, 0.f, 500.f, 0, 0);
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_model_p1)) {
            CM.p1_enabled = CM.p1_enabled == 1 ? 0 : 1;
            plotENU.refreshData(0.1f, 0.f, 500.f, 0, 0);
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_model_p2)) {
            CM.p2_enabled = CM.p2_enabled == 1 ? 0 : 1;
            plotENU.refreshData(0.1f, 0.f, 500.f, 0, 0);
            reset_selection = false;
        } else if (isButtonClicked(ofVec2f(x, y), &btn_model_p3)) {
            CM.p3_enabled = CM.p3_enabled == 1 ? 0 : 1;
            plotENU.refreshData(0.1f, 0.f, 500.f, 0, 0);
            reset_selection = false;
        // FLOW OPTIONS BUTTONS
        } else if (WM.worm_size == 2 && isButtonClicked(ofVec2f(x, y), &btn_options_small)) {
            WM.setWormSize(5);
            WM.wormPreset(WM.preset);
            WM.refreshWorms(true);
            reset_selection = false;
        } else if (WM.worm_size == 5 && isButtonClicked(ofVec2f(x, y), &btn_options_medium)) {
            WM.setWormSize(10);
            WM.wormPreset(WM.preset);
            WM.refreshWorms(true);
            reset_selection = false;
        } else if (WM.worm_size == 10 && isButtonClicked(ofVec2f(x, y), &btn_options_large)) {
            WM.setWormSize(2);
            WM.wormPreset(WM.preset);
            WM.refreshWorms(true);
            reset_selection = false;
        } else if (WM.density_preset == 0 && isButtonClicked(ofVec2f(x, y), &btn_options_density_low)) {
            WM.density_preset = 1;
            WM.wormPreset(WM.preset);
            WM.refreshWorms(true);
            reset_selection = false;
        } else if (WM.density_preset == 1 && isButtonClicked(ofVec2f(x, y), &btn_options_density_med)) {
            WM.density_preset = 2;
            WM.wormPreset(WM.preset);
            WM.refreshWorms(true);
            reset_selection = false;
        } else if (WM.density_preset == 2 && isButtonClicked(ofVec2f(x, y), &btn_options_density_high)) {
            WM.density_preset = 0;
            WM.wormPreset(WM.preset);
            WM.refreshWorms(true);
            reset_selection = false;
        }

        // by checking if reset_selection == true here before allowing a selection or worm creation
        // we prevent selections/creations from occuring if a button has been pressed, which is
        // desirable behavior i reckon
        if (show_worms && reset_selection == true) {
            if (map_mode == 2 && vMap.width > 1) { // need to be in 2D map mode and not in fullscreen for a different view to make selections
                if (mouse_mode == 0) {
                    WM.createWorm((x - position.x) / map_zoom, (y - position.y) / map_zoom);
                } else if (mouse_mode == 1) {
                    // TODO don't make a selection if a button was clicked..
                    if (isPointInRect(ofVec2f(x, y), ofVec2f(position.x, position.y), WIDTH * map_zoom, HEIGHT * map_zoom)) {
                        // check if there's valid data for the point, and if it is undefined there don't make a selection/worm
                        if (WM.isValidPos((x - position.x)/ map_zoom, (y - position.y) / map_zoom)) {
                            selection_pos.set((x - position.x) / map_zoom, (y - position.y) / map_zoom);
                            is_selection = true;
                            reset_selection = false;
                            plotENU.refreshData(0.1f, 0.f, 500.f, 0, 0);
                            active_tracks.clear();
                            for (it = tracks.begin(), end = tracks.end(); it != end; it++) {
                                if (it->isClickInTrack(selection_pos.x, selection_pos.y) == true) {// && gui_tracks.getToggle("track " + to_string(it->trackno))) {
                                    active_tracks[it->trackno] = 1;
                                } else {
                                    active_tracks[it->trackno] = 0;
                                }
                            }
                        }
                    }
                }
            } else if (map_mode == 3) {
                reset_selection = false; // don't clear selections while in 3d mode

                /* 3D worm creation is broken
                int n = topo3d.getNumVertices();
                float nearestDistance = 0;
                ofVec2f nearestVertex;
                int nearestIndex = 0;
                ofVec2f mouse (mouseX, mouseY);
                for (int i=0; i<n; i+=500) {
                    ofVec3f cur = cam.worldToScreen(topo3d.getVertex(i));
                    float distance = cur.distance(mouse);
                    if (i==0 || distance < nearestDistance) {
                        nearestDistance = distance;
                        nearestVertex = cur;
                        nearestIndex = i;
                    }
                }
                */
            }
        }

        if (reset_selection && mouse_mode == 1) {
            selection_pos.set(-1, -1);
            is_selection = false;
            plotENU.clearData();
            // now that there's no selection, we should hide the model and pair plots
            if (!fullscreen)
                setViewportSizes();
        } else {
            // if a selection has been made we need to make sure we see the model plot
            if (!fullscreen)
                setViewportSizes();
        }

    } // end of left-mouse clicks

    // right mouse clicks
    if (button == 2) {
        bool reset_selection2 = true; // if we don't click a gui button or somewhere on the map, this remains true

        if (show_worms && reset_selection2 == true) {
            if (map_mode == 2 && vMap.width > 1) { // need to be in 2D map mode and not in fullscreen for a different view to make selections
                if (mouse_mode == 1) {
                    // TODO don't make a selection if a button was clicked..
                    if (isPointInRect(ofVec2f(x, y), ofVec2f(position.x, position.y), WIDTH * map_zoom, HEIGHT * map_zoom)) {
                        // check if there's valid data for the point, and if it is undefined there don't make a selection/worm
                        if (WM.isValidPos((x - position.x)/ map_zoom, (y - position.y) / map_zoom)) {
                            selection2_pos.set((x - position.x) / map_zoom, (y - position.y) / map_zoom);
                            is_selection2 = true;
                            reset_selection2 = false;
                            plotENU.refreshData(0.1f, 0.f, 500.f, 0, 0);
                        }
                    }
                }
            } else if (map_mode == 3) {
                reset_selection2 = false; // don't clear selections while in 3d mode
            }
        }

        if (reset_selection2 && mouse_mode == 1) {
            selection2_pos.set(-1, -1);
            is_selection2 = false;
            plotENU.clearData2();
            // now that there's no selection, we should hide the model and pair plots
            if (!fullscreen)
                setViewportSizes();
        } else {
            // if a selection has been made we need to make sure we see the model plot
            if (!fullscreen)
                setViewportSizes();
        }
    } // end of right-mouse clicks
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
    float mx = ofGetMouseX();
    float my = ofGetMouseY();
    // 2D & 3D topography navigation
    if (isPointInRect(ofVec2f(mx, my), vMap.position, vMap.width, vMap.height)) {
        if (map_mode == 2) {
            map_zoom += scrollY * 0.01f;
            map_zoom = max(0.5f, min(map_zoom, 2.f)); // clamp map_zoom
        } else if (map_mode == 3) {
            cam_zoom -= scrollY * 5.f;
        }
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
    fullscreen = false;

    // make sure the map is within the newly-resized viewport
    position = restrictPosition(position, WIDTH, HEIGHT, vMap);
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

//--------------------------------------------------------------
bool ofApp::isButtonClicked(ofVec2f mpos, Button *btn){
    if (mpos.x > btn->rect.x && mpos.x < btn->rect.x + btn->rect.width && mpos.y > btn->rect.y && mpos.y < btn->rect.y + btn->rect.height) {
        return true;
    }
    return false;
}

//--------------------------------------------------------------
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

//--------------------------------------------------------------
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
        // modelspace
        vModel.x = 0;
        vModel.y = 0;
        vModel.width = 0;
        vModel.height = 0;
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

        // set the chosen viewport to fullscreen
        view->x = 0;
        view->y = 0;
        view->width = w;
        view->height = h - timeslider_height;

        // refresh the button placement now that viewport sizes have changed size
        setupButtons();

        // keep the guis in their respective viewports
        ofVec2f restricted_options_pos = restrictPosition(gOptions->getPosition(),\
                                                          gOptions->getWidth(), gOptions->getHeight(), vMap);
        gOptions->setPosition(restricted_options_pos.x, restricted_options_pos.y);
    }
    fullscreen = !fullscreen;
}
