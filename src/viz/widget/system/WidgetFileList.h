#pragma once
#include "../WidgetTable.h"

namespace Aquamarine
{
#if defined(TARGET_WIN32)
#include <windows.h>
#include <stdio.h>
#endif

    /*
    #include "Poco/Base64Encoder.h"
    #include "Poco/Crypto/RSADigestEngine.h"
    #include "Poco/HMACEngine.h"


    class SHA256Engine : public Poco::Crypto::DigestEngine
    {
    public:
        enum
        {
            BLOCK_SIZE = 64,
            DIGEST_SIZE = 32
        };

        SHA256Engine()
                : DigestEngine("SHA256")
        {
        }

    };
    */

    class WidgetFileList : public WidgetTable
    {
    public:
        WidgetFileList(string persistentId, string widgetXML) : WidgetTable(persistentId, widgetXML)
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
            setShowFileSize(settings.getAttribute("properties", "showFileSize", true));
            setShowTitleIcons(settings.getAttribute("properties", "showIcons", true));
            setMediaMode(settings.getAttribute("properties", "mediaMode", false));
            setDoesRespondToFileDrop(settings.getAttribute("properties", "doesRespondToFileDrop", true));

            /* setPath Should always be last */
            setPath(settings.getAttribute("properties", "path", ""));

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = Widget::saveState();
            settings.setAttribute("widget", "class", WidgetManager::WIDGET_CLASS_FILE_LIST, 0);

            settings.pushTag("widget");
            settings.setAttribute("properties", "doesRespondToFileDrop", getDoesRespondToFileDrop(), 0);
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget
            return settings;
        }

        virtual ~WidgetFileList() {
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
                    if (ofFile::doesFileExist(eventCell->key, false))
                    {
                        mDragData = eventCell->key;
                    }
                    else
                    {
                        mDragData = eventRow->key;
                    }
                }
            }

            if (args.eventName == WIDGET_EVENT::RECEIVE_DRAG_AND_DROP_DATA)
            {
                if (getDoesRespondToFileDrop())
                {
                    string path = args.eventXML.getValue("event", "");
                    if (ofDirectory::doesDirectoryExist(path, false) && ofFile::doesFileExist(path, false))
                    {
                    }
                }
            }
        }

        string getPath()
        {
            return mPath;
        }

#if defined(TARGET_WIN32)

        std::vector<std::string> getWindowsListOfDrives()
        {
            std::vector<std::string> arrayOfDrives;
            char *szDrives = new char[MAX_PATH]();
            if (GetLogicalDriveStringsA(MAX_PATH, szDrives))
                ;
            for (int i = 0; i < 100; i += 4)
                if (szDrives[i] != (char)0)
                    arrayOfDrives.push_back(std::string{szDrives[i], szDrives[i + 1], szDrives[i + 2]});
            delete[] szDrives;
            return arrayOfDrives;
        }

        vector<ofFile> getWindowsRoot()
        {
            vector<std::string> drives = getWindowsListOfDrives();
            vector<ofFile> files;
            for (string drive : drives)
            {
                files.emplace_back(drive, ofFile::Reference);
            }
            return files;
        }
