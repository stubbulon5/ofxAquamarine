//
//  uiViz.h
//  bloom-macos
//
//
#pragma once
#include "uiVizShared.h"
#include "widget/uiVizWidget.h"

namespace Aquamarine
{
    //------------------------------ UI VIZ WIDGET MANAGER CLASS --------------------------------
    class uiVizWidgetManager
    {

    public:
        struct ApplicationProperties
        {
            string applicationName = "ApplicationName";
            string applicationVersion = "0.0.0";
            string fileExtension = "xml";
            ApplicationProperties(string applicationName, string applicationVersion, string fileExtension) : applicationName(applicationName), applicationVersion(applicationVersion), fileExtension(fileExtension) {};
        };

        struct ProjectProperties
        {
            string projectName = "";
            string absolutePath = "";
            string path = "";
            string fileName = "";
            bool isDirty = false;
            ProjectProperties(string projectName, string absolutePath, string path, string fileName) : projectName(projectName), absolutePath(absolutePath), path(path), fileName(fileName) {};
        };

        typedef std::map<std::string, uiVizWidget *(*)(string persistentId, string widgetXML)> widget_map_type;

        static void addWidget(uiVizWidget &widget, bool shouldPersist, string ownerWidgetId, bool transmitOsc);
        static void addWidget(uiVizWidget &widget, bool shouldPersist, string ownerWidgetId);
        static void addWidget(uiVizWidget &widget, bool shouldPersist);
        static void addWidgetForPopout(uiVizWidget &widget, string ownerWidgetId, bool shouldCloseIfOtherWidgetClicked);
        static uiVizWidget *loadWidget(string WIDGET_CLASS, string persistentId, string widgetXML);
        static uiVizWidget *loadWidget(string widgetXML);
        static uiVizWidget *loadWidgetFromFile(string WIDGET_CLASS, string persistentId, string widgetFilePath);
        static uiVizWidget *loadWidgetFromFile(string widgetFilePath);
        static void loadWidgetFromFileToExisting(string widgetFilePath, uiVizWidget &widget);

        static void loadChildWidgets(uiVizWidget &targetWidget, string widgetXML);
        static uiVizWidget *loadChildWidget(uiVizWidget &targetWidget, string widgetXML);

        static bool removeWidget(uiVizWidget &widget);
        static bool removeWidget(string widgetId);
        static bool removeWidgetByPersistentId(string persistentId);
        static void highlightWidget(string widgetId, bool highlight, bool clearOtherHighlights);
        static void clearAllWidgetHighlights();

        static int getCountOfWidgetClassType(string widgetClassType);
        static string getSuggestedNewWidgetPersistentId(string widgetClassType);
        static std::vector<std::reference_wrapper<uiVizWidget>> getWidgets();
        static uiVizWidget *getFrontWidget();
        static string getWidgetPrettyName(string widgetClassType);
        static uiVizWidget *getWidget(string widgetId);
        static uiVizWidget *getWidget(uiVizWidget *parentWidget, string widgetId);
        static uiVizWidget *getWidgetByPersistentId(string persistentId);
        static uiVizWidget *getWidgetByPersistentId(uiVizWidget *parentWidget, string persistentId);
        static uiVizWidgetBase *getWidgetBase(string widgetId, uiVizWidget &searchInsideWidget);
        static uiVizWidgetBase *getWidgetBase(string widgetId);
        static int getWidgetIndex(string widgetId);
        static int getWidgetZIndex(string widgetId);
        static void removeAllWidgetsExceptMultiple(vector<uiVizWidgetBase *> exceptWidgets);
        static void removeAllWidgetsExcept(uiVizWidget *exceptWidget);
        static void removeAllWidgets();
        static void moveWidgetToFront(uiVizWidget &w);
        static void moveWidgetToBack(uiVizWidget &w);
        static void drawWidget(uiVizWidget &widget, int widgetZIndex);
        static void drawWidgets();
        static void updateWidget(uiVizWidget &widget);
        static void updateWidgets();
        static bool checkShouldCloseOrDelete(uiVizWidget *widget);
        static void handleEventReceive(string eventSenderType, string eventSenderId, string eventTargetId, string eventName, string eventXML);
        static void recalculateAllWidgetExpressions();
        static void recalculateWidgetExpressions(uiVizWidget &w);
        static bool hasActiveChildWidgets(uiVizWidget &parentWidget);
        static int childWidgetCount(uiVizWidget &parentWidget);
        static uiVizWidget *getActiveParentWidget();
        static uiVizWidgetBase *getTargetDropWidget();
        static bool save(string fileName);
        static bool load(string fileName, bool clearExisting);
        static bool load(string fileName, bool clearExisting, uiVizWidget *caller);
        static void showSaveDialog(string proposedFilePath, string proposedFileName, const std::function<void(const string &)> &saveButtonPressedCallback);
        static void setTheme(uiVizWidgetTheme theme);
        static void initWidgetManager(string applicationName, string applicationVersion, string fileExtention);
        static void setProjectProperties(string projectName, string absolutePath);
        static ProjectProperties getCurrentProjectProperties();
        static void setCurrentProjectIsDirty(bool val);
        static int getAppMajorVersion(string appVersionString);
        static int getAppMinorVersion(string appVersionString);
        static int getAppPointVersion(string appVersionString);

        static void drawDebugInfo(bool debug);

        static std::vector<std::reference_wrapper<uiVizWidget>> mWidgets;

        /* MAP STRING REPRESENTATION <---> ACTUAL CLASS */
        static void bootstrapWidgetMapDefault();
        static widget_map_type getWidgetMap();

        static void bootstrapWidgetMap(widget_map_type map);
        static void showClipboardPopupMenu(int x, int y, string clipboardOperationTargetWidgetId);
        static void hideClipboardPopupMenu();

        static void showModal(uiVizWidget *widget, bool center);
        static void hideModal();
        static void centerWidget(uiVizWidget *w);

        static uiVizWidgetElmTextbox *getTextboxById(string widgetId);

        /* DEFINE ALL SUPPORTED WIDGET CLASS NAME HERE */
        static const string WIDGET_CLASS;
        static const string WIDGET_CLASS_MENU;
        static const string WIDGET_CLASS_MENU_COLLECTION;
        static const string WIDGET_CLASS_TABLE;
        static const string WIDGET_CLASS_MATRIX;
        static const string WIDGET_CLASS_SEQUENCER;
        static const string WIDGET_CLASS_PIANO_ROLL;
        static const string WIDGET_CLASS_VIDEO_PLAYER;
        static const string WIDGET_CLASS_VIDEO_GRABBER;
        static const string WIDGET_CLASS_SOUND_PLAYER;
        static const string WIDGET_CLASS_IMAGE_VIEW;
        static const string WIDGET_CLASS_DEBUG;
        static const string WIDGET_CLASS_EVENT_LISTENER;
        static const string WIDGET_CLASS_CLIPBOARD_MENU;
        static const string WIDGET_CLASS_SETTINGS;
        static const string WIDGET_CLASS_TEXT_EDITOR;

        static const string WIDGET_CLASS_FILE_LIST;
        static const string WIDGET_CLASS_FILE_LOCATIONS_LIST;
        static const string WIDGET_CLASS_FILE_EXPLORER;
        static const string WIDGET_CLASS_FILE_LOAD;
        static const string WIDGET_CLASS_FILE_SAVE;
        static const string WIDGET_CLASS_THEME_EDITOR;
        static const string WIDGET_CLASS_THEME_PREVIEW;
        static const string WIDGET_CLASS_WIDGET_LIST;
        static const string WIDGET_CLASS_DIALOG;
    };

}