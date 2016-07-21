/**
 *  @file interactiveObj.hpp
 *  @brief A class that provides basic functionality for any interactive object.
 *  
 * @author Andrew Cox
 * @version July 7, 2016
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

#pragma once

#include "ofMain.h"

// Forward Declarations
class DataSelectedEventArgs;

class InteractiveObj{
public:
	InteractiveObj();
	virtual ~InteractiveObj();
	
	virtual void setup();
	virtual void update();
	virtual void draw();
	
	void enableKeyInput();
	void enableMouseInput();
	void disableKeyInput();
	void disableMouseInput();

	virtual void setBGColor(ofColor);
	virtual void setEdgeColor(ofColor);
	virtual void setFont(ofTrueTypeFont);
	virtual void setPosition(float, float);
	void setPosition(ofVec2f);
	virtual void setSize(float, float);
	void setSize(ofVec2f);

	virtual void dataSelected(DataSelectedEventArgs&);
	virtual void dataDeselected(DataSelectedEventArgs&);
	
	virtual void mouseMoved(ofMouseEventArgs&);
	virtual void mousePressed(ofMouseEventArgs&);
	virtual void mouseReleased(ofMouseEventArgs&);
	virtual void mouseDragged(ofMouseEventArgs&);

	virtual void keyPressed(ofKeyEventArgs&);
	virtual void keyReleased(ofKeyEventArgs&);

protected:

	bool areEventsSet = false;			//!< Whether or not the events have been set
	bool isDrawingOutline = false;		//!< Whether or not to draw an outline
	bool isMouseInputEnabled = false;	//!< Whether or not the user can interact with the plot using the mouse
	bool isMousePressedInside = false;	//!< Whether or not the mouse was pressed while inside the plot
	bool isMouseDragged = false;		//!< Whether or not the mouse is being dragged
	bool isKeyInputEnabled = false;		//!< Whether or not the use can interact with the plot using the keyboard

	int heldKey = 0;					//!< Identifies which key is currently held down

	float mouseX, mouseY;				//!< Store current mouse coordinates
	ofVec2f mousePressedPt;				//!< Last point where the mouse was pressed, screen coordinates

	ofCoreEvents *events;				//!< Pointer to events object

	ofColor bgColor = ofColor(50, 50, 50, 0.85*255);		//!< Background color
	ofColor edgeColor = ofColor(200, 200, 200, 0.95*255);	//!< Edge color

	ofRectangle viewport = ofRectangle(0, 0, 350, 350);		//!< Describes the area the object occupies in screen space
	ofTrueTypeFont font;									//!< Font used to render text

	void setEvents(ofCoreEvents&);
};