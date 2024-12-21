//
//  uiVizWidgetTable.h
//  bloom-macos
//
//
#pragma once
#include "uiVizWidget.h"

namespace Aquamarine
{
    class uiVizWidgetTable;
    class uiVizWidgetTableCell;
    class uiVizWidgetTableRow;

    class uiVizWidgetTableCellSlice
    {
    public:
        uiVizCoord::vizPoint labelPoint = uiVizCoord::vizPoint(0, 0);
        uiVizCoord::vizBounds bounds = uiVizCoord::vizBounds(0, 0, 0, 0);
        ofColor regularColor;
        ofColor selectedColor;
        ofColor hoveredColor;
        ofColor regularFontColor;
        ofColor selectedFontColor;
        ofColor hoveredFontColor;
        ofColor disabledFontColor;
        bool isEnabled = true;

        ofFbo fbo;
        bool needsUpdate = true;

        float currentAlpha = 0.0f;

        bool isHovered()
        {
            int mX = ofGetMouseX();
            int mY = ofGetMouseY();
            return mX > bounds.x && mY > bounds.y && mX < bounds.x + bounds.width && mY < bounds.y + bounds.height;
        }

        bool isHoveredOffset(int offsetX, int offsetY)
        {
            int mX = ofGetMouseX();
            int mY = ofGetMouseY();
            return mX > (bounds.x - offsetX) && mY > (bounds.y - offsetY) && mX < (bounds.x - offsetX + bounds.width) && mY < (bounds.y - offsetY + bounds.height);
        }

        void allocateFbo(bool initialize, int width, int height)
        {

            if (width <= 0 || height <= 0)
                return;

            fbo.allocate(width, height, GL_RGBA, 16);
            if (initialize)
            {
                fbo.begin();
                ofClear(255, 255, 255, 0);
                fbo.end();
            }
        }

        void clearFbo()
        {
            fbo.clear();
            needsUpdate = true;
        }

        uiVizWidgetTableCellSlice() {};

        ~uiVizWidgetTableCellSlice()
        {
            fbo.clear();
        }
    };

    class uiVizWidgetRowSlice
    {
    public:
        uiVizCoord::vizPoint labelPoint = uiVizCoord::vizPoint(0, 0);
        uiVizCoord::vizBounds bounds = uiVizCoord::vizBounds(0, 0, 0, 0);
        ofColor regularColor;
        ofColor selectedColor;
        ofColor hoveredColor;
        ofColor regularFontColor;
        ofColor selectedFontColor;
        ofColor hoveredFontColor;
        float currentAlpha = 0.0f;

        bool isHovered()
        {
            int mX = ofGetMouseX();
            int mY = ofGetMouseY();
            return mX > bounds.x && mY > bounds.y && mX < bounds.x + bounds.width && mY < bounds.y + bounds.height;
        }

        bool isHoveredOffset(int offsetX, int offsetY)
        {
            int mX = ofGetMouseX();
            int mY = ofGetMouseY();
            return mX > (bounds.x - offsetX) && mY > (bounds.y - offsetY) && mX < (bounds.x - offsetX + bounds.width) && mY < (bounds.y - offsetY + bounds.height);
        }

        uiVizWidgetRowSlice() {};

        ~uiVizWidgetRowSlice()
        {
        }
    };

    class uiVizWidgetTableCell
    {
    public:
        string label = "";
        string key = "";
        bool isSelected = false;
        uiVizWidgetBase *cellWidget = nullptr; // can be a uiVizWidget or uiVizEidgetElm
        bool cellWidgetInitialized = false;
        string horizontalAlign = "left"; // left || center || right

        string get_WIDGET_CLASS()
        {
            return WIDGET_CLASS;
        }

        uiVizWidgetTableCellSlice cellSlice = uiVizWidgetTableCellSlice();
        uiVizWidgetTableCell(string key, string label) : key(key), label(label) {};
        uiVizWidgetTableCell(string key, string label, string horizontalAlign) : key(key), label(label), horizontalAlign(horizontalAlign) {};
        uiVizWidgetTableCell(string key, string label, int width) : key(key), label(label), width(width)
        {
            if (width > 10)
                mWasWidthSet = true;
        };
        uiVizWidgetTableCell(string key, string label, int width, string horizontalAlign) : key(key), label(label), horizontalAlign(horizontalAlign), width(width)
        {
            if (width > 10)
                mWasWidthSet = true;
        };
        uiVizWidgetTableCell(string key, string label, string horizontalAlign, int width, string WIDGET_CLASS) : key(key), label(label), horizontalAlign(horizontalAlign), width(width), WIDGET_CLASS(WIDGET_CLASS)
        {
            if (width > 10)
                mWasWidthSet = true;
        };

        uiVizWidgetTableCell(string key, string label, int widthPerc, int minWidth) : key(key), label(label), widthPerc(widthPerc), width(minWidth)
        {
            if (widthPerc > 0)
                mWasWidthPercSet = true;
        }; //! Use minWidth = 0 to set min width to Cell label width
        uiVizWidgetTableCell(string key, string label, string horizontalAlign, int widthPerc, int minWidth) : key(key), label(label), horizontalAlign(horizontalAlign), widthPerc(widthPerc), width(minWidth)
        {
            if (widthPerc > 0)
                mWasWidthPercSet = true;
        }; //! Use minWidth = 0 to set min width to Cell label width
        uiVizWidgetTableCell(string key, string label, string horizontalAlign, int widthPerc, int minWidth, string WIDGET_CLASS) : key(key), label(label), horizontalAlign(horizontalAlign), widthPerc(widthPerc), width(minWidth), WIDGET_CLASS(WIDGET_CLASS)
        {
            if (widthPerc > 0)
                mWasWidthPercSet = true;
        }; //! Use minWidth = 0 to set min width to Cell label width

        ~uiVizWidgetTableCell()
        {
        }

        void setWidth(int val)
        {
            width = val;
            mWasWidthSet = true;
        }

        int getWidth()
        {
            return width;
        }

        bool getWasWidthSet()
        {
            return mWasWidthSet;
        }

        int getWidthPerc()
        {
            return widthPerc;
        }

        bool getWasWidthPercSet()
        {
            return mWasWidthPercSet;
        }

        void setMetadata(string key, string val)
        {
            mMetadata[key] = val;
        }

        string getMetadata(string key)
        {
            std::map<string, string>::iterator it = mMetadata.find(key);
            if (it != mMetadata.end())
            {
                return it->second;
            }
            else
            {
                return "";
            }
        }

        void deleteMetadata(string key)
        {
            std::map<string, string>::iterator it = mMetadata.find(key);
            if (it != mMetadata.end())
            {
                mMetadata.erase(it);
            }
        }

        void clearMetadata()
        {
            mMetadata.clear();
        }

    private:
        bool mWasWidthSet = false;
        bool mWasWidthPercSet = false;
        int width = 10; // The min with
        int widthPerc = 0;
        string WIDGET_CLASS = "";
        map<string, string> mMetadata;
    };

    class uiVizWidgetTableRow
    {
    public:
        string key = "";
        bool isSelected = false;
        vector<uiVizWidgetTableCell> cells;
        uiVizWidgetRowSlice rowSlice;
        uiVizWidgetTableRow() {};
        uiVizWidgetTableRow(string key, vector<uiVizWidgetTableCell> rowCells) : key(key), cells(rowCells) {};
        ~uiVizWidgetTableRow()
        {
        }

        int calculateWidth()
        {
            int accumWidth = 0;
            for (uiVizWidgetTableCell &cell : cells)
            {
                accumWidth += cell.cellSlice.bounds.width;
            }
            return accumWidth;
        }

        int calculateWidthNonPercCells()
        {
            int accumWidth = 0;
            for (uiVizWidgetTableCell &cell : cells)
            {
                if (!cell.getWasWidthPercSet())
                {
                    accumWidth += cell.cellSlice.bounds.width;
                }
            }
            return accumWidth;
        }

        int calculateNumberOfPercCells()
        {
            int val = 0;
            for (uiVizWidgetTableCell &cell : cells)
            {
                if (cell.getWasWidthPercSet())
                    val++;
            }
            return val;
        }

        int calculateNumberOfNonPercCells()
        {
            int val = 0;
            for (uiVizWidgetTableCell &cell : cells)
            {
                if (!cell.getWasWidthPercSet())
                    val++;
            }
            return val;
        }

        void setMetadata(string key, string val)
        {
            mMetadata[key] = val;
        }

        string getMetadata(string key)
        {
            std::map<string, string>::iterator it = mMetadata.find(key);
            if (it != mMetadata.end())
            {
                return it->second;
            }
            else
            {
                return "";
            }
        }

        void deleteMetadata(string key)
        {
            std::map<string, string>::iterator it = mMetadata.find(key);
            if (it != mMetadata.end())
            {
                mMetadata.erase(it);
            }
        }

        void clearMetadata()
        {
            mMetadata.clear();
        }

    private:
        map<string, string> mMetadata;
    };

    class uiVizWidgetTableFilterData
    {
    public:
        int columnIndex = 0;
        string filterText = "";
        string sortDirection = "asc";
        uiVizWidgetTableFilterData(int columnIndex, string filterText, string sortDirection) : columnIndex(columnIndex), filterText(filterText), sortDirection(sortDirection) {};

        ~uiVizWidgetTableFilterData()
        {
        }
    };

    class uiVizWidgetTableArgs : public ofEventArgs
    {
    public:
        uiVizWidgetTableArgs(uiVizWidgetTable &sender, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex, int cellIndex) : sender(sender), row(row), cell(cell), rowIndex(rowIndex), cellIndex(cellIndex) {}
        uiVizWidgetTable &sender;
        uiVizWidgetTableRow &row;
        uiVizWidgetTableCell &cell;
        int rowIndex;
        int cellIndex;
    };

    class uiVizWidgetTable : public uiVizWidget
    {
    public:
        enum class SelectionMode
        {
            ROW,
            CELL,
            NONE
        };

        string getSelectionModeString(SelectionMode val)
        {
            switch (val)
            {
            case SelectionMode::ROW:
                return "ROW";
            case SelectionMode::CELL:
                return "CELL";
            case SelectionMode::NONE:
                return "NONE";
            default:
                return "ROW";
            }
        };

        SelectionMode getSelectionModeEnum(string val)
        {
            val = ofToUpper(val);
            if (val == "ROW")
                return SelectionMode::ROW;
            if (val == "CELL")
                return SelectionMode::CELL;
            if (val == "NONE")
                return SelectionMode::NONE;
            return SelectionMode::ROW;
        };

        // Events
        ofEvent<uiVizWidgetTableArgs> cellHovered;
        ofEvent<uiVizWidgetTableArgs> cellSelected;
        ofEvent<uiVizWidgetTableArgs> cellUnSelected;
        ofEvent<uiVizWidgetTableArgs> cellWidgetInit;

        uiVizWidgetTable(string persistentId, string widgetXML) : uiVizWidget(persistentId, widgetXML)
        {
            initWidget();
            loadState(widgetXML);
        }

        uiVizWidgetTable(string persistentId, string widgetXML, uiVizWidgetTableRow headerRow, int fixedFooterRowHeight, vector<uiVizWidgetTableRow> tableRows, int rowHeight, bool showColumnFilters) : uiVizWidget(persistentId, widgetXML)
        {
            mHeaderRow = headerRow;
            mFixedFooterRowHeight = fixedFooterRowHeight;
            mTableRows = tableRows;
            mTableRowsFull = tableRows;
            setShowColumnFilters(showColumnFilters, false);
            mRowHeight = rowHeight;
            initWidget();
        }

