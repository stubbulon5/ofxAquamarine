#pragma once
#include "../Widget.h"

#include "ofxFontStash2.h"

namespace Aquamarine
{
    class WidgetElmTextarea : public WidgetElm
    {

    public:
        WidgetElmTextarea(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
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

            setIsFormatted(settings.getAttribute("properties", "isFormatted", "0", 0) == "1");

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

            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::TEXTAREA, 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual ~WidgetElmTextarea()
        {
        }

        virtual void update(WidgetContext context) override
        {

            getIsRoundedRectangle() ? vizElm_TEXTAREA.setRectRounded(Shared::getViz()->scale(getX()), Shared::getViz()->scale(getY()), Shared::getViz()->scale(getWidth()), Shared::getViz()->scale(getHeight()), scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_TEXTAREA.setRectangle(Shared::getViz()->scale(getX()), Shared::getViz()->scale(getY()), Shared::getViz()->scale(getWidth()), Shared::getViz()->scale(getHeight()), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));

            ofRectangle r;
            ofColor fontColor = getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha);
            if (getIsFormatted())
            {
                ofPushStyle();
                ofSetColor(fontColor);
                r = Shared::getViz()->getFonts(getTheme())->drawFormattedColumn(getValue(), scale(getUsableWidth() * -1 - 100), scale(getUsableY() + getNonScaledPadding()), scale(getUsableWidth() - Shared::getViz()->getNonScaledPadding() * 2.0f));
                ofPopStyle();
            }
            else
            {
                defaultStyle.color = fontColor;
                r = Shared::getViz()->getFonts(getTheme())->drawColumn(getValue(), defaultStyle, scale(getUsableWidth() * -1 - 100), scale(getUsableY() + getNonScaledPadding()), scale(getUsableWidth() - getNonScaledPadding() * 2.0f), ofAlignHorz::OF_ALIGN_HORZ_LEFT, false);
            }

            setContentBoundsScaled(
                Coord::vizBounds(r.x, r.y, r.width, r.height));
        }

        virtual void draw(WidgetContext context) override
        {

            vizElm_TEXTAREA.setColor(getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));
            vizElm_TEXTAREA.draw();

            ofRectangle r;
            ofColor fontColor = getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha);
            if (getIsFormatted())
            {
                ofPushStyle();
                ofSetColor(fontColor);
                r = Shared::getViz()->getFonts(getTheme())->drawFormattedColumn(getValue(), scale(getUsableX()), scale(getUsableY() + getNonScaledPadding()), scale(getUsableWidth() - Shared::getViz()->getNonScaledPadding() * 2.0f));
                ofPopStyle();
            }
            else
            {
                defaultStyle.color = fontColor;
                r = Shared::getViz()->getFonts(getTheme())->drawColumn(getValue(), defaultStyle, scale(getUsableX()), scale(getUsableY() + getNonScaledPadding()), scale(getUsableWidth() - getNonScaledPadding() * 2.0f), ofAlignHorz::OF_ALIGN_HORZ_LEFT, false);
            }

            if (getIsMousePressedAndReleasedOverWidget(false))
            {
                parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::CLICK, "<event></event>", *this);
            }
        }

        bool getIsFormatted()
        {
            return mIsFormatted;
        }

        void setIsFormatted(bool val)
        {
            mIsFormatted = val;
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
        Elm vizElm_TEXTAREA;
        ofxFontStash2::Style defaultStyle;
        bool mIsFormatted = false;

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