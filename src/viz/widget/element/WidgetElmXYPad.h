#pragma once

#include "../Widget.h"

namespace Aquamarine
{
    class WidgetElmXYPad : public WidgetElm
    {

    public:
        WidgetElmXYPad(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
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

            setRange(
                mWidgetXML.getAttribute("element", "min", 0.0f),
                mWidgetXML.getAttribute("element", "max", 10.0f),
                mWidgetXML.getAttribute("element", "steps", 0));

            setShowValueWhenDragging(mWidgetXML.getAttribute("element", "showValueWhenDragging", true));

            __LoadedValueX = mWidgetXML.getAttribute("element", "valueX", 0.0f);
            __LoadedValueY = mWidgetXML.getAttribute("element", "valueY", 0.0f);
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

            mWidgetXML.setAttribute("element", "min", mMinValue, 0);
            mWidgetXML.setAttribute("element", "max", mMaxValue, 0);
            mWidgetXML.setAttribute("element", "steps", mTotalSteps, 0);
            mWidgetXML.setAttribute("element", "valueX", getValueX(), 0);
            mWidgetXML.setAttribute("element", "valueY", getValueY(), 0);
            mWidgetXML.setAttribute("element", "showValueWhenDragging", getShowValueWhenDragging(), 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual void onAllSiblingElementsLoaded() override
        {
            setValue(__LoadedValueX, __LoadedValueY, true);
        }

        virtual ~WidgetElmXYPad()
        {
        }

        void setRange(float min, float max)
        {
            setRange(min, max, 0);
        }

        void setRange(float min, float max, int steps)
        {

            if (min < 0.0f)
                min = 0.0f;

            if (min > max)
            {
                mMinValue = max;
                mMaxValue = min;          
            }
            else
            {
                mMinValue = min;
                mMaxValue = max;              
            }

            if (steps > 1)
            {
                mTotalSteps = steps;
            }
        }

        float getValueX()
        {
            return mValueX;
        }

        float getPercX()
        {
            return mPercX;
        }

        float getValueY()
        {
            return mValueY;
        }

        float getPercY()
        {
            return mPercY;
        }        

        void setPercX(float val, bool fireEvent)
        {
            val = min(val, 1.0f);
            val = max(val, 0.0f);
            setValue(val * (float)(mMaxValue - mMinValue), getValueY(), fireEvent);
        }

        void setPercY(float val, bool fireEvent)
        {
            val = min(val, 1.0f);
            val = max(val, 0.0f);
            setValue(getValueX(), val * (float)(mMaxValue - mMinValue), fireEvent);
        }        

        void setValue(float valX, float valY, bool fireEvent)
        {
            NOT_SET = false;
            float previousValX = mValueX;
            mValueX = valX;
            mValueX = min(mValueX, mMaxValue);
            mValueX = max(mValueX, mMinValue);
            mPercX = (float)(mValueX - mMinValue) / (float)(mMaxValue - mMinValue);

            float previousValY = mValueY;
            mValueY = valY;
            mValueY = min(mValueY, mMaxValue);
            mValueY = max(mValueY, mMinValue);
            mPercY = (float)(mValueY - mMinValue) / (float)(mMaxValue- mMinValue);            

            // Steps?
            if (mTotalSteps > 0 && mPercX > 0.0f)
            {
                float range = (float)mMaxValue - (float)mMinValue;
                float incAmt = (float)range / float(mTotalSteps);

                for (int i = 1; i <= mTotalSteps + 10; i++)
                {
                    float currIncVal = i * incAmt + mMinValue;
                    if ((currIncVal) >= mValueX)
                    {
                        mValueX = min(currIncVal, (float)mMaxValue);
                        mPercX = (float)(mValueX - mMinValue) / (float)(mMaxValue - mMinValue);
                        break;
                    }
                    if ((currIncVal) >= mValueY)
                    {
                        mValueY = min(currIncVal, (float)mMaxValue);
                        mPercY = (float)(mValueY - mMinValue) / (float)(mMaxValue - mMinValue);
                        break;
                    }                    
                }
            }

            bool valChanged = (mValueX != previousValX || mValueY != previousValY);
            if (fireEvent && valChanged)
            {
                string eventValX = Shared::encodeForXML(ofToString(mValueX));
                string eventPercX = Shared::encodeForXML(ofToString(mPercX));

                string eventValY = Shared::encodeForXML(ofToString(mValueY));
                string eventPercY = Shared::encodeForXML(ofToString(mPercY));
               
                if (parent())
                    parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::VALUE_CHANGED, "<event><valueX>" + eventValX + "</valueX><percentX>" + eventPercX + "</percentX><valueY>" + eventValY + "</valueY><percentY>" + eventPercY + "</percentY></event>", *this);
            }

            setNeedsUpdate(true);
            setWidgetNeedsUpdate(true);
        }

        bool getShowValueWhenDragging()
        {
            return mShowValueWhenDragging;
        }

        void setShowValueWhenDragging(bool val)
        {
            mShowValueWhenDragging = val;
        }

        void onWidgetMouseScrolled(ofMouseEventArgs &e) override
        {
            setValue(getValueX() - e.scrollX, getValueY() - e.scrollY, true);
        }

        void onWidgetResized(int width, int height) override
        {
            setValue(getValueX(), getValueY(), false);
        }

        virtual void update(WidgetContext context) override
        {

            mSliderBGSize = getUsableWidth(); //  - getTitleWidth();

            mX_TargetMax = getSliderBG_x() + scale(mSliderBGSize) - scale(mSliderSize) - getScaledPadding() / 2.0f;
            mX_TargetMin = getSliderBG_x() + getScaledPadding() / 2.0f;

            mY_TargetMax = getSliderBG_y() + scale(mSliderBGSize) - scale(mSliderSize) - getScaledPadding() / 2.0f;
            mY_TargetMin = getSliderBG_y() + getScaledPadding() / 2.0f;


            if (NOT_SET)
            {
                mSliderCurrentX = mX_TargetMax / 2.0f;
                mSliderCurrentY = mY_TargetMax / 2.0f;
                NOT_SET = false;
            }
            else
            {
                mSliderCurrentX = getSliderBG_x() + getScaledPadding() / 2.0f + scale(mPercX * (float)(mSliderBGSize - mSliderSize - getNonScaledPadding())); // - scale((mMinValue / (float)mSliderBGSize)*mSliderBGSize);
                mSliderCurrentY = getSliderBG_y() + getScaledPadding() / 2.0f + scale(mPercY * (float)(mSliderBGSize - mSliderSize - getNonScaledPadding())); // - scale((mMinValue / (float)mSliderBGSize)*mSliderBGSize);
            }

            int sliderX = mSliderCurrentX;
            int sliderY = mSliderCurrentY;

            // Slider BG
            getIsRoundedRectangle() ? vizElm_BG.setRectRounded(getSliderBG_x(), scale(getY()), scale(mSliderBGSize), scale(mSliderBGSize), scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_BG.setRectangle(mSliderBG_x, scale(getY()), scale(mSliderBGSize), scale(mSliderBGSize), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));

            // Slider
            getIsRoundedRectangle() ? vizElm_SLIDER.setRectRounded(sliderX, sliderY, scale(mSliderSize), scale(mSliderSize), scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_SLIDER.setRectangle(sliderX, sliderY, scale(mSliderSize), scale(mSliderSize), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));
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

                string title = getTitle();
                ofStringReplace(title, "${VALUE}", ofToString(getValueX()) + ":" + ofToString(getValueY()));
                ofStringReplace(title, "${PERC}", ofToString(getPercX()) + ":" + ofToString(getPercY()));
                ofRectangle r = getViz()->getFonts(getTheme())->drawColumn(title, defaultStyle, scale(getUsableX()),
                                                                           scale(getUsableY() + getHeight() / 2.0f), // scale(getUsableY()) + bbox.height*1.5f - getHeight()/2.5f,
                                                                           scale(getTitleWidth()), ofAlignHorz::OF_ALIGN_HORZ_LEFT, false);

                // if (getShowValueWhenDragging() && getIsElmDragging())
                // {
                //     defaultStyle.color = getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha);
                //     ofRectangle r = getViz()->getFonts(getTheme())->drawColumn(ofToString(getValueX()) + ":" + ofToString(getValueY()), defaultStyle,
                //                                                                getViz()->scale(getUsableX()), 
                //                                                                getViz()->scale(getUsableY()),
                //                                                                getHeight() / 2.0f,
                //                                                                ofAlignHorz::OF_ALIGN_HORZ_LEFT, false);

                // }
            }
        }

