#pragma once
#include <string>
#include "Shared.h"
#include "WidgetManager.h"

namespace Aquamarine
{
    class App
    {
    public:
        static void BOOTSTRAP(
            string name,
            WidgetManager::widget_map_type = {},
            map<string, Icon *> customIconMap = {},
            string version = "1.0.0",
            string documentFileExtension = "xml",
            string defaultLanguagXML = "<LANGUAGE name='english'><abc>Hello world!</abc></LANGUAGE>");

        static string APPLICATION_NAME();
        static string APPLICATION_NAME_FOR_FILE_SYSTEM();
        static string APPLICATION_VERSION();
        static string DOCUMENT_FILE_EXTENSION();
        static string APPLICATION_SETTINGS_FOLDER();
        static string APPLICATION_CACHE_FOLDER();
        static string APPLICATION_SETTINGS_FILE();
        static string APPLICATION_SETTINGS_FILE_FULL_PATH();
        static int getAppMajorVersion();
        static int getAppMinorVersion();
        static int getAppPointVersion();

#ifdef TARGET_OF_IOS
        static void setIOSGetDocumentsDirectory(std::string val);
        static std::string getIOSGetDocumentsDirectory();
#endif

    private:
        static string mAPPLICATION_NAME;
        static string mAPPLICATION_NAME_FOR_FILE_SYSTEM;
        static string mAPPLICATION_VERSION;
        static string mDOCUMENT_FILE_EXTENSION;
        static string mDEFAULT_LANGUAGE_XML;
    };

}