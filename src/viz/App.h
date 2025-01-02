#pragma once
#include <string>
#include "Shared.h"
#include "Widgets.h"
#include "Icon.h"

namespace Aquamarine
{

    typedef map<string, Widget *(*)(string persistentId, string widgetXML)> widget_map_type;
    // static widget_map_type widgetClassMap;


    class App
    {
    public:
        App(
            string name,
            map<string, Icon *> customIconMap = {},
            string version = "1.0.0",
            string documentFileExtension = "xml",
            string defaultLanguagXML = "<LANGUAGE name='english'><abc>Hello world!</abc></LANGUAGE>");


        void BOOTSTRAP();

        // ------------------------- WIDGET STUFF -------------------------
        template <typename T>
        string getWidgetTypeIdentifier(T widgetType)
        {
            // for (auto widgetReg : widgetClassMap)
            // {
            // }

            return "";
        }

        template <typename T>
        static Widget *createWidget(string persistentId, string widgetXML)
        {
            Widget *createdWidgetPtr = new T(persistentId, widgetXML);
            return createdWidgetPtr;
        }

        template <typename T>
        void registerWidgetType(string widgetTypeIdentifier)
        {
            getRegisteredWidgets()[widgetTypeIdentifier] = &createWidget<T>;
        }

        // Pure virtual function for user to implement
        virtual widget_map_type& getRegisteredWidgets() = 0;
        // ------------------------- WIDGET STUFF -------------------------

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

        // System / default widgets provided by Aquamarine
        void registerDefaultWidgets();

        // Override this method to register custom, user written widgets (derived from Widget class)
        // like so:  ` registerWidgetType<WidgetMyAwesomeThing>("WidgetMyAwesomeThing"); `
        virtual void registerCustomWidgets()
        {
            ofLogNotice("Aquamarine::App") << "No custom widgets registered for App `" << App::APPLICATION_NAME() << "`";
        }

        // Override this method to register custom icons used in your own app
        virtual map<string, Icon *> getCustomIcons(float scale)
        {
            ofLogNotice("Aquamarine::App") << "No custom icons registered for App `" << App::APPLICATION_NAME() << "`";
            return {};
        }        

#ifdef TARGET_OF_IOS
        static void setIOSGetDocumentsDirectory(std::string val);
        static std::string getIOSGetDocumentsDirectory();
#endif

    // private:
    //     widget_map_type widgetClassMap;


        /*
        if make static ^^ 


Undefined symbols for architecture arm64:
  "Aquamarine::App::widgetClassMap", referenced from:
      void Aquamarine::App::registerWidgetType<WidgetMusical>(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>) in ofApp.o
      Aquamarine::App::getRegisteredWidgets() in WidgetManager.o
      void Aquamarine::App::registerWidgetType<Aquamarine::Widget>(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>) in App.o
      void Aquamarine::App::registerWidgetType<Aquamarine::WidgetDebug>(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>) in App.o
      void Aquamarine::App::registerWidgetType<Aquamarine::WidgetEventListener>(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>) in App.o
      void Aquamarine::App::registerWidgetType<Aquamarine::WidgetMenu>(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>) in App.o
      void Aquamarine::App::registerWidgetType<Aquamarine::WidgetMenuCollection>(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>) in App.o

        
        */
        static string mAPPLICATION_NAME;
        static string mAPPLICATION_NAME_FOR_FILE_SYSTEM;
        static string mAPPLICATION_VERSION;
        static string mDOCUMENT_FILE_EXTENSION;
        static string mDEFAULT_LANGUAGE_XML;
    };

}