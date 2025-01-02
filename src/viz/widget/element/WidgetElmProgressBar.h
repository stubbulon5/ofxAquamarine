#pragma once

#include "../Widget.h"

namespace Aquamarine
{
    class WidgetElmProgressBar : public WidgetElm
    {

    public:
        WidgetElmProgressBar(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
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

            // Set to true for a an operation with an undeterminate completion, like an async web request
            setIsAsyncProgressBar(mWidgetXML.getAttribute("element", "isAsyncProgressBar", false));

            // Set to true to start animating progress... (if isAsyncProgressBar)
            setIsProgressing(mWidgetXML.getAttribute("element", "isProgressing", false));

            __LoadedValue = mWidgetXML.getAttribute("element", "value", 0.0f);
            if (getAllSiblingElementsLoaded())
            {
                // Firing events should ONLY ever happen once all sibling elements are loaded.
                // This is to ensure relationship consistency and prevent eventReceived events
                // firing on widgets before all elements are loaded...
                onAllSiblingElementsLoaded();
            }

            mWidgetXML.pushTag("element");
            // gget stuff
            mWidgetXML.popTag();

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings mWidgetXML = WidgetElm::saveState();
            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::SLIDER, 0);

            mWidgetXML.setAttribute("element", "isAsyncProgressBar", getIsAsyncProgressBar(), 0);
            mWidgetXML.setAttribute("element", "isProgressing", getIsProgressing(), 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual void onAllSiblingElementsLoaded() override
        {
            setValue(__LoadedValue, true);
        }

        virtual ~WidgetElmProgressBar()
        {
        }

        float getValue()
        {
            return mValue;
        }

        float getPerc()
        {
            return mPerc;
        }

        void setPerc(float val, bool fireEvent)
        {
            val = min(val, 1.0f);
            val = max(val, 0.0f);
            setValue(val * (float)(mMaxValue - mMinValue), fireEvent);
        }

        void setValue(float val, bool fireEvent)
        {
            NOT_SET = false;
            float previousVal = mValue;
            mValue = val;
            mValue = min(mValue, mMaxValue);
            mValue = max(mValue, mMinValue);
            mPerc = (float)(mValue - mMinValue) / (float)(mMaxValue - mMinValue);

            bool valChanged = (mValue != previousVal);
            if (fireEvent && valChanged)
            {
                string eventVal = Shared::encodeForXML(ofToString(mValue));
                string eventPerc = Shared::encodeForXML(ofToString(mPerc));
                if (parent())
                    parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::VALUE_CHANGED, "<event><value>" + eventVal + "</value><percent>" + eventPerc + "</percent></event>", *this);
            }

            setNeedsUpdate(true);
            setWidgetNeedsUpdate(true);
        }

        void onWidgetResized(int width, int height) override
        {
            setValue(getValue(), false);
        }

        virtual void update(WidgetContext context) override
        {

            mSliderBGSize = getUsableWidth() - getTitleWidth();

            float progressBarSpeed = 100.0f; // todo: introduce current frame rate ino this equation

            if (getIsProgressing())
            {
                if (getIsDescending())
                {
                    if (getPerc() > 0)
                    {
                        setPerc(getPerc() - 1.0f / progressBarSpeed, true);
                    }
                    else
                    {
                        mIsDescending = false; // Start ascending again!
                        setNeedsUpdate(true);
                    }
                }
                else
                {
                    if (getPerc() < 1.0)
                    {
                        setPerc(getPerc() + 1.0f / progressBarSpeed, true);
                    }
                    else
                    {
                        mIsDescending = true; // Start descending again!
                        setNeedsUpdate(true);
                    }
                }
            }

            if (NOT_SET)
            {
                mSliderCurrentWidth = 0;
                NOT_SET = false;
            }
            else
            {
                mSliderCurrentWidth = scale((mSliderBGSize - getNonScaledPadding()) * getPerc());
            }

            int sliderHeight = scale(getHeight()) - getScaledPadding();
            int sliderX = scale(getX()) + getScaledPadding() / 2.0f;
            int sliderY = scale(getY()) + getScaledPadding() / 2.0f;

            if (getIsProgressing())
            {
                // Slider BG
                getIsRoundedRectangle() ? vizElm_BG.setRectRounded(getSliderBG_x(), scale(getY()), scale(mSliderBGSize), scale(getHeight()), scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_BG.setRectangle(mSliderBG_x, scale(getY()), scale(mSliderBGSize), scale(getHeight()), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));

                // Slider
                if (getIsAsyncProgressBar())
                {
                    if (getIsDescending())
                    {
                        getIsRoundedRectangle() ? vizElm_SLIDER.setRectRounded(sliderX + scale(mSliderBGSize) - mSliderCurrentWidth, sliderY, mSliderCurrentWidth, sliderHeight, scale(5), getTheme().ElementForegroundColor_withAlpha(getCurrentForegroundAlpha())) : vizElm_SLIDER.setRectangle(sliderX + scale(mSliderBGSize) - mSliderCurrentWidth, sliderY, mSliderCurrentWidth, sliderHeight, getTheme().ElementForegroundColor_withAlpha(getCurrentForegroundAlpha()));
                    }
                    else
                    {
                        getIsRoundedRectangle() ? vizElm_SLIDER.setRectRounded(sliderX, sliderY, mSliderCurrentWidth, sliderHeight, scale(5), getTheme().ElementForegroundColor_withAlpha(getCurrentForegroundAlpha())) : vizElm_SLIDER.setRectangle(sliderX, sliderY, mSliderCurrentWidth, sliderHeight, getTheme().ElementForegroundColor_withAlpha(getCurrentForegroundAlpha()));
                    }
                }
                else
                {
                    getIsRoundedRectangle() ? vizElm_SLIDER.setRectRounded(sliderX, sliderY, mSliderCurrentWidth, sliderHeight, scale(5), getTheme().ElementForegroundColor_withAlpha(getCurrentForegroundAlpha())) : vizElm_SLIDER.setRectangle(sliderX, sliderY, mSliderCurrentWidth, sliderHeight, getTheme().ElementForegroundColor_withAlpha(getCurrentForegroundAlpha()));
                }
            }
        }

