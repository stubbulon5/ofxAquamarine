# Aquamarine UI

### Introduction

OpenGL based UI Framework written on top of OpenFrameworks. It is:
- Extensible
- Optimized (target is 60fps)
- Vectorial and scalable
- Supports expression based layout
- Highly configurable
- Supports loading and saving state
- Interprets a very simple markup language similar to HTML


### Show me some bare bones usage:

### TL;DR What's it look like?

### Object hierarchy
```
- uiVizWidgetBase
    + uiVizWidget
    | ---+ uiVizWidgetPopout
    | ---+ uiVizWidgetMenu
    |
    | ---+ your own widget
    |
    + uiVizWidgetElm
        + uiVizWidgetElementTextbox
        ...
```

### Widget Manager
The uiVizWidgetManager static class maintains a collection of widget (pointers) and helps manage their state. It also offers public methods to:
- Save and load widget collection state to/from file
- Return a pointer to a widget by a widget's id
- Remove a particular widget by id or remove all widgets altogether
- Dynamically layout a widget (in relation to other widgets) by providing an expression syntax
  - (eg widget.xExpr="${OTHER_WIDGET.RIGHT}")
```

### Widget Manager
+--------------------+
| uiVizWidgetManager | ----[uiVizWidget w]---> addWidget, w->draw(), etc
+--------------------+
```
Usage of the Widget Manager is however, entirely optional and you may opt for instantiating and drawing widgets manually.  Please see here for full documentation.



### Widget
The uiVizWidget class is a container for modularized functionality. It could be a form containing a collection of elements or a highly complex visualization. It's provides you a core Widget lifecycle and management thereof while giving you the starting point to implement your own core functionality. Please see here for full documentation.


### Elements
The uiVizElm class is the base class for all....


### Debugging
Preprocessor directives:  VIZ_DEBUG_LEVEL
-1 or (none): no debugging
0: core tech / animation
1: core / base widget
2: user widget (3rd party developers use this one)

### Markup syntax reference

# Platform specific project setup guides

Updating a project using projectGenerator will cause some project config / settings to be lost. This means most (if not all) of the below steps need to be performed each time a project is updated using projectGenerator.

## General
- Use the corresponding openFrameworks framework projectGenerator to generate new project
- Inside project, right click src group and add GROUP references (and make sure "Copy items" is not enabled) to the master app_widgets and uiViz folders git repo (this is common code across platforms / devices / sdk's). These groups location group should be set to "Relative to Group"
- Follow any additional instructions below and compile!

- Stick with Legacy build system, see:

## macOS
Update with projectGenerator, then open project.
Locate the OpenFrameworks-Info.plist and add new entry to Information Property list:
```
High Resolution Capable   YES
```

## iOS

REQUIRED DEFINES / Preprocessor Macros:
```
USE_OFX_FONTSTASH2
NANOVG_GLES2_IMPLEMENTATION # (nanoVG / fontStash fonts wont appear without using this...)

NANOVG_GL2_IMPLEMENTATION = TRUE
NANOVG_GLES2 = TRUE
USE_OFX_FONTSTASH2 = TRUE
VIZ_DISABLE_GLEDITOR = TRUE


Done by clicking Top left project, project,
Build Settings, Apple Clang - Preprocessing
- preprocessor Macros

or adding to
Project.xcconfig file!

Build using legacy build:
https://forum.openframeworks.cc/t/xcode-10-of-0-10-1-will-not-build-for-simulators/31234



```

Update with projectGenerator, then open project.
Click the topmost node of the project and ensure target is selected. Click the General Tab. Scroll down to the Linked frameworks and libraries section. And add necessary libraries. For:
```
ofxXMLSettings -> libxml2.tbd
ofxMidi -> CoreFoundation.framework
```

HAving cloned the code boom / cognition repo, left drag the files onto the XCode ide, drop the files under 'src' folder and create GROUP, RELATIVE TO GROUP (don't copy). Making relative to group is acheived by:
1.) After the drop operation into the src folder and creating reference
2.) Left click the uiViz folder and on the right hand pane of Xcode, click the dropdown and select location "Relative to Group"

*Clean build folder -> Use New build system*
Copy / refresh files in:
Macintosh HD⁩ ▸ ⁨Users⁩ ▸ {user} ▸ ⁨projects⁩ ▸ ⁨of_v0.10.1_ios_release⁩ ▸ ⁨libs⁩ ▸ ⁨openFrameworksCompiled⁩ ▸ ⁨lib⁩ ▸ ⁨ios⁩
*Set Deployment Target to 12*

If you're experiencing any issues with openGL check the settings defined under main.mm

## Windows
Follow instructions here : https://openframeworks.cc/setup/msys2/
Very, very NB, you must run mingw32.exe when carrying out these instructions!!

* REQUIRES WINDOWS 10 SDK TO BE INSTALLED:
* https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk


# Documentation setup (Doxygen and GraphViz)
Instructions followed from https://www.youtube.com/watch?v=LZ5E4vEhsKs
https://www.doxygen.nl/manual/starting.html
Install tools:
```
# macOs
brew install doxygen
brew install graphviz

# Linux
 sudo apt-get install doxygen
 sudo apt-get install graphviz
 ```
 ## Generating documentation:
``` 
 cd doc/ && doxygen
x-www-browser file://`pwd`/html/index.html
```