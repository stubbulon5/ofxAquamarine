
#pragma once
#include "../uiVizWidget.h"
#include "uiVizWidgetFileList.h"
#include "uiVizWidgetFileLocationsList.h"
#include "uiVizWidgetDialog.h"

namespace Aquamarine
{
    class uiVizWidgetFileExplorer : public uiVizWidget
    {
    public:
        uiVizWidgetFileExplorer(string persistentId, string widgetXML) : uiVizWidget(persistentId, widgetXML)
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
            setMediaMode(settings.getAttribute("properties", "mediaMode", false));

            /* setPath Should always be last */
            setPath(settings.getAttribute("properties", "path", ""));

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = uiVizWidget::saveState();
            settings.setAttribute("widget", "class", uiVizWidgetManager::WIDGET_CLASS_FILE_EXPLORER, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~uiVizWidgetFileExplorer()
        {
            ofRemoveListener(fileLocationsList->widgetEventReceived, this, &uiVizWidgetFileExplorer::onFileLocationsEventReceived);
            ofRemoveListener(fileList->widgetEventReceived, this, &uiVizWidgetFileExplorer::onFileLlistEventReceived);
        };

        string getPath()
        {
            return mPath;
        }

        virtual void setPath(string val)
        {
            mPath = val;

            if (mPath.size() > 0 && mPath[0] == '[')
            {
                // Special location eg, cloud, network
            }
            else
            {
                if (!ofDirectory::doesDirectoryExist(mPath, false))
                    mPath = ofFilePath::getUserHomeDir();
                breadcrumb->setBreadcrumbItemItems(mPath, OS_SLASH_CHAR);

                mSelectedDir = mPath;
                mSelectedDirExists = ofDirectory::doesDirectoryExist(mSelectedDir, false);
            }

            if (fileLocationsList)
                fileLocationsList->setPath(mPath);
            if (fileList)
                fileList->setPath(mPath);
        }

        void onFileLocationsEventReceived(uiVizWidgetEventArgs &args)
        {
            string event = args.getFullEventName();
            if (fileLocationsList && args.sender.getPersistentId() == fileLocationsList->getPersistentId())
            {
                if (args.eventName == WIDGET_EVENT::TABLE_CELL_CLICKED)
                {
                    mSelectedFile = "";
                    mSelectedFileExists = false;
                    mSelectedDir = "";
                    mSelectedDirExists = false;
                    uiVizWidgetTableRow *eventRow = fileLocationsList->getTableRow(args.eventXML);
                    uiVizWidgetTableCell *eventCell = fileLocationsList->getTableCell(args.eventXML);
                    if (eventCell)
                    {
                        string key = eventRow->key;
                        setPath(key);
                    }
                }
                else if (args.eventName == WIDGET_EVENT::RESIZED)
                {
                    fileLocationsList->setHeight(getUsableHeight() + getNonScaledPadding());
                    fileLocationsList->setMaxWidth(getUsableWidth() - fileList->getMinWidth());
                    uiVizWidgetManager::recalculateWidgetExpressions(*fileList);
                    setWidgetNeedsUpdate(true);
                }
            }
        }

        void onFileLlistEventReceived(uiVizWidgetEventArgs &args)
        {
            string event = args.getFullEventName();
            if (fileList && args.sender.getPersistentId() == fileList->getPersistentId())
            {

                if (args.eventName == WIDGET_EVENT::TABLE_CELL_SELECTED || args.eventName == WIDGET_EVENT::TABLE_CELL_UNSELECTED)
                {
                    mSelectedFile = "";
                    mSelectedFileExists = false;
                    mSelectedDir = "";
                    mSelectedDirExists = false;

                    if (args.eventName == WIDGET_EVENT::TABLE_CELL_UNSELECTED)
                    {
                        setNeedsUpdate(true);
                        return;
                    }

                    uiVizWidgetTableRow *eventRow = fileList->getTableRow(args.eventXML);
                    uiVizWidgetTableCell *eventCell = fileList->getTableCell(args.eventXML);
                    if (eventCell && eventRow->getMetadata("TYPE") == "DIR")
                    {
                        string key = eventRow->key;
                        setPath(key);
                    }
                    else if (eventCell && eventCell->getMetadata("TYPE") == "DIR")
                    {
                        string key = eventCell->key;
                        setPath(key);
                    }
                    else
                    {
                        mSelectedFile = eventRow->key;
                        mSelectedFileExists = ofFile::doesFileExist(mSelectedFile, false);
                        setNeedsUpdate(true);
                    }
                }
            }
        }

        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {
            string event = args.getFullEventName();
            if (args.eventName == WIDGET_EVENT::CLICK && breadcrumb && args.sender.getPersistentId() == breadcrumb->getPersistentId())
            {
                string key = args.eventXML.getValue("event", "");

#ifdef TARGET_WIN32
                if (key.find("\\", 1))
                    key = key.replace(0, 1, ""); // remove the leading "\"" for windows path
#endif

                if (key != "")
                {
                    fileList->setPath(key);
                    // string path = breadcrumb->getBreadcrumbItemKeyString(index);
                }
            }
        }

        string getFileExtensionFilter()
        {
            return fileList ? fileList->getFileExtensionFilter() : "";
        }

        void setFileExtensionFilter(string val)
        {
            if (fileList)
            {
                fileList->setFileExtensionFilter(val);
                fileList->setPath(fileList->getPath());
            }
        }

        string getSelectedFile()
        {
            return mSelectedFile;
        }

        bool getSelectedFileExists()
        {
            return mSelectedFileExists;
        }

        string getSelectedDir()
        {

            // size_t lastdot = mSelectedDir.find_last_of("/");
            // if (lastdot == std::string::npos) {
            //     mSelectedDir = mSelectedDir.substr(0, lastdot);
            // }

            // lastdot = mSelectedDir.find_last_of("\\");
            // if (lastdot == std::string::npos) {
            //     mSelectedDir = mSelectedDir.substr(0, lastdot-1);
            // }

            return mSelectedDir;
        }

        bool getSelectedDirExists()
        {
            return mSelectedDirExists;
        }

        bool getMediaMode()
        {
            return mMediaMode;
        }

        void setMediaMode(bool val)
        {
            mMediaMode = val;
            fileList->setMediaMode(val);
            fileList->setNeedsUpdate(true);
        }

        virtual void setTheme(uiVizWidgetTheme val) override
        {
            uiVizWidget::setTheme(val);

            // Theme overrides...

            if (breadcrumb)
            {
                breadcrumb->setIsRoundedRectangle(false);
            }

            if (fileLocationsList)
            {
                fileLocationsList->setTheme(val);
                fileLocationsList->getTheme().setWidgetColor(ofColor(0, 0, 0, 0)); // done in setTheme
                fileLocationsList->setRowBGColor(ofColor(0, 0, 0, 0));
                fileLocationsList->setRowAltBGColor(ofColor(0, 0, 0, 0));
                fileLocationsList->setCellLabelColor(getTheme().TypographyTertiaryColor);
                fileLocationsList->setIsRoundedRectangle(false);
            }

            if (fileList)
            {
                fileList->setIsRoundedRectangle(false);
            }
        }

        uiVizWidgetFileList *getFileList()
        {
            return fileList;
        }

        uiVizWidgetDialog *getDialog()
        {
            if (!DIALOG)
            {
                DIALOG = new uiVizWidgetDialog("DIALOG", R"(
                <widget>
                    <bounds width="500" height="300" />
                </widget>
                )");

                uiVizWidgetManager::addWidgetForPopout(*DIALOG, getWidgetId(), true);
            }
            return DIALOG;
        }

    protected:
        string mSelectedFile = "";
        bool mSelectedFileExists = false;

        string mSelectedDir = "";
        bool mSelectedDirExists = false;

        string OS_SLASH_CHAR = "";

    private:
        string mPath = "";
        uiVizWidgetFileLocationsList *fileLocationsList = nullptr;
        uiVizWidgetFileList *fileList = nullptr;
        uiVizWidgetElmBreadcrumb *breadcrumb = nullptr;
        bool mMediaMode = false;
        uiVizWidgetDialog *DIALOG = nullptr;

        // File mask eg (jpg|png) or (wav|mp3) (mp4)
        // Show folders option

        void initWidget() override
        {

            setIsSystemWidget(true);

            string userHome = ofFilePath::getPathForDirectory(ofFilePath::getUserHomeDir());
            OS_SLASH_CHAR = userHome.back(); // Last char of home directory folder
            if (OS_SLASH_CHAR != "/" && OS_SLASH_CHAR != "\\")
            {
                OS_SLASH_CHAR = "/";
#ifdef TARGET_WIN32
                OS_SLASH_CHAR = "\\";
#endif
            }

            setMinWidth(400);
            setMinHeight(300);

            setWidth(400);
            setHeight(300);

            string breadcrumbId = getPersistentId() + "_BREADCRUMB";
            string fileLocationsId = getPersistentId() + "_FILE_LOCATIONS";
            string fileListId = getPersistentId() + "_FILE_LIST";

            if (!breadcrumb)
            {
                breadcrumb = dynamic_cast<uiVizWidgetElmBreadcrumb *>(addOrLoadWidgetElement(breadcrumb, WIDGET_ELM_CLASS::BREADCRUMB, breadcrumbId, R"(
                <element></element>
                )"));

                breadcrumb->setX_Expr("${PARENT.LEFT}+${" + fileLocationsId + ".WIDTH}");
                breadcrumb->setY_Expr("${PARENT.TOP}-${PADDING}");
                breadcrumb->setHeight_Expr("30");
                breadcrumb->setWidth_Expr("${PARENT.USABLE_WIDTH}-${" + fileLocationsId + ".WIDTH}");
            }

            if (!fileLocationsList)
            {
                fileLocationsList = new uiVizWidgetFileLocationsList(fileLocationsId, R"(
                <widget><bounds width="200"/></widget>
                )");
                fileLocationsList->setX_Expr("${PARENT.ABSOLUTE_USABLE_X}");
                fileLocationsList->setY_Expr("${PARENT.ABSOLUTE_USABLE_Y} - ${PADDING}");
                fileLocationsList->setHeight_Expr("${PARENT.USABLE_HEIGHT} + ${PADDING}");
                // done in setTheme
                // fileLocationsList->getTheme().setWidgetColor(ofColor(0,0,0,0));
                //  fileLocationsList->setRowBGColor(ofColor(0,0,0,0));
                //  fileLocationsList->setRowAltBGColor(ofColor(0,0,0,0));
                //  fileLocationsList->setCellLabelColor(getTheme().TypographyTertiaryColor);
                //  fileLocationsList->setIsRoundedRectangle(false);
                addChildWidget(*fileLocationsList, true);
                fileLocationsList->setIsResizable(true);
                ofAddListener(fileLocationsList->widgetEventReceived, this, &uiVizWidgetFileExplorer::onFileLocationsEventReceived);
            }

            if (!fileList)
            {
                fileList = new uiVizWidgetFileList(fileListId, R"(
                <widget></widget>
                )");

                fileList->setX_Expr("${" + fileLocationsId + ".RIGHT}");
                fileList->setY_Expr("${" + breadcrumbId + ".BOTTOM}");
                fileList->setHeight_Expr("${PARENT.USABLE_HEIGHT} + ${PADDING} - ${" + breadcrumbId + ".HEIGHT}");
                fileList->setWidth_Expr("${PARENT.USABLE_WIDTH} - ${" + fileLocationsId + ".WIDTH}");
                // done in setTheme
                // fileList->setIsRoundedRectangle(false);
                addChildWidget(*fileList, true);
                ofAddListener(fileList->widgetEventReceived, this, &uiVizWidgetFileExplorer::onFileLlistEventReceived);
            }

            setStaticTopTitleBarSize(40);

            setTitle("Explorer");
        }
    };
}