        virtual void draw(WidgetContext context) override
        {
            vizElm_BG.setColor(getTheme().ElementBackgroundColor_withAlpha(0.3f));
            vizElm_BG.draw();

            vizElm_SLIDER.setColor(getTheme().ElementForegroundColor_withAlpha(0.75f));
            vizElm_SLIDER.draw();

            if (getTitle() != "")
            {
                defaultStyle.color = getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha);

                /*
                vector<ofxFontStash2::StyledLine> laidOutLines;
                Shared::getViz()->getFonts(getTheme())->layoutLines({{getTitle(), defaultStyle}}, scale(getTitleWidth()), laidOutLines);
                ofRectangle bbox = Shared::getViz()->getFonts(getTheme())->getTextBounds(laidOutLines, scale(getUsableX()), scale(getUsableY()));
                */

                //  Shared::getViz()->scale(getUsableY() + getHeight()/2.0f),

                string title = getTitle();
                ofStringReplace(title, "${VALUE}", ofToString(getValue()));
                ofStringReplace(title, "${PERC}", ofToString(getPerc()));
                ofRectangle r = Shared::getViz()->getFonts(getTheme())->drawColumn(title, defaultStyle, scale(getUsableX()),
                                                                           scale(getUsableY() + getHeight() / 2.0f), // scale(getUsableY()) + bbox.height*1.5f - getHeight()/2.5f,
                                                                           scale(getTitleWidth()), ofAlignHorz::OF_ALIGN_HORZ_LEFT, false);
            }
        }

        bool getIsAsyncProgressBar()
        {
            return mIsAsyncProgressBar;
        }

        void setIsAsyncProgressBar(bool val)
        {
            mIsAsyncProgressBar = val;
        }

        bool getIsProgressing()
        {
            if (getIsAsyncProgressBar())
            {
                return mIsProgressing;
            }
            else
            {
                return true;
            }
        }

        void setIsProgressing(bool val)
        {
            mIsProgressing = val;
        }

        bool getIsDescending()
        {
            return mIsDescending;
        }

    private:
        int getSliderBG_x()
        {
            return scale(getX() + getWidth() - mSliderBGSize);
        }

        float __LoadedValue = 0.0f;

        bool NOT_SET = true;
        float mValue = 0.0f;
        float mPerc = 0.0f;
        bool mShowValueWhenDragging = true;

        float mMinValue = 0.0f;
        float mMaxValue = 1.0f;

        bool mIsAsyncProgressBar = false;
        bool mIsProgressing = false;
        bool mIsDescending = false;

        int mSliderBG_x = 0;
        int mSliderBGSize = 100;
        int mSliderCurrentWidth = 0;

        Elm vizElm_BG;
        Elm vizElm_SLIDER;
        ofxFontStash2::Style defaultStyle;

        void initWidget() override
        {
            setWidth(100);
            setHeight(30);
            defaultStyle = Shared::getViz()->getMediumFontStyleMono();
            setScrollAction(ScrollAction::BASIC);
        }
    };

}