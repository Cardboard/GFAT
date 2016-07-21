#include "wormman.h"

WormMan::WormMan()
{

}

WormMan::~WormMan()
{
    // delete all those dank worms

}

void WormMan::setup(CompMan cm, string hname, string vname, ofVec2f p)
{
    horizontalName = hname;
    verticalName = vname;
    // NOTE: the assumption is made that the components are of the same size and have
    // the same range of usable data
    horizontalComponent = cm.getComponent(hname);
    verticalComponent = cm.getComponent(vname);

    width = cm.width;
    height = cm.height;
    samples = cm.samples;
    lines = cm.lines;
    scale = horizontalComponent.comp.scale;
    pos.set(p);

    mode = 0;
    colormode = 0;
    arrowmode = 0;
    worm_spawnrate = 0.5;
    worm_lifespan = 5;
    worm_size = 5;
    worm_tailsize = 5;
    worm_speed = 10;
    spawn_x = -1;
    spawn_xrange = -1;
    spawn_y = -1;
    spawn_yrange = -1;

    array.resize(lines);
      for (int i = 0; i < lines; ++i)
        array[i].resize(samples);

    // set up binary array telling us which pixels are valid for drawing on,
    // and also get the range from which we can draw pixels
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < samples; j++) {
            float val = cm.current.comp.array[i][j];
            if (!isnan(val)) {
                array[i][j] = 1;
                if (spawn_x == -1 || j < spawn_x) spawn_x = j;
                if (spawn_y == -1 || i < spawn_y) spawn_y = i;
                if (spawn_xrange == -1 || j > spawn_xrange) spawn_xrange = j;
                if (spawn_yrange == -1 || i > spawn_yrange) spawn_yrange = i;
            } else {
                array[i][j] = 0;
            }
        }
    }

}

void WormMan::setColorMode(int i)
{
    colormode = i;
    refreshWorms(false);
}

void WormMan::setMode(int i)
{
    // lagrangrian:0, eulerian:1
    mode = i;
}

void WormMan::setOpaque(bool b)
{
    worm_opaque = b;
    refreshWorms(false);
}

void WormMan::setArrowMode(int i){ // worms:0, arrows:1
    arrowmode = i;
    //refreshWorms(true);
}

void WormMan::setWormDensity(float d)
{
    // don't spawn anything
    if (d == 0.0) {
        worm_density = 0.0;
        worm_spawnrate = 100000;
        refreshWorms(true);
    } else {
        worm_density = d; // density should be in range (0.0, 1.0]
        float magicno = 9;
        worm_spawnrate = (1.0 - d) * magicno;
        refreshWorms(true);
        clock = -1; // start spawning right away without waiting for the clock to tick
    }
}

void WormMan::setWormLifespan(float t)
{
    worm_lifespan = t;
    refreshWorms(true);
}

void WormMan::setWormTailSize(float ts)
{
    worm_tailsize = ts;
    refreshWorms(true);
}

void WormMan::setWormSize(int s)
{
    worm_size = s;
    if (mode == 0) {
        refreshWorms(false);
    } else {
        // we have to delete the worms and start again because size is used in
        // determining density
        refreshWorms(true);
    }
}

void WormMan::setWormSpeed(float s)
{
    worm_speed = s;
}

void WormMan::setUniform(bool b)
{
    worm_uniform = b;
    refreshWorms(true);
}


void WormMan::refreshWorms(bool respawn) // removes or updates current worms with updated properties
{
    // delete all worms and respawn them (necessary if using eularian / changing density)
    if (respawn) {
        // delete current worms, if any
        if (worms.size() != 0) {
            worms.clear();
        }
        if (mode == 1) {
            // create worms based on the density property
            for (int i = 0; i < width + pos.x; i+=2 * (1/0.2) * worm_size) {
                for (int j = 0; j < height + pos.y; j+=2 * (1/0.2) * worm_size) {
                    // TODO spawn worms either uniformly or only somewhat uniformly
                    if (worm_uniform == true) { // false) { // somewhat uniform placement
                        createWorm((float) i + rand() % (worm_size * 4) + (worm_size * 2), (float) j + rand() % (worm_size * 4) + (worm_size * 2));
                    } else { // uniform placement
                        createWorm((float) i, (float) j);
                    }
                }
            }
        }

    // don't delete worms, simply update some properties
    } else {
        vector<vector<Worm>>::iterator it, end;
        vector<Worm>::iterator w;
        for (it = worms.begin(), end = worms.end(); it != end; ++it) {
            // draw each individual worm in each array of worms, with the
            // back of the tail drawn first so the order of drawing is better
            for (w = it->begin(); w != it->end(); w++) {
                w->mode = mode;
                w->lifespan = worm_lifespan;
                w->size = worm_size;
                w->scaled_size = worm_size * w->alpha / 255.0;
                w->opaque = worm_opaque;
                w->colormode = colormode;
                //w->startenu = w->enu;
            }
        }
    }
}

