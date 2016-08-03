#include "enuplot.h"

ENUPlot::ENUPlot() {

}

ENUPlot::ENUPlot(CompMan *cm, ofVec2f *pt, ofVec2f *pt2, float *time, string label_plot, string label_x, string label_y) {
    this->cm = cm;
    this->pt = pt;
    this->pt2 = pt2;
    this->time = time;
    // setup labels
    this->label_plot = label_plot;
    this->label_x = label_x;
    this->label_y = label_y;

    empty = true;
    font.load("fonts/AkzidenzGrotesk-LightSC.otf", 8);

    cBackground = ofColor(51);
    cLabels = ofColor(255);
    cNumbers = ofColor(101);
    cData = ofColor(ofColor::lightCoral);
    cData2 = ofColor(ofColor(150));
    cAxes = ofColor(152);
}

// this will be called whenever viewport sizes change
void ENUPlot::setRects(ofRectangle rect) {
    // setup plot & data rectangles
    // -- rect_plot
    this->rect_plot = rect; // rect_plot is used as-is
    // -- rect_data
    float padding_top = 30;
    float padding_left = 60;
    float padding_bottom = 60;
    float padding_right = 30;
    float data_height = (rect.height - padding_top - padding_bottom)/3;
    this->rect_data_e = ofRectangle(rect.x + padding_left, rect.y + padding_top, // x & y
                                  rect.width - padding_left - padding_right, // width
                                  data_height); // height
    this->rect_data_n = ofRectangle(rect.x + padding_left, rect.y + padding_top + data_height, // x & y
                                  rect.width - padding_left - padding_right, // width
                                  data_height); // height
    this->rect_data_u = ofRectangle(rect.x + padding_left, rect.y + padding_top + 2*data_height, // x & y
                                  rect.width - padding_left - padding_right, // width
                                  data_height); // height
}


void ENUPlot::clearData(){
    data_e.clear();
    data_n.clear();
    data_u.clear();
    empty = true;
}

void ENUPlot::clearData2(){
    data_e2.clear();
    data_n2.clear();
    data_u2.clear();
    empty2 = true;
}

// (re)compute data using a specified step, range of x-values, and tick scales
void ENUPlot::refreshData(float step, float xstart, float xend, float xscale, float yscale) {
    if (pt->x != -1 && pt->y != -1) {
        // clear the previous data so we can get a fresh start
        clearData();

        ENU result;

        // find min & max of data, for each component: E, N, U
        x_min_e = xstart;
        x_max_e = xend;
        x_min_n = xstart;
        x_max_n = xend;
        x_min_u = xstart;
        x_max_u = xend;

        for (float i = xstart; i <= xend; i+= step) {
            // get value at a selected point
            result = cm->solveEquation(*pt, i);

            // EAST
            if (i == xstart || result.east < y_min_e)
                y_min_e = result.east;
            if (i == xstart || result.east > y_max_e)
                y_max_e = result.east;
            data_e.push_back(DataPt(i, result.east));

            // NORTH
            if (i == xstart || result.north < y_min_n)
                y_min_n = result.north;
            if (i == xstart || result.north > y_max_n)
                y_max_n = result.north;
            data_n.push_back(DataPt(i, result.north));


            // UP
            if (i == xstart || result.up < y_min_u)
                y_min_u = result.up;
            if (i == xstart || result.up > y_max_u)
                y_max_u = result.up;
            data_u.push_back(DataPt(i, result.up));
        }

        if (xscale != 0) {
            this->xscale = xscale;
        } else {
            this->xscale = (xend - xstart) / 10; //
        }
        if (yscale != 0) {
            this->yscale_e = yscale;
            this->yscale_n = yscale;
            this->yscale_u = yscale;
        } else {
            this->yscale_e = abs(y_max_e - y_min_e) / 12.f;
            this->yscale_n = abs(y_max_n - y_min_n) / 12.f;
            this->yscale_u = abs(y_max_u - y_min_u) / 12.f;
        }
    empty = false;
    }
    if (pt2->x != -1 && pt2->y != -1) {
        // clear the previous data so we can get a fresh start
        clearData2();

        ENU result;

        // find min & max of data, for each component: E, N, U
        x_min_e = xstart;
        x_max_e = xend;
        x_min_n = xstart;
        x_max_n = xend;
        x_min_u = xstart;
        x_max_u = xend;

        for (float i = xstart; i <= xend; i+= step) {
            // get value at a selected point
            result = cm->solveEquation(*pt2, i);

            // EAST
            if (result.east < y_min_e)
                y_min_e = result.east;
            if (result.east > y_max_e)
                y_max_e = result.east;
            data_e2.push_back(DataPt(i, result.east));

            // NORTH
            if (result.north < y_min_n)
                y_min_n = result.north;
            if (result.north > y_max_n)
                y_max_n = result.north;
            data_n2.push_back(DataPt(i, result.north));


            // UP
            if (result.up < y_min_u)
                y_min_u = result.up;
            if (result.up > y_max_u)
                y_max_u = result.up;
            data_u2.push_back(DataPt(i, result.up));
        }

        if (xscale != 0) {
            this->xscale = xscale;
        } else {
            this->xscale = (xend - xstart) / 10; //
        }
        if (yscale != 0) {
            this->yscale_e = yscale;
            this->yscale_n = yscale;
            this->yscale_u = yscale;
        } else {
            this->yscale_e = abs(y_max_e - y_min_e) / 12.f;
            this->yscale_n = abs(y_max_n - y_min_n) / 12.f;
            this->yscale_u = abs(y_max_u - y_min_u) / 12.f;
        }
    empty2 = false;
    }
}

void ENUPlot::draw() {
    // draw bounding rectangle for entire plot
    ofSetColor(cBackground);
    ofDrawRectangle(rect_plot);

    float text_height = font.stringHeight("tlypghfbd");
    float padding_title = font.stringWidth(label_plot)/2;
    // -- plot label
    font.drawString(label_plot, \
                    rect_plot.x + rect_plot.width/2 - padding_title, rect_plot.y - text_height);

    drawPlot(&rect_data_e, &data_e, &data_e2, x_min_e, x_max_e, y_min_e, y_max_e, yscale_e, "East");
    drawPlot(&rect_data_n, &data_n, &data_n2, x_min_n, x_max_n, y_min_n, y_max_n, yscale_n, "North");
    drawPlot(&rect_data_u, &data_u, &data_u2, x_min_u, x_max_u, y_min_u, y_max_u, yscale_u, "Up");

    // set the default color back to white
    ofSetColor(255);
}

void ENUPlot::drawPlot(ofRectangle *rect, vector<DataPt> *data, vector<DataPt> *data2, float x_min, float x_max, float y_min, float y_max, float yscale, string label) {
    // draw inner rectangle bounded by the x and y axes (and their invisible opposites)
    ofSetColor(cBackground);
    ofDrawRectangle(*rect);

    // draw x & y axes
    ofSetColor(cAxes);
    ofSetLineWidth(2);

    // -- x axis
    ofDrawLine(rect->x, rect->y + rect->height, \
               rect->x + rect->width, rect->y + rect->height);
    // -- y axis
    ofDrawLine(rect->x, rect->y, \
               rect->x, rect->y + rect->height);

    // draw x & y labels
    ofSetColor(cLabels);
    float text_height = font.stringHeight("tlypghfbd");
    float padding_x = font.stringWidth(label_x)/2;
    float padding_y = font.stringWidth(label) + 8;

    // -- x label
    font.drawString(label_x, \
                    rect->x + rect->width / 2 - padding_x, rect->y + rect->height + 3*text_height);
    // -- y label
    font.drawString(label, \
                       rect->x - padding_y, rect->y + rect->height/2 - text_height/2);


    if (!empty) {
        // draw data & ticks
        vector<DataPt>::iterator it;

        // return ((limitMax - limitMin) * (valueIn - baseMin) / (baseMax - baseMin)) + limitMin;

        // draw data points
        for (it = data->begin(); it != data->end(); it++) {
            if (round(*time) == round(it->xval)) {
                ofSetColor(ofColor(100, 100, 255));
                ofDrawCircle(rect->x + ((rect->width) * (it->xval - x_min) / (x_max - x_min)), // x
                             rect->y + ((rect->height) - ((rect->height) * (it->yval - y_min) / (y_max - y_min))), // y
                             2); // circle size
            } else {
                ofSetColor(cData);
                ofDrawCircle(rect->x + ((rect->width) * (it->xval - x_min) / (x_max - x_min)), // x
                             rect->y + ((rect->height) - ((rect->height) * (it->yval - y_min) / (y_max - y_min))), // y
                             0.5); // circle size
            }
        }

        if (!empty2) {
            // draw data & ticks
            vector<DataPt>::iterator it;

            // return ((limitMax - limitMin) * (valueIn - baseMin) / (baseMax - baseMin)) + limitMin;

            // draw data points
            for (it = data2->begin(); it != data2->end(); it++) {
                if (round(*time) == round(it->xval)) {
                    ofSetColor(ofColor(200, 200, 50));
                    ofDrawCircle(rect->x + ((rect->width) * (it->xval - x_min) / (x_max - x_min)), // x
                                 rect->y + ((rect->height) - ((rect->height) * (it->yval - y_min) / (y_max - y_min))), // y
                                 2); // circle size
                } else {
                    ofSetColor(cData2);
                    ofDrawCircle(rect->x + ((rect->width) * (it->xval - x_min) / (x_max - x_min)), // x
                                 rect->y + ((rect->height) - ((rect->height) * (it->yval - y_min) / (y_max - y_min))), // y
                                 0.5); // circle size
                }
            }
        }


        // draw ticks and their value labels
        ofSetLineWidth(2);
        float line_size = 12;
        // -- x ticks
        float tick_x;
        string tick_str;
        int cnt = 0;
        for (float i = x_min; i <= x_max; i += xscale) {
            tick_x = (rect->width) * (i - x_min) / (x_max - x_min);
            // draw larger lines every so many ticks
            ofSetColor(cAxes);
            if (cnt % 2 == 0 || cnt == 0) {
                stringstream rounded;
                rounded << fixed << setprecision(1) << i;
                tick_str = ofToString(rounded.str());
                ofDrawLine(tick_x + rect->x, rect->y + rect->height - line_size, tick_x + rect->x, rect->y + rect->height);
                ofSetColor(cNumbers);
                font.drawString(ofToString(tick_str), tick_x + rect->x - font.stringWidth(tick_str)/2, rect->y + rect->height + font.stringHeight("!") + 2);
            } else {
                ofDrawLine(tick_x + rect->x, rect->y + rect->height - line_size/2, tick_x + rect->x, rect->y + rect->height);
            }
            cnt++;
        }

        // -- y ticks
        float tick_y;
        cnt = 0;
        for (float i = y_min; i <= y_max; i += yscale) {
            tick_y = (rect->height) - (rect->height) * (i - y_min) / (y_max - y_min);
            // draw larger lines every so many ticks
            ofSetColor(cAxes);
            if (cnt % 4 == 0 || cnt == 0) {
                stringstream rounded;
                rounded << fixed << setprecision(3) << i;
                tick_str = ofToString(rounded.str());
                ofDrawLine(rect->x, tick_y + rect->y, rect->x + line_size, tick_y + rect->y);
                ofSetColor(cNumbers);
                font.drawString(ofToString(tick_str), rect->x - font.stringWidth(tick_str) - 2, rect->y + tick_y + font.stringHeight("!")/2);
            } else {
                ofDrawLine(rect->x, tick_y + rect->y, rect->x + line_size/2, tick_y + rect->y);
            }
            cnt++;
        }
    }
}