#endif

        void setPath(string val)
        {
            mPath = val;

            setTableUseFbo(false);

            // Header
            vector<WidgetTableCell> headerCells = vector<WidgetTableCell>();

            int cols = 3;

            if (getMediaMode())
            {

                for (int i = 1; i <= cols; i++)
                {
                    headerCells.push_back(
                        WidgetTableCell(
                            "FILE",
                            "",
                            30,
                            0));
                }
            }
            else
            {

                if (getShowIcons())
                {
                    headerCells.push_back(
                        WidgetTableCell(
                            "ICON",
                            "",
                            22));
                }

                headerCells.push_back(
                    WidgetTableCell(
                        "FILE",
                        "",
                        100,
                        0));

                if (getShowFileSize())
                {
                    headerCells.push_back(
                        WidgetTableCell(
                            "SIZE",
                            "",
                            100));
                }
            }

            vector<WidgetTableRow> dir_rows = vector<WidgetTableRow>();
            vector<WidgetTableRow> file_rows = vector<WidgetTableRow>();

            if (mPath == "[ROOT]")
            {
                // vector<std::string> roots;
                // Poco::Foundation_API::Path::listRoots(roots);
            }
            else if (mPath == "[NETWORK]")
            {
            }
            else if (mPath == "[GOOGLE_DRIVE]")
            {

                /*
                string JWT_Raw = R"(
                {"alg":"RS256","typ":"JWT"}.
                {
                "iss":"${iss}",
                "scope":"https://www.googleapis.com/auth/prediction",
                "aud":"https://oauth2.googleapis.com/token",
                "exp":${exp},
                "iat":${iat}
                }.
                ${signature_bytes}
                )";

                uint64_t sys_time = ofGetUnixTime();
                ofStringReplace(JWT_Raw, "${iss}", "888996358641-orro2bh1t7c1g4e04uf8m5ocopqggg16.apps.googleusercontent.com");
                ofStringReplace(JWT_Raw, "${exp}", ofToString(sys_time+60*60));
                ofStringReplace(JWT_Raw, "${iat}", ofToString(sys_time));
                ofStringReplace(JWT_Raw, "${signature_bytes}", "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9"); // Base 64'd {"alg":"RS256","typ":"JWT"}

                cout << JWT_Raw;

                stringstream ss;
                ss.str("");
                Poco::Base64Encoder encoder(ss);
                encoder << JWT_Raw;
                encoder.close();
                string JWT = ss.str();
                //ofStringReplace(JWT, "\n", "");

                cout << JWT;


                Poco::HMACEngine<SHA256Engine> hmac{"privateKey"};
                hmac.update(JWT);
                std::cout << "HMACE hex:" << Poco::DigestEngine::digestToHex(hmac.digest()) << std::endl;// lookout difest() calls reset ;)


                        ofHttpRequest request;
                        request.method = ofHttpRequest::POST;
                        request.url = R"(
                https://accounts.google.com/o/oauth2/v2/auth?
                grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion=${jwt}
                )";
                ofStringReplace(request.url, "${jwt}", JWT);
                ofStringReplace(request.url, "\n", "");



                    //    request.body = "{ \"test\": \"post\" }";
                        ofURLFileLoader http;
                        auto response = http.handleRequest(request);
                        ofLogNotice() << response.status << std::endl;
                        ofLogNotice() << response.data.getText() << std::endl;
                */

                ofDirectory dir("/home/zabba/Downloads");
                int numFiles;
                std::vector<ofFile> files;

                numFiles = dir.listDir();
                dir.sort();
                files = dir.getFiles();

                for (ofFile file : files)
                {

                    // if(ofToUpper(file.getExtension()) != "MP4") continue;

                    string fileName = file.getFileName();

                    WidgetTableRow row;

                    row.key = file.getAbsolutePath();

                    if (getShowIcons())
                    {
                        row.cells.push_back(
                            WidgetTableCell(file.isDirectory() ? "DIR" : "FILE", ""));
                    }

                    row.cells.push_back(
                        WidgetTableCell(fileName, fileName));

                    string fileSize = "";
                    if (!file.isDirectory())
                        fileSize = Shared::Pretty_Bytes(file.getSize());

                    if (getShowFileSize())
                    {
                        row.cells.push_back(
                            WidgetTableCell("", file.isDirectory() ? "" : fileSize));
                    }

                    row.key = file.getAbsolutePath();

                    file.isDirectory() ? dir_rows.push_back(row) : file_rows.push_back(row);
                }
            }
            else
            {

                if (!ofDirectory::doesDirectoryExist(mPath, false))
                {
                    cout << mPath << " does not exist, reverting to Home directory";
                    mPath = ofFilePath::getUserHomeDir();
                }
                ofDirectory dir(mPath);
                int numFiles;
                std::vector<ofFile> files;

                try
                {
                    numFiles = dir.listDir();
                    dir.sort();
                    files = dir.getFiles();

#if defined(TARGET_WIN32)
                    if (mPath == "/")
                        files = getWindowsRoot();
#endif
                }
                catch (...)
                {
                    mPermissionDenied = true;
                    WidgetTableRow row;
                    row.key = "***DENIED***";

                    if (getShowIcons())
                    {
                        row.cells.push_back(
                            WidgetTableCell("Access denied", ""));
                    }

                    row.cells.push_back(
                        WidgetTableCell("DIR", "Access denied"));

                    if (getShowFileSize())
                    {
                        row.cells.push_back(
                            WidgetTableCell("", ""));
                    }
                    row.key = "***DENIED***";
                    dir_rows.push_back(row);
                }

                if (getMediaMode())
                {

                    setTableUseFbo(true);

                    setRowHeight(200);
                    setSelectionMode("CELL");

                    WidgetTableRow row;
                    WidgetTableRow dirrow;
                    int dirCount = 0, fileCount = 0, totalCount = 0;
                    for (ofFile file : files)
                    {

                        string fileName = file.getFileName();
                        string fullFilePath = file.getAbsolutePath();
                        // ofStringReplace(fullFilePath, " ", "\\ ");

                        // cout << "EXTR" << file.getExtension();

                        if (file.isDirectory() || file.isDevice())
                        {

                            WidgetTableCell c = WidgetTableCell(fullFilePath, (mPath == "/" ? fullFilePath : fileName), "center");
                            c.setMetadata("TYPE", "DIR");

                            dirrow.cells.push_back(
                                c);

                            if (cols == 1 || dirCount > 0 && (dirCount + 1) % cols == 0 || totalCount == files.size() - 1)
                            {
                                dir_rows.push_back(dirrow);
                                dirrow.cells.clear();
                            }
                            dirCount++;
                            totalCount++;
                        }
                        else
                        {

                            if (file.getExtension() == "" || !ofIsStringInString(".jpg|.jpeg|.png|.bmp|.svg|", file.getExtension() + "|"))
                            {
                                continue;
                            }

                            WidgetTableCell c = WidgetTableCell(fullFilePath, "", "center");
                            c.setMetadata("TYPE", "FILE");

                            row.cells.push_back(
                                c);

                            if (cols == 1 || fileCount > 0 && (fileCount + 1) % cols == 0 || totalCount == files.size() - 1)
                            {
                                file_rows.push_back(row);
                                row.cells.clear();
                            }
                            fileCount++;
                            totalCount++;
                        }
                    }
                }
                else
                {

                    setSelectionMode("ROW");
                    string fileExtensionFilter = ofToLower(getFileExtensionFilter());

                    for (ofFile file : files)
                    {
                        string fileName = file.getFileName();
                        string fullFilePath = file.getAbsolutePath();

                        if (fileExtensionFilter != "" && !ofIsStringInString(fileExtensionFilter, ofToLower(file.getExtension()) + "|"))
                        {
                            continue;
                        }

                        WidgetTableRow row;

                        row.key = fullFilePath;
                        row.setMetadata("TYPE", file.isDirectory() ? "DIR" : "FILE");

                        if (getShowIcons())
                        {
                            row.cells.push_back(
                                WidgetTableCell(file.isDirectory() ? "DIR" : "FILE", ""));
                        }

                        row.cells.push_back(
                            WidgetTableCell(fileName, (mPath == "/" ? fullFilePath : fileName)));

                        string fileSize = "";
                        if (!file.isDirectory())
                            fileSize = Shared::Pretty_Bytes(file.getSize());

                        if (getShowFileSize())
                        {
                            row.cells.push_back(
                                WidgetTableCell("", file.isDirectory() ? "" : fileSize));
                        }

                        row.key = file.getAbsolutePath();

                        file.isDirectory() ? dir_rows.push_back(row) : file_rows.push_back(row);
                    }
                }
            }

            std::vector<WidgetTableRow> rows(dir_rows);
            rows.insert(rows.end(), file_rows.begin(), file_rows.end());

            setHeaderRow(WidgetTableRow("FILES", headerCells));
            setTableRows(rows);
            setIsAutoResizeToContentsWidth(false); // Not compatible setting to true and using cell percentages at same time
            setIsAutoResizeToContentsHeight(false);
        }

        string getFileExtensionFilter()
        {
            return mFileExtensionFilter;
        }

        void setFileExtensionFilter(string val)
        {
            mFileExtensionFilter = val;
        }

        void addCustomFileRow(string TYPE, string key, string title, int rowIndex, string rowKey)
        { // type = [DIR|FILE]

            vector<WidgetTableCell> cells = vector<WidgetTableCell>();
            if (getShowIcons())
            {
                cells.push_back(
                    WidgetTableCell(
                        "ICON",
                        "",
                        22));
            }

            cells.push_back(
                WidgetTableCell(
                    key,
                    title,
                    100,
                    0));

            if (getShowFileSize())
            {
                cells.push_back(
                    WidgetTableCell(
                        "SIZE",
                        "",
                        100));
            }

            WidgetTableRow row(rowKey, cells);
            row.key = key;
            row.setMetadata("TYPE", TYPE);
            addRow(row, rowIndex);
        }

        void onWidgetMouseContentDragged(ofMouseEventArgs &e) override
        {
            setDragData(mDragData);
        }

        void onWidgetMouseContentDragReleased(ofMouseEventArgs &e, string dragData) override
        {

            string zzz = getTargetDropWidgetId();

            if (getTargetDropWidgetId() == "" || getTargetDropWidgetId() == getWidgetId())
            {
                // target was "widget desktop" instantiate a new image control
                cout << " HIYA !";
            }
        }

        virtual void drawCellContent(bool isRowSelected, bool isRowHovered, bool isCellHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, WidgetTableRow &row, WidgetTableCell &cell, int rowIndex, int colIndex)
        {

            if (getMediaMode())
            {

                if (cell.getMetadata("TYPE") == "FILE")
                {
                    Icon img = Icon(
                        cell.key,
                        Coord::vizBounds(absoluteScaledX + scale(10), absoluteScaledY + scale(10), scaledWidth - scale(20), scaledHeight - scale(20)),
                        1.0f, Icon::IconSize::REGULAR, ofColor::white, -1);
                    img.draw();

                    /*
                                    ofPushStyle();
                                    ofSetColor(cell.cellSlice.isHovered() ? getCellLabelSelectedColor() : getCellLabelColor());
                                    Coord::vizPoint p = getAlignmentPointForCellLabel(cell, absoluteScaledLabelX, absoluteScaledLabelY + scale(35), scaledWidth, scaledHeight);
                                    getFont()->draw(cell.label, p.x, p.y);
                                    ofPopStyle();
                                    */
                }
                else
                {
                    WidgetTable::drawCellContent(isRowSelected, isRowHovered, isCellHovered, absoluteScaledX, absoluteScaledY, absoluteScaledLabelX, absoluteScaledLabelY, scaledWidth, scaledHeight, row, cell, rowIndex, colIndex);
                }
            }
            else
            {
                WidgetTable::drawCellContent(isRowSelected, isRowHovered, isCellHovered, absoluteScaledX, absoluteScaledY, absoluteScaledLabelX, absoluteScaledLabelY, scaledWidth, scaledHeight, row, cell, rowIndex, colIndex);

                if (getShowIcons() && colIndex == 0)
                {
                    if (cell.key == "DIR")
                    {
                        iconDir.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                        iconDir.setScaledPos(absoluteScaledX + scale(4), absoluteScaledY + scale(4));
                        iconDir.drawSvg();
                    }
                    else
                    {
                        iconFile.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                        iconFile.setScaledPos(absoluteScaledX + scale(4), absoluteScaledY + scale(4));
                        iconFile.drawSvg();
                    }
                }
            }
        }

        virtual void drawRowHighlight_Hover(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int scaledWidth, int scaledHeight, WidgetTableRow &row, int rowIndex)
        {
            if (getMediaMode())
            {
            }
            else
            {
                WidgetTable::drawRowHighlight_Hover(isRowSelected, isRowHovered, absoluteScaledX, absoluteScaledY, scaledWidth, scaledHeight, row, rowIndex);
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

        bool getMediaMode()
        {
            return mMediaMode;
        }

        void setMediaMode(bool val)
        {
            mMediaMode = val;
        }

        void setShowFileSize(bool val)
        {
            mShowFileSize = val;
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

    private:
        string mPath = "";
        bool mShowIcons = true;
        bool mMediaMode = false;
        bool mShowFileSize = true;
        string mDragData = "";
        bool mDoesRespondToFileDrop = true;
        bool mPermissionDenied = false;
        string mFileExtensionFilter = ""; // eg |.jpg|

        Icon iconFile, iconDir, iconDrag;
        shared_ptr<ofxSmartFont> font;

        void initWidget() override
        {
            font = getViz()->getMediumFont();
            setShowColumnFilters(false, false);
            setFixedHeaderRowHeight(0);
            setRowHeight(26);
            iconFile = IconCache::getIcon("REG_FILE");
            iconDir = IconCache::getIcon("REG_FOLDER");
            iconDrag = IconCache::getIcon("REG_FILE");
            iconDrag.scaleSvg(2, 2);
        }
    };
}