# Aquamarine UI

## Introduction

Aquamarine is an OpenGL based UI Framework written on top of OpenFrameworks. It is:
- Extensible
- Optimized (target is 60fps)
- Vectorial and scalable
- Supports expression based layout
- Highly configurable, including full control over theme
- Supports loading and saving state
- Interprets a very simple markup language similar to HTML

## Development
[PLEASE NOTE: plenty of examples coming SOON!]
This is a sole development, but I welcome collaboration and input. Have a feature request? You can maybe tempt me with coffee :-) [Buy me a coffee!](https://buymeacoffee.com/stubbulon5)

## TL;DR What's it look like?


## Setup
VSCode is the preferred IDE. All instructions are written based on the assumption that it is being used. 

To start using the addon, add the ofxAquamarine addon to the `addons.make` file and unzip `addons.zip` to your `/<OF_ROOT>/addons` folder.

### Intellisense
Using macOs, set `"compilerPath": "/usr/bin/clang"`, the inside the `.vscode/c_cpp_properties.json` 

### New project
If you would like to bootstrap from a "pre-baked" empty project, copy `example-VSCode-project` into your `/<OF_ROOT>/apps/myApps/` folder and rename the folder.


## Show me some bare bones usage:
```

```



# Platform specific project setup guides

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



HAving cloned the code boom / cognition repo, left drag the files onto the XCode ide, drop the files under 'src' folder and create GROUP, RELATIVE TO GROUP (don't copy). Making relative to group is acheived by:
1.) After the drop operation into the src folder and creating reference
2.) Left click the Viz folder and on the right hand pane of Xcode, click the dropdown and select location "Relative to Group"

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



## Documentation setup (Doxygen and GraphViz)
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

# Generate  Aquamarine UI Documentation
``` 
cd src/viz/doc/ && doxygen && x-www-browser file://`pwd`/html/index.html && cd ../../../
```