void WormMan::createWorm()
{
    // whenever we create a worm we want to create an array of worms, with the worm at
    // index zero being the "leading" worm, and the rest making up the tail of the worm
    float x, y;
    x = spawn_x + (rand() % (int)(spawn_xrange - spawn_x));
    y = spawn_y + (rand() % (int)(spawn_yrange - spawn_y));

    // convert unscaled position to scaled position
    float scaledx = x / scale;
    float scaledy = y / scale;

    // only create a worm if it will be on the map
    if (array[y][x] == 1) {
        vector<Worm> newWormGroup;
        // add a bunch of worms to the wormgroup
        for (int i = 0; i < worm_tailsize; i++) {
            Worm newWorm;
            newWorm.setup(mode, worm_lifespan, scaledx, scaledy, worm_size);
            double scaled_alpha = ((double) i / ((worm_tailsize - 0.0) / (1.0 - 0.0))) + 0.0;
            newWorm.alpha = 255.0 - 255.0*scaled_alpha;
            newWorm.alpha *= newWorm.alpha / 255.0;
            newWorm.scaled_size = worm_size * newWorm.alpha / 255.0;
            newWorm.opaque = worm_opaque;
            newWorm.colormode = colormode;
            newWormGroup.push_back(newWorm);
        }
        // add the wormgroup to the WormMan's worm collection
        worms.push_back(newWormGroup);
    }
}

// create worm at mouse
void WormMan::createWorm(float mx, float my)
{
    if (mx > spawn_x/scale + pos.x && mx < spawn_xrange/scale + pos.x && my > spawn_y/scale + pos.y && my < spawn_yrange/scale + pos.y) {
        // convert unscaled position to scaled position
        float scaledx = (mx - pos.x) * scale;
        float scaledy = (my - pos.y) * scale;

        // only create a worm if it will be on the map
        if (array[scaledy][scaledx] == 1) {
            vector<Worm> newWormGroup;
            // add a bunch of worms to the wormgroup
            for (int i = 0; i < worm_tailsize; i++) {
                Worm newWorm;               
                newWorm.setup(mode, worm_lifespan, mx - pos.x, my - pos.y, 2);
                double scaled_alpha = ((double) i / ((worm_tailsize - 0.0) / (1.0 - 0.0))) + 0.0;
                newWorm.alpha = 255.0 - 255.0*scaled_alpha;
                newWorm.alpha *= newWorm.alpha / 255.0;
                newWorm.scaled_size = worm_size * newWorm.alpha / 255.0;
                newWorm.opaque = worm_opaque;
                newWorm.colormode = colormode;
                newWormGroup.push_back(newWorm);
            }
            // add the wormgroup to the WormMan's worm collection
            worms.push_back(newWormGroup);
        }
    }
}

void WormMan::drawWorms()
{
    vector<vector<Worm>>::reverse_iterator it, end;
    vector<Worm>::iterator w;
    for (it = worms.rbegin(), end = worms.rend(); it != end; ++it) {
        // draw eulerian arrows
        if (arrowmode == 1) {
            it->begin()->drawArrow(pos);
        } else {
            // draw each individual worm in each array of worms
            if (it->size() >= 1) {
                for (w = it->begin(); w != it->end(); w++) {
                    //if (w->alpha == 255.0)
                    w->draw(pos);
                }
            }
        }
    }
}

void WormMan::updateComponents(CompMan cm)
{
    // change the components we use for displacement mid-game
    horizontalComponent = cm.getComponent(horizontalName);
    verticalComponent = cm.getComponent(verticalName);
}

void WormMan::updateWorms(CompMan *cm, int t)
{
    float dt = ofGetLastFrameTime();

    if (mode == 0 && (clock > worm_spawnrate || clock == -1)) {
        for (int i=0; i<5; i++) {
            createWorm();
        }
        clock = 0;
    }

    clock += dt * 10;

    // update all the existing worms
    // (iterate in reverse so we can delete things and not screw up the rest of the iteration)
    vector<vector<Worm>>::reverse_iterator it, end;
    int i, j;
    for (i = 0, it = worms.rbegin(), end = worms.rend(); it != end; i++, it++) {

        for (j = worms[i].size()-1; j > 0; --j) {
            if (worms[i][j].status > 0 || worms[i][j].lifespan < 0) {
                worms[i][j].age += worms[i][j].status;
            }

            if (worms[i][j].age > 300 && worms[i][j].status > 0) {
                worms[i][j].age = 300;
                worms[i][j].status = - worms[i][j].status;
            }

            worms[i][j].x = worms[i][j-1].x;
            worms[i][j].y = worms[i][j-1].y;
            worms[i][j].enu = worms[i][j-1].enu;
            worms[i][j].color.red = worms[i][j-1].color.red;
            worms[i][j].color.green = worms[i][j-1].color.green;
            worms[i][j].color.blue = worms[i][j-1].color.blue;

            // delete worms when they die, unless viewing eularian worms
            if (mode == 0) {
                if (worms[i][j].status < 0) {
                    worms[i][j].lifespan -= 1;
                }
                if (worms[i][j].age < 0) {
                    worms[i].erase(worms[i].begin() + j);
                }
            }
        }

        ofVec2f pt;
        if (mode == 0) {
            pt.set(it->begin()->x + pos.x, it->begin()->y + pos.y); // eularian->lagrangian <= startx, starty -> x, y
        } else {
            pt.set(it->begin()->startx + pos.x, it->begin()->starty + pos.y); // eularian->lagrangian <= startx, starty -> x, y
        }
        ENU enu = cm->solveEquation(pt, t);
        it->begin()->setENU(enu);

        // update the leader node last
        bool endoflife = (it->begin())->update(mode, worm_speed, dt);

        // delete if the leader is past his lifetime and all tail worms have been deleted
        if (endoflife && worms[i].size() == 1) {
            // kill worms that leave the drawable zone or that leave the drawable zone
            worms[i].erase(worms[i].begin());
            worms.erase(worms.begin() + i);
        }
    }

}
