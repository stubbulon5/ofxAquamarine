#pragma once

#include "../Widget.h"

namespace Aquamarine
{
    class WidgetElmDropdown : public WidgetElm
    {

    public:
        WidgetElmDropdown(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            WidgetElm::loadState(widgetXML);
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
            ofxXmlSettings mWidgetXML = WidgetElm::saveState();
            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::DROPDOWN, 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual ~WidgetElmDropdown()
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

        virtual void update(WidgetContext context) override
        {
        }

        virtual void draw(WidgetContext context) override
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

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
        }

    private:
        string mValue = "";

        Elm vizElm_TEXTBOX;
        Elm vizElm_CURSOR;
        ofxFontStash2::Style defaultStyle;

        void initWidget() override
        {
            setWidth(100);
            setHeight(30);
            defaultStyle = Shared::getViz()->getMediumFontStyleMono();
            setScrollAction(ScrollAction::NONE);
        }
    };

}