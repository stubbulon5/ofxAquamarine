
#pragma once
#include "../uiVizWidget.h"
#include "uiVizWidgetFileExplorer.h"

namespace Aquamarine
{
    class uiVizWidgetFileLoad : public uiVizWidgetFileExplorer
    {
    public:
        uiVizWidgetFileLoad(string persistentId, string widgetXML) : uiVizWidgetFileExplorer(persistentId, widgetXML)
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
            setIsDirectorySelectionMode(settings.getAttribute("properties", "isDirectorySelectionMode", getIsDirectorySelectionMode()));

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            setTitleStyle(TitleStyle::TOP_STATIC);
            setStaticTopTitleBarSize(40);

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = uiVizWidget::saveState();
            settings.setAttribute("widget", "class", uiVizWidgetManager::WIDGET_CLASS_FILE_LOAD, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~uiVizWidgetFileLoad() {
        };

        void update(uiVizWidgetContext context) override
        {
            openButton->setIsEnabled((getIsDirectorySelectionMode() ? getSelectedDirExists() : getSelectedFileExists()));
        }
        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {

            uiVizWidgetFileExplorer::onWidgetEventReceived(args);

            string event = args.getFullEventName();
            if (args.eventName == WIDGET_EVENT::CLICK && openButton && args.sender.getPersistentId() == openButton->getPersistentId())
            {

                if (getIsDirectorySelectionMode())
                {
                    if (getSelectedDirExists())
                    {
                        cout << "dir selected " << getSelectedDir();
                        uiVizWidgetFileLocationsList::addRecentDirectory(getSelectedDir());
                        // Fire event here
                        mPathSelectedCallback(getSelectedDir());
                        closeThisWidget("SUCCESS");
                    }
                }
                else
                {
                    if (getSelectedFileExists())
                    {
                        cout << "file selected " << getSelectedFile();
                        uiVizWidgetFileLocationsList::addRecentFile(getSelectedFile());
                        uiVizWidgetFileLocationsList::addRecentDirectory(ofFilePath::getEnclosingDirectory(getSelectedFile(), false));
                        // Fire event here
                        mPathSelectedCallback(getSelectedFile());
                        closeThisWidget("SUCCESS");
                    }
                }
            }
            else if (args.eventName == WIDGET_EVENT::CLICK && cancelButton && args.sender.getPersistentId() == cancelButton->getPersistentId())
            {
                mSelectedFile = "";
                mSelectedDir = "";
                mSelectedFileExists = false;
                mSelectedDirExists = false;
                // Fire event here
                closeThisWidget("CANCELLED");
            }
        }

        bool getIsDirectorySelectionMode()
        {
            return mIsDirectorySelectionMode;
        }

        void setIsDirectorySelectionMode(bool val)
        {
            mIsDirectorySelectionMode = val;
        }

        void setPathSelectedCallback(const std::function<void(const string &)> &callback)
        {
            mPathSelectedCallback = callback;
        }

    private:
        uiVizWidgetElmButton *openButton = nullptr;
        uiVizWidgetElmButton *cancelButton = nullptr;
        bool mIsDirectorySelectionMode = false;
        std::function<void(const string &)> mPathSelectedCallback;

        void initWidget() override
        {

            string openButtonId = getPersistentId() + "_OPEN_BUTTON";
            string cancelButtonId = getPersistentId() + "_CANCEL_BUTTON";

            if (!openButton)
            {
                openButton = dynamic_cast<uiVizWidgetElmButton *>(addOrLoadWidgetElement(openButton, WIDGET_ELM_CLASS::BUTTON, openButtonId, R"(
                <element>
                </element>
                )"));

                openButton->setX_Expr("${PARENT.RIGHT}");
                openButton->setY_Expr("${PARENT.ABSOLUTE_Y}+${PADDING}");
                openButton->setHeight_Expr("30");
                openButton->setWidth_Expr("100");
                openButton->setTitle("Open");
                openButton->setIsEnabled(false);
            }

            if (!cancelButton)
            {
                cancelButton = dynamic_cast<uiVizWidgetElmButton *>(addOrLoadWidgetElement(cancelButton, WIDGET_ELM_CLASS::BUTTON, cancelButtonId, R"(
                <element>
                </element>
                )"));

                cancelButton->setX_Expr("${PARENT.LEFT}");
                cancelButton->setY_Expr("${PARENT.ABSOLUTE_Y}+${PADDING}");
                cancelButton->setHeight_Expr("30");
                cancelButton->setWidth_Expr("100");
                cancelButton->setTitle("Cancel");
            }

            setShowTitleIcons(false);
            setTitle("");
        }
    };

}