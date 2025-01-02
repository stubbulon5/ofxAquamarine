#pragma once

#include "widget/Widget.h"

// Widgets
#include "widget/WidgetDebug.h"
#include "widget/WidgetEventListener.h"
#include "widget/WidgetClipboardMenu.h"
#include "widget/WidgetTable.h"
#include "widget/WidgetMenuCollection.h"
#include "widget/WidgetMatrix.h"
#include "widget/WidgetSequencer.h"
#include "widget/WidgetPianoRoll.h"
#include "widget/WidgetVideoPlayer.h"
#include "widget/WidgetVideoGrabber.h"
#include "widget/WidgetSoundPlayer.h"
#include "widget/WidgetImageView.h"
#include "widget/WidgetSettings.h"
#include "widget/WidgetTextEditor.h"
#include "widget/WidgetColorPicker.h"

// System widgets
#include "widget/system/WidgetFileList.h"
#include "widget/system/WidgetFileLocationsList.h"
#include "widget/system/WidgetFileExplorer.h"
#include "widget/system/WidgetFileLoad.h"
#include "widget/system/WidgetFileSave.h"
#include "widget/system/WidgetThemeEditor.h"
#include "widget/system/WidgetThemePreview.h"
#include "widget/system/WidgetWidgetList.h"
#include "widget/system/WidgetDialog.h"
#include "widget/system/WidgetAquamarineAbout.h"

// Elements
#include "viz/widget/element/WidgetElmCheckbox.h"
#include "viz/widget/element/WidgetElmLabel.h"
#include "viz/widget/element/WidgetElmTextarea.h"
#include "viz/widget/element/WidgetElmBreadcrumb.h"
#include "viz/widget/element/WidgetElmDropdown.h"
#include "viz/widget/element/WidgetElmProgressBar.h"
#include "viz/widget/element/WidgetElmTextbox.h"
#include "viz/widget/element/WidgetElmButton.h"
#include "viz/widget/element/WidgetElmImage.h"
#include "viz/widget/element/WidgetElmSlider.h"
#include "viz/widget/element/WidgetElmXYPad.h"

namespace Aquamarine
{

    // typedef map<string, Widget *(*)(string persistentId, string widgetXML)> widget_map_type;
    // static widget_map_type widgetClassMap;

    class Widgets
    {
  
    // public:

    //     template <typename T>
    //     static string getWidgetTypeIdentifier(T widgetType)
    //     {
    //         // for (auto widgetReg : widgetClassMap)
    //         // {
    //         // }

    //         return "";
    //     }

    //     template <typename T>
    //     static Widget *createWidget(string persistentId, string widgetXML)
    //     {
    //         Widget *createdWidgetPtr = new T(persistentId, widgetXML);
    //         return createdWidgetPtr;
    //     }

    //     template <typename T>
    //     static void registerWidgetType(string widgetTypeIdentifier)
    //     {
    //         widgetClassMap[widgetTypeIdentifier] = &createWidget<T>;
    //     }

    //     static widget_map_type &getRegisteredWidgets()
    //     {
    //         return widgetClassMap;
    //     }

        // static void registerDefaultWidgets()
        // {
        //     registerWidgetType<Widget>("Widget");
        //     registerWidgetType<WidgetDebug>("WidgetDebug");
        //     registerWidgetType<WidgetEventListener>("WidgetEventListener");
        //     registerWidgetType<WidgetMenu>("WidgetMenu");
        //     registerWidgetType<WidgetMenuCollection>("WidgetMenuCollection");
        //     registerWidgetType<WidgetTable>("WidgetTable");
        //     registerWidgetType<WidgetMatrix>("WidgetMatrix");
        //     registerWidgetType<WidgetSequencer>("WidgetSequencer");
        //     registerWidgetType<WidgetPianoRoll>("WidgetPianoRoll");
        //     registerWidgetType<WidgetVideoPlayer>("WidgetVideoPlayer");
        //     registerWidgetType<WidgetVideoGrabber>("WidgetVideoGrabber");
        //     registerWidgetType<WidgetSoundPlayer>("WidgetSoundPlayer");
        //     registerWidgetType<WidgetImageView>("WidgetImageView");
        //     registerWidgetType<WidgetClipboardMenu>("WidgetClipboardMenu");
        //     registerWidgetType<WidgetSettings>("WidgetSettings");
        //     registerWidgetType<WidgetTextEditor>("WidgetTextEditor");
        //     registerWidgetType<WidgetColorPicker>("WidgetColorPicker");

        //     registerWidgetType<WidgetFileList>("WidgetFileList");
        //     registerWidgetType<WidgetFileLocationsList>("WidgetFileLocationsList");
        //     registerWidgetType<WidgetFileExplorer>("WidgetFileExplorer");
        //     registerWidgetType<WidgetFileLoad>("WidgetFileLoad");
        //     registerWidgetType<WidgetFileSave>("WidgetFileSave");
        //     registerWidgetType<WidgetThemeEditor>("WidgetThemeEditor");
        //     registerWidgetType<WidgetThemePreview>("WidgetThemePreview");
        //     registerWidgetType<WidgetWidgetList>("WidgetWidgetList");
        //     registerWidgetType<WidgetDialog>("WidgetDialog");
        //     registerWidgetType<WidgetAquamarineAbout>("WidgetAquamarineAbout");
        // }

        
    };
}
