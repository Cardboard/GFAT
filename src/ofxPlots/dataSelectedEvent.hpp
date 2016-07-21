#pragma once

#include "ofMain.h"

class DataSelectedEventArgs : public ofEventArgs {
public:
	DataSelectedEventArgs(){}
	DataSelectedEventArgs(int ix){
		indices.push_back(ix);
	}
	
	std::vector<int> indices;	//!< Indices or IDs of selected data objects
};

class DataSelectedEvent{
public:
	static ofEvent<DataSelectedEventArgs> selected;
	static ofEvent<DataSelectedEventArgs> deselected;
};