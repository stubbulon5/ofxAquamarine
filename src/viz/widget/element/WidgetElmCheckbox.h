#pragma once

#include "../Widget.h"

namespace Aquamarine
{
    class WidgetElmCheckbox : public WidgetElm
    {

    public:
        WidgetElmCheckbox(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
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

            string boolString = ofToLower(mWidgetXML.getAttribute("element", "value", "false"));
            __LoadedValue = (boolString == "true" || boolString == "1" || boolString == "yes" || boolString == "on");
            if (getAllSiblingElementsLoaded())
            {
                // Firing events should ONLY ever happen once all sibling elements are loaded.
                // This is to ensure relationship consistency and prevent eventReceived events
                // firing on widgets before all elements are loaded...
                onAllSiblingElementsLoaded();
            }

            mWidgetXML.pushTag("element");
            // get stuff
            mWidgetXML.popTag();

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings mWidgetXML = WidgetElm::saveState();
            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::CHECKBOX, 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual void onAllSiblingElementsLoaded() override
        {
            setValue(__LoadedValue, true, false);
        }

        virtual ~WidgetElmCheckbox()
        {
        }

        bool getValue()
        {
            return mValue;
        }

        void setValue(bool val, bool fireEvent, bool animate)
        {
            mAnimate = animate;
            bool valChanged = (mValue != val);
            if (fireEvent && valChanged)
            {
                string eventVal = Shared::encodeForXML(ofToString(mValue));
                if (parent())
                    parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::VALUE_CHANGED, "<event><value>" + eventVal + "</value></event>", *this);
            }

            mValue = val;
            setNeedsUpdate(true);
            setWidgetNeedsUpdate(true);
        }

        void toggle(bool fireEvent, bool animate)
        {
            setValue(!getValue(), fireEvent, animate);
        }

        void onWidgetMouseScrolled(ofMouseEventArgs &e) override
        {

            if (getTimerMillis() >= 1000)
            {
                resetTimer();
                m_DidToggle = false;
            }

            if (m_DidToggle)
                return;
            toggle(true, false);
            m_DidToggle = true;
        }

        void onWidgetResized(int width, int height) override
        {
            setValue(getValue(), false, false);
        }

        virtual void update(WidgetContext context) override
        {

            int sliderBGSize = scale(mSliderBGSize);

            int sliderSize = scale(mSliderSize);
            int sliderHeight = scale(getHeight()) - getScaledPadding();

            mX_TargetTrue = sliderSize - getScaledPadding() / 2.0f;
            mX_TargetFalse = getScaledPadding() / 2.0f;

            if (NOT_SET)
            {
                mSliderCurrentX = mX_TargetFalse;
                NOT_SET = false;
            }
            else
            {
                if (mAnimate)
                {
                    if (getValue() == false)
                    {
                        if (mSliderCurrentX != mX_TargetFalse)
                        {
                            mSliderCurrentX = scaleAnimationForUI(getWidgetId() + "_slider", mSliderCurrentX, mX_TargetFalse, 0.4f);
                            setNeedsUpdate(true);
                        }
                    }
                    else
                    {
                        if (mSliderCurrentX != mX_TargetTrue)
                        {
                            mSliderCurrentX = scaleAnimationForUI(getWidgetId() + "_slider", mSliderCurrentX, mX_TargetTrue, 0.4f);
                            setNeedsUpdate(true);
                        }
                    }
                }
                else
                {
                    mSliderCurrentX = (getValue() ? mX_TargetTrue : mX_TargetFalse);
                }
            }

            int sliderX = getSliderBG_x() + mSliderCurrentX;
            int sliderY = scale(getY()) + getScaledPadding() / 2.0f;

            // Slider BG
            getIsRoundedRectangle() ? vizElm_BG.setRectRounded(getSliderBG_x(), scale(getY()), sliderBGSize, scale(getHeight()), scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_BG.setRectangle(getSliderBG_x(), scale(getY()), sliderBGSize, scale(getHeight()), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));

            // Slider
            getIsRoundedRectangle() ? vizElm_SLIDER.setRectRounded(sliderX, sliderY, sliderSize, sliderHeight, scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_SLIDER.setRectangle(sliderX, sliderY, sliderSize, sliderHeight, getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));
        }

        int getSliderBG_x()
        {
            return scale(getX() + (getTitle() == "" ? 0 : getWidth() - mSliderBGSize));
        }

        virtual void draw(WidgetContext context) override
        {
            vizElm_BG.setColor(getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));
            vizElm_BG.draw();

            vizElm_SLIDER.setColor(getTheme().ElementForegroundColor_withAlpha(getTheme().ElementHoveredForegroundAlpha));
            vizElm_SLIDER.draw();

            if (getTitle() != "")
            {
                defaultStyle.color = getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha);

                ofRectangle r = getViz()->getFonts(getTheme())->drawColumn(getTitle(), defaultStyle, getViz()->scale(getUsableX()), getViz()->scale(getUsableY() + getHeight() / 2.0f), scale(getTitleWidth()), ofAlignHorz::OF_ALIGN_HORZ_LEFT, false);
            }
        }

        virtual void onWidgetMousePressed(ofMouseEventArgs &e) override
        {
            if (e.button == OF_MOUSE_BUTTON_LEFT)
            {
                toggle(true, true);
                parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::FOCUS_ELEMENT, "<event></event>", *this);
                parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::CLICK, "<event></event>", *this);
            }
        }

        virtual void onWidgetKeyPressed(ofKeyEventArgs &e) override
        {

            if (!getIsFocused(true))
                return;

            if (e.key == 32)
            { // space to toggle
                toggle(true, true);
            }
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
        }

    private:
        bool NOT_SET = true;
        bool mValue = false;
        bool __LoadedValue = false;
        bool mAnimate = true;
        int mSliderBGSize = 50;
        int mSliderSize = 25;
        float mSliderCurrentX = 0.0f;
        int mX_TargetTrue = 0;
        int mX_TargetFalse = 0;
        bool m_DidToggle = false;

        Elm vizElm_BG;
        Elm vizElm_SLIDER;
        ofxFontStash2::Style defaultStyle;

        void initWidget() override
        {
            setWidth(100);
            setHeight(30);
            defaultStyle = getViz()->getMediumFontStyleMono();
            setScrollAction(ScrollAction::BASIC);
        }
    };

}