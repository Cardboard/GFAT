# Geophysical Fluid Analysis Tool

## Purpose
The geophysical fluid analysis tool is a prototype of an interactive environment for visualizing interferometry of geophysical fluids, which might have a multitude of flow components operating on vastly different time scales.

## Build Instructions

So far this code has been tested on XUbuntu 14.x and OSX 10.10.5

###OSX (with XCode)

1. Make sure openFrameworks is installed. Learn how to do that [here](http://openframeworks.cc/download/)
1. Install the DatGui oF addon. Find that [here](https://github.com/braitsch/ofxDatGui)
1. Using the oF projectGenerator, create a new project with the DatGui addon
1. Add all the files in this repo to the source folder. Add all the data to the `bin\data` folder
1. In Build Settings, make sure the following files are added to the build target
 <insert pic of XCode build settings>
1. Build and Run!

###Linux

## Usage

### Data Loading
Since this tool is still a prototype its sample dataset is hardcoded into the source, but with a little understanding of how the velocity component model is put together, other datasets can be swapped in.

The sample data is in binary utm format (add more here). It is important to know that all topographically distributed data is reprojected into cartesian space! If you want to swap in a new dataset make sure it is in cartesian coordinates.

#### Flow Equation:
The sample dataset uses a simple flow model with one secular velocity component and 3 periodic components. These components are assumed to be sinusoidal, and the phase and amplitude information of each component carry the nomeclature SinPhz# and SinAmp# respectively throughout the model reconstruction code.  Data for each of these components are further decomposed into East, North, and Up subcomponents.
```
 // solve for east component
 result_e = vel_e + \
         p1_enabled * (amp1_e * sin((2 * M_PI / period_1) * t + phz1_e)) + \
         p2_enabled * (amp2_e * sin((2 * M_PI / period_2) * t + phz2_e)) + \
         p3_enabled * (amp3_e * sin((2 * M_PI / period_3) * t + phz3_e));

 // solve for north component
 result_n = vel_n + \
         p1_enabled * (amp1_n * sin((2 * M_PI / period_1) * t + phz1_n)) + \
         p2_enabled * (amp2_n * sin((2 * M_PI / period_2) * t + phz2_n)) + \
         p3_enabled * (amp3_n * sin((2 * M_PI / period_3) * t + phz3_n));
         
 // solve for up component
 result_u = vel_u + \
         p1_enabled * (amp1_u * sin((2 * M_PI / period_1) * t + phz1_u)) + \
         p2_enabled * (amp2_u * sin((2 * M_PI / period_2) * t + phz2_u)) + \
         p3_enabled * (amp3_u * sin((2 * M_PI / period_3) * t + phz3_u));
         
 // aggregate components into EastNorthUp object to drive flow viz + plots
 ENU u(result_e, result_n, result_u);
```
This equation can be modified in the `solveEquation()` methods in `CompMan.cpp`, which is the component manager class that aggregates the required data from the input components to drive the flow representation on the topology as well as the plots.

#### Topography:
The sample topography provided is the Rutford Glacier. In the `setup()` method found in `ofApp.cpp`, there are 3 layers of topography defined: the glacier bed, the surface, and the ice thickness.
```
img_topo_bed.load("bedmap2_bed_rutford.png");
img_topo_surface.load("bedmap2_surface_rutford.png");
img_topo_thickness.load("bedmap2_thickness_rutford.png");
```

For the 3d view, the surface topography is doubly utilized as a heightmap. Theoretically other topography can be swapped out without issue as long as all 3 layers are provided, but other resolutions have not been performance tested. Geographic measures of model confidence have also been integrated as topographic layers. GDOP and OBSCOV have been hardcoded for the sample, and are treated the same as the ice surface, thickness, and bed layers.

### Menu Options
The menu is pretty self explanatory and is primarily toggle based. There are controls both for the flow and the topography available in the same menu.

#### Flow Options:
* Toggle on/off model components
* Brush mode: draw flow worms/streamlines onto the topography
* Selection mode: view decomposition plots of up to 2 selected areas (left click to select point 1, right click to select point 2)
* Flow representation options: worms, streamlines, dots
* Flow sampling options: lagrangian, eulerian

#### Topography Options:
* Toggle between 2D/3D
* Toggle on/off topography layers (surface, ice thickness, bed)
* Toggle on/off model confidence layers (GDOP, OBSCOV)


