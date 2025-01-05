#include "App.h"

using namespace std;

namespace Aquamarine
{
    string App::mAPPLICATION_NAME = "myApp";
    string App::mAPPLICATION_NAME_FOR_FILE_SYSTEM = "myApp";
    string App::mAPPLICATION_VERSION = "1.0.0";
    string App::mDOCUMENT_FILE_EXTENSION = "xml";
    string App::mDEFAULT_LANGUAGE_XML = "<LANGUAGE name='english'><abc>Hello world!</abc></LANGUAGE>";

    void App::BOOTSTRAP(
        string name,
        WidgetManager::widget_map_type customWidgetMap,
        map<string, Icon *> customIconMap,
        string version,
        string documentFileExtension,
        string defaultLanguagXML)
    {

#ifdef TARGET_OSX
        // Use the "internal" data folder if it exitst
        string dataPathRoot = ofFilePath::getCurrentExeDir() + "data/";
        if (ofDirectory::doesDirectoryExist(dataPathRoot, false))
        {
            ofSetDataPathRoot(dataPathRoot);
        }
#endif

#if VIZ_DEBUG_LEVEL > -1
        ofSetLogLevel(ofLogLevel::OF_LOG_NOTICE);
#else
        ofSetLogLevel(ofLogLevel::OF_LOG_SILENT);
#endif

        mAPPLICATION_NAME = name;
        mAPPLICATION_NAME_FOR_FILE_SYSTEM = Shared::safeFileName(mAPPLICATION_NAME);
        mAPPLICATION_VERSION = version;
        mDOCUMENT_FILE_EXTENSION = documentFileExtension;
        mDEFAULT_LANGUAGE_XML = defaultLanguagXML;

        ofDirectory::createDirectory(App::APPLICATION_SETTINGS_FOLDER(), true, false);
        ofDirectory::createDirectory(App::APPLICATION_CACHE_FOLDER(), true, false);

        json settings =
            Shared::initSettingsFile(
                App::APPLICATION_SETTINGS_FOLDER(),
                App::APPLICATION_SETTINGS_FILE());


        float userInterfaceScaling = Shared::getDefaultScaling();
        string language = "english";
        int userExperience = Shared::getDefaultFPS();
        bool useFbo = true;
        bool showFps = false;
        string fontPath = "fonts/Verdana.ttf";
        bool autoLoadMostRecentProject = true;
        string themeName = "System";

        settings["settings"]["userInterfaceScaling"].get_to(userInterfaceScaling);
        settings["settings"]["language"].get_to(language);
        settings["settings"]["userExperience"].get_to(userExperience);
        settings["settings"]["useFbo"].get_to(useFbo);
        settings["settings"]["showFps"].get_to(showFps);
        settings["settings"]["fontPath"].get_to(fontPath);
        settings["settings"]["autoLoadMostRecentProject"].get_to(autoLoadMostRecentProject);
        settings["settings"]["themeName"].get_to(themeName);

        Shared::configure(
            userInterfaceScaling,
            language,
            userExperience,
            useFbo,
            showFps,
            fontPath,
            autoLoadMostRecentProject,
            themeName
        );

        Shared::langLoadXML(App::mDEFAULT_LANGUAGE_XML);

        WidgetManager::initWidgetManager(name,
                                         version,
                                         documentFileExtension,
                                         true);

        WidgetManager::bootstrapWidgetMap(customWidgetMap);
        IconCache::bootstrapIconMap(Shared::getViz()->getScale(), customIconMap);
    }

    string App::APPLICATION_NAME()
    {
        return mAPPLICATION_NAME;
    }
    string App::APPLICATION_NAME_FOR_FILE_SYSTEM()
    {
        return mAPPLICATION_NAME_FOR_FILE_SYSTEM;
    }
    string App::APPLICATION_VERSION()
    {
        return mAPPLICATION_VERSION;
    }
    string App::DOCUMENT_FILE_EXTENSION()
    {
        return mDOCUMENT_FILE_EXTENSION;
    }

    string App::APPLICATION_SETTINGS_FOLDER()
    {
#ifdef TARGET_OF_IOS
        return ofFilePath::join(getIOSGetDocumentsDirectory(), "." + App::APPLICATION_NAME_FOR_FILE_SYSTEM());
#else
        return ofFilePath::join(ofFilePath::getUserHomeDir(), "." + App::APPLICATION_NAME_FOR_FILE_SYSTEM()); // eg /Users/abc/.myapp
#endif
    }

    string App::APPLICATION_CACHE_FOLDER()
    {
        return ofFilePath::join(App::APPLICATION_SETTINGS_FOLDER(), "cache"); // eg /Users/abc/.myapp/cache
    }

    string App::APPLICATION_SETTINGS_FILE()
    {
        return "settings.json";
    }

    string App::APPLICATION_SETTINGS_FILE_FULL_PATH()
    {
        return ofFilePath::join(App::APPLICATION_SETTINGS_FOLDER(), App::APPLICATION_SETTINGS_FILE()); // eg ~/.myapp/settings.json
    }

    int App::getAppMajorVersion()
    {
        return WidgetManager::getAppMajorVersion(App::APPLICATION_VERSION());
    }

    int App::getAppMinorVersion()
    {
        return WidgetManager::getAppMinorVersion(App::APPLICATION_VERSION());
    }

    int App::getAppPointVersion()
    {
        return WidgetManager::getAppPointVersion(App::APPLICATION_VERSION());
    }

#ifdef TARGET_OF_IOS
    string mIOSGetDocumentsDirectory = "";
    void App::setIOSGetDocumentsDirectory(string val)
    {
        mIOSGetDocumentsDirectory = val;
    }
    string App::getIOSGetDocumentsDirectory()
    {
        return mIOSGetDocumentsDirectory;
    }
#endif

}