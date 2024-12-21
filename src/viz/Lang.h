
#pragma once
#include "ofMain.h"
#include "ofxXmlSettings.h"

namespace Aquamarine
{
    class Lang
    {
    public:
        void populateFromHelpFile(string language)
        {
            string fileName = "ui/widgetHelp/" + ofToLower(language) + ".xml";

            mLangXML = ofxXmlSettings();
            if (mLangXML.load(ofToDataPath(fileName)))
            {
                cout << "Loaded help file " << fileName;
                mLangLoaded = mLangXML.pushTag("LANGUAGE");
            }
        }

        void populateFromFile(string language)
        {

            // load up the file names  <language>.xml
            // set mLangLoaded correctly!
            // move this to application stack (ie make a file called appConstants with the xml string
            // then look at how to move custom widghets

            populate("");
        }

        /*
         EXAMPLE XML:

         "<LANGUAGE name='english'>"
             "<HELLO>Hello!</HELLO>"
             "<WORLD>World...</WORLD>"
         "</LANGUAGE>";

         ....

         get("WORLD");

         */
        void populate(string xml)
        {
            mLangXML = ofxXmlSettings();
            if (mLangXML.loadFromBuffer(xml))
            {
                mLangLoaded = mLangXML.pushTag("LANGUAGE");
            }
            else
            {
                mLangLoaded = false;
            }
        }

        string get(string tag)
        {
            if (!mLangLoaded)
            {
                return "!!NOT_LOADED";
            }
            return mLangXML.getValue(tag, "!!NOT_FOUND");
        }

        ~Lang()
        {
        }

    private:
        ofxXmlSettings mLangXML;
        bool mLangLoaded = false;
    };
}