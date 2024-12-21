
#pragma once
#include "../Widget.h"

namespace Aquamarine
{
    class WidgetDialog : public Widget
    {
    public:
        WidgetDialog(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {

            ofxXmlSettings settings = ofxXmlSettings();
            if (!settings.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            settings.pushTag("widget");
            setMessage(settings.getAttribute("properties", "message", getMessage()));
            setButtons(
                settings.getAttribute("properties", "buttonTitles", getButtons()[0]),
                settings.getAttribute("properties", "buttonIDs", getButtons()[1]));

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            setTitleStyle(TitleStyle::TOP_STATIC);
            setStaticTopTitleBarSize(40);

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = Widget::saveState();
            settings.setAttribute("widget", "class", WidgetManager::WIDGET_CLASS_DIALOG, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~WidgetDialog() {
        };

        void update(WidgetContext context) override
        {
        }
        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {

            Widget::onWidgetEventReceived(args);

            string event = args.getFullEventName();
            if (args.eventName == WIDGET_EVENT::CLICK && (ofIsStringInString(ofToUpper(args.widgetId), "BTN_")))
            {
                mDialogCallback(args);
                string priorModalWidgetId = getViz()->getPriorModalWidgetId();
                clearIfModal();
                if (priorModalWidgetId != "" && priorModalWidgetId != getWidgetId())
                { // and not same
                    Widget *widget = WidgetManager::getWidget(priorModalWidgetId);
                    widget->setModalWidget();
                    widget->setActiveWidget();
                    WidgetManager::moveWidgetToFront(*widget);
                }
                setIsVisible(false);
            }
        }

        void showDialog(string title, string message, bool formattedMessage, string buttonTitles, string buttonIDs, bool clearExistingElms,
                        const std::function<void(const WidgetEventArgs &)> &callback)
        {
            // Clean up any residue
            if (clearExistingElms)
            {
                for (WidgetElm &elm : getWidgetElements())
                {
                    removeWidgetElement(&elm);
                }
            }

            setTitle(title);
            setMessage(message);
            setButtons(buttonTitles, buttonIDs);
            labelMessage->setIsFormatted(formattedMessage);

            setDialogCallback(callback);

            WidgetManager::showModal(this, true);
        }

        void showDialog(string title, string message, bool formattedMessage, string buttonTitles, string buttonIDs,
                        const std::function<void(const WidgetEventArgs &)> &callback)
        {

            showDialog(title, message, formattedMessage, buttonTitles, buttonIDs, true, callback);
        }

        void showDialog(string title, string message, bool formattedMessage, string buttonTitles, string buttonIDs)
        {
            showDialog(title, message, true, buttonTitles, buttonIDs, mDialogCallback);
        }

        void showDialog(string title, string message, string buttonTitles, string buttonIDs)
        {
            showDialog(title, message, true, buttonTitles, buttonIDs);
        }

        void setDialogCallback(const std::function<void(const WidgetEventArgs &)> &callback)
        {
            mDialogCallback = callback;
        }

        void setMessage(string val, bool formatted)
        {
            if (labelMessage)
                labelMessage->setIsFormatted(formatted);
            setMessage(val);
        }

        void setMessage(string val)
        {
            mMessage = val;

            bool origVisibility = getIsVisible();
            setIsVisible(true);

            labelMessage = dynamic_cast<WidgetElmTextarea *>(addOrLoadWidgetElement(labelMessage, WIDGET_ELM_CLASS::TEXTAREA, getPersistentId() + "_LBL", R"(
            <element>
            </element>
            )"));

            labelMessage->setX_Expr("${PARENT.LEFT}");
            labelMessage->setY_Expr("${PARENT.TOP}");
            labelMessage->setHeight_Expr("${PARENT.USABLE_HEIGHT}-30-${PADDING}*2");
            labelMessage->setWidth_Expr("${PARENT.USABLE_WIDTH}");
            labelMessage->setValue(getMessage());

            labelMessage->setIsVisible(mMessage.length() > 0);

            setIsVisible(origVisibility);
        }

        string getMessage()
        {
            return mMessage;
        }

        void setButtons(string buttonTitles, string buttonIDs)
        {
            mButtonTitles = buttonTitles;
            mButtonIDs = buttonIDs;

            bool origVisibility = getIsVisible();
            setIsVisible(true);

            vector<string> titles = ofSplitString(mButtonTitles, "|");
            vector<string> ids = ofSplitString(mButtonIDs, "|");

            if (ids.size() < titles.size())
                return;

            string lastId = "";
            for (int i = 0; i < titles.size(); i++)
            {
                string buttonTitle = titles[i];
                string buttonId = ids[i];

                WidgetElmButton *b = dynamic_cast<WidgetElmButton *>(addOrLoadWidgetElement(nullptr, WIDGET_ELM_CLASS::BUTTON, buttonId, R"(
                <element>
                </element>
                )"));

                lastId == "" ? b->setX_Expr("${PARENT.RIGHT}") : b->setX_Expr("${" + lastId + ".X}-${SELF.WIDTH}-${PADDING}");
                b->setY_Expr("${PARENT.BOTTOM}-${PADDING}");
                b->setHeight_Expr("30");
                b->setWidth_Expr("100");
                b->setTitle(buttonTitle);
                lastId = ofToUpper(buttonId);
            }

            setIsVisible(origVisibility);
        }

        vector<string> getButtons()
        {
            return {mButtonTitles, mButtonIDs};
        }

    private:
        WidgetElmTextarea *labelMessage = nullptr;
        std::function<void(const WidgetEventArgs &)> mDialogCallback;
        string mMessage = "Message";
        string mButtonTitles = "OK|Cancel";
        string mButtonIDs = "BTN_OK|BTN_CANCEL";

        void initWidget() override
        {
            setShowTitleIcons(false);
            setTitle("");
            setIsVisible(false);

            setDialogCallback([&, this](WidgetEventArgs args)
                              { cout << "Dialog button clicked: " + args.sender.getPersistentId(); });
        }
    };
}