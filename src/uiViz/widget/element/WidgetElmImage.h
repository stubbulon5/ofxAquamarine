
#pragma once
#include "../Widget.h"

namespace Aquamarine
{
    class WidgetElmImage : public WidgetElm
    {
    public:
        WidgetElmImage(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {

            ofxXmlSettings settings = ofxXmlSettings();
            if (!settings.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            settings.pushTag("element");

            loadFile(settings.getAttribute("properties", "file", ""));
            setDoesRespondToFileDrop(settings.getAttribute("properties", "doesRespondToFileDrop", false));

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = WidgetElm::saveState();
            settings.setAttribute("element", "class", WIDGET_ELM_CLASS::IMAGE, 0);

            settings.pushTag("element");
            settings.setAttribute("properties", "file", filePath, 0);
            settings.setAttribute("properties", "doesRespondToFileDrop", getDoesRespondToFileDrop(), 0);
            settings.popTag();

            return settings;
        }

        virtual ~WidgetElmImage() {
        };

        virtual void draw(WidgetContext context) override
        {
            if (!getIsLoaded())
                return;
            image.draw(
                scale(getX()),
                scale(getY()),
                scale(getWidth()),
                scale(getHeight()));
            if (getIsMousePressedAndReleasedOverWidget(false))
            {
                showPopoutWidgetIfSet();
            }
        }

        virtual bool loadFile(string path)
        {
            // Override this
            filePath = path;
            isLoaded = image.load(path);
            return false;
        }

        bool getIsLoaded()
        {
            return isLoaded;
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {

            string event = args.getFullEventName();

            if (args.eventName == WIDGET_EVENT::RECEIVE_DRAG_AND_DROP_DATA)
            {
                if (!getDoesRespondToFileDrop())
                    return;
                string filePath = args.eventXML.getValue("event", "");
                if (ofFile::doesFileExist(filePath, false))
                {
                    loadFile(filePath);
                }
            }
        }

        bool getDoesRespondToFileDrop()
        {
            return mDoesRespondToFileDrop;
        }

        void setDoesRespondToFileDrop(bool val)
        {
            mDoesRespondToFileDrop = val;
        }

    protected:
        string filePath;
        bool mDoesRespondToFileDrop = false;
        bool isLoaded = false;
        ofImage image;

    private:
        void initWidget() override
        {
        }
    };
}