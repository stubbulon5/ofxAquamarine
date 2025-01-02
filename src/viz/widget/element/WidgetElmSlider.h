#pragma once

#include "../Widget.h"

namespace Aquamarine
{
    class WidgetElmSlider : public WidgetElm
    {

    public:
        WidgetElmSlider(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
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

            mWidgetXML.setAttribute("element", "min", mMinValue, 0);
            mWidgetXML.setAttribute("element", "max", mMaxValue, 0);
            mWidgetXML.setAttribute("element", "steps", mTotalSteps, 0);
            mWidgetXML.setAttribute("element", "value", getValue(), 0);
            mWidgetXML.setAttribute("element", "showValueWhenDragging", getShowValueWhenDragging(), 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual void onAllSiblingElementsLoaded() override
        {
            setValue(__LoadedValue, true);
        }

        virtual ~WidgetElmSlider()
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

        float getValue()
        {
            return mValue;
        }

        float getPerc()
        {
            return mPerc;
        }
        /*
       todo:
           textbox unicode support
           possible grix / x/y key support
           multiline label


           test cases for WidgetElm :
               Dragging on and off focus to ensure it still responds
               resizing parent widget
               rescale parent window
               set ui scaling
               drag from low dpi to high dpi window
               test activity / responsiveness when another widget / elms is overlayed on top

               known bugs:
               textbox cursor on scaled / high dpi
               resize widget (checkbox and slider) slider elms
               text oveshoots
               text always refreshes when resizing other widgets ?!!!

               dragging parent widget : doesnt update checkbox pos (x)
               use fbo to only render visible text


                 when dragged to new screen - title changes and arrow shows up too!! (it's because the xml is loaded AFTER it's already added : fix - load the xml from mainmenu class!!
                 and the value is being reset back to zero

               investigate HTML rendered (canvas object)

         // doxygen

           */

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
            mPerc = (float)(mValue - mMinValue) / (float)(mMaxValue - mMinValue); // slider doesn't get to start, only the minval offset

            // Steps?
            if (mTotalSteps > 0 && mPerc > 0.0f)
            {
                float range = (float)mMaxValue - (float)mMinValue;
                float incAmt = (float)range / float(mTotalSteps);

                for (int i = 1; i <= mTotalSteps + 10; i++)
                {
                    float currIncVal = i * incAmt + mMinValue;
                    if ((currIncVal) >= mValue)
                    {
                        mValue = min(currIncVal, (float)mMaxValue);
                        mPerc = (float)(mValue - mMinValue) / (float)(mMaxValue - mMinValue);
                        break;
                    }
                }
            }

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
            setValue(getValue() + e.scrollY, true);
        }

        void onWidgetResized(int width, int height) override
        {
            setValue(getValue(), false);
        }

        virtual void update(WidgetContext context) override
        {

            mSliderBGSize = getUsableWidth() - getTitleWidth();

            mX_TargetMax = getSliderBG_x() + scale(mSliderBGSize) - scale(mSliderSize) - getScaledPadding() / 2.0f;
            mX_TargetMin = getSliderBG_x() + getScaledPadding() / 2.0f;

            if (NOT_SET)
            {
                mSliderCurrentX = mX_TargetMin;
                NOT_SET = false;
            }
            else
            {
                mSliderCurrentX = getSliderBG_x() + getScaledPadding() / 2.0f + scale(mPerc * (float)(mSliderBGSize - mSliderSize - getNonScaledPadding())); // - scale((mMinValue / (float)mSliderBGSize)*mSliderBGSize);
            }

            int sliderHeight = scale(getHeight()) - getScaledPadding();
            int sliderX = mSliderCurrentX;
            int sliderY = scale(getY()) + getScaledPadding() / 2.0f;

            // Slider BG
            getIsRoundedRectangle() ? vizElm_BG.setRectRounded(getSliderBG_x(), scale(getY()), scale(mSliderBGSize), scale(getHeight()), scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_BG.setRectangle(mSliderBG_x, scale(getY()), scale(mSliderBGSize), scale(getHeight()), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));

            // Slider
            getIsRoundedRectangle() ? vizElm_SLIDER.setRectRounded(sliderX, sliderY, scale(mSliderSize), sliderHeight, scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_SLIDER.setRectangle(sliderX, sliderY, scale(mSliderSize), sliderHeight, getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));
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

                if (getShowValueWhenDragging() && getIsElmDragging())
                {
                    defaultStyle.color = getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha);
                    ofRectangle r = Shared::getViz()->getFonts(getTheme())->drawColumn(ofToString(mValue), defaultStyle,
                                                                               mX_TargetMin, // mSliderCurrentX
                                                                               scale(getUsableY() + getHeight() / 2.0f), (getPerc() > 0.5f) ? vizElm_BG.getRectangle().width : vizElm_BG.getRectangle().width - getScaledPadding(), (getPerc() > 0.5f) ? ofAlignHorz::OF_ALIGN_HORZ_LEFT : ofAlignHorz::OF_ALIGN_HORZ_RIGHT, false);
                }
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
                setPerc(getPerc() - 0.1f, true);
            }
            else if (e.key == OF_KEY_RIGHT)
            {
                setPerc(getPerc() + 0.05f, true);
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

                float range = (float)(mX_TargetMax - mX_TargetMin);
                float perc = (float)(mX - mX_TargetMin - scale(mSliderSize) / 2.0f) / range;
                setValue(perc * (float)((float)mMaxValue - (float)mMinValue) + mMinValue, true);
            }
        }

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
        float mMaxValue = 10.0f;
        int mTotalSteps = 0;

        int mSliderBG_x = 0;
        int mSliderBGSize = 100;
        int mSliderSize = 25;
        int mSliderCurrentX = 0;
        int mX_TargetMin = 0;
        int mX_TargetMax = 0;

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