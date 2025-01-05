
#pragma once
#include "../WidgetTable.h"
#include "core/nlohmann/json.hpp"
using json = nlohmann::json;

namespace Aquamarine
{
    class WidgetFileLocationsList : public WidgetTable
    {
    public:
        WidgetFileLocationsList(string persistentId, string widgetXML) : WidgetTable(persistentId, widgetXML)
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
            setShowTitleIcons(settings.getAttribute("properties", "showIcons", true));
            setDoesRespondToFileDrop(settings.getAttribute("properties", "doesRespondToFileDrop", true));

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = Widget::saveState();
            settings.setAttribute("widget", "class", WidgetManager::WIDGET_CLASS_FILE_LOCATIONS_LIST, 0);

            settings.pushTag("widget");
            settings.setAttribute("properties", "doesRespondToFileDrop", getDoesRespondToFileDrop(), 0);
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~WidgetFileLocationsList() {
        };

        virtual void update(WidgetContext context) override
        {
            WidgetTable::update(context);
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
            WidgetTable::onWidgetEventReceived(args);
            string event = args.getFullEventName();

            if (args.eventName == WIDGET_EVENT::TABLE_CELL_HOVERED && args.sender.getPersistentId() == getPersistentId())
            {
                WidgetTableRow *eventRow = getTableRow(args.eventXML);
                WidgetTableCell *eventCell = getTableCell(args.eventXML);
                if (eventCell)
                {
                    mDragData = eventRow->key;
                }
            }

            if (args.eventName == WIDGET_EVENT::RECEIVE_DRAG_AND_DROP_DATA)
            {
                if (getDoesRespondToFileDrop() && getPersistentId() != args.sender.getPersistentId())
                {
                    string path = args.eventXML.getValue("event", "");
                    if (ofDirectory::doesDirectoryExist(path, false))
                    {

                        string faveName = ofFilePath::getFileName(path + ".F_A_V_O_R_I_T_E__E_X_T");
                        ofStringReplace(faveName, ".F_A_V_O_R_I_T_E__E_X_T", "");

                        addFavouriteLocation(faveName, path);
                        populateLocations();
                    }
                }
            }
        }

        /*
            String[] mExternalStorageDirectories = new String[] {
                    "/mnt/sdcard-ext",
                    "/mnt/sdcard/external_sd",
                    "/sdcard/sd",
                    "/mnt/external_sd",
                    "/emmc",
                    "/mnt/sdcard/bpemmctest",
                    "/mnt/sdcard/_ExternalSD",
                    "/mnt/Removable/MicroSD",
                    "/Removable/MicroSD",
                    "/sdcard"};
        */

        void populateRemovableLocations()
        {

#ifdef TARGET_WIN32

#endif

#ifdef TARGET_OSX

            ofDirectory dir("/Volumes");

            if (dir.exists())
            {

                dir.listDir();

                for (int i = 0; i < dir.size(); i++)
                {
                    if (dir[i].isDirectory())
                    {
                        string guessIcon = "REG_DRIVE";
                        string fileName = dir[i].getFileName();
                        if (fileName == "/")
                            continue;

                        if (ofIsStringInString(fileName, "GoogleDrive"))
                        {
                            guessIcon = "REG_CLOUD";
                        }

                        addLocation(dir[i].getFileName(), dir.getPath(i), guessIcon, "DIR");
                    }
                }
            }
#endif

#ifdef TARGET_LINUX

#endif
        }

        void populateLocations()
        {
            // Header

            mRows.clear();

            vector<WidgetTableCell> headerCells = vector<WidgetTableCell>();

            if (getShowIcons())
            {
                headerCells.push_back(
                    WidgetTableCell(
                        "ICON",
                        "",
                        22 * 1.5f));
            }

            headerCells.push_back(
                WidgetTableCell(
                    "PATH",
                    "",
                    100,
                    100));

            addLocation("Computer", "/", "REG_COMPUTER", "DIR");
            addLocation("Home", ofFilePath::getUserHomeDir(), "REG_HOME", "DIR");
            addLocation("Desktop", ofFilePath::join(ofFilePath::getUserHomeDir(), "Desktop"), "REG_DESKTOP", "DIR");
            addLocation("Documents", ofFilePath::join(ofFilePath::getUserHomeDir(), "Documents"), "REG_DOCUMENTS", "DIR");
            addLocation("Downloads", ofFilePath::join(ofFilePath::getUserHomeDir(), "Downloads"), "REG_DOWNLOADS", "DIR");

            populateRemovableLocations();

            // addLocation("Google Drive", "[GOOGLE_DRIVE]", "REG_CLOUD", "DIR");
            // addLocation("AWS S3", "[AWS_S3]", "REG_CLOUD", "DIR");

            // Favourite locations
            json settings = WidgetSettings::getSettingsJson();

            if (settings.contains("settings") && settings["settings"].contains("favouriteLocations")) {
                for (auto& loc : settings["settings"]["favouriteLocations"].items()) {            
                    string path, name;
                    loc.value()["path"].get_to(path);
                    loc.value()["name"].get_to(name);
                    addLocation(name, path, "", "DIR");
                }
            }

            setHeaderRow(WidgetTableRow("LOCATIONS", headerCells));
            setTableRows(getLocations());
        }

