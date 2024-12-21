#pragma once
#include "../uiVizWidget.h"
#include "ofxFontStash2.h"

namespace Aquamarine
{
    class uiVizWidgetElmLabel : public uiVizWidgetElm
    {

    public:
        uiVizWidgetElmLabel(string persistentId, string widgetXML, uiVizWidgetBase *parent) : uiVizWidgetElm(persistentId, widgetXML, parent)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            uiVizWidgetElm::loadState(widgetXML);
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
            ofxXmlSettings mWidgetXML = uiVizWidgetElm::saveState();
            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::LABEL, 0);
            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();
            return mWidgetXML;
        }

        virtual ~uiVizWidgetElmLabel()
        {
        }

        virtual void update(uiVizWidgetContext context) override
        {
            labelRect = getViz()->getFonts(getTheme())->getTextBoundsNVG(getValue(), defaultStyle, getViz()->scale(getUsableX()), getViz()->scale(getUsableY()), getViz()->scale(getUsableWidth() - getViz()->getNonScaledPadding() * 2.0f), OF_ALIGN_HORZ_LEFT);

            setContentBoundsScaled(uiVizCoord::vizBounds(labelRect.x, labelRect.y, labelRect.width, labelRect.height));
        }

        virtual void draw(uiVizWidgetContext context) override
        {
            ofRectangle r = getViz()->getFonts(getTheme())->drawFormattedColumn(getValue(), getViz()->scale(getUsableX()), getViz()->scale(getUsableY()) + labelRect.height / 2, getViz()->scale(getUsableWidth() - getViz()->getNonScaledPadding() * 2.0f));

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
            defaultStyle = getViz()->getMediumFontStyle();
            setScrollAction(ScrollAction::SCROLL);
            setEnableScrollYPositive(true);
            setEnableScrollYNegative(true);
            setEnableScrollXPositive(false);
            setEnableScrollXNegative(false);
        }
    };
}