        virtual void onWidgetMouseDragged(ofMouseEventArgs &e) override
        {
            handleDrag(e);
            // WIDGET_EVENT::VALUE_CHANGED is fired from the setValue event.
        }

        virtual void onWidgetMousePressed(ofMouseEventArgs &e) override
        {
            handleDrag(e);
            parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::FOCUS_ELEMENT, "<event></event>", *this);
            parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::CLICK, "<event></event>", *this);
        }

        virtual void onWidgetMouseReleased(ofMouseEventArgs &e) override
        {
        }

        virtual void onWidgetKeyPressed(ofKeyEventArgs &e) override
        {

            if (!getIsFocused(true))
                return;

            if (e.key == OF_KEY_LEFT)
            {
                setPercX(getPercX() - 0.1f, true);
            }
            else if (e.key == OF_KEY_RIGHT)
            {
                setPercX(getPercX() + 0.05f, true);
            } else if (e.key == OF_KEY_UP)
            {
                setPercY(getPercY() - 0.1f, true);
            }
            else if (e.key == OF_KEY_DOWN)
            {
                setPercY(getPercY() + 0.05f, true);
            }
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
        }

    private:
        void handleDrag(ofMouseEventArgs &e)
        {
            if (e.button == OF_MOUSE_BUTTON_LEFT)
            {
                int mX = ofGetMouseX();
                int mY = ofGetMouseY();

                float rangeX = (float)(mX_TargetMax - mX_TargetMin);
                float percX = (float)(mX - mX_TargetMin - scale(mSliderSize) / 2.0f) / rangeX;

                float rangeY = (float)(mY_TargetMax - mY_TargetMin);
                float percY = (float)(mY - mY_TargetMin - scale(mSliderSize) / 2.0f) / rangeY;

                setValue(
                    percX * (float)((float)mMaxValue - (float)mMinValue) + mMinValue, 
                    percY * (float)((float)mMaxValue - (float)mMinValue) + mMinValue, 
                    true
                );
            }
        }

        int getSliderBG_x()
        {
            return scale(getX() + getWidth() - mSliderBGSize);
        }

        int getSliderBG_y()
        {
            return scale(getY() + getHeight() - mSliderBGSize);
        }        

        float __LoadedValueX = 0.0f;
        float __LoadedValueY = 0.0f;

        bool NOT_SET = true;
        float mValueX = 0.0f;
        float mValueY = 0.0f;
        float mPercX = 0.0f;
        float mPercY = 0.0f;
        bool mShowValueWhenDragging = true;

        float mMinValue = 0.0f;
        float mMaxValue = 10.0f;
        int mTotalSteps = 0;

        int mSliderBG_x = 0;
        int mSliderBG_y = 0;
        int mSliderBGSize = 100;
        int mSliderSize = 25;
        int mSliderCurrentX = 0;
        int mSliderCurrentY = 0;
        int mX_TargetMin = 0;
        int mX_TargetMax = 0;

        int mY_TargetMin = 0;
        int mY_TargetMax = 0;        

        Elm vizElm_BG;
        Elm vizElm_SLIDER;
        ofxFontStash2::Style defaultStyle;

        void initWidget() override
        {
            setLockAspectRatio(true);
            setAspectRatio(1);            
            setWidth(100);
            setHeight(100);
            defaultStyle = getViz()->getMediumFontStyleMono();
            setScrollAction(ScrollAction::BASIC);
        }
    };
}