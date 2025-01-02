#pragma once
#include "../Widget.h"

namespace Aquamarine
{
    class WidgetElmButton : public WidgetElm
    {

    public:
        WidgetElmButton(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
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

            setIconTag(mWidgetXML.getAttribute("properties", "icon", ""));

            /*
                    if (mWidgetXML.pushTag("properties")) {
                        setIconTag(mWidgetXML.getAttribute("popoutWidget", "popoutDirection", ""));
                        setIconTag(mWidgetXML.getAttribute("popoutWidget", "theme", ""));

                        if (mWidgetXML.pushTag("popoutWidget")) {
                            // Popout widget...
                            string popoutWidgetXML = Shared::getXMLAtTag(&mWidgetXML, "element:properties:popoutWidget", 0);
                            if (popoutWidgetXML != "") {
                                WidgetBase* w = new WidgetBase(
                                    getPersistentId()+"_POPOUT",
                                    popoutWidgetXML, parent() ? parent()->getWidgetId() : getWidgetId()
                                );
                                if (w) {
                                    //WidgetManager::addWidgetForPopout(*w, parent() ? parent()->getWidgetId() : getWidgetId(), true);
                                    setPopoutWidget(w, PopoutDirection::LEFT, WidgetTheme());
                                }
                            }
                        }
                    }
            */

            // Get some values here
            mWidgetXML.popTag();

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings mWidgetXML = WidgetElm::saveState();

            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::BUTTON, 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual ~WidgetElmButton()
        {
        }

        virtual void update(WidgetContext context) override
        {

            getIsRoundedRectangle() ? vizElm_BUTTON.setRectRounded(Shared::getViz()->scale(getX()), Shared::getViz()->scale(getY()), Shared::getViz()->scale(getWidth()), Shared::getViz()->scale(getHeight()), scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_BUTTON.setRectangle(Shared::getViz()->scale(getX()), Shared::getViz()->scale(getY()), Shared::getViz()->scale(getWidth()), Shared::getViz()->scale(getHeight()), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));

            // titleFont = getLargeFontSizedForDimensions(getUsableWidth() * 0.95f, getUsableHeight() * 0.95f);

            titleRect = Shared::getViz()->getFonts(getTheme())->getTextBounds(getTitle(), defaultStyle, 0, 0);
        }

        virtual void draw(WidgetContext context) override
        {
            ofPushStyle();
            vizElm_BUTTON.setColor(getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));
            vizElm_BUTTON.draw();

            if (getIsFocused(true))
            {
                vizElm_BUTTON_FOCUS.setRectangle(scale(getX() + (getWidth() / 2.0f)) - getScaledPadding() - titleRect.getWidth() / 2.0f,
                                                 scale(getY() + (getHeight() / 2.0f)) - getScaledPadding() * 2,
                                                 titleRect.width + getScaledPadding() * 2,
                                                 titleRect.height + getScaledPadding() * 2.5f,
                                                 getTheme().ElementForegroundColor_withAlpha(0.1f));

                vizElm_BUTTON_FOCUS.draw();
            }
            ofPopStyle();

            ofPushStyle();
            ofSetColor(getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha));

            if (getIconTag() != "")
            {
                icon.setScaledPos(
                    scale(getX()),
                    scale(getY()));
                icon.drawSvg();
            }

            if (!titleFont)
                update(context);

            defaultStyle.color = getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha);

            Shared::getViz()->getFonts(getTheme())->draw(getTitle(), defaultStyle, scale(getX() + (getWidth() / 2.0f)) - titleRect.getWidth() / 2.0f, scale(getY() + (getHeight() / 2.0f)) + titleRect.getHeight() / 2.75f);

            // titleFont->draw(getTitle(),
            //                 scale(getX() + (getWidth() / 2.0f)) - titleRect.getWidth() / 2.0f,
            //                 scale(getY() + (getHeight() / 2.0f ))  + titleRect.getHeight() / 2.0f);

            ofPopStyle();

            if (getIsMousePressedAndReleasedOverWidget(false))
            {
                press();
            }
        }

        void press()
        {
            showPopoutWidgetIfSet();
            parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::CLICK, "<event></event>", *this);
            m_PressedFunc();
        }

        void handlePressed(const std::function<void()> &func)
        {
            m_PressedFunc = func;
        }

        virtual void onWidgetKeyPressed(ofKeyEventArgs &e) override
        {

            if (!getIsFocused(true))
                return;

            if (e.key == 32 || e.key == OF_KEY_RETURN)
            { // space or enter to press
                press();
            }
        }

        void setIconTag(string val)
        {
            mIconTag = val;
            icon = IconCache::getIcon(val);
        }

        string getIconTag()
        {
            return mIconTag;
        }

    private:
        Elm vizElm_BUTTON;
        Elm vizElm_BUTTON_FOCUS;
        string mIconTag = "";

        ofRectangle titleRect;
        shared_ptr<ofxSmartFont> titleFont;
        ofxFontStash2::Style defaultStyle;

        std::function<void()> m_PressedFunc = [this]() {};

        Icon icon;

        void initWidget() override
        {
            setWidth(100);
            defaultStyle = Shared::getViz()->getLargeFontStyleMono();
        }
    };

}