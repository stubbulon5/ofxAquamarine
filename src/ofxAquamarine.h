
#ifndef _ofxAquamarine
#define _ofxAquamarine

/*

         __                                                      _
        / _|         /\                                         (_)
   ___ | |___  __   /  \   __ _ _   _  __ _ _ __ ___   __ _ _ __ _ _ __   ___
  / _ \|  _\ \/ /  / /\ \ / _` | | | |/ _` | '_ ` _ \ / _` | '__| | '_ \ / _ \
 | (_) | |  >  <  / ____ \ (_| | |_| | (_| | | | | | | (_| | |  | | | | |  __/
  \___/|_| /_/\_\/_/    \_\__, |\__,_|\__,_|_| |_| |_|\__,_|_|  |_|_| |_|\___|
                             | |
                             |_|


                                               built for openFrameworks 0.12.0
                                  maintained by: https://github.com/stubbulon5
*/

#include "ofMain.h"

// Core
#include "./viz/core/exprtk/exprtk.hpp"
#include "./viz/core/machineid/machineid.h"
#include "./viz/core/nlohmann/json.hpp"

// Viz
#include "./viz/App.h"
#include "./viz/Lang.h"
#include "./viz/Coord.h"
#include "./viz/Shared.h"
#include "./viz/Viz.h"
#include "./viz/Icon.h"
#include "./viz/BG.h"
#include "./viz/Elm.h"
#include "./viz/WidgetManager.h"

// Widgets
#include "./viz/widget/WidgetConstants.h"
#include "./viz/widget/WidgetMenu.h"
#include "./viz/widget/WidgetSoundPlayer.h"
#include "./viz/widget/WidgetDebug.h"
#include "./viz/widget/WidgetMenuCollection.h"
#include "./viz/widget/WidgetTable.h"
#include "./viz/widget/Widget.h"
#include "./viz/widget/WidgetEventListener.h"
#include "./viz/widget/WidgetPianoRoll.h"
#include "./viz/widget/WidgetTextEditor.h"
#include "./viz/widget/WidgetClipboardMenu.h"
#include "./viz/widget/WidgetImageView.h"
#include "./viz/widget/WidgetSequencer.h"
#include "./viz/widget/WidgetVideoGrabber.h"
#include "./viz/widget/WidgetCollection.h"
#include "./viz/widget/WidgetMatrix.h"
#include "./viz/widget/WidgetSequencerBase.h"
#include "./viz/widget/WidgetVideoPlayer.h"
#include "./viz/widget/WidgetVideoPlayer.h"
#include "./viz/widget/WidgetMediaPlayerBase.h"
#include "./viz/widget/WidgetSettings.h"
#include "./viz/widget/WidgetWindowTitle.h"
#include "./viz/widget/WidgetColorPicker.h"

// System widgets
#include "./viz/widget/system/WidgetDialog.h"
#include "./viz/widget/system/WidgetFileLoad.h"
#include "./viz/widget/system/WidgetThemeEditor.h"
#include "./viz/widget/system/WidgetFileExplorer.h"
#include "./viz/widget/system/WidgetFileLocationsList.h"
#include "./viz/widget/system/WidgetThemePreview.h"
#include "./viz/widget/system/WidgetFileList.h"
#include "./viz/widget/system/WidgetFileSave.h"
#include "./viz/widget/system/WidgetWidgetList.h"
#include "./viz/widget/system/WidgetAquamarineAbout.h"

// Elements
#include "./viz/widget/element/WidgetElmCheckbox.h"
#include "./viz/widget/element/WidgetElmLabel.h"
#include "./viz/widget/element/WidgetElmTextarea.h"
#include "./viz/widget/element/WidgetElmBreadcrumb.h"
#include "./viz/widget/element/WidgetElmDropdown.h"
#include "./viz/widget/element/WidgetElmProgressBar.h"
#include "./viz/widget/element/WidgetElmTextbox.h"
#include "./viz/widget/element/WidgetElmButton.h"
#include "./viz/widget/element/WidgetElmImage.h"
#include "./viz/widget/element/WidgetElmSlider.h"
#include "./viz/widget/element/WidgetElmXYPad.h"
// #include "./viz/widget/element/WIP/WidgetElmAudioClip.h"

#endif
