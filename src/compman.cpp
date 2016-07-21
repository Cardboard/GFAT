#include <math.h>

#include "compman.h"

CompMan::CompMan()
{

}

void CompMan::setup(int s, int l, int w, int h, int x, int y) {
    samples = s;
    lines = l;
    width = w;
    height = h;
    pos.set(x,y);
}

void CompMan::setup(int s, int l, int w, int h) {
    setup(w, h, 0, 0);
}

void CompMan::setPos(ofVec2f pt) {
    pos = pt;
    std::vector<NamedComp>::iterator it, end;
    for (it = components.begin(), end = components.end(); it != end; ++it) {
        it->comp.setPos(pt.x, pt.y);
    }
}

// TODO make sure we don't add a component with the same name & time as an existent component
void CompMan::addComponent(string name, string filename) {
    Component newComp;
    newComp.setInfo(name, samples, lines);
    newComp.setPos(pos.x, pos.y);
    newComp.setSize(width);
    newComp.loadAndConvert(filename);
    newComp.setupImage();

    NamedComp newNamedComp;
    newNamedComp.name = name;
    newNamedComp.comp = newComp;

    components.push_back(newNamedComp);
    //cout << "added component " << name << endl;
}

// find a component for a given name & time
// TODO return something other than 'current' if the search finds no match?
NamedComp CompMan::getComponent(string name) {
    std::vector<NamedComp>::iterator it, end;
    for (it = components.begin(), end = components.end(); it != end; ++it) {
        // found a match!
        if (it->name == name) {
            return *it;
        }
    }

    cout << "didn't find a match, returning current\n";
    return current;
}

// set the current component
void CompMan::setCurrentComponent(string name) {
    current = getComponent(name);
    cout << "set current component to " << name << " at time " << time << endl;
}

// draw designated component
void CompMan::drawComponent(string name) {
    NamedComp drawComp = getComponent(name);
    //drawComp.comp.img.draw(pos.x, pos.y); //, width, height);
    drawComp.comp.circlePixels();
}

// draw current component
void CompMan::drawComponent() {
    //current.comp.img.draw(pos.x, pos.y); //, width, height);
    //current.comp.overlay.draw(current.comp.pos.x, current.comp.pos.y); // uses the whole screen
    if (current.comp.click_i_x != -1 && current.comp.click_i_y != -1)
        current.comp.circlePixels();
}

// TODO transfer click handling to CompMan class
void CompMan::checkClicks(int x, int y) {
    current.comp.checkClicks(x, y);
}

// solves the velocity equation for the given time
// NOTE: requires the appropriate components to have already been loaded by the CompMan
ENU CompMan::solveEquation(float t) {
    float vel_n, vel_e, vel_u;
    float amp1_n, amp1_e, amp1_u;
    float amp2_n, amp2_e, amp2_u;
    float amp3_n, amp3_e, amp3_u;
    float phz1_n, phz1_e, phz1_u;
    float phz2_n, phz2_e, phz2_u;
    float phz3_n, phz3_e, phz3_u;
    float period_1 = 12.42;
    float period_2 = 25.82;
    float period_3 = 14.77 * 24; // 14.77 days;
    float result_n, result_e, result_u;
    ofVec2f pt;
    pt.set(current.comp.click_pos.x-pos.x, current.comp.click_pos.y-pos.y);

    std::vector<NamedComp>::iterator it, end;
    for (it = components.begin(), end = components.end(); it != end; ++it) {
        // found a match!
        if (it->name == "E") {
            vel_e = it->comp.getValue(pt);
        } else if (it->name == "N") {
            vel_n = it->comp.getValue(pt);
        } else if (it->name == "U") {
            vel_u = it->comp.getValue(pt);
        // X = 1
        } else if (it->name == "AMP1E") {
            amp1_e = it->comp.getValue(pt);
        } else if (it->name == "AMP1N") {
            amp1_n = it->comp.getValue(pt);
        } else if (it->name == "AMP1U") {
            amp1_u = it->comp.getValue(pt);
        } else if (it->name == "PHZ1E") {
            phz1_e = it->comp.getValue(pt);
        } else if (it->name == "PHZ1N") {
            phz1_n = it->comp.getValue(pt);
        } else if (it->name == "PHZ1U") {
            phz1_u = it->comp.getValue(pt);
        // X = 2
        } else if (it->name == "AMP2E") {
            amp2_e = it->comp.getValue(pt);
        } else if (it->name == "AMP2N") {
            amp2_n = it->comp.getValue(pt);
        } else if (it->name == "AMP2U") {
            amp2_u = it->comp.getValue(pt);
        } else if (it->name == "PHZ2E") {
            phz2_e = it->comp.getValue(pt);
        } else if (it->name == "PHZ2N") {
            phz2_n = it->comp.getValue(pt);
        } else if (it->name == "PHZ2U") {
            phz2_u = it->comp.getValue(pt);
        // X = 3
        } else if (it->name == "AMP3E") {
            amp3_e = it->comp.getValue(pt);
        } else if (it->name == "AMP3N") {
            amp3_n = it->comp.getValue(pt);
        } else if (it->name == "AMP3U") {
            amp3_u = it->comp.getValue(pt);
        } else if (it->name == "PHZ3E") {
            phz3_e = it->comp.getValue(pt);
        } else if (it->name == "PHZ3N") {
            phz3_n = it->comp.getValue(pt);
        } else if (it->name == "PHZ3U") {
            phz3_u = it->comp.getValue(pt);
        }
    }

    // solve for east component
    result_e = vel_e + \
            amp1_e * sin((2 * M_PI / period_1) * t + phz1_e) + \
            amp2_e * sin((2 * M_PI / period_2) * t + phz2_e) + \
            amp3_e * sin((2 * M_PI / period_3) * t + phz3_e);

    // solve for north component
    result_n = vel_n + \
            amp1_n * sin((2 * M_PI / period_1) * t + phz1_n) + \
            amp2_n * sin((2 * M_PI / period_2) * t + phz2_n) + \
            amp3_n * sin((2 * M_PI / period_3) * t + phz3_n);
    // solve for up component
    result_u = vel_u + \
            amp1_u * sin((2 * M_PI / period_1) * t + phz1_u) + \
            amp2_u * sin((2 * M_PI / period_2) * t + phz2_u) + \
            amp3_u * sin((2 * M_PI / period_3) * t + phz3_u);

    ENU u(result_e, result_n, result_u);

    return u;
}

