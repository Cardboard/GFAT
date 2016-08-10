# Geophysical Fluid Analysis Tool

## Purpose
The geophysical fluid analysis tool is a prototype of an interactive environment for visualizing interferometry of geophysical fluids, which might have a multitude of flow components operating on vastly different time scales.

## Build Instructions

So far this code has been tested on XUbuntu 14.x and OSX 10.10.5

###OSX (with XCode)

1. Make sure openFrameworks is installed. Learn how to do that [here](http://openframeworks.cc/download/)
1. Install the DatGui oF addon. Find that [here](https://github.com/braitsch/ofxDatGui)
1. Using the oF projectGenerator, create a new project with the DatGui addon
1. Add all the files in this repo to the source folder. Add all the data to the data folder
1. In Build Settings, make sure the following files are added to the build target
 <insert pic of XCode build settings>
1. Build and Run!

###Linux

## Usage

### Data Loading
Since this tool is still a prototype its sample dataset is hardcoded into the source, but with a little understanding of how the velocity component model is put together, other datasets can be swapped in


