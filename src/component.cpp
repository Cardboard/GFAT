#include "component.h"

Component::Component ()
{
}

Component::~Component() {
}

void Component::setInfo(string n, int s, int l)
{
    // set array-related variables
    name = n;
    samples = s;
    lines = l;

    //pos.set(0, 0);

    min = 999999;
    max = 999999;
    array.resize(lines);
      for (int i = 0; i < lines; ++i)
        array[i].resize(samples);

  threshold = 0.01; // threshold for drawing values as the "zero" color
  // click-related variables
  click_i_x = -1;
  click_i_y = -1;
}

void Component::setInfo(int w, int h)
{
    name = "unnamed";

    setInfo(w, h);
}

void Component::setPos(float x, float y) {
    pos.x = x;
    pos.y = y;
}

// TODO: set scale and whatnot
void Component::setSize(int w, int h) {
    width = w;
    height = h;
}

// sets size proportionatelly based on given width
void Component::setSize(int w) {
    width = w;
    scale = samples / w;
    height = lines / scale;
}

void Component::checkClicks(int x, int y) {
    // first check if somewhere on the image's rectangle was clicked
    if (x > pos.x && x < pos.x + width && y > pos.y && y < pos.y + height) {
        // now check if a non-nan value was clicked, taking scale into account
        float rel_x = x - pos.x;
        float rel_y = y - pos.y;
        float x_index = rel_x * scale;
        float y_index = rel_y * scale;
        float val = array[(int)y_index][(int)x_index];
        if (!isnan(val)) {
            click_i_x = x_index;
            click_i_y = y_index;
            click_pos.x = x;
            click_pos.y = y;
            //highlightPixel();
        } else {
            click_i_x = -1;
            click_i_y = -1;
        }
    } else {
        click_i_x = -1;
        click_i_y = -1;
   }
}

void Component::highlightPixel() {
    //ofSetColor(ofColor(150, 240, 100));
    //ofDrawCircle(click_pos.x, click_pos.y, );
    //ofSetColor(ofColor(255,255,255));
    //overlay.setColor(click_x, click_y, ofColor(0, 255, 0));
    //overlay.update();
}

float Component::getValue(float x, float y) {
    if (x > pos.x && x < pos.x + width && y > pos.y && y < pos.y + height) {
        float rel_x = x - pos.x;
        float rel_y = y - pos.y;
        float x_index = rel_x * scale;
        float y_index = rel_y * scale;
        float val = array[(int)y_index][(int)x_index];
        if (!isnan(val)) return val;
        return 0;
    }
    return 0; // val == nan (returning this shouldn't be possible since we kill worms that touch nan values)
}

float Component::getValue(ofVec2f pt) {
    if (pt.x > pos.x && pt.x < pos.x + width && pt.y > pos.y && pt.y < pos.y + height) {
        float rel_x = pt.x - pos.x;
        float rel_y = pt.y - pos.y;
        float x_index = rel_x * scale;
        float y_index = rel_y * scale;
        float val = array[(int)y_index][(int)x_index];
        if (!isnan(val)) return val;
        return 0;
    }
    return 0; // val == nan (returning this shouldn't be possible since we kill worms that touch nan values)
}

void Component::circlePixels() {
    if (click_i_x != -1 && click_i_y != -1) {
        float val = array[click_i_y][click_i_x];
        /*
        if (val < -threshold) {
            ofSetColor(150, 0, 0, 150);
        } else if (val > threshold) {
            ofSetColor(0, 0, 150, 150);
        } else if (val >= -threshold && val <= threshold) {
            ofSetColor(255, 255, 255, 150);
        }
        */
        ofSetColor(50,250,250,255);

        ofDrawCircle(click_pos.x, click_pos.y, 10);

        // make everything else draw normally
        ofSetColor(255,255,255);
    }
}

void Component::loadAndConvert(string filename)
{
    ifstream file(filename, ios::ate | ios::binary);

    if (file) {
        cout << "loadAndConvert: Successfully opened file " << filename << endl;
        streampos length = file.tellg();
        file.seekg(0, ios::beg);

        float *values;
        values = new float [samples*lines];

        int line_length = 0;
        int cur_row = 0;

        cout << "  length: " << length <<endl;

        file.read((char *)values, length);

        float val;
        for (int i=0; i<samples*lines; i++) {
            val = values[i];
            array[cur_row][i % samples] = val;

            // for making 1D 'values' into a 2D 'array'
            line_length++;
            if (line_length >= samples) {
                cur_row += 1;
                line_length = 0;
            }

            // update min and max
            if (!isnan(val) && (val < min || min == 999999)) {
                min = val;
            } else if (!isnan(val) && (val > max || max == 999999)) {
                max = val;
            }
        }
        cout << "  min: " << min << ", max: " << max << endl;
    }
    file.close();
}

void Component::setupImage() {
    img.allocate(samples, lines, OF_IMAGE_COLOR_ALPHA);
    overlay.allocate(samples, lines, OF_IMAGE_COLOR_ALPHA);
    overlay.setColor(ofColor(0,0,0,0));

    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < samples; j++) {
            float val = array[i][j];
            ofColor color;
            if (isnan(val)) {
                color = ofColor(0, 0, 0, 0);
                overlay.setColor(j, i, ofColor(120, 120, 120, 100));
            }
            else if (val < -threshold) {
                color = ofColor(abs(val) * abs(255 / min), 0, 0);
            }
            else if (val > threshold) {
                color = ofColor(0, 0, val * (255 / max));
            }
            else {
                color = ofColor(0, 0, 0);
            }
            img.setColor(j, i, color);
        }
    }
    cout << "setupImage: Done setting up image for '" << name << "'" << endl;
    img.resize(width, height);
    img.update();
    overlay.resize(width, height);
    overlay.update();
}
