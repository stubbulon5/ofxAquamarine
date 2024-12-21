#pragma once

#include "../uiVizWidget.h"

namespace Aquamarine
{
    class uiVizWidgetElmDropdown : public uiVizWidgetElm
    {

    public:
        uiVizWidgetElmDropdown(string persistentId, string widgetXML, uiVizWidgetBase *parent) : uiVizWidgetElm(persistentId, widgetXML, parent)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            uiVizWidgetElm::loadState(widgetXML);
            ofxXmlSettings mWidgetXML = ofxXmlSettings();

            if (!mWidgetXML.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            mWidgetXML.pushTag("element");
            // gget stuff
            mWidgetXML.popTag();

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings mWidgetXML = uiVizWidgetElm::saveState();
            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::DROPDOWN, 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual ~uiVizWidgetElmDropdown()
        {
        }

        string getValue()
        { // convert this to tuple
            return mValue;
        }

        void setValue(string val, bool fireEvent)
        {
            mValue = val;
        }

        void setValue(int val, bool fireEvent)
        {
            //.. look up the id and select it...
        }

        void onWidgetMouseScrolled(ofMouseEventArgs &e) override
        {
        }

        virtual void update(uiVizWidgetContext context) override
        {
        }

        virtual void draw(uiVizWidgetContext context) override
        {
        }

        virtual void onWidgetMousePressed(ofMouseEventArgs &e) override
        {
            if (e.button == OF_MOUSE_BUTTON_LEFT)
            {
                parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::FOCUS_ELEMENT, "<event></event>", *this);
                parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::CLICK, "<event></event>", *this);
            }
        }

        virtual void onWidgetKeyPressed(ofKeyEventArgs &e) override
        {
        }

        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {
        }

    private:
        string mValue = "";

        uiVizElm vizElm_TEXTBOX;
        uiVizElm vizElm_CURSOR;
        ofxFontStash2::Style defaultStyle;

        void initWidget() override
        {
            setWidth(100);
            setHeight(30);
            defaultStyle = getViz()->getMediumFontStyleMono();
            setScrollAction(ScrollAction::NONE);
        }
    };

}