        static void addFavouriteLocation(string name, string path)
        {
            addRecentLocation(path, "favouriteLocations", name);
        }

        static void addRecentLocation(string path)
        {
            addRecentLocation(path, "recentLocations");
        }

        static void addRecentFile(string path)
        {
            addRecentLocation(path, "recentFiles");
        }

        static vector<string> getRecentFiles()
        {
            return getRecentLocation("recentFiles");
        }

        static string getMostRecentFile()
        {
            return getMostRecentLocation("recentFiles");
        }

        static void addRecentDirectory(string path)
        {
            addRecentLocation(path, "recentDirectories");
        }

        static vector<string> getRecentDirectories()
        {
            return getRecentLocation("recentDirectories");
        }

        static string getMostRecentDirectory()
        {
            return getMostRecentLocation("recentDirectories");
        }

        static void addRecentProject(string path)
        {
            addRecentLocation(path, "recentProjects");
        }

        static vector<string> getRecentProjects()
        {
            return getRecentLocation("recentProjects");
        }

        static string getMostRecentProject()
        {
            return getMostRecentLocation("recentProjects");
        }

        static void addRecentLocation(string filePath, string tagName, string name = "")
        {

            json sett = WidgetSettings::getSettingsJson();

            vector<json> locationsAdded;
            locationsAdded.push_back( { {"path", filePath}, {"name", name} });

            if (!sett.contains("settings")) sett["settings"] = nullptr;
            if (!sett["settings"].contains(tagName)) sett["settings"][tagName] = {};


            auto ary = sett["settings"][tagName];

            for (auto& loc : ary.items()) {            
                string path;
                loc.value()["path"].get_to(path);

                string name;
                loc.value()["name"].get_to(name);

                if (std::find(ary.begin(), ary.end(), path) == ary.end())
                {
                    if (locationsAdded.size() < 5) locationsAdded.push_back( { {"path", path}, {"name", name} });
                }
            }

            sett["settings"][tagName] = locationsAdded;

            std::ofstream ofs(Shared::getSettingsFileFullPath());
            ofs << sett;
        }

        static vector<string> getRecentLocation(string tagName)
        {
            json sett = WidgetSettings::getSettingsJson();

            if (!sett.contains("settings")) sett["settings"] = nullptr;
            if (!sett["settings"].contains(tagName)) sett["settings"][tagName] = {};

            vector<string> locations = vector<string>();
            auto ary = sett["settings"][tagName];

            for (auto& loc : ary.items()) {            
                string path;
                loc.value()["path"].get_to(path);
                locations.push_back(path);
            }
            return locations;
        }

        static string getMostRecentLocation(string tagName)
        {
            vector<string> locations = getRecentLocation(tagName);
            return (locations.size() > 0 ? locations[0] : "");
        }

        virtual void addLocation(string name, string path, string iconTag, string type)
        {
            WidgetTableRow row;

            row.key = path;

            if (getShowIcons())
            {
                row.cells.push_back(
                    WidgetTableCell(type, iconTag));
            }

            row.cells.push_back(
                WidgetTableCell(path, name));
            mRows.push_back(row);
        }

        vector<WidgetTableRow> getLocations()
        {
            return mRows;
        }

        void onWidgetMouseContentDragged(ofMouseEventArgs &e) override
        {
            setDragData(mDragData);
        }

        virtual void onWidgetThemeChanged(WidgetEventArgs args) override
        {
            setRowBGColor(ofColor(0, 0, 0, 0));
            setRowAltBGColor(ofColor(0, 0, 0, 0));
        }

