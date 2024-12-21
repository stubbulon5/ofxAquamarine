
#pragma once
#include "Shared.h"
#include "widget/Widget.h"

namespace Aquamarine
{
    //------------------------------ UI VIZ WIDGET MANAGER CLASS --------------------------------
    class WidgetManager
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

        typedef std::map<std::string, Widget *(*)(string persistentId, string widgetXML)> widget_map_type;
        /* MAP STRING REPRESENTATION <---> ACTUAL CLASS */
        static void bootstrapWidgetMapDefault();
        
        static void addWidget(Widget &widget, bool shouldPersist, string ownerWidgetId, bool transmitOsc);
        static void addWidget(Widget &widget, bool shouldPersist, string ownerWidgetId);
        static void addWidget(Widget &widget, bool shouldPersist);
        static void addWidgetForPopout(Widget &widget, string ownerWidgetId, bool shouldCloseIfOtherWidgetClicked);
        static Widget *loadWidget(string WIDGET_CLASS, string persistentId, string widgetXML);
        static Widget *loadWidget(string widgetXML);
        static Widget *loadWidgetFromFile(string WIDGET_CLASS, string persistentId, string widgetFilePath);
        static Widget *loadWidgetFromFile(string widgetFilePath);
        static void loadWidgetFromFileToExisting(string widgetFilePath, Widget &widget);

        static void loadChildWidgets(Widget &targetWidget, string widgetXML);
        static Widget *loadChildWidget(Widget &targetWidget, string widgetXML);

        static bool removeWidget(Widget &widget);
        static bool removeWidget(string widgetId);
        static bool removeWidgetByPersistentId(string persistentId);
        static void highlightWidget(string widgetId, bool highlight, bool clearOtherHighlights);
        static void clearAllWidgetHighlights();

        static int getCountOfWidgetClassType(string widgetClassType);
        static string getSuggestedNewWidgetPersistentId(string widgetClassType);
        static std::vector<std::reference_wrapper<Widget>> getWidgets();
        static Widget *getFrontWidget();
        static string getWidgetPrettyName(string widgetClassType);
        static Widget *getWidget(string widgetId);
        static Widget *getWidget(Widget *parentWidget, string widgetId);
        static Widget *getWidgetByPersistentId(string persistentId);
        static Widget *getWidgetByPersistentId(Widget *parentWidget, string persistentId);
        static WidgetBase *getWidgetBase(string widgetId, Widget &searchInsideWidget);
        static WidgetBase *getWidgetBase(string widgetId);
        static int getWidgetIndex(string widgetId);
        static int getWidgetZIndex(string widgetId);
        static void removeAllWidgetsExceptMultiple(vector<WidgetBase *> exceptWidgets);
        static void removeAllWidgetsExcept(Widget *exceptWidget);
        static void removeAllWidgets();
        static void moveWidgetToFront(Widget &w);
        static void moveWidgetToBack(Widget &w);
        static void drawWidget(Widget &widget, int widgetZIndex);
        static void drawWidgets();
        static void updateWidget(Widget &widget);
        static void updateWidgets();
        static bool checkShouldCloseOrDelete(Widget *widget);
        static void handleEventReceive(string eventSenderType, string eventSenderId, string eventTargetId, string eventName, string eventXML);
        static void recalculateAllWidgetExpressions();
        static void recalculateWidgetExpressions(Widget &w);
        static bool hasActiveChildWidgets(Widget &parentWidget);
        static int childWidgetCount(Widget &parentWidget);
        static Widget *getActiveParentWidget();
        static WidgetBase *getTargetDropWidget();
        static bool save(string fileName);
        static bool load(string fileName, bool clearExisting);
        static bool load(string fileName, bool clearExisting, Widget *caller);
        static void showSaveDialog(string proposedFilePath, string proposedFileName, const std::function<void(const string &)> &saveButtonPressedCallback);
        static void setTheme(WidgetTheme theme);
        static void initWidgetManager(string applicationName, string applicationVersion, string fileExtention, bool drawBG);
        static void setProjectProperties(string projectName, string absolutePath);
        static ProjectProperties getCurrentProjectProperties();
        static void setCurrentProjectIsDirty(bool val);
        static int getAppMajorVersion(string appVersionString);
        static int getAppMinorVersion(string appVersionString);
        static int getAppPointVersion(string appVersionString);

        static void drawDebugInfo(bool debug);

        static std::vector<std::reference_wrapper<Widget>> mWidgets;
        static widget_map_type getWidgetMap();

        static void bootstrapWidgetMap(widget_map_type map);
        static void showClipboardPopupMenu(int x, int y, string clipboardOperationTargetWidgetId);
        static void hideClipboardPopupMenu();

        static void showModal(Widget *widget, bool center);
        static void hideModal();
        static void centerWidget(Widget *w);

        static WidgetElmTextbox *getTextboxById(string widgetId);

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
        static const string WIDGET_CLASS_COLOR_PICKER;

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