        virtual bool loadState(string widgetXML) override
        {

            ofxXmlSettings settings = ofxXmlSettings();
            if (!settings.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            settings.pushTag("widget");

            setRowHeight(settings.getAttribute("properties", "rowHeight", getRowHeight()));
            setFreezeHeaderRow(settings.getAttribute("properties", "freezeHeaderRow", getFreezeHeaderRow()));
            setFreezeFirstColumn(settings.getAttribute("properties", "freezeFirstColumn", getFreezeFirstColumn()));
            setAllowMultiSelect(settings.getAttribute("properties", "allowMultiSelect", getAllowMultiSelect()));
            setShowColumnFilters(settings.getAttribute("properties", "showColumnFilters", getShowColumnFilters()), false);
            setSelectionMode(settings.getAttribute("properties", "selectionMode", "ROW"));
            setEnableColSnapScrolling(settings.getAttribute("properties", "enableColSnapScrolling", getEnableColSnapScrolling()));
            setFixedColWidth(settings.getAttribute("properties", "fixedColWidth", getFixedColWidth()));
            setFixedHeaderRowHeight(settings.getAttribute("properties", "fixedHeaderRowHeight", getFixedHeaderRowHeight()));
            setFixedFooterRowHeight(settings.getAttribute("properties", "fixedFooterRowHeight", getFixedFooterRowHeight()));
            setIsAutoResizeToContentsWidth(settings.getAttribute("properties", "autoResizeToContentsWidth", false));
            setIsAutoResizeToContentsHeight(settings.getAttribute("properties", "autoResizeToContentsHeight", false));

            string cellLabelColor_RGBA = settings.getAttribute("properties", "cellLabelColor", "");
            setCellLabelColor(cellLabelColor_RGBA != "" ? uiVizWidgetTheme::getColor_fromRGBAHex(cellLabelColor_RGBA) : getCellLabelColor());

            string cellLabelSelectedColor_RGBA = settings.getAttribute("properties", "cellLabelSelectedColor", "");
            setCellLabelSelectedColor(cellLabelSelectedColor_RGBA != "" ? uiVizWidgetTheme::getColor_fromRGBAHex(cellLabelSelectedColor_RGBA) : getCellLabelSelectedColor());

            string cellHoverColor_RGBA = settings.getAttribute("properties", "cellHoverColor", "");
            setCellHoverColor(cellHoverColor_RGBA != "" ? uiVizWidgetTheme::getColor_fromRGBAHex(cellHoverColor_RGBA) : getCellHoverColor());

            string cellSelectionColor_RGBA = settings.getAttribute("properties", "cellSelectionColor", "");
            setCellSelectionColor(cellSelectionColor_RGBA != "" ? uiVizWidgetTheme::getColor_fromRGBAHex(cellSelectionColor_RGBA) : getCellSelectionColor());

            settings.pushTag("properties");

            if (settings.pushTag("header"))
            {

                vector<uiVizWidgetTableCell> headerCells = vector<uiVizWidgetTableCell>();
                string rowKey = settings.getAttribute("header", "key", "HEADER");
                for (int j = 0; j < settings.getNumTags("cell"); j++)
                {
                    string widthValStr = settings.getAttribute("cell", "width", "0", j);
                    if (ofIsStringInString(widthValStr, "%"))
                    {
                        ofStringReplace(widthValStr, "%", "");
                        int colWidthPerc = ofToInt(widthValStr);
                        headerCells.push_back(
                            uiVizWidgetTableCell(
                                settings.getAttribute("cell", "key", "HC" + ofToString(j), j),
                                settings.getValue("cell", "", j),
                                colWidthPerc,
                                0));
                    }
                    else
                    {
                        headerCells.push_back(
                            uiVizWidgetTableCell(
                                settings.getAttribute("cell", "key", "HC" + ofToString(j), j),
                                settings.getValue("cell", "", j),
                                settings.getAttribute("cell", "width", getFreezeFirstColumn() ? 0 : 10, j)));
                    }
                }
                setHeaderRow(uiVizWidgetTableRow(rowKey, headerCells));
                settings.popTag(); // header
            }

            vector<uiVizWidgetTableRow> tableRows = vector<uiVizWidgetTableRow>();
            if (settings.pushTag("rows"))
            {
                if (settings.getNumTags("row") > 0)
                {
                    for (int i = 0; i < settings.getNumTags("row"); i++)
                    {
                        vector<uiVizWidgetTableCell> rowCells = vector<uiVizWidgetTableCell>();
                        string rowKey = settings.getAttribute("row", "key", "R" + ofToString(i), i);
                        settings.pushTag("row", i);
                        for (int j = 0; j < settings.getNumTags("cell"); j++)
                        {
                            rowCells.push_back(
                                uiVizWidgetTableCell(
                                    settings.getAttribute("cell", "key", "R" + ofToString(i) + "C" + ofToString(j), j),
                                    settings.getValue("cell", "", j),
                                    settings.getAttribute("cell", "width", getFreezeFirstColumn() ? 0 : 10, j)));
                        }
                        uiVizWidgetTableRow row = uiVizWidgetTableRow(rowKey, rowCells);
                        tableRows.push_back(row);
                        settings.popTag(); // row
                    }
                }
                settings.popTag(); // rows
            }

            settings.popTag(); // properties
            settings.popTag(); // widget

            setTableRows(tableRows);

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = uiVizWidget::saveState();
            settings.setAttribute("widget", "class", uiVizWidgetManager::WIDGET_CLASS_TABLE, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~uiVizWidgetTable() {
        };

        void update(uiVizWidgetContext context) override
        {
            loadTableWidgets();

            ofRectangle rectLabel = font->rect("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

            int maxItemWidth = 0;
            int maxItemHeight = 0;

            int rowWidthAppend = 0;
            int rowHeightAppend = 0;

            clearContentBoundsScaled();

            int titleRowHeight = scale(getFixedHeaderRowHeight());
            int rowHeight = scale(getRowHeight());
            int cellWidth;
            int labelHeight = font->rect("ABCDefgh").height;

            // Header row
            int accumX = getScaledPadding();
            int accumHeight = mTableRows.size() * rowHeight - getScaledPadding();
            int accumWidth = 0;

            // Hide ALL initially, then show only the "in view" ones
            //  if (getShowColumnFilters()) hideColumnFilters();
            mMaxHeaderRowHeight = titleRowHeight;

            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            // Determine header height
            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            if (getShowColumnFilters() && !getIsPivoted())
            {
                uiVizWidgetElmTextbox *TXT_COL_FILTER = getTextbox("COL_FILTER_0");
                int colFilterHeight = 0;
                if (TXT_COL_FILTER)
                    colFilterHeight = TXT_COL_FILTER->getHeight();
                mMaxHeaderRowHeight += scale(colFilterHeight) + getScaledPadding() * 2;
            }

            if (getFixedHeaderRowHeight() > mMaxHeaderRowHeight || getFixedHeaderRowHeight() == 0)
            {
                mMaxHeaderRowHeight = getFixedHeaderRowHeight();
            }
            mHeaderRow.rowSlice.bounds.height = mMaxHeaderRowHeight;

            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            // Visible indexes + offsets
            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            uiVizCoord::vizIndexOffset visRowStart = calculateVisibleRowIndexStart(mMaxHeaderRowHeight);
            uiVizCoord::vizIndexOffset visColStart = calculateVisibleColIndexStart();

            visibleRowIndexStart = visRowStart.index; // also calculates offsetY
            offsetY = visRowStart.offset;

            visibleColIndexStart = visColStart.index;
            offsetX = (getEnableColSnapScrolling() ? 0 : visColStart.offset);

            visibleRowIndexEnd = calculateVisibleRowIndexEnd(mMaxHeaderRowHeight);
            visibleColIndexEnd = calculateVisibleColIndexEnd();

            if (getTableUseFbo())
                clearOutOfViewCellFbo();

            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            // Header bounds
            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            for (int j = visibleColIndexStart; j < visibleColIndexEnd; j++)
            {
                cellWidth = calculateHeaderCellBounds(j, accumX, mMaxHeaderRowHeight, labelHeight);
                accumX += cellWidth;
            }
            if (getFreezeFirstColumn() && visibleColIndexStart > 0)
                calculateHeaderCellBounds(0, accumX, mMaxHeaderRowHeight, labelHeight);

            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            // Table rows
            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            for (int i = visibleRowIndexStart; i < visibleRowIndexEnd; i++)
            {

                uiVizWidgetTableRow &row = mTableRows.at(i); // ref, so we can set vals

                row.rowSlice.bounds = uiVizCoord::vizBounds(
                    scale(uiVizWidget::getUsableX(false)),
                    // scale(uiVizWidget::getUsableY(false)) + mHeaderRow.rowSlice.bounds.height + (i+1) * rowHeight - getScaledPadding(),
                    scale(uiVizWidget::getUsableY(false)) + mMaxHeaderRowHeight + i * rowHeight - getScaledPadding(),
                    scale(uiVizWidget::getUsableWidth()),
                    rowHeight);

                accumX = getScaledPadding();
                for (int j = visibleColIndexStart; j < visibleColIndexEnd; j++)
                {
                    accumX += dynamicallySizeCol(row, j, accumX, cellWidth, rowHeight, labelHeight);
                    if (j == visibleColIndexStart && getFreezeFirstColumn() && visibleColIndexStart > 0)
                        dynamicallySizeCol(row, 0, accumX, cellWidth, rowHeight, labelHeight);
                }
            }

            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            // Header / Filter row - final size adjusting
            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            accumX = getScaledPadding();
            hideColumnFilters();
            for (int j = visibleColIndexStart; j < visibleColIndexEnd; j++)
            {
                int cellX = scale(uiVizWidget::getUsableX(false));

                uiVizWidgetTableCell &cell = mHeaderRow.cells[j];
                cell.cellSlice.bounds.height = mMaxHeaderRowHeight;
                cell.cellSlice.bounds.x = cellX + accumX;
                cell.cellSlice.labelPoint.x = cellX + accumX;
                layoutHeaderRowFilters(j, cell, mHeaderRow.rowSlice.bounds.height);
                accumX += cell.cellSlice.bounds.width;

                // First col
                if (j == visibleColIndexStart && getFreezeFirstColumn() && visibleColIndexStart > 0)
                {
                    layoutHeaderRowFilters(0, cell, mHeaderRow.rowSlice.bounds.height);
                }
            }

            // Table rows -  widget
            for (int i = std::max(0, (int)visibleRowIndexStart - 5); i < std::min((int)mTableRows.size(), visibleRowIndexEnd); i++)
            {
                uiVizWidgetTableRow &row = mTableRows.at(i); // ref, so we can set vals

                // Cell bounds
                for (int j = 0; j < row.cells.size(); j++)
                {
                    // Hide element
                    if (row.cells[j].cellWidget != nullptr)
                    {
                        row.cells[j].cellWidget->setIsVisible(false);
                    }
                }
            }

            // Table rows - cell bounds
            for (int i = visibleRowIndexStart; i < visibleRowIndexEnd; i++)
            {
                uiVizWidgetTableRow &row = mTableRows.at(i); // ref, so we can set vals
                setCellBounds(row, i, visibleColIndexStart, visibleColIndexEnd);
            }

            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            // Bounds
            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            accumWidth = mHeaderRow.calculateWidth() - getScaledPadding() * 2;
            setContentBoundsScaled(uiVizCoord::vizBounds(
                getContentBoundsScaled().x,
                getContentBoundsScaled().y,
                accumWidth,
                accumHeight + mMaxHeaderRowHeight + scale(getFixedFooterRowHeight()) - getScaledPadding()));

            setScrollScaleY(mTableRows.size() > 100 ? getRowHeight() : 5);

            /*
            #if VIZ_DEBUG_LEVEL >= 2
                setTitle("COL DEBUG: IDX:" + ofToString(visibleColIndexStart) + "->" +
                ofToString(visibleColIndexEnd) + "=" + ofToString(visibleColIndexEnd-visibleColIndexStart) +
                "s:" + ofToString(getScrollOffsetX())+ "w:" + ofToString(scale(getWidth())) + " oX: " + ofToString(offsetX) + " oY:" + ofToString(offsetY) + "HovX:" + ofToString(getHoveredIndex().x) + "HovY:" + ofToString(getHoveredIndex().y));

                setTitle("BOUNDS_WL" + ofToString(getContentBoundsScaled().width) + "->sX" + ofToString(getScrollOffsetX()) + " BOUNDS_H:" + ofToString(getContentBoundsScaled().height) +  + "->sY" + ofToString(getScrollOffsetY()) );

            #endif

        int prevContentBoundsWidth = 0;
        int prevContentBoundsHeight = 0;

            */

            // AUTO RESIZING STUFF
            if (getIsAutoResizeToContentsWidth() && prevContentBoundsWidth != getContentBoundsScaled().width)
            {
                setWidgetSize(deScale(getContentBoundsScaled().width) + getWidth() - getUsableWidth(), getHeight(), false);
                prevContentBoundsWidth = getContentBoundsScaled().width;
            }

            if (getIsAutoResizeToContentsHeight() && prevContentBoundsHeight != getContentBoundsScaled().height)
            {
                setWidgetSize(getWidth(), deScale(getContentBoundsScaled().height) + getHeight() - getUsableHeight(), false);
                prevContentBoundsHeight = getContentBoundsScaled().height;
            }
        }

        void draw(uiVizWidgetContext context) override
        {

            if (mCellPercentagesWereCalculated)
            {
                update(context);
                mCellPercentagesWereCalculated = false;
            }

            setEnableScrollYPositive(true);
            setEnableScrollYNegative(true);
            setMinWidth(150);

            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            // Table Rows
            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            bool clicked = getIsMousePressedAndReleasedOverWidget(false);

            int totalRowsVisible = 0;
            int iterations = 0;

            int abs_cellX = 0;
            int abs_cellY = 0;
            int abs_cellLabelX = 0;
            int abs_cellLabelY = 0;
            int abs_cellWidth = 0;
            int abs_cellHeight = 0;

            bool isFirstDrawCellContent = true;

            for (int i = visibleRowIndexStart; i < visibleRowIndexEnd; i++)
            {

                iterations++;
                bool isRowHovered = false;
                bool isCellHovered = false;

                uiVizWidgetTableRow &row = mTableRows.at(i); // ref, so we can set vals
                bool eventFiredForRow = false;

                totalRowsVisible++;
                isRowHovered = row.rowSlice.isHoveredOffset(0, offsetY);

                for (int j = visibleColIndexStart; j < visibleColIndexEnd; j++)
                {
                    if (j > row.cells.size() - 1)
                        break;
                    uiVizWidgetTableCell &cell = row.cells[j]; // ref, so we can set vals

                    //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                    // Selection / Hover
                    //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                    if ((row.isSelected || clicked || isRowHovered) && !getIsDragging())
                    {

                        isCellHovered = cell.cellSlice.isHoveredOffset(offsetX, offsetY);
                        mHoveredColIndex = (isCellHovered ? j : -1);
                        mHoveredRowIndex = (isRowHovered ? i : -1);

                        if (isCellHovered)
                        {
                            if (mHoveredColIndex > -1 && mHoveredRowIndex > -1 && (mHoveredColIndexPrev != mHoveredColIndex || mHoveredRowIndexPrev != mHoveredRowIndex))
                            {
                                uiVizWidgetTableArgs args(*this, row, cell, i, j);

                                ofNotifyEvent(cellHovered, args);
                                eventTransmit(getPersistentId(), WIDGET_EVENT::TABLE_CELL_HOVERED, "<event rowIndex=\"" + ofToString(mHoveredRowIndex) + "\" colIndex=\"" + ofToString(mHoveredColIndex) + "\"></event>", *this);

                                if (mFunction_Cell_Hovered != NULL)
                                    mFunction_Cell_Hovered(args);
                            }
                            mHoveredColIndexPrev = mHoveredColIndex;
                            mHoveredRowIndexPrev = mHoveredRowIndex;
                        }

                        if (!eventFiredForRow && clicked && isCellHovered)
                        {
                            uiVizWidgetTableRow &rowFull = mTableRowsFull.at(i); // ref, so we can set vals
                            if ((!row.isSelected && getSelectionMode() == SelectionMode::ROW) || (!cell.isSelected && getSelectionMode() == SelectionMode::CELL))
                            {
                                // Fire cellSelected event

                                if (!getAllowMultiSelect())
                                {
                                    clearSelectedRows();
                                }

                                // if (!getIsPivoted()) setShowColumnFilters(!getShowColumnFilters(), true); // toggle filters :P
                                // setFixedColWidth(getFixedColWidth() == 0 ? 50 : 0); // toggle fixed / dyna width :P

                                row.isSelected = true;
                                row.cells[j].isSelected = true;
                                rowFull.isSelected = true;
                                rowFull.cells[j].isSelected = true;

                                uiVizWidgetTableArgs args(*this, row, cell, i, j);

                                eventTransmit(getPersistentId(), WIDGET_EVENT::TABLE_CELL_CLICKED, "<event rowIndex=\"" + ofToString(i) + "\" colIndex=\"" + ofToString(j) + "\"></event>", *this);
                                ofNotifyEvent(cellSelected, args);
                                eventTransmit(getPersistentId(), WIDGET_EVENT::TABLE_CELL_SELECTED, "<event rowIndex=\"" + ofToString(i) + "\" colIndex=\"" + ofToString(j) + "\"></event>", *this);

                                if (mFunction_Cell_Selected != NULL)
                                    mFunction_Cell_Selected(args);

                                if (getShouldCloseUponSelection())
                                    closeThisWidget();
                            }
                            else
                            {
                                // Fire rowUnSelected  event
                                row.isSelected = false;
                                row.cells[j].isSelected = false;
                                rowFull.isSelected = false;
                                rowFull.cells[j].isSelected = false;
                                uiVizWidgetTableArgs args(*this, row, cell, i, j);

                                eventTransmit(getPersistentId(), WIDGET_EVENT::TABLE_CELL_CLICKED, "<event rowIndex=\"" + ofToString(i) + "\" colIndex=\"" + ofToString(j) + "\"></event>", *this);
                                ofNotifyEvent(cellUnSelected, args);
                                eventTransmit(getPersistentId(), WIDGET_EVENT::TABLE_CELL_UNSELECTED, "<event rowIndex=\"" + ofToString(i) + "\" colIndex=\"" + ofToString(j) + "\"></event>", *this);

                                if (mFunction_Cell_UnSelected != NULL)
                                    mFunction_Cell_UnSelected(args);
                            }
                            eventFiredForRow = true;
                        }
                    }

                    // Jump out if one of the above events changed the
                    // structure of the table
                    if (getTableStructChanged())
                    {
                        mTableStructChanged = false;
                        setWidgetNeedsUpdate(true);
                        return;
                    }

                    abs_cellX = cell.cellSlice.bounds.x - offsetX - getScaledPadding();
                    abs_cellY = row.rowSlice.bounds.y - offsetY;
                    abs_cellLabelX = cell.cellSlice.labelPoint.x - offsetX - getScaledPadding();
                    abs_cellLabelY = cell.cellSlice.labelPoint.y - offsetY;
                    abs_cellWidth = cell.cellSlice.bounds.width;
                    abs_cellHeight = row.rowSlice.bounds.height;

                    // Hover
                    if (
                        (j == visibleColIndexStart && getSelectionMode() == SelectionMode::ROW && (isRowHovered)) ||
                        (getSelectionMode() == SelectionMode::CELL && (isCellHovered))

                    )
                    {
                        switch (getSelectionMode())
                        {
                        case SelectionMode::ROW:
                        {
                            mFunction_DrawRowHighlight_Hover(row.isSelected, isRowHovered, row.rowSlice.bounds.x - getScaledPadding(), row.rowSlice.bounds.y - offsetY, row.rowSlice.bounds.width + getScaledPadding() * 2, row.rowSlice.bounds.height, row, i);
                        }
                        break;

                        case SelectionMode::CELL:
                        {
                            mFunction_DrawCellHighlight_Hover(row.isSelected, isRowHovered, abs_cellX, abs_cellY, abs_cellWidth, abs_cellHeight, row, cell, i, j);
                        }
                        break;
                        default:
                        {
                        }
                        }
                    }

                    // Selection
                    if (
                        (j == visibleColIndexStart && getSelectionMode() == SelectionMode::ROW && (row.isSelected)) ||
                        (getSelectionMode() == SelectionMode::CELL && (cell.isSelected))

                    )
                    {
                        switch (getSelectionMode())
                        {
                        case SelectionMode::ROW:
                        {
                            mFunction_DrawRowHighlight_Selected(row.isSelected, isRowHovered, row.rowSlice.bounds.x - getScaledPadding(), row.rowSlice.bounds.y - offsetY, row.rowSlice.bounds.width + getScaledPadding() * 2, row.rowSlice.bounds.height, row, i);
                        }
                        break;

                        case SelectionMode::CELL:
                        {
                            mFunction_DrawCellHighlight_Selected(row.isSelected, isRowHovered, abs_cellX, abs_cellY, abs_cellWidth, abs_cellHeight, row, cell, i, j);
                        }
                        break;
                        default:
                        {
                        }
                        }
                    }

                    //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                    // Content
                    //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                    if (j > 0 || (j == 0 && !getFreezeFirstColumn()))
                    {

                        if (isFirstDrawCellContent)
                        {
                            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                            // For some unknown / bizzare reason, the very first call of vizElm_cell.draw() in this loop
                            // has the wrong alpha. As a workaround, draw an offscreen 1x1 pixel to mitigate...
                            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                            uiVizElm vizElm_cell;
                            vizElm_cell.setRectangle(-1, -1, 1, 1, getTheme().TitleColor_withAlpha(0.1f));
                            vizElm_cell.draw();
                            isFirstDrawCellContent = false;
                        }

                        if (getTableUseFbo())
                        {
                            if (cell.cellSlice.needsUpdate)
                            {
                                cell.cellSlice.allocateFbo(true, abs_cellWidth, abs_cellHeight);
                                cell.cellSlice.fbo.begin();
                                ofClear(0, 0, 0, 0);
                                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                                ofPushMatrix();
                                ofTranslate(-1 * abs_cellX, -1 * abs_cellY);
                                ofPushStyle();

                                mFunction_DrawCellContent(
                                    row.isSelected, isRowHovered, isCellHovered,
                                    abs_cellX, abs_cellY,
                                    abs_cellLabelX, abs_cellLabelY,
                                    abs_cellWidth, abs_cellHeight,
                                    row, cell,
                                    i, j);

                                ofPopStyle();
                                ofPopMatrix();
                                cell.cellSlice.fbo.end();
                                cell.cellSlice.needsUpdate = false;
                            }
                            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                            cell.cellSlice.fbo.draw(abs_cellX, abs_cellY, abs_cellWidth, abs_cellHeight);
                            ofEnableAlphaBlending();
                        }
                        else
                        {
                            mFunction_DrawCellContent(
                                row.isSelected, isRowHovered, isCellHovered,
                                abs_cellX, abs_cellY,
                                abs_cellLabelX, abs_cellLabelY,
                                abs_cellWidth, abs_cellHeight,
                                row, cell,
                                i, j);
                        }
                    }
                }

                //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                // Header Col
                //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                if (getFreezeFirstColumn())
                {
                    uiVizWidgetTableCell &cell = row.cells[0]; // ref, so we can set vals

                    abs_cellX = (getFreezeFirstColumn() ? scale(uiVizWidget::getUsableX(false)) - getScaledPadding() : cell.cellSlice.bounds.x - offsetX - getScaledPadding() * 2);
                    abs_cellLabelX = (getFreezeFirstColumn() ? scale(uiVizWidget::getUsableX(false)) : cell.cellSlice.labelPoint.x - offsetX - getScaledPadding());
                    abs_cellWidth = mHeaderRow.cells[0].cellSlice.bounds.width;

                    if (getTableUseFbo())
                    {

                        uiVizCoord::vizBounds fboBounds = getHeaderColFboBounds(
                            row.isSelected, isRowHovered,
                            abs_cellX, abs_cellY,
                            abs_cellLabelX, abs_cellLabelY,
                            abs_cellWidth, abs_cellHeight,
                            row, cell,
                            i);

                        if (cell.cellSlice.needsUpdate)
                        {
                            cell.cellSlice.allocateFbo(true, fboBounds.width, fboBounds.height);
                            cell.cellSlice.fbo.begin();
                            ofClear(0, 0, 0, 0);
                            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                            ofPushMatrix();
                            ofTranslate(-1 * fboBounds.x, -1 * fboBounds.y);
                            ofPushStyle();

                            // reason piano roll is missing stuff: it's drawing outside fbo area!
                            // test with more complex content...

                            drawCellHeaderCol(
                                row.isSelected, isRowHovered,
                                abs_cellX, abs_cellY,
                                abs_cellLabelX, abs_cellLabelY,
                                abs_cellWidth, abs_cellHeight,
                                row, cell,
                                i);

                            ofPopStyle();
                            ofPopMatrix();
                            cell.cellSlice.fbo.end();
                            cell.cellSlice.needsUpdate = false;
                        }
                        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                        cell.cellSlice.fbo.draw(fboBounds.x, fboBounds.y, fboBounds.width, fboBounds.height);
                        ofEnableAlphaBlending();
                    }
                    else
                    {
                        drawCellHeaderCol(
                            row.isSelected, isRowHovered,
                            abs_cellX, abs_cellY,
                            abs_cellLabelX, abs_cellLabelY,
                            abs_cellWidth, abs_cellHeight,
                            row, cell,
                            i);
                    }
                }
            }

            if (getFixedHeaderRowHeight() != 0 || getFixedFooterRowHeight() > 0)
            {
                uiVizWidgetTableRow &row = mHeaderRow; // ref, so we can set vals
                bool rowHovered = row.rowSlice.isHoveredOffset(0, offsetY);
                bool cellHovered = false;

                for (int j = visibleColIndexEnd - 1; j >= visibleColIndexStart; j--)
                { // iterate backwards (so 1st header cell drawn last)!!!
                    if (getFixedHeaderRowHeight() != 0)
                        drawHeaderRow(j, rowHovered, row);
                    if (getFixedFooterRowHeight() > 0)
                        drawFooterRow(j, rowHovered, row);
                }
                if (getFreezeFirstColumn() && visibleColIndexStart > 0)
                {
                    if (getFixedHeaderRowHeight() != 0)
                        drawHeaderRow(0, rowHovered, row);
                    if (getFixedFooterRowHeight() > 0)
                        drawFooterRow(0, rowHovered, row);
                }
            }

            afterRowsDrawn();
        }

        virtual uiVizCoord::vizBounds getHeaderColFboBounds(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex)
        {
            return uiVizCoord::vizBounds(
                absoluteScaledX,
                absoluteScaledY,
                scaledWidth,
                scaledHeight);
        }

        /********* Override this to draw your own content *********/
        virtual void drawCellHighlight_Hover(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex, int colIndex)
        {
            uiVizElm vizElm_cell;
            vizElm_cell.setRectangle(
                absoluteScaledX, absoluteScaledY,
                scaledWidth, scaledHeight,
                getCellHoverColor());
            vizElm_cell.draw();
        }

        /********* Override this to draw your own content *********/
        virtual void drawRowHighlight_Hover(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, int rowIndex)
        {
            uiVizElm vizElm_row;
            vizElm_row.setRectangle(
                absoluteScaledX,
                absoluteScaledY, // subtracting getScrollOffsetY to prevent content from disappearing
                scaledWidth,
                scaledHeight,
                getCellHoverColor());
            vizElm_row.draw();
        }

        /********* Override this to draw your own content *********/
        virtual void drawCellHighlight_Selected(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex, int colIndex)
        {
            uiVizElm vizElm_cell;
            vizElm_cell.setRectangle(
                absoluteScaledX, absoluteScaledY,
                scaledWidth, scaledHeight,
                getCellSelectionColor());
            vizElm_cell.draw();
        }

        /********* Override this to draw your own content *********/
        virtual void drawRowHighlight_Selected(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, int rowIndex)
        {
            uiVizElm vizElm_row;
            vizElm_row.setRectangle(
                absoluteScaledX,
                absoluteScaledY, // subtracting getScrollOffsetY to prevent content from disappearing
                scaledWidth,
                scaledHeight,
                getCellSelectionColor());
            vizElm_row.draw();
        }

        /********* Override this to draw your own content *********/
        virtual void drawCellContent(bool isRowSelected, bool isRowHovered, bool isCellHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex, int colIndex)
        {

            if (!isRowSelected && !isRowHovered)
            {
                uiVizElm vizElm_cell;
                vizElm_cell.setRectangle(
                    absoluteScaledX,
                    absoluteScaledY,
                    scaledWidth,
                    scaledHeight,
                    rowIndex % 2 == 0 ? getRowBGColor() : getRowAltBGColor());
                vizElm_cell.draw();
            }

            if (cell.label != "")
            {
                ofPushStyle();
                ofSetColor((isRowSelected || isRowHovered) ? getCellLabelSelectedColor() : getCellLabelColor());
                uiVizCoord::vizPoint p = getAlignmentPointForCellLabel(cell, absoluteScaledLabelX, absoluteScaledLabelY, scaledWidth, scaledHeight);
                font->draw(cell.label, p.x, p.y);
                ofPopStyle();
            }

            /*
            #if VIZ_DEBUG_LEVEL >= 2
                shared_ptr<ofxSmartFont> debugFont = getViz()->getTinyFont();
                //debugFont->draw(ofToString(cell.cellSlice.bounds.x) + "," + ofToString(cell.cellSlice.bounds.y) + ":" + ofToString(absoluteScaledX) + "," + ofToString(absoluteScaledY) , absoluteScaledLabelX, absoluteScaledLabelY + scale(20)); //, scaledWidth, scaledHeight
                debugFont->draw(ofToString(cell.cellSlice.bounds.width) + "," + ofToString(cell.cellSlice.bounds.height), absoluteScaledLabelX, absoluteScaledLabelY + scale(20));
            #endif
            */
        }

        void setFunction_DrawCellHighlight_Hover(const std::function<void(bool, bool, int, int, int, int, uiVizWidgetTableRow &, uiVizWidgetTableCell &cell, int, int &)> &func)
        {
            mFunction_DrawCellHighlight_Hover = func;
        }

        void setFunction_DrawRowHighlight_Hover(const std::function<void(bool, bool, int, int, int, int, uiVizWidgetTableRow &, int &)> &func)
        {
            mFunction_DrawRowHighlight_Hover = func;
        }

        void setFunction_DrawCellHighlight_Selected(const std::function<void(bool, bool, int, int, int, int, uiVizWidgetTableRow &, uiVizWidgetTableCell &, int, int &)> &func)
        {
            mFunction_DrawCellHighlight_Selected = func;
        }

        void setFunction_DrawRowHighlight_Selected(const std::function<void(bool, bool, int, int, int, int, uiVizWidgetTableRow &, int &)> &func)
        {
            mFunction_DrawRowHighlight_Selected = func;
        }

        void setFunction_DrawCellContent(const std::function<void(bool, bool, bool, int, int, int, int, int, int, uiVizWidgetTableRow &, uiVizWidgetTableCell &, int, int &)> &func)
        {
            mFunction_DrawCellContent = func;
        }

        void setFunction_Cell_Selected(const std::function<void(uiVizWidgetTableArgs &)> &func)
        {
            mFunction_Cell_Selected = func;
        }

        void setFunction_Cell_UnSelected(const std::function<void(uiVizWidgetTableArgs &)> &func)
        {
            mFunction_Cell_UnSelected = func;
        }

        void setFunction_Cell_Hovered(const std::function<void(uiVizWidgetTableArgs &)> &func)
        {
            mFunction_Cell_Hovered = func;
        }

        uiVizCoord::vizPoint getAlignmentPointForCellLabel(uiVizWidgetTableCell &cell, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight)
        {
            string horizontalAlign = ofToLower(cell.horizontalAlign);
            if (horizontalAlign == "left" || horizontalAlign == "")
            {
                // Do nothing, left by default
            }
            else if (horizontalAlign == "center")
            {
                ofRectangle rect = font->rect(cell.label);
                absoluteScaledLabelX = absoluteScaledLabelX + ((scaledWidth - rect.width) / 2.0f - getScaledPadding());
            }
            else if (horizontalAlign == "right")
            {
                ofRectangle rect = font->rect(cell.label);
                absoluteScaledLabelX = absoluteScaledLabelX + (scaledWidth - rect.width) - getScaledPadding() * 2.0f;
            }

            return uiVizCoord::vizPoint(absoluteScaledLabelX, absoluteScaledLabelY);
        }

        /********* Override this to draw your own content *********/
        virtual void drawCellHeaderRow(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int colIndex)
        {

            uiVizElm vizElm_cell, vizElm_cell2;
            vizElm_cell.setRectangle(
                absoluteScaledX,
                absoluteScaledY,
                scaledWidth,
                scaledHeight,
                getTheme().WidgetContentColor_withAlpha(getStrongChromeAlpha()));
            vizElm_cell.draw();

            ofPushStyle();
            ofSetColor((isRowSelected || isRowHovered) ? getTheme().WidgetAccent1Color_withAlpha(getTheme().ElementHoveredForegroundAlpha) : getTheme().WidgetAccent1Color_withAlpha(getTheme().ElementUnhoveredForegroundAlpha));
            // font->draw(cell.label, absoluteScaledLabelX, absoluteScaledLabelY);
            uiVizCoord::vizPoint p = getAlignmentPointForCellLabel(cell, absoluteScaledLabelX, absoluteScaledLabelY, scaledWidth, scaledHeight);
            font->draw(cell.label, p.x, p.y);
            ofPopStyle();
        }

        virtual void drawCellFooterRow(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int colIndex)
        {

            uiVizElm vizElm_cell;
            vizElm_cell.setRectangle(
                absoluteScaledX,
                absoluteScaledY,
                scaledWidth,
                scaledHeight,
                getTheme().WidgetContentColor);
            vizElm_cell.draw();
        }

        /********* Override this to draw your own content *********/
        virtual void drawCellHeaderCol(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex)
        {

            uiVizElm vizElm_cell;
            vizElm_cell.setRectangle(
                absoluteScaledX,
                absoluteScaledY,
                scaledWidth,
                scaledHeight,
                getTheme().WidgetContentColor);
            vizElm_cell.draw();

            ofPushStyle();
            ofSetColor((isRowSelected || isRowHovered) ? getTheme().TypographyPrimaryColor : getTheme().TypographyPrimaryColor);
            // font->draw(cell.label, absoluteScaledLabelX, absoluteScaledLabelY);

            uiVizCoord::vizPoint p = getAlignmentPointForCellLabel(cell, absoluteScaledLabelX, absoluteScaledLabelY, scaledWidth, scaledHeight);
            font->draw(cell.label, p.x, p.y);
            ofPopStyle();
        }

        /********* Override this to draw your own content *********/
        virtual void afterRowsDrawn()
        {
        }

        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {

            string event = args.getFullEventName();

#if VIZ_DEBUG_LEVEL >= 2
            string xml;
            args.eventXML.copyXmlToString(xml);
            cout << args.eventName << " " << xml;
#endif

            if (ofIsStringInString(event, "COL_FILTER_"))
            {
                string col_idx_str = event;
                ofStringReplace(col_idx_str, "COL_FILTER_", "");
                int col_idx = std::atoi(col_idx_str.c_str());

                if (args.eventName == WIDGET_EVENT::VALUE_CHANGED)
                {
                    uiVizWidgetElmTextbox *TXT_COL_FILTER = getTextbox(args.widgetId);
                    if (TXT_COL_FILTER)
                    {
                        filterTable(col_idx, TXT_COL_FILTER->getValue());
                    }
                }
            }
        }

        void pivot()
        {
            if (getIsPivoted())
                return;

            uiVizWidgetTableRow mNewHeaderRow = uiVizWidgetTableRow();
            vector<uiVizWidgetTableRow> mNewTableRows = vector<uiVizWidgetTableRow>();
            vector<uiVizWidgetTableRow> mNewTableRowsFull = vector<uiVizWidgetTableRow>();

            for (int x = 0; x < mHeaderRow.cells.size(); x++)
            {

                uiVizWidgetTableCell &col = mHeaderRow.cells[x]; // convert each col into rows
                vector<uiVizWidgetTableCell> newRowCells = vector<uiVizWidgetTableCell>();
                newRowCells.push_back(col); // The new row's "header" cell but on the left, not top!

                for (int i = 0; i < mTableRows.size(); i++)
                {
                    if (x > mTableRows[i].cells.size() - 1)
                        break;
                    uiVizWidgetTableCell &colToRowCell = mTableRows[i].cells.at(x); // ref, so we can set vals
                    newRowCells.push_back(colToRowCell);
                }
                mNewTableRows.push_back(uiVizWidgetTableRow(col.key, newRowCells));
            }

            for (int i = 0; i <= mTableRows.size(); i++)
            { // not sure why the extra col is needed to make last visible in pivot mode?!?!
                mNewHeaderRow.cells.push_back(uiVizWidgetTableCell("", ""));
            }

            mHeaderRow = mNewHeaderRow;
            mTableRows = mNewTableRows;
            mTableRowsFull = mNewTableRows;

            mIsPivoted = true;
            mTableStructChanged = true;
            setFreezeHeaderRow(true);
            initWidget();
            setNeedsUpdate(true);
            setWidgetNeedsUpdate(true);
        }

        void setColumnFilterVal(int col_idx, string value, bool applyFilter)
        {
            uiVizWidgetElmTextbox *TXT_COL_FILTER = getTextbox("COL_FILTER_" + ofToString(col_idx));
            if (TXT_COL_FILTER)
            {
                TXT_COL_FILTER->setValue(value, false);
                if (applyFilter)
                    filterTable(col_idx, TXT_COL_FILTER->getValue());
            }
        }

        void populateWithNumberRange(int from, int to, string header, bool clearExistingRows, bool showFilter)
        {
            if (clearExistingRows)
                clearTableRows();
            if (to < from)
                return;

            setHeaderRow(uiVizWidgetTableRow("HEADER",
                                             {uiVizWidgetTableCell(
                                                 "HEADER",
                                                 header,
                                                 100,
                                                 100)}));

            for (int i = from; i <= to; i++)
            {

                string value = ofToString(i);

                uiVizWidgetTableRow row(
                    value,
                    {uiVizWidgetTableCell(
                        value,
                        value,
                        100,
                        100)});

                row.setMetadata("META_VALUE", value);
                addRow(row);
            }

            setShowColumnFilters(showFilter, showFilter);

            updateWidget();
            drawWidget();
        }

        uiVizCoord::vizIndexOffset getVisibleStartRowOffset()
        {
            return uiVizCoord::vizIndexOffset(visibleRowIndexStart, offsetY);
        }

        uiVizCoord::vizIndexOffset getVisibleStartColOffset()
        {
            return uiVizCoord::vizIndexOffset(visibleColIndexStart, offsetX);
        }

        uiVizCoord::vizIndexOffset getVisibleEndRowOffset()
        {
            return uiVizCoord::vizIndexOffset(visibleRowIndexEnd, offsetY);
        }

        uiVizCoord::vizIndexOffset getVisibleEndColOffset()
        {
            return uiVizCoord::vizIndexOffset(visibleColIndexEnd, offsetX);
        }

        vector<int> getSelectedRowIndexes()
        {
            vector<int> selectedRowIndexes = vector<int>();
            for (int i = 0; i < mTableRows.size(); i++)
            {
                uiVizWidgetTableRow &row = mTableRows.at(i);
                if (row.isSelected)
                    selectedRowIndexes.push_back(i);
            }
            return selectedRowIndexes;
        }

        vector<uiVizWidgetTableRow> getSelectedRows()
        {
            vector<int> selectedRowIndexes = getSelectedRowIndexes();
            vector<uiVizWidgetTableRow> selectedRows = vector<uiVizWidgetTableRow>();
            for (int i = 0; i < selectedRowIndexes.size(); i++)
            {
                uiVizWidgetTableRow &row = mTableRows.at(selectedRowIndexes[i]);
                if (row.isSelected)
                    selectedRows.push_back(row);
            }
            return selectedRows;
        }

        void filterTable(vector<uiVizWidgetTableFilterData> columnFilters)
        {
            mTableRows.clear();
            mTableStructChanged = true;

            bool addRow = false;
            for (uiVizWidgetTableRow row : mTableRowsFull)
            {
                for (uiVizWidgetTableFilterData filter : columnFilters)
                {

                    if (filter.columnIndex > row.cells.size() - 1)
                        continue;

                    if (filter.filterText == "" || (row.cells.size() >= filter.columnIndex &&
                                                    ofIsStringInString(ofToLower(row.cells[filter.columnIndex].label), ofToLower(filter.filterText))))
                    {
                        addRow = true;
                    }
                    else
                    {
                        addRow = false;
                        break;
                    }
                }

                if (addRow)
                {
                    mTableRows.push_back(row);
                }
            }
            setScrollOffsetY(0, true);
        }

        uiVizWidgetTableRow updateCell(int rowIndex, int columnIndex, uiVizWidgetTableCell cell)
        {

            uiVizWidgetTableRow updatedRow;

            if (rowIndex < mTableRows.size())
            {
                uiVizWidgetTableRow &row = mTableRows.at(rowIndex); // ref, so we can set vals
                if (columnIndex < row.cells.size())
                {
                    uiVizWidgetTableCell &cellOld = row.cells.at(columnIndex); // ref, so we can set vals
                    cellOld = cell;
                }
            }

            if (rowIndex < mTableRowsFull.size())
            {
                uiVizWidgetTableRow &row = mTableRowsFull.at(rowIndex); // ref, so we can set vals
                if (columnIndex < row.cells.size())
                {
                    uiVizWidgetTableCell &cellOld = row.cells.at(columnIndex); // ref, so we can set vals
                    cellOld = cell;
                    updatedRow = row;
                }
            }
            setNeedsUpdate(true);
            return updatedRow;
        }

        void filterTable(int columnIndex, string filterValue)
        {
            uiVizWidgetTableFilterData newFilter = uiVizWidgetTableFilterData(columnIndex, filterValue, "asc");

            bool foundFilter = false;
            for (uiVizWidgetTableFilterData &existingFilter : mColumnFilters)
            {
                if (newFilter.columnIndex == existingFilter.columnIndex)
                {
                    existingFilter = newFilter;
                    foundFilter = true;
                }
            }

            if (!foundFilter)
                mColumnFilters.push_back(newFilter);

            // Set the text values for rowfilter textboxes
            for (uiVizWidgetTableFilterData &existingFilter : mColumnFilters)
            {
                uiVizWidgetElmTextbox *TXT_COL_FILTER = getTextbox("COL_FILTER_" + ofToString(existingFilter.columnIndex));
                if (TXT_COL_FILTER)
                {
                    TXT_COL_FILTER->setValue(existingFilter.filterText, false);
                }
            }

            filterTable(mColumnFilters);
            setNeedsUpdate(true);
        }

        void clearSelectedRows()
        {
            for (int i = 0; i < mTableRows.size(); i++)
            {
                uiVizWidgetTableRow &row = mTableRows.at(i);         // ref, so we can set vals
                uiVizWidgetTableRow &rowFull = mTableRowsFull.at(i); // ref, so we can set vals
                row.isSelected = false;
                rowFull.isSelected = false;

                for (int j = 0; j < row.cells.size(); j++)
                {
                    row.cells[j].isSelected = false;
                    rowFull.cells[j].isSelected = false;
                }
            }
        }

        float getRowHeight()
        {
            return mRowHeight;
        }

        void setRowHeight(int val)
        {
            mRowHeight = val;
        }

        virtual void setIsAutoResizeToContentsWidth(bool val) override
        {
            if (mHeaderRow.calculateNumberOfPercCells() > 0)
            {
                uiVizWidget::setIsAutoResizeToContentsWidth(false);
                return;
            }
            uiVizWidget::setIsAutoResizeToContentsWidth(val);
        }

        int getTableRowsSize()
        {
            return mTableRows.size();
        }

        vector<uiVizWidgetTableRow> getTableRows()
        {
            return mTableRows;
        }

        void clearTableRows()
        {
            setTableRows(vector<uiVizWidgetTableRow>());
        }

        void setFreezeHeaderRow(bool val)
        {
            mFreezeHeaderRow = val;
        }

        bool getFreezeHeaderRow()
        {
            return mFreezeHeaderRow;
        }

        void setFreezeFirstColumn(bool val)
        {
            mFreezeFirstColumn = val;
        }

        bool getFreezeFirstColumn()
        {
            return mFreezeFirstColumn;
        }

        void setAllowMultiSelect(bool val)
        {
            mAllowMultiSelect = val;
        }

        bool getAllowMultiSelect()
        {
            return mAllowMultiSelect;
        }

        void setShowColumnFilters(bool val, bool addElms)
        {

            if (getIsPivoted() && val)
                val = false;

            mShowColumnFilters = val;

            if (!addElms)
                return;

            // Filter cols
            if (mShowColumnFilters && getFixedHeaderRowHeight() != 0)
            {
                for (int i = 0; i < mHeaderRow.cells.size(); i++)
                {
                    uiVizWidgetElm *e = addOrLoadWidgetElement(e, WIDGET_ELM_CLASS::TEXTBOX, "COL_FILTER_" + ofToString(i), "<element></element>");
                    if (e)
                        e->setTitle(mHeaderRow.cells[i].label + "...");
                }
            }
            else
            {
                for (int i = 0; i < mHeaderRow.cells.size(); i++)
                {
                    uiVizWidgetElm *e = getWidgetElement("COL_FILTER_" + ofToString(i));
                    if (e)
                        removeWidgetElement(e);
                }
            }

            setNeedsUpdate(true);
            setWidgetNeedsUpdate(true);
        }

        bool getShowColumnFilters()
        {
            return mShowColumnFilters;
        }

        SelectionMode getSelectionMode()
        {
            return mSelectionMode;
        }

        void setSelectionMode(SelectionMode val)
        {
            mSelectionMode = val;
        }

        void setSelectionMode(string val)
        {
            SelectionMode enumVal = getSelectionModeEnum(val);
            setSelectionMode(enumVal);
        }

        bool getEnableColSnapScrolling()
        {
            return mEnableColSnapScrolling;
        }

        void setEnableColSnapScrolling(bool val)
        {
            mEnableColSnapScrolling = val;
        }

        bool getIsPivoted()
        {
            return mIsPivoted;
        }

        int getFixedColWidth()
        {
            return mFixedColWidth;
        }

        void setFixedColWidth(int val)
        {
            setNeedsUpdate(true);
            setWidgetNeedsUpdate(true);
            setScrollOffsetX(getScrollOffsetX(), true); // Clamp if needed
            setScrollOffsetY(getScrollOffsetY(), true); // Clamp if needed

            setScrollOffsetX(0, true);
            offsetX = 0;

            mFixedColWidth = val;
        }

        int getFixedHeaderRowHeight()
        {
            return mFixedHeaderRowHeight == -1 ? 30 : mFixedHeaderRowHeight;
        }

        void setFixedHeaderRowHeight(int val)
        {
            mFixedHeaderRowHeight = val;
            setNeedsUpdate(true);
        }

        int getFixedFooterRowHeight()
        {
            return mFixedFooterRowHeight;
        }

        void setFixedFooterRowHeight(int val)
        {
            mFixedFooterRowHeight = val;
            setNeedsUpdate(true);
        }

        shared_ptr<ofxSmartFont> getFont()
        {
            return font;
        }

        ofColor getCellLabelColor()
        {
            return mCellLabelColor;
        }

        void setCellLabelColor(ofColor val)
        {
            mCellLabelColor = val;
            mCellLabelColorSetManually = true;
        }

        ofColor getCellLabelSelectedColor()
        {
            return mCellLabelSelectedColor;
        }

        void setCellLabelSelectedColor(ofColor val)
        {
            mCellLabelSelectedColor = val;
            mCellLabelSelectedColorSetManually = true;
        }

        ofColor getCellHoverColor()
        {
            return mCellHoverColor;
        }

        void setCellHoverColor(ofColor val)
        {
            mCellHoverColor = val;
            mCellHoverColorSetManually = true;
        }

        ofColor getCellSelectionColor()
        {
            return mCellSelectionColor;
        }

        void setCellSelectionColor(ofColor val)
        {
            mCellSelectionColor = val;
            mCellSelectionColorSetManually = true;
        }

        ofColor getRowBGColor()
        {
            return mRowBGColor;
        }

        void setRowBGColor(ofColor val)
        {
            mRowBGColor = val;
            mRowBGColorSetManually = true;
        }

        ofColor getRowAltBGColor()
        {
            return mRowAltBGColor;
        }

        void setRowAltBGColor(ofColor val)
        {
            mRowAltBGColor = val;
            mRowAltBGColorSetManually = true;
        }

        uiVizCoord::vizPoint getHoveredIndex()
        {
            return uiVizCoord::vizPoint(mHoveredColIndex, mHoveredRowIndex);
        }

        uiVizCoord::vizBounds getHeaderColBounds()
        {
            return uiVizCoord::vizBounds(
                mHeaderRow.cells[0].cellSlice.bounds.x,
                mHeaderRow.cells[0].cellSlice.bounds.y,
                mHeaderRow.cells[0].cellSlice.bounds.width - getScaledPadding() * 2,
                scale(getUsableHeight()) + getScaledPadding());
        }

        void addRow(uiVizWidgetTableRow row, int index)
        {
            if (index >= mTableRows.size())
            {
                mTableRows.push_back(row);
            }
            else
            {
                auto pos = mTableRows.begin() + index;
                mTableRows.insert(pos, row);
            }
            setTableRows(mTableRows);
        }

        int getCellCount()
        {
            if (mTableRows.size() > 0)
            {
                int initial = (mTableRows.size() - 1) * mTableRows[0].cells.size();
                return initial + mTableRows[mTableRows.size() - 1].cells.size();
            }
            return 0;
        }

        void addCol(uiVizWidgetTableCell headerCell, vector<uiVizWidgetTableCell> insertCells, int index)
        {
            if (index >= mHeaderRow.cells.size())
            {
                mHeaderRow.cells.push_back(headerCell);
                for (int i = 0; i <= insertCells.size(); i++)
                {
                    if (mTableRows.size() > i)
                    {
                        mTableRows[i].cells.push_back(insertCells[i]);
                    }
                }
            }
            else
            {
                mHeaderRow.cells.insert(mHeaderRow.cells.begin() + index, headerCell);
                for (int i = 0; i <= insertCells.size(); i++)
                {
                    uiVizWidgetTableCell cellToInsert = insertCells[i];
                    if (mTableRows.size() > i)
                    {
                        mTableRows[i].cells.insert(mTableRows[i].cells.begin() + index, cellToInsert);
                    }
                }
            }
            setTableRows(mTableRows);
        }

        uiVizWidgetTableCell getHeaderCellByIndex(int index)
        {
            return (index > -1 && index < mHeaderRow.cells.size() ? mHeaderRow.cells[index] : uiVizWidgetTableCell("NOT_FOUND", ""));
        }

        uiVizWidgetTableRow getHeaderRow()
        {
            return mHeaderRow;
        }

        vector<uiVizWidgetTableRow> getFilteredRows()
        {
            return mTableRows;
        }

        vector<uiVizWidgetTableRow> getRows()
        {
            return mTableRowsFull;
        }

        void wrapRows(int cellWidth, int cellHeight)
        {
            // Call this on onWidgetResized to have cells
            // automatically "flow"

            setRowHeight(cellHeight);
            setIsAutoResizeToContentsWidth(false);
            setIsAutoResizeToContentsHeight(false);
            setFreezeHeaderRow(true);
            setFreezeFirstColumn(false);
            setAllowMultiSelect(false);
            setShowColumnFilters(false, false);
            setSelectionMode("CELL");
            setEnableColSnapScrolling(false);
            setFixedColWidth(cellHeight);

            setEnableScrollXPositive(false);
            setEnableScrollXNegative(false);

            int cols = std::max(int(getUsableWidth() / cellWidth), 1);
            // populateLinks({editIcon, tickIcon}, cols);

            uiVizWidgetTableRow row;
            std::vector<uiVizWidgetTableRow> rows;
            int existingCellCount = getCellCount();

            // Header
            vector<uiVizWidgetTableCell> headerCells = vector<uiVizWidgetTableCell>();

            for (int i = 1; i <= cols; i++)
            {
                headerCells.push_back(
                    uiVizWidgetTableCell(
                        "COL" + ofToString(i),
                        "",
                        cellWidth));
            }

            int i = 0;
            for (uiVizWidgetTableRow existingRows : getTableRows())
            {
                for (uiVizWidgetTableCell cell : existingRows.cells)
                {
                    row.cells.push_back(cell);
                    if (cols == 1 || i > 0 && (i + 1) % cols == 0 || i == existingCellCount - 1)
                    {
                        rows.push_back(row);
                        row.cells.clear();
                    }
                    i++;
                }
            }
            setHeaderRow(uiVizWidgetTableRow("HEADER", headerCells));
            setTableRows(rows);
        }

        void addRow(uiVizWidgetTableRow row)
        {
            addRow(row, mTableRows.size());
        }

        void addCol(uiVizWidgetTableCell headerCell, vector<uiVizWidgetTableCell> insertCells)
        {
            addCol(headerCell, insertCells, mHeaderRow.cells.size());
        }

        void addCol(uiVizWidgetTableCell headerCell, int index)
        {
            vector<uiVizWidgetTableCell> insertCells = vector<uiVizWidgetTableCell>();
            for (int i = 0; i < mTableRows.size(); i++)
            {
                insertCells.push_back(uiVizWidgetTableCell("", ""));
            }
            addCol(headerCell, insertCells, index);
        }

        void addCol(uiVizWidgetTableCell headerCell)
        {
            addCol(headerCell, mHeaderRow.cells.size());
        }

        uiVizWidgetTableRow *getTableRow(int rowIndex)
        {
            if (mTableRows.size() > rowIndex)
            {
                uiVizWidgetTableRow &row = mTableRows.at(rowIndex);
                return &row;
            }
            else
            {
                return nullptr;
            }
        }

        uiVizWidgetTableCell *getTableCell(int rowIndex, int cellIndex)
        {
            uiVizWidgetTableRow *row = getTableRow(rowIndex);
            if (row)
            {
                if (row->cells.size() > cellIndex)
                {
                    uiVizWidgetTableCell &cell = row->cells.at(cellIndex);
                    return &cell;
                }
                else
                {
                    return nullptr;
                }
            }
            else
            {
                return nullptr;
            }
        }

        uiVizWidgetTableRow *getTableRow(ofxXmlSettings eventXML)
        {
            int rowIndex = eventXML.getAttribute("event", "rowIndex", 0);
            return getTableRow(rowIndex);
        }

        uiVizWidgetTableCell *getTableCell(ofxXmlSettings eventXML)
        {
            int rowIndex = eventXML.getAttribute("event", "rowIndex", 0);
            int colIndex = eventXML.getAttribute("event", "colIndex", 0);
            return getTableCell(rowIndex, colIndex);
        }

        uiVizCoord::vizPoint getTableRowAndColForEvent(ofxXmlSettings eventXML)
        {
            int rowIndex = eventXML.getAttribute("event", "rowIndex", 0);
            int colIndex = eventXML.getAttribute("event", "colIndex", 0);
            return uiVizCoord::vizPoint(colIndex, rowIndex); // x,y
        }

        uiVizCoord::vizPoint getScrollOffsetForCell(int rowIndex, int cellIndex)
        {
            int scrollY = scale(getRowHeight()) * rowIndex;
            int scrollX = calculateVisibleColIndexStart().offset;
            return uiVizCoord::vizPoint(scrollX, scrollY);
        }

        void selectCellByIndex(int rowIndex, int cellIndex, bool scrollToRow, bool scrollToCell, bool fireEvent)
        {
            if (rowIndex > mTableRowsFull.size() - 1)
                return;

            if (!getAllowMultiSelect())
            {
                clearSelectedRows();
            }

            // todo : highlight not working, scroll not working

            uiVizWidgetTableRow &row = mTableRows.at(rowIndex);
            uiVizWidgetTableRow &rowFull = mTableRowsFull.at(rowIndex); // ref, so we can set vals
            uiVizWidgetTableCell cell("", "");

            row.isSelected = true;
            rowFull.isSelected = true;

            if (row.cells.size() > cellIndex)
            {
                row.cells[cellIndex].isSelected = true;
                rowFull.cells[cellIndex].isSelected = true;
                cell = rowFull.cells[cellIndex];
            }

            for (int cellIndex = 0; cellIndex < row.cells.size(); cellIndex++)
            {
                row.cells[cellIndex].cellSlice.needsUpdate = true;
                rowFull.cells[cellIndex].cellSlice.needsUpdate = true;
            }

            uiVizCoord::vizPoint p = getScrollOffsetForCell(rowIndex, cellIndex);
            if (scrollToRow)
            {
                if (getUsableHeight() < deScale(getContentBoundsScaled().height))
                {
                    setScrollOffsetY(deScale(p.y) * -1, true);
                }
            }

            if (scrollToCell)
            {
                if (getUsableWidth() < deScale(getContentBoundsScaled().width))
                {
                    setScrollOffsetX(deScale(p.x), true);
                }
            }

            if (fireEvent)
            {
                uiVizWidgetTableArgs args(*this, row, cell, rowIndex, cellIndex);
                eventTransmit(getPersistentId(), WIDGET_EVENT::TABLE_CELL_CLICKED, "<event rowIndex=\"" + ofToString(rowIndex) + "\" colIndex=\"" + ofToString(cellIndex) + "\"></event>", *this);
                ofNotifyEvent(cellSelected, args);
                eventTransmit(getPersistentId(), WIDGET_EVENT::TABLE_CELL_SELECTED, "<event rowIndex=\"" + ofToString(rowIndex) + "\" colIndex=\"" + ofToString(cellIndex) + "\"></event>", *this);
            }
            setWidgetNeedsUpdate(true);
            setNeedsUpdate(true);
        }

        void selectRowByIndex(int rowIndex, bool scrollToRow, bool fireEvent)
        {
            if (rowIndex > mTableRows.size() - 1)
                return;
            selectCellByIndex(rowIndex, 0, scrollToRow, false, fireEvent);
        }

        void selectRowByKey(string rowKey, bool scrollToRow, bool fireEvent)
        {
            for (int i = 0; i < mTableRows.size(); i++)
            {
                uiVizWidgetTableRow &row = mTableRows.at(i);
                if (row.key == rowKey)
                {
                    selectCellByIndex(i, 0, scrollToRow, false, fireEvent);
                }
            }
        }

        virtual void setTheme(uiVizWidgetTheme val) override
        {
            mCellLabelColorSetManually = false;
            mCellLabelSelectedColorSetManually = false;
            mCellHoverColorSetManually = false;
            mCellSelectionColorSetManually = false;
            mRowBGColorSetManually = false;
            mRowAltBGColorSetManually = false;
            uiVizWidget::setTheme(val);
            populateTableTheme();
        }

        /*
            workaround for "low res invisibility bug"
            For some reason, when the uiVizWidgetTable is drawn on lower res display (ie non-reina),
            it often is invisible at first, if hovered it magically appears. he below hooks attemt to
            work around this bug until root cause is found!
            -----------------------------------------------------------------------------------------
        */

        virtual void onWidgetVisibilityChanged(bool visible) override
        {
            updateWidget();
            drawWidget();
        }

        virtual void onWidgetWasJustInitialized() override
        {
            updateWidget();
            drawWidget();
        }

        virtual void onWidgetShownAsPopout(uiVizWidgetBase *poppedOutFrom) override
        {
            updateWidget();
            drawWidget();
        }

        virtual void onChildWidgetsLoaded() override
        {
            updateWidget();
            drawWidget();
        }

        void setHeaderRow(uiVizWidgetTableRow val)
        {
            mTableStructChanged = true;
            mHeaderRow = val;
            setWidgetNeedsUpdate(true);
            setNeedsUpdate(true);
        }

        void setTableRows(vector<uiVizWidgetTableRow> val)
        {
            mTableStructChanged = true;
            visibleRowIndexStart = 0;
            visibleRowIndexEnd = 0;
            visibleColIndexStart = 0;
            visibleColIndexEnd = 0;

            visibleRowIndexStartPrev = 0;
            visibleRowIndexEndPrev = 0;
            visibleColIndexStartPrev = 0;
            visibleColIndexEndPrev = 0;

            offsetX = 0;
            offsetY = 0;

            mTableRows = val;
            mTableRowsFull = val;
            mTableStructChanged = true;
            setScrollOffsetX(0, true);
            setScrollOffsetY(0, true);
            setWidgetNeedsUpdate(true);
            setNeedsUpdate(true);
        }

        void setTableUseFbo(bool val)
        {
            mTableUseFbo = val;
        }

        bool getTableUseFbo()
        {
            return mTableUseFbo;
        }

        void setShouldCloseUponSelection(bool val)
        {
            mShouldCloseUponSelection = val;
        }

        bool getShouldCloseUponSelection()
        {
            return mShouldCloseUponSelection;
        }

        /*
            -----------------------------------------------------------------------------------------
        */

    protected:
        void populateTableTheme()
        {
            if (!mCellLabelColorSetManually)
                mCellLabelColor = getTheme().TypographySecondaryColor;
            if (!mCellLabelSelectedColorSetManually)
                mCellLabelSelectedColor = getTheme().SelectionForegroundColor;
            if (!mCellHoverColorSetManually)
                mCellHoverColor = getTheme().HoverBackgroundColor;
            if (!mCellSelectionColorSetManually)
                mCellSelectionColor = getTheme().SelectionBackgroundColor;
            if (!mRowBGColorSetManually)
                mRowBGColor = getTheme().WidgetAlternating1Color;
            if (!mRowAltBGColorSetManually)
                mRowAltBGColor = getTheme().WidgetAlternating2Color;
        }

        virtual void onWidgetThemeChanged(uiVizWidgetEventArgs args) override
        {
            populateTableTheme();
        }

        virtual void initWidgetScale(float scale) override
        {
            font = getViz()->getMediumFont();
        }

        void initWidget() override
        {
            // setUseFbo(false); // We internally use an fbo per cell... so don't use one at parent level

            mFunction_DrawCellHighlight_Hover = [this](bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex, int colIndex)
            {
                drawCellHighlight_Hover(isRowSelected, isRowHovered, absoluteScaledX, absoluteScaledY, scaledWidth, scaledHeight, row, cell, rowIndex, colIndex);
            };

            mFunction_DrawRowHighlight_Hover = [this](bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, int rowIndex)
            {
                drawRowHighlight_Hover(isRowSelected, isRowHovered, absoluteScaledX, absoluteScaledY, scaledWidth, scaledHeight, row, rowIndex);
            };

            mFunction_DrawCellHighlight_Selected = [this](bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex, int colIndex)
            {
                drawCellHighlight_Selected(isRowSelected, isRowHovered, absoluteScaledX, absoluteScaledY, scaledWidth, scaledHeight, row, cell, rowIndex, colIndex);
            };

            mFunction_DrawRowHighlight_Selected = [this](bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, int rowIndex)
            {
                drawRowHighlight_Selected(isRowSelected, isRowHovered, absoluteScaledX, absoluteScaledY, scaledWidth, scaledHeight, row, rowIndex);
            };

            mFunction_DrawCellContent = [this](bool isRowSelected, bool isRowHovered, bool isCellHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex, int colIndex)
            {
                drawCellContent(isRowSelected, isRowHovered, isCellHovered, absoluteScaledX, absoluteScaledY, absoluteScaledLabelX, absoluteScaledLabelY, scaledWidth, scaledHeight, row, cell, rowIndex, colIndex);
            };

            font = getViz()->getMediumFont();
            setIsAutoUpdateWhenActive(true);
            setTitleStyle(TitleStyle::TOP_STATIC);
            setScrollAction(ScrollAction::SCROLL);

            setEnableScrollYPositive(true);
            setEnableScrollYNegative(true);
            setEnableScrollXPositive(true);
            setEnableScrollXNegative(true);

            setMinWidth(150);
            // setMaxHeight(300);

            populateTableTheme();
        }

    private:
        uiVizWidgetTableRow mHeaderRow = uiVizWidgetTableRow();
        vector<uiVizWidgetTableRow> mTableRows = vector<uiVizWidgetTableRow>();
        vector<uiVizWidgetTableRow> mTableRowsFull = vector<uiVizWidgetTableRow>();
        vector<uiVizWidgetTableFilterData> mColumnFilters = vector<uiVizWidgetTableFilterData>();

        ofColor mCellLabelColor = NULL;
        ofColor mCellLabelSelectedColor = NULL;
        ofColor mCellHoverColor = NULL;
        ofColor mCellSelectionColor = NULL;
        ofColor mRowBGColor = NULL;
        ofColor mRowAltBGColor = NULL;

        bool mCellLabelColorSetManually = false;
        bool mCellLabelSelectedColorSetManually = false;
        bool mCellHoverColorSetManually = false;
        bool mCellSelectionColorSetManually = false;
        bool mRowBGColorSetManually = false;
        bool mRowAltBGColorSetManually = false;

        shared_ptr<ofxSmartFont> font;
        float mFixedHeaderRowHeight = -1;
        float mFixedFooterRowHeight = 0;
        float mRowHeight = 30;
        float mFixedColWidth = 0;
        int mActiveMenuTabIndex = 0;
        int mActiveMenuTabIndexPrev = 0;
        int prevContentBoundsWidth = 0;
        int prevContentBoundsHeight = 0;
        int mHoveredRowIndex = -1;
        int mHoveredColIndex = -1;
        int mHoveredRowIndexPrev = -2;
        int mHoveredColIndexPrev = -2;

        // header styling
        int mMaxHeaderRowHeight = 0;

        bool mFreezeHeaderRow = true;
        bool mFreezeFirstColumn = false;
        bool mAllowMultiSelect = false;
        bool mShowColumnFilters = true;
        bool mEnableColSnapScrolling = false;

        bool mIsPivoted = false;

        bool mRowChildWidgetsInitialized = false;
        bool mNeedsFullWidthRecalculation = false;

        int visibleRowIndexStart = 0;
        int visibleRowIndexEnd = 0;
        int visibleColIndexStart = 0;
        int visibleColIndexEnd = 0;

        int visibleRowIndexStartPrev = 0;
        int visibleRowIndexEndPrev = 0;
        int visibleColIndexStartPrev = 0;
        int visibleColIndexEndPrev = 0;

        int offsetX = 0;
        int offsetY = 0;
        bool mTableUseFbo = false;
        bool mTableStructChanged = false;
        bool mCellPercentagesWereCalculated = false;
        bool mShouldCloseUponSelection = false;

        std::function<void(bool, bool, int, int, int, int, uiVizWidgetTableRow &, uiVizWidgetTableCell &cell, int, int &)> mFunction_DrawCellHighlight_Hover;
        std::function<void(bool, bool, int, int, int, int, uiVizWidgetTableRow &, int &)> mFunction_DrawRowHighlight_Hover;
        std::function<void(bool, bool, int, int, int, int, uiVizWidgetTableRow &, uiVizWidgetTableCell &, int, int &)> mFunction_DrawCellHighlight_Selected;
        std::function<void(bool, bool, int, int, int, int, uiVizWidgetTableRow &, int &)> mFunction_DrawRowHighlight_Selected;
        std::function<void(bool, bool, bool, int, int, int, int, int, int, uiVizWidgetTableRow &, uiVizWidgetTableCell &, int, int &)> mFunction_DrawCellContent;

        std::function<void(uiVizWidgetTableArgs &)> mFunction_Cell_Selected = NULL;
        std::function<void(uiVizWidgetTableArgs &)> mFunction_Cell_UnSelected = NULL;
        std::function<void(uiVizWidgetTableArgs &)> mFunction_Cell_Hovered = NULL;

        SelectionMode mSelectionMode = SelectionMode::ROW;

        bool getTableStructChanged()
        {
            return mTableStructChanged;
        }

        uiVizCoord::vizIndexOffset calculateVisibleRowIndexStart(int headerRowHeight)
        {
            float scaledRowHeight = scale(getRowHeight());
            return uiVizCoord::vizIndexOffset(std::max((int)((scale(getScrollOffsetY()) * -1 - headerRowHeight) / scaledRowHeight) - 1, 0), scale(getScrollOffsetY()) * -1);
        }

        int calculateVisibleRowIndexEnd(int headerRowHeight)
        {
            uiVizCoord::vizIndexOffset visibleRowStartOffset = calculateVisibleRowIndexStart(headerRowHeight);
            int addIdx = (scale(getScrollOffsetY()) * -1 > headerRowHeight ? 2 : 1);
            return std::min((int)(visibleRowStartOffset.index + getHeight() / getRowHeight() + addIdx), (int)mTableRows.size());
        }

        uiVizCoord::vizIndexOffset calculateVisibleColIndexStart()
        {
            // Cant hide 1st col if only 1 col exists!
            if (mHeaderRow.cells.size() <= 1)
            {
                return uiVizCoord::vizIndexOffset(0, scale(getScrollOffsetX()) * -1);
            }
            int accumWidthStart = 0;
            int colIndexStart = 0;

            int lastVal = 0;

            for (int j = 0; j < mHeaderRow.cells.size(); j++)
            {
                if (accumWidthStart >= (scale(getScrollOffsetX()) * -1))
                {
                    accumWidthStart -= lastVal;
                    break;
                }
                accumWidthStart += mHeaderRow.cells[j].cellSlice.bounds.width;
                lastVal = mHeaderRow.cells[j].cellSlice.bounds.width;
                colIndexStart++;
            }

            int _offsetX = 0;

            int retVal = std::max((int)colIndexStart - 1, 0);
            _offsetX = (accumWidthStart - scale(getScrollOffsetX()) * -1) * -1; // Sweet but psycho?

            return uiVizCoord::vizIndexOffset(retVal, _offsetX);
        }

        int calculateVisibleColIndexEnd()
        {
            uiVizCoord::vizIndexOffset visibleColStartOffset = calculateVisibleColIndexStart();

            int colIndexEnd = visibleColStartOffset.index;
            int accumWidthEnd = 0;

            for (int j = visibleColStartOffset.index; j < mHeaderRow.cells.size(); j++)
            {
                if ((deScale(accumWidthEnd) >= (getWidth()) + deScale(visibleColStartOffset.offset) + (getFreezeFirstColumn() ? deScale(mHeaderRow.cells[0].cellSlice.bounds.width) : 0)))
                    break;
                accumWidthEnd += mHeaderRow.cells[j].cellSlice.bounds.width;
                colIndexEnd++;
            }

            return std::min((int)colIndexEnd, (int)mHeaderRow.cells.size());
        }

        void hideColumnFilters()
        {

            // Filter cols
            for (int i = 0; i < mHeaderRow.cells.size(); i++)
            {
                uiVizWidgetElm *e = getWidgetElement("COL_FILTER_" + ofToString(i));
                if (e)
                    e->setIsVisible(false);
            }
        }

        void loadTableWidgets()
        {
            if (!mRowChildWidgetsInitialized)
            {

                setShowColumnFilters(getShowColumnFilters(), true);

                for (int i = 0; i < mTableRows.size(); i++)
                {
                    uiVizWidgetTableRow &row = mTableRows.at(i); // ref, so we can set vals
                    for (int j = 0; j < row.cells.size(); j++)
                    {
                        uiVizWidgetTableCell &cell = row.cells[j]; // ref, so we can set vals

                        if (cell.get_WIDGET_CLASS() != "")
                        {

                            uiVizWidgetElm *elm = addOrLoadWidgetElement(
                                elm, cell.get_WIDGET_CLASS(), getPersistentId() + "_R" + ofToString(i) + "_C" + ofToString(j), "<element></element>");

                            if (elm != nullptr)
                            {

                                elm->setIsVisible(false);
                                cell.cellWidget = elm;
                            }
                            else
                            {

                                cell.cellWidget = uiVizWidgetManager::loadWidget(
                                    cell.get_WIDGET_CLASS(), getPersistentId() + "_R" + ofToString(i) + "_C" + ofToString(j), "<widget></widget>");
                                uiVizWidget *widget = dynamic_cast<uiVizWidget *>(cell.cellWidget);
                                widget->setIsVisible(false);
                                addChildWidget(*widget);
                                // uiVizWidgetManager::addWidget(*widget,false);
                            }
                        }
                    }
                }
                mRowChildWidgetsInitialized = true;
            }
        }

        void drawHeaderRow(int colIndex, bool rowHovered, uiVizWidgetTableRow &row)
        {

            uiVizWidgetTableCell &cell = mHeaderRow.cells[colIndex]; // ref, so we can set vals

            bool cellHovered = cell.cellSlice.isHoveredOffset(offsetX, offsetY);

            int abs_cellX = (getFreezeFirstColumn() && colIndex == 0 ? scale(uiVizWidget::getUsableX(false)) - getScaledPadding() : cell.cellSlice.bounds.x - offsetX - getScaledPadding() * 2);
            int abs_cellY = (getFreezeHeaderRow() ? scale(uiVizWidget::getUsableY(false)) - getScaledPadding() : cell.cellSlice.bounds.y - offsetY - getScaledPadding());

            int abs_cellLabelX = (getFreezeFirstColumn() && colIndex == 0 ? scale(uiVizWidget::getUsableX(false)) : cell.cellSlice.labelPoint.x - offsetX - getScaledPadding());
            int abs_cellLabelY = (getFreezeHeaderRow() ? cell.cellSlice.labelPoint.y : cell.cellSlice.labelPoint.y - offsetY);

            int abs_cellWidth = cell.cellSlice.bounds.width;
            int abs_cellHeight = (getShowColumnFilters() ? mHeaderRow.rowSlice.bounds.height : mHeaderRow.rowSlice.bounds.height);

            if (getTableUseFbo())
            {

                if (cell.cellSlice.needsUpdate)
                {
                    cell.cellSlice.allocateFbo(true, abs_cellWidth, abs_cellHeight);
                    cell.cellSlice.fbo.begin();
                    ofClear(0, 0, 0, 0);
                    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                    ofPushMatrix();
                    ofTranslate(-1 * abs_cellX, -1 * abs_cellY);
                    ofPushStyle();

                    drawCellHeaderRow(
                        row.isSelected, rowHovered,
                        abs_cellX, abs_cellY,
                        abs_cellLabelX, abs_cellLabelY,
                        abs_cellWidth, abs_cellHeight,
                        row, cell,
                        colIndex);

                    ofPopStyle();
                    ofPopMatrix();
                    cell.cellSlice.fbo.end();
                    cell.cellSlice.needsUpdate = false;
                }
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                cell.cellSlice.fbo.draw(abs_cellX, abs_cellY, abs_cellWidth, abs_cellHeight);
                ofEnableAlphaBlending();
            }
            else
            {
                drawCellHeaderRow(
                    row.isSelected, rowHovered,
                    abs_cellX, abs_cellY,
                    abs_cellLabelX, abs_cellLabelY,
                    abs_cellWidth, abs_cellHeight,
                    row, cell,
                    colIndex);
            }
        }

        void drawFooterRow(int colIndex, bool rowHovered, uiVizWidgetTableRow &row)
        {

            uiVizWidgetTableCell &cell = mHeaderRow.cells[colIndex]; // ref, so we can set vals

            bool cellHovered = cell.cellSlice.isHoveredOffset(offsetX, offsetY);

            int abs_cellX = (getFreezeFirstColumn() && colIndex == 0 ? scale(uiVizWidget::getUsableX(false)) - getScaledPadding() : cell.cellSlice.bounds.x - offsetX - getScaledPadding() * 2);
            int abs_cellY = scale(uiVizWidget::getUsableY(false)) + scale(getUsableHeight()) - scale(getFixedFooterRowHeight()) + getScaledPadding();

            int abs_cellWidth = cell.cellSlice.bounds.width;
            int abs_cellHeight = scale(getFixedFooterRowHeight());

            drawCellFooterRow(
                row.isSelected, rowHovered,
                abs_cellX, abs_cellY,
                abs_cellWidth, abs_cellHeight,
                row, cell,
                colIndex);
        }

        int calculateHeaderCellBounds(int colIndex, int accumX, int titleRowHeight, int labelHeight)
        {
            int cellWidth = 0;
            uiVizWidgetTableCell &cell = mHeaderRow.cells[colIndex]; // ref, so we can set vals

            // int cellX = scale(uiVizWidget::getUsableX(j > 0 || !getFreezeFirstColumn()));
            int cellX = scale(uiVizWidget::getUsableX(false));

            ofRectangle rectLabel;
            if (cell.label != "")
                rectLabel = font->rect(cell.label);

            if (getFixedColWidth() == 0)
            {
                if (cell.getWasWidthPercSet())
                {
                    cellWidth = scale(calculateWidthPerc(cell, rectLabel));
                }
                else if (cell.getWasWidthSet())
                {
                    cellWidth = scale(cell.getWidth());
                }
                else
                {
                    cellWidth = std::max((int)scale(100), (int)(rectLabel.width + getScaledPadding() * 2));
                }
            }
            else
            {
                if (cell.getWasWidthPercSet())
                {
                    cellWidth = scale(calculateWidthPerc(cell, rectLabel));
                }
                else if (cell.getWasWidthSet())
                {
                    cellWidth = scale(cell.getWidth());
                }
                else
                {
                    cellWidth = std::max((int)scale(cell.getWidth()), (int)scale(getFixedColWidth()));
                }
            }

            cell.cellSlice.labelPoint = uiVizCoord::vizPoint(
                cellX + accumX,
                scale(uiVizWidget::getUsableY(false)) + (getShowColumnFilters() ? labelHeight + getScaledPadding() : titleRowHeight * 0.5f + labelHeight * 0.25f) // Top of cell if showing filters, otherwise center
            );

            cell.cellSlice.bounds.width = cellWidth;
            cell.cellSlice.bounds.height = (getFixedHeaderRowHeight() == 0 ? 0 : labelHeight);
            cell.cellSlice.bounds.x = cellX + accumX;
            cell.cellSlice.bounds.y = scale(uiVizWidget::getUsableY(false));

            if (rectLabel.height > mMaxHeaderRowHeight && getFixedHeaderRowHeight() != 0)
            {
                mMaxHeaderRowHeight = rectLabel.height;
            }
            return cellWidth;
        }

        int dynamicallySizeCol(uiVizWidgetTableRow &row, int colIndex, int accumX, int cellWidth, int rowHeight, int labelHeight)
        {
            if (colIndex > row.cells.size() - 1)
                return 0;
            uiVizWidgetTableCell &cell = row.cells[colIndex]; // ref, so we can set vals
            int cellX = scale(uiVizWidget::getUsableX(false));
            cell.cellSlice.labelPoint = uiVizCoord::vizPoint(
                cellX + accumX,
                row.rowSlice.bounds.y + ((float)(rowHeight + labelHeight * 0.25f + getScaledPadding() * 1.25f) / 2.0f));

            ofRectangle rectLabel;
            if (cell.label != "")
                rectLabel = font->rect(cell.label);

            // Dynamic or fixed col width ?
            if (getFixedColWidth() == 0)
            {
                if (mHeaderRow.cells.size() - 1 >= colIndex && (mHeaderRow.cells[colIndex].getWasWidthSet() || mHeaderRow.cells[colIndex].getWasWidthPercSet()))
                {
                    if (mHeaderRow.cells[colIndex].getWasWidthPercSet())
                    {
                        cellWidth = scale(calculateWidthPerc(mHeaderRow.cells[colIndex], rectLabel));
                    }
                    else
                    {
                        cellWidth = scale(mHeaderRow.cells[colIndex].getWidth());
                    }
                }
                else
                {
                    cellWidth = std::max((int)scale(cell.getWidth()), (int)(rectLabel.width + getScaledPadding() * 2));
                }
            }
            else
            {
                if (mHeaderRow.cells.size() - 1 >= colIndex && (mHeaderRow.cells[colIndex].getWasWidthSet() || mHeaderRow.cells[colIndex].getWasWidthPercSet()))
                {
                    if (mHeaderRow.cells[colIndex].getWasWidthPercSet())
                    {
                        cellWidth = scale(calculateWidthPerc(mHeaderRow.cells[colIndex], rectLabel));
                    }
                    else
                    {
                        cellWidth = scale(mHeaderRow.cells[colIndex].getWidth());
                    }
                }
                else
                {
                    cellWidth = std::max((int)scale(cell.getWidth()), (int)scale(getFixedColWidth()));
                }
            }

            if (cellWidth > mHeaderRow.cells[colIndex].cellSlice.bounds.width)
            {
                mHeaderRow.cells[colIndex].cellSlice.bounds.width = cellWidth;
            }

            if ((cellX + accumX) > mHeaderRow.cells[colIndex].cellSlice.bounds.x)
            {
                mHeaderRow.cells[colIndex].cellSlice.bounds.x = cellX + accumX;
                mHeaderRow.cells[colIndex].cellSlice.labelPoint.x = cellX + accumX;
            }
            accumX += cellWidth;
            return accumX;
        }

        void setCellBounds(uiVizWidgetTableRow &row, int rowIndex, int colIndexStart, int colIndexEnd)
        {

            // Cell bounds
            for (int j = colIndexStart; j < colIndexEnd; j++)
            {

                if (j > row.cells.size() - 1)
                    return;

                uiVizWidgetTableCell &cell = row.cells[j]; // ref, so we can set vals

                cell.cellSlice.bounds = uiVizCoord::vizBounds(
                    mHeaderRow.cells[j].cellSlice.bounds.x - getScaledPadding(),
                    row.rowSlice.bounds.y,
                    mHeaderRow.cells[j].cellSlice.bounds.width,
                    row.rowSlice.bounds.height);

                cell.cellSlice.labelPoint.x = mHeaderRow.cells[j].cellSlice.bounds.x;

                // Cell widget
                if (cell.cellWidget != nullptr && !getIsPivoted())
                {
                    cell.cellWidget->setIsVisible(true);
                    cell.cellWidget->setX(deScale(mHeaderRow.cells[j].cellSlice.labelPoint.x - offsetX));
                    cell.cellWidget->setY(deScale(row.rowSlice.bounds.y - offsetY));
                    cell.cellWidget->setWidth(deScale(cell.cellSlice.bounds.width) - getNonScaledPadding() * 2);
                    cell.cellWidget->setHeight(std::min((int)getRowHeight(), (int)cell.cellWidget->getHeight()));
                    cell.cellWidget->setWidgetNeedsUpdate(true);
                    cell.cellWidget->setNeedsUpdate(true);

                    if (uiVizWidgetElm *elm = dynamic_cast<uiVizWidgetElm *>(cell.cellWidget))
                    {
                    }
                    else if (uiVizWidget *widget = dynamic_cast<uiVizWidget *>(cell.cellWidget))
                    {
                        widget->setDrawWidgetChrome(false);
                        widget->setDrawWidgetTitle(false);
                    }

                    if (!cell.cellWidgetInitialized)
                    {
                        uiVizWidgetTableArgs args(*this, row, row.cells[j], rowIndex, j);
                        ofNotifyEvent(cellWidgetInit, args);
                        eventTransmit(getPersistentId(), WIDGET_EVENT::TABLE_CELL_WIDGET_INITIALIZED, "<event rowIndex=\"" + ofToString(rowIndex) + "\" colIndex=\"" + ofToString(j) + "\"></event>", *this);
                        cell.cellWidgetInitialized = true;
                    }
                }
            }
        }

        int calculateWidthPerc(uiVizWidgetTableCell &cell, ofRectangle rectLabel)
        {

            int percWidthVal = (float)getUsableWidth() * ((float)cell.getWidthPerc() / 100.0f);

            int numPerCells = max(1, mHeaderRow.calculateNumberOfPercCells());
            int numNonPerCells = mHeaderRow.calculateNumberOfNonPercCells();
            int widthToSubtract = deScale(mHeaderRow.calculateWidthNonPercCells() / numPerCells) - (numNonPerCells * getNonScaledPadding() * 2) / numPerCells - getNonScaledPadding();
            percWidthVal -= widthToSubtract;

            int cellWidthNonPercVal = cell.getWidth() <= 0 ? std::max((int)10, (int)(deScale(rectLabel.width) + getNonScaledPadding() * 2)) : cell.getWidth();

            mCellPercentagesWereCalculated = true;

            return max(percWidthVal, cellWidthNonPercVal);
        }

        void layoutHeaderRowFilters(int colIndex, uiVizWidgetTableCell &cell, int headerRowHeight)
        {
            if (getShowColumnFilters() && !getIsPivoted())
            {
                uiVizWidgetElmTextbox *TXT_COL_FILTER = getTextbox("COL_FILTER_" + ofToString(colIndex));
                if (TXT_COL_FILTER)
                {
                    TXT_COL_FILTER->setIsVisible(true);
                    TXT_COL_FILTER->setX(deScale(cell.cellSlice.bounds.x) - getUsableX(false) - (getFreezeFirstColumn() && (colIndex == 0) ? 0 : deScale(offsetX)));
                    TXT_COL_FILTER->setY(deScale(cell.cellSlice.bounds.y) + deScale(headerRowHeight) - TXT_COL_FILTER->getHeight() - getNonScaledPadding() * 2 - getY() - (getFreezeHeaderRow() ? 0 : deScale(offsetY)));
                    TXT_COL_FILTER->setWidth(deScale(cell.cellSlice.bounds.width) - getNonScaledPadding() * 2);
                    //   TXT_COL_FILTER->setTheme(uiVizShared::getViz()->getThemeManager()->getContrastingDefaultTheme(getTheme(), false));

                    if (getFreezeFirstColumn())
                    {
                        if (scale(getScrollOffsetX()) * -1 > mHeaderRow.cells[0].cellSlice.bounds.width * 0.8f)
                        {
                            uiVizWidgetElmTextbox *TXT_COL_FILTER_FIRST = getTextbox("COL_FILTER_0");
                            TXT_COL_FILTER_FIRST->setIsVisible(false);
                        }
                    }
                }
            }
        }

        void clearOutOfViewCellFbo()
        {

            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            // Rows
            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*

            if (visibleRowIndexStart != visibleRowIndexStartPrev)
            {

                int startVector = visibleRowIndexStart;
                int endVector = visibleRowIndexStartPrev;

                if (visibleRowIndexStartPrev < visibleRowIndexStart)
                {
                    startVector = visibleRowIndexStartPrev;
                    endVector = visibleRowIndexStart;
                }

                for (int i = startVector; i < endVector; i++)
                {
                    uiVizWidgetTableRow &row = mTableRows.at(i);
                    for (int j = visibleColIndexStart; j < visibleColIndexEnd; j++)
                    {
                        if (j > row.cells.size() - 1)
                            break;
                        uiVizWidgetTableCell &cell = row.cells.at(j);
                        cell.cellSlice.clearFbo();
                    }
                }
            }

            if (visibleRowIndexEnd != visibleRowIndexEndPrev)
            {

                int startVector = visibleRowIndexEnd;
                int endVector = visibleRowIndexEndPrev;

                if (visibleRowIndexEndPrev < visibleRowIndexStart)
                {
                    startVector = visibleRowIndexEndPrev;
                    endVector = visibleRowIndexEnd;
                }

                for (int i = startVector; i < endVector; i++)
                {
                    uiVizWidgetTableRow &row = mTableRows.at(i);
                    for (int j = visibleColIndexStart; j < visibleColIndexEnd; j++)
                    {
                        if (j > row.cells.size() - 1)
                            break;
                        uiVizWidgetTableCell &cell = row.cells.at(j);
                        cell.cellSlice.clearFbo();
                    }
                }
            }

            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            // Cols
            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            if (visibleColIndexStart != visibleColIndexStartPrev)
            {

                int startVector = visibleColIndexStart;
                int endVector = visibleColIndexStartPrev;

                if (visibleColIndexStartPrev < visibleColIndexStart)
                {
                    startVector = visibleColIndexStartPrev;
                    endVector = visibleColIndexStart;
                }

                for (int i = visibleRowIndexStart; i < visibleRowIndexEnd; i++)
                {
                    uiVizWidgetTableRow &row = mTableRows.at(i);
                    for (int j = startVector; j < endVector; j++)
                    {
                        if (j > row.cells.size() - 1)
                            break;
                        uiVizWidgetTableCell &cell = row.cells.at(j);
                        cell.cellSlice.clearFbo();
                    }
                }
            }

            if (visibleColIndexEnd != visibleColIndexEndPrev)
            {

                int startVector = visibleColIndexEnd;
                int endVector = visibleColIndexEndPrev;

                if (visibleColIndexEndPrev < visibleColIndexEnd)
                {
                    startVector = visibleColIndexEndPrev;
                    endVector = visibleColIndexEnd;
                }

                for (int i = visibleRowIndexStart; i < visibleRowIndexEnd; i++)
                {
                    uiVizWidgetTableRow &row = mTableRows.at(i);
                    for (int j = startVector; j < endVector; j++)
                    {
                        if (j > row.cells.size() - 1)
                            break;
                        uiVizWidgetTableCell &cell = row.cells.at(j);
                        cell.cellSlice.clearFbo();
                    }
                }
            }

            // Capture///
            visibleRowIndexStartPrev = visibleRowIndexStart;
            visibleRowIndexEndPrev = visibleRowIndexEnd;
            visibleColIndexStartPrev = visibleColIndexStart;
            visibleColIndexEndPrev = visibleColIndexEnd;
        }
    };
}