#include "worm.h"
#include "compman.h"
#include "modelcomponent.h"

#ifndef WORMMAN_H
#define WORMMAN_H


class WormMan
{
public:
    WormMan();
    ~WormMan();

    void setup(CompMan *cm, string horizontalName, string verticalName, ofVec2f p);
    void wormPreset(string name);
    void setColorMode(int i); // color: 0, b&w: 1
    void setMode(int i); // lagrangrian:0, eulerian:1
    void setOpaque(bool b);
    void setArrowMode(int i); // worms:0, arrows:1
    void setWormDensity(float d);
    void setWormLifespan(float t);
    void setWormTailSize(float ts);
    void setWormSize(int s);
    void setWormSpeed(float s);
    void setUniform(bool b);
    void refreshWorms(bool respawn); // removes all worms and respawns them based on the current density
    void drawWorms();
    bool isValidPos(float mx, float my);
    void createWorm();
    void createWorm(float mx, float my);
    void updateComponents();
    void updateWorms(int t);

    CompMan *cm;
    string horizontalName;
    string verticalName;
    NamedComp horizontalComponent;
    NamedComp verticalComponent;

    vector<vector<Worm>> worms;
    vector<vector<int>> array;
    ofVec2f pos;
    float clock;
    string preset;
    int density_preset;
    int mode;
    bool worm_opaque;
    int include_up;
    bool colormode; // controls whether worms are b&w or color
    int arrowmode;
    int worm_spawnrate; // used for lagrangian worms only
    int worm_lifespan;
    int worm_tailsize;
    int worm_size;
    float worm_speed;
    float worm_density; // used for eulerian worms only
    bool worm_uniform;
    float spawn_x;
    float spawn_xrange; // x index of farthest left value
    float spawn_y;
    float spawn_yrange; // y index of farthest up value

    int width; // scaled height
    int height; // scaled width
    int samples; // unscaled width
    int lines; // unscaled height
    float scale; // how much we have scaled the original data by

};

#endif // WORMMAN_H
