#pragma once
#include "../Widget.h"
#include "ofxFontStash2.h"

namespace Aquamarine
{
    class WidgetElmLabel : public WidgetElm
    {

    public:
        WidgetElmLabel(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            WidgetElm::loadState(widgetXML);
            ofxXmlSettings settings = ofxXmlSettings();

            if (!settings.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            settings.pushTag("element");
            // Get some values here
            settings.popTag(); // element

            TiXmlElement *rootElm = settings.doc.RootElement();
            TiXmlElement *propertiesElm = rootElm->FirstChildElement("properties");

            if (propertiesElm != NULL)
            {
                TiXmlElement *valElm = propertiesElm->FirstChildElement("value");
                if (valElm != NULL)
                {
                    TiXmlPrinter printer;
                    valElm->Accept(&printer);
                    setValue(printer.CStr());
                }
            }
            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings mWidgetXML = WidgetElm::saveState();
            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::LABEL, 0);
            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();
            return mWidgetXML;
        }

        virtual ~WidgetElmLabel()
        {
        }

        virtual void update(WidgetContext context) override
        {
            labelRect = Shared::getViz()->getFonts(getTheme())->getTextBoundsNVG(getValue(), defaultStyle, Shared::getViz()->scale(getUsableX()), Shared::getViz()->scale(getUsableY()), Shared::getViz()->scale(getUsableWidth() - Shared::getViz()->getNonScaledPadding() * 2.0f), OF_ALIGN_HORZ_LEFT);

            setContentBoundsScaled(Coord::vizBounds(labelRect.x, labelRect.y, labelRect.width, labelRect.height));
        }

        virtual void draw(WidgetContext context) override
        {
            ofRectangle r = Shared::getViz()->getFonts(getTheme())->drawFormattedColumn(getValue(), Shared::getViz()->scale(getUsableX()), Shared::getViz()->scale(getUsableY()) + labelRect.height / 2, Shared::getViz()->scale(getUsableWidth() - Shared::getViz()->getNonScaledPadding() * 2.0f));

            if (getIsMousePressedAndReleasedOverWidget(false))
            {
                showPopoutWidgetIfSet();
            }
        }

        string getValue()
        {
            return mValue;
        }

        void setValue(string val)
        {
            mValue = val;
        }

        void onWidgetMouseScrolled(ofMouseEventArgs &e) override
        {
        }

    private:
        string mValue = "";
        ofxFontStash2::Style defaultStyle;
        ofRectangle labelRect;

        void initWidget() override
        {
            setWidth(100);
            defaultStyle = Shared::getViz()->getMediumFontStyle();
            setScrollAction(ScrollAction::SCROLL);
            setEnableScrollYPositive(true);
            setEnableScrollYNegative(true);
            setEnableScrollXPositive(false);
            setEnableScrollXNegative(false);
        }
    };
}