// solves the velocity equation for the given time
// NOTE: requires the appropriate components to have already been loaded by the CompMan
ENU CompMan::solveEquation(ofVec2f p, float t) {
    float vel_n, vel_e, vel_u;
    float amp1_n, amp1_e, amp1_u;
    float amp2_n, amp2_e, amp2_u;
    float amp3_n, amp3_e, amp3_u;
    float phz1_n, phz1_e, phz1_u;
    float phz2_n, phz2_e, phz2_u;
    float phz3_n, phz3_e, phz3_u;
    float period_1 = 12.42;
    float period_2 = 25.82;
    float period_3 = 14.77 * 24; // 14.77 days;
    float result_n, result_e, result_u;

    ofVec2f pt(p.x, p.y);
    std::vector<NamedComp>::iterator it, end;
    for (it = components.begin(), end = components.end(); it != end; ++it) {
        // found a match!
        if (it->name == "E") {
            vel_e = it->comp.getValue(pt);
        } else if (it->name == "N") {
            vel_n = it->comp.getValue(pt);
        } else if (it->name == "U") {
            vel_u = it->comp.getValue(pt);
        // X = 1
        } else if (it->name == "AMP1E") {
            amp1_e = it->comp.getValue(pt);
        } else if (it->name == "AMP1N") {
            amp1_n = it->comp.getValue(pt);
        } else if (it->name == "AMP1U") {
            amp1_u = it->comp.getValue(pt);
        } else if (it->name == "PHZ1E") {
            phz1_e = it->comp.getValue(pt);
        } else if (it->name == "PHZ1N") {
            phz1_n = it->comp.getValue(pt);
        } else if (it->name == "PHZ1U") {
            phz1_u = it->comp.getValue(pt);
        // X = 2
        } else if (it->name == "AMP2E") {
            amp2_e = it->comp.getValue(pt);
        } else if (it->name == "AMP2N") {
            amp2_n = it->comp.getValue(pt);
        } else if (it->name == "AMP2U") {
            amp2_u = it->comp.getValue(pt);
        } else if (it->name == "PHZ2E") {
            phz2_e = it->comp.getValue(pt);
        } else if (it->name == "PHZ2N") {
            phz2_n = it->comp.getValue(pt);
        } else if (it->name == "PHZ2U") {
            phz2_u = it->comp.getValue(pt);
        // X = 3
        } else if (it->name == "AMP3E") {
            amp3_e = it->comp.getValue(pt);
        } else if (it->name == "AMP3N") {
            amp3_n = it->comp.getValue(pt);
        } else if (it->name == "AMP3U") {
            amp3_u = it->comp.getValue(pt);
        } else if (it->name == "PHZ3E") {
            phz3_e = it->comp.getValue(pt);
        } else if (it->name == "PHZ3N") {
            phz3_n = it->comp.getValue(pt);
        } else if (it->name == "PHZ3U") {
            phz3_u = it->comp.getValue(pt);
        }
    }

    // solve for east component
    result_e = vel_e + \
            amp1_e * sin((2 * M_PI / period_1) * t + phz1_e) + \
            amp2_e * sin((2 * M_PI / period_2) * t + phz2_e) + \
            amp3_e * sin((2 * M_PI / period_3) * t + phz3_e);

    // solve for north component
    result_n = vel_n + \
            amp1_n * sin((2 * M_PI / period_1) * t + phz1_n) + \
            amp2_n * sin((2 * M_PI / period_2) * t + phz2_n) + \
            amp3_n * sin((2 * M_PI / period_3) * t + phz3_n);
    // solve for up component
    result_u = vel_u + \
            amp1_u * sin((2 * M_PI / period_1) * t + phz1_u) + \
            amp2_u * sin((2 * M_PI / period_2) * t + phz2_u) + \
            amp3_u * sin((2 * M_PI / period_3) * t + phz3_u);

    ENU u(result_e, result_n, result_u);

    return u;
}
