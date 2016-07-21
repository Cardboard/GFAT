import qbs
import qbs.Process
import qbs.File
import qbs.FileInfo
import qbs.TextFile
import "/home/matthew/of_v0.9.3_linux64_release/libs/openFrameworksCompiled/project/qtcreator/ofApp.qbs" as ofApp

Project{
    property string of_root: '/home/matthew/of_v0.9.3_linux64_release'

    ofApp {
        name: { return FileInfo.baseName(path) }

        files: [
            "src/compman.cpp",
            "src/compman.h",
            "src/component.cpp",
            "src/component.h",
            "src/date.h",
            "src/enu.h",
            "src/ofxPlots/dataSelectedEvent.cpp",
            "src/ofxPlots/dataSelectedEvent.hpp",
            "src/ofxPlots/interactiveObj.cpp",
            "src/ofxPlots/interactiveObj.hpp",
            "src/ofxPlots/ofxPlot.cpp",
            "src/ofxPlots/ofxPlot.hpp",
            "src/ofxPlots/ofxPlotArrow.cpp",
            "src/ofxPlots/ofxPlotArrow.hpp",
            "src/point.h",
            "src/track.cpp",
            "src/track.h",
            "src/worm.cpp",
            "src/worm.h",
            "src/wormman.cpp",
            "src/wormman.h",
            'src/main.cpp',
            'src/ofApp.cpp',
            'src/ofApp.h',
        ]

        // This project is using addons.make to include the addons
        // since it was imported from old code. To change it to include
        // the addons from the qbs file change the following lines to
        // the list of used addons in array format. eg:
        //
        of.addons: [
            'ofxGui',
            'ofxDatGui',
        ]

        // additional flags for the project. the of module sets some
        // flags by default to add the core libraries, search paths...
        // this flags can be augmented through the following properties:
        of.pkgConfigs: []       // list of additional system pkgs to include
        of.includePaths: []     // include search paths
        of.cFlags: []           // flags passed to the c compiler
        of.cxxFlags: []         // flags passed to the c++ compiler
        of.linkerFlags: []      // flags passed to the linker
        of.defines: []          // defines are passed as -D to the compiler
        // and can be checked with #ifdef or #if in the code
        of.frameworks: []       // osx only, additional frameworks to link with the project

        // other flags can be set through the cpp module: http://doc.qt.io/qbs/cpp-module.html
        // eg: this will enable ccache when compiling
        //
        // cpp.compilerWrapper: 'ccache'

        Depends{
            name: "cpp"
        }

        // common rules that parse the include search paths, core libraries...
        Depends{
            name: "of"
        }

        // dependency with the OF library
        Depends{
            name: "openFrameworks"
        }
    }

    references: [FileInfo.joinPaths(of_root, "/libs/openFrameworksCompiled/project/qtcreator/openFrameworks.qbs")]
}