        virtual void drawCellContent(bool isRowSelected, bool isRowHovered, bool isCellHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, WidgetTableRow &row, WidgetTableCell &cell, int rowIndex, int colIndex)
        {

            if (colIndex > 0)
                WidgetTable::drawCellContent(isRowSelected, isRowHovered, isCellHovered, absoluteScaledX, absoluteScaledY, absoluteScaledLabelX, absoluteScaledLabelY, scaledWidth, scaledHeight, row, cell, rowIndex, colIndex);

            if (getShowIcons() && colIndex == 0)
            {
                if (cell.key == "DIR")
                {
                    if (cell.label == "REG_COMPUTER")
                    {
                        iconComputer.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                        iconComputer.setScaledPos(absoluteScaledX + scale(4) * 1.5f, absoluteScaledY + scale(4) * 1.5f);
                        iconComputer.drawSvg();
                    }
                    else if (cell.label == "REG_HOME")
                    {
                        iconHome.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                        iconHome.setScaledPos(absoluteScaledX + scale(4) * 1.5f, absoluteScaledY + scale(4) * 1.5f);
                        iconHome.drawSvg();
                    }
                    else if (cell.label == "REG_DESKTOP")
                    {
                        iconDesktop.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                        iconDesktop.setScaledPos(absoluteScaledX + scale(4) * 1.5f, absoluteScaledY + scale(4) * 1.5f);
                        iconDesktop.drawSvg();
                    }
                    else if (cell.label == "REG_DOCUMENTS")
                    {
                        iconDocuments.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                        iconDocuments.setScaledPos(absoluteScaledX + scale(4) * 1.5f, absoluteScaledY + scale(4) * 1.5f);
                        iconDocuments.drawSvg();
                    }
                    else if (cell.label == "REG_DOWNLOADS")
                    {
                        iconDownloads.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                        iconDownloads.setScaledPos(absoluteScaledX + scale(4) * 1.5f, absoluteScaledY + scale(4) * 1.5f);
                        iconDownloads.drawSvg();
                    }
                    else if (cell.label == "REG_CLOUD")
                    {
                        iconCloud.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                        iconCloud.setScaledPos(absoluteScaledX + scale(4) * 1.5f, absoluteScaledY + scale(4) * 1.5f);
                        iconCloud.drawSvg();
                    }
                    else if (cell.label == "REG_DRIVE")
                    {
                        iconDrive.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                        iconDrive.setScaledPos(absoluteScaledX + scale(4) * 1.5f, absoluteScaledY + scale(4) * 1.5f);
                        iconDrive.drawSvg();
                    }
                    else
                    {
                        iconDir.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                        iconDir.setScaledPos(absoluteScaledX + scale(4) * 1.5f, absoluteScaledY + scale(4) * 1.5f);
                        iconDir.drawSvg();
                    }
                }
                else
                {
                    iconFile.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                    iconFile.setScaledPos(absoluteScaledX + scale(4) * 1.5f, absoluteScaledY + scale(4) * 1.5f);
                    iconFile.drawSvg();
                }
            }
        }

        virtual void drawContentDragIcon()
        {
            iconDrag.setScaledPos(ofGetMouseX(), ofGetMouseY());
            iconDrag.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
            iconDrag.drawSvg();
        }

        void setIcons(bool val)
        {
            mShowIcons = val;
        }

        bool getShowIcons()
        {
            return mShowIcons;
        }

        bool getShowFileSize()
        {
            return mShowFileSize;
        }

        bool getDoesRespondToFileDrop()
        {
            return mDoesRespondToFileDrop;
        }

        void setDoesRespondToFileDrop(bool val)
        {
            mDoesRespondToFileDrop = val;
        }

        virtual void setPath(string val)
        {
            selectRowByKey(val, true, false);
        }

        virtual void setPath(string val, bool fireEvent)
        {
            selectRowByKey(val, true, fireEvent);
        }

    protected:
        void initWidgetScale(float scale) override
        {
            WidgetTable::initWidgetScale(scale);
            iconComputer = IconCache::getIcon("REG_COMPUTER");
            iconHome = IconCache::getIcon("REG_HOME");
            iconDesktop = IconCache::getIcon("REG_DESKTOP");
            iconDocuments = IconCache::getIcon("REG_DOCUMENTS");
            iconDownloads = IconCache::getIcon("REG_DOWNLOADS");
            iconCloud = IconCache::getIcon("REG_CLOUD");
            iconDrive = IconCache::getIcon("REG_DRIVE");

            iconFile = IconCache::getIcon("REG_FILE");
            iconDir = IconCache::getIcon("REG_FOLDER");
            iconDrag = IconCache::getIcon("REG_FILE");
            iconDrag.scaleSvg(2, 2);

            iconDir.scaleSvg(1.5f, 1.5f);
            iconFile.scaleSvg(1.5f, 1.5f);
            iconComputer.scaleSvg(1.5f, 1.5f);
            iconHome.scaleSvg(1.5f, 1.5f);
            iconDesktop.scaleSvg(1.5f, 1.5f);
            iconDocuments.scaleSvg(1.5f, 1.5f);
            iconDownloads.scaleSvg(1.5f, 1.5f);
            iconCloud.scaleSvg(1.5f, 1.5f);
        }

        void initWidget() override
        {
            setShowColumnFilters(false, false);
            setFixedHeaderRowHeight(0);
            setRowHeight(26 * 1.5f);

            setIsAutoResizeToContentsWidth(false);
            setIsAutoResizeToContentsHeight(false);

            if (!mWasPopulated)
            {
                populateLocations();
                mWasPopulated = true;
            }
        }

    private:
        string mPath = "";
        bool mShowIcons = true;
        bool mShowFileSize = true;
        string mDragData = "";
        bool mDoesRespondToFileDrop = true;

        Icon iconFile, iconDir, iconDrag, iconComputer, iconHome, iconDesktop, iconDocuments, iconDownloads, iconCloud, iconDrive;
        vector<WidgetTableRow> mRows;
        bool mWasPopulated = false;
    };
}