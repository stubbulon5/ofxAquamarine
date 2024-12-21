
#pragma once
#include "uiVizWidget.h"

namespace Aquamarine
{
    class uiVizWidgetImageView : public uiVizWidget
    {
    public:
        uiVizWidgetImageView(string persistentId, string widgetXML) : uiVizWidget(persistentId, widgetXML)
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

            settings.pushTag("widget");

            loadFile(settings.getAttribute("properties", "file", ""));
            setDoesRespondToFileDrop(settings.getAttribute("properties", "doesRespondToFileDrop", false));

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = uiVizWidget::saveState();
            settings.setAttribute("widget", "class", uiVizWidgetManager::WIDGET_CLASS_IMAGE_VIEW, 0);

            settings.pushTag("widget");
            settings.setAttribute("properties", "file", filePath, 0);
            settings.setAttribute("properties", "doesRespondToFileDrop", getDoesRespondToFileDrop(), 0);
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~uiVizWidgetImageView() {
        };

        virtual void draw(uiVizWidgetContext context) override
        {
            if (!getIsLoaded())
                return;
            image.draw(
                scale(getUsableX()),
                scale(getUsableY()),
                scale(getUsableWidth()),
                scale(getUsableHeight()));
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

        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
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
        bool mShowScrubber = true;
        bool mShowPlaybackButtons = true;

        void initWidget() override
        {
            setTitle("");
            setIsTitleAutohide(true);
        }
    };
}