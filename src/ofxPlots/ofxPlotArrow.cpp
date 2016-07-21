/**
 * @file ofxPlotArrow.hpp
 * 
 * @author Andrew Cox
 * @version July 5, 2016
 * 
 * Copyright (c) 2016 Andrew Cox
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in 
 * the Software without restriction, including without limitation the rights to use, 
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
 * Software, and to permit persons to whom the Software is furnished to do so, subject 
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR 
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ofxPlotArrow.hpp"

#include "ofMain.h"

/**
 *  @brief Draw an arrow
 * 
 *  @param origin Origin point of the arrow, global pixel coordinates
 *  @param end Tip of the arrow, global pixel coordinates
 *  @param arrowSize size of the arrow, projected onto the arrow stem, pixels
 */
void ofxPlotArrow::draw(ofVec2f origin, ofVec2f end, float arrowSize){

	// Determine arrow orientation
	float angle = 0;
	if(std::abs(end.x - origin.x) < 1e-10){
		// vertical arrow
		angle = ofSign(end.y - origin.y)*PI/2;
	}else{
		// non-vertical arrow
		angle = atan((end.y - origin.y)/(end.x - origin.x));
		
		// quadrant check
		if(end.x - origin.x < 0)
			angle += PI;
	}

	float pointAngle = arrowSpread - angle;
	float pointLen = arrowSize/cos(arrowSpread);	// hypotenuse

	// arrowPtF/B are vectors from the end of the arrow to the corners of the arrow point
	ofVec2f arrowPtF(-pointLen*cos(pointAngle), pointLen*sin(pointAngle));
	ofVec2f arrowPtB(-pointLen*cos(pointAngle + 2*angle), -pointLen*sin(pointAngle + 2*angle));

	// arrowPt_stem is a vector along the arrow stem that spans the length of the arrow point
	ofVec2f arrowPt_stem(pointLen*cos(angle), pointLen*sin(angle));

	ofPushStyle();

	// Draw Stem
	ofPolyline stem;
	stem.addVertex(origin);
	stem.addVertex(end - 0.75*arrowPt_stem);

	ofSetLineWidth(lineWidth);
	ofSetColor(color);
	stem.draw();

	ofPath arrow;
	arrow.moveTo(end);
	arrow.lineTo(end + arrowPtF);
	arrow.lineTo(end + arrowPtB);
	arrow.lineTo(end);

	arrow.setColor(color);
	arrow.draw();

	ofPopStyle();
}//====================================================


/**
 *  @brief Set the arrow point half-angle
 * 
 *  @param angle arrow point half-angle, radians
 */
void ofxPlotArrow::setArrowSpread(float angle){
	arrowSpread = angle;
}//====================================================

/**
 *  @brief Set the arrow color]
 * 
 *  @param c arrow color
 */
void ofxPlotArrow::setColor(ofColor c){
	color = c;
}//====================================================

/**
 *  @brief Set the line width of the arrow stem
 * 
 *  @param w width, in pixels
 */
void ofxPlotArrow::setLineWidth(float w){
	lineWidth = w;
}