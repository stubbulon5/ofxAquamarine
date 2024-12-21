
#pragma once
#include "../uiVizWidget.h"
#include "uiVizWidgetFileExplorer.h"

namespace Aquamarine
{
    class uiVizWidgetFileSave : public uiVizWidgetFileExplorer
    {
    public:
        uiVizWidgetFileSave(string persistentId, string widgetXML) : uiVizWidgetFileExplorer(persistentId, widgetXML)
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

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = uiVizWidget::saveState();
            settings.setAttribute("widget", "class", uiVizWidgetManager::WIDGET_CLASS_FILE_SAVE, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~uiVizWidgetFileSave() {
        };

        void update(uiVizWidgetContext context) override
        {
            saveButton->setIsEnabled(getSelectedFile() != "" || fileName->getValue() != "");
        }
        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {

            uiVizWidgetFileExplorer::onWidgetEventReceived(args);

            string event = args.getFullEventName();
            if (
                args.eventName == WIDGET_EVENT::CLICK && saveButton && args.sender.getPersistentId() == saveButton->getPersistentId())
            {
                string fileNameCleansed = fileName->getValue();

                ofStringReplace(fileNameCleansed, "/", "");
                ofStringReplace(fileNameCleansed, "\\", "");

                if (fileNameCleansed.size() == 0)
                    return; // user did not type a file name...

                mSelectedFile = getSelectedDir() + OS_SLASH_CHAR + fileNameCleansed;
                bool doesFileExist = ofFile::doesFileExist(mSelectedFile, false);

                if (doesFileExist)
                {

                    getDialog()->showDialog(
                        "Overwite file? ",
                        "<xlarge><br/>Do you want to ovewrite this file?</xlarge>",
                        true,
                        "Yes|No",
                        "BTN_YES|BTN_NO",
                        [&, this](uiVizWidgetEventArgs args)
                        {
                            cout << "Dialog button clicked: " + args.sender.getPersistentId();
                            if (args.sender.getPersistentId() == "BTN_YES")
                            {
                                cout << "overwite file " << getSelectedFile();
                                mSavePathSelectedCallback(getSelectedFile());

                                uiVizWidgetFileLocationsList::addRecentDirectory(getSelectedDir());
                                uiVizWidgetFileLocationsList::addRecentFile(getSelectedFile());

                                closeThisWidget("SUCCESS");
                            }
                        });
                }
                else
                {
                    mSavePathSelectedCallback(getSelectedFile());

                    uiVizWidgetFileLocationsList::addRecentDirectory(getSelectedDir());
                    uiVizWidgetFileLocationsList::addRecentFile(getSelectedFile());

                    closeThisWidget("SUCCESS");
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
            else if (args.eventName == WIDGET_EVENT::TABLE_CELL_SELECTED)
            {

                uiVizWidgetFileList *fileList = getFileList();
                if (fileList && fileList->getSelectedRows().size() > 0 && fileList->getSelectedRows()[0].cells.size() > 1)
                {
                    uiVizWidgetTableRow row = fileList->getSelectedRows()[0];
                    if (row.cells[0].key == "FILE")
                    {
                        string newName = row.cells[1].key;
                        setProposedFileName(newName);
                    }
                }
            }
        }

        void setSavePathSelectedCallback(const std::function<void(const string &)> &callback)
        {
            mSavePathSelectedCallback = callback;
        }

        void setProposedFileName(string name)
        {
            if (fileName)
                fileName->setValue(name, true);
        }

        string getProposedFileName()
        {
            if (fileName)
                return fileName->getValue();
            return "";
        }

    private:
        uiVizWidgetElmButton *saveButton = nullptr;
        uiVizWidgetElmButton *cancelButton = nullptr;
        uiVizWidgetElmTextbox *fileName = nullptr;
        std::function<void(const string &)> mSavePathSelectedCallback;

        void initWidget() override
        {

            string saveButtonId = getPersistentId() + "_SAVE_BUTTON";
            string cancelButtonId = getPersistentId() + "_CANCEL_BUTTON";
            string fileNameId = getPersistentId() + "_FILENAME_TEXT";

            if (!saveButton)
            {
                saveButton = dynamic_cast<uiVizWidgetElmButton *>(addOrLoadWidgetElement(saveButton, WIDGET_ELM_CLASS::BUTTON, saveButtonId, R"(
                <element>
                </element>
                )"));

                saveButton->setX_Expr("${PARENT.RIGHT}");
                saveButton->setY_Expr("${PARENT.ABSOLUTE_Y}+${PADDING}");
                saveButton->setHeight_Expr("30");
                saveButton->setWidth_Expr("100");
                saveButton->setTitle("Save");
                saveButton->setIsEnabled(false);
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

            if (!fileName)
            {
                fileName = dynamic_cast<uiVizWidgetElmTextbox *>(addOrLoadWidgetElement(fileName, WIDGET_ELM_CLASS::TEXTBOX, fileNameId, R"(
                <element>
                </element>
                )"));

                fileName->setX_Expr("${PARENT.LEFT}+${" + cancelButtonId + ".WIDTH}+${PADDING}");
                fileName->setY_Expr("${PARENT.ABSOLUTE_Y}+${PADDING}");
                fileName->setHeight_Expr("30");
                fileName->setWidth_Expr("${PARENT.USABLE_WIDTH}-${" + cancelButtonId + ".WIDTH}-${" + saveButtonId + ".WIDTH}-${PADDING}*2");
                fileName->setTitle("Name...");
            }

            setShowTitleIcons(false);
            setTitle("");
        }
    };
}