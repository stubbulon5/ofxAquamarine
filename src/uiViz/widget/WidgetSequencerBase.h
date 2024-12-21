
#pragma once
#include "WidgetTable.h"

namespace Aquamarine
{
    class WidgetSequencerBase : public WidgetTable
    {
    public:
        WidgetSequencerBase(string persistentId, string widgetXML) : WidgetTable(persistentId, widgetXML)
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

            setRowHeight(settings.getAttribute("properties", "rowHeight", 30));
            setFreezeHeaderRow(settings.getAttribute("properties", "freezeHeaderRow", true));
            setFreezeFirstColumn(settings.getAttribute("properties", "freezeFirstColumn", true));
            setAllowMultiSelect(settings.getAttribute("properties", "allowMultiSelect", true));
            setShowColumnFilters(false, false);
            setSelectionMode(settings.getAttribute("properties", "selectionMode", "CELL"));
            setEnableColSnapScrolling(false);
            setFixedColWidth(settings.getAttribute("properties", "fixedColWidth", 4 * getRowHeight()));
            setFixedHeaderRowHeight(settings.getAttribute("properties", "fixedHeaderRowHeight", 30));

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = Widget::saveState();
            settings.setAttribute("widget", "class", WidgetManager::WIDGET_CLASS_SEQUENCER, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~WidgetSequencerBase() {
        };

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
            WidgetTable::onWidgetEventReceived(args);
        }

        virtual void drawCellHeaderCol(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, WidgetTableRow &row, WidgetTableCell &cell, int rowIndex)
        {
        }

        /********* Override this to draw your own content *********/
        virtual void drawCellContent(bool isRowSelected, bool isRowHovered, bool isCellHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, WidgetTableRow &row, WidgetTableCell &cell, int rowIndex, int colIndex) override
        {

            /*
            Elm vizElm_cell;
            vizElm_cell.setRectangle(
                absoluteScaledX,
                absoluteScaledY,
                scale(5),
                scale(ofRandom(5, 10)),
                ofColor::red
            );
            vizElm_cell.draw();

            vizElm_cell.setRectangle(
                absoluteScaledX+scale(10),
                absoluteScaledY,
                scale(5),
                scale(ofRandom(5, 10)),
                ofColor::green
            );
            vizElm_cell.draw();

            vizElm_cell.setRectangle(
                absoluteScaledX+scale(15),
                absoluteScaledY,
                scale(5),
                scale(ofRandom(5, 10)),
                ofColor::blue
            );
            vizElm_cell.draw();
            ofPushStyle();
            ofSetColor(ofColor::black);
            font->draw("HI!", absoluteScaledLabelX, absoluteScaledLabelY);
            ofPopStyle();
            */

            if (getShowGridX())
            {
                Elm vizElmX;
                vizElmX.setRectangle(
                    absoluteScaledX,
                    absoluteScaledY + scaledHeight - scale(2),
                    scaledWidth - (getShowGridY() ? scale(2) : 0),
                    scale(2),
                    getGridColorX(0.6));
                vizElmX.draw();
            }

            if (getShowGridY())
            {
                Elm vizElmY;
                vizElmY.setRectangle(
                    absoluteScaledX + scaledWidth - scale(2),
                    absoluteScaledY,
                    scale(2),
                    scaledHeight,
                    getGridColorY(0.8f));
                vizElmY.draw();

                int quarterWidth = (float)scaledWidth * 0.25f;
                for (int j = 1; j <= 3; j++)
                {

                    vizElmY.setRectangle(
                        absoluteScaledX + j * quarterWidth - scale(2),
                        absoluteScaledY,
                        scale(2),
                        scaledHeight,
                        getGridColorY(0.6f));
                    vizElmY.draw();
                }
            }
        }

        bool getShowGridX()
        {
            return mShowGridX;
        }

        void getShowGridX(bool val)
        {
            mShowGridX = val;
        }

        bool getShowGridY()
        {
            return mShowGridY;
        }

        void getShowGridY(bool val)
        {
            mShowGridY = val;
        }

        ofColor getGridColorX(float alpha)
        {
            return ofColor(mGridColorX.r, mGridColorX.g, mGridColorX.b, 255.0f * alpha);
        }

        void setGridColorX(ofColor val)
        {
            mGridColorX = val;
        }

        ofColor getGridColorY(float alpha)
        {
            return ofColor(mGridColorY.r, mGridColorY.g, mGridColorY.b, 255.0f * alpha);
        }

        void setGridColorY(ofColor val)
        {
            mGridColorY = val;
        }

    private:
        shared_ptr<ofxSmartFont> font;
        ofColor borderColor;
        bool mShowGridX = true;
        bool mShowGridY = true;

        ofColor mGridColorX;
        ofColor mGridColorY;

        void initWidget() override
        {
            setTableUseFbo(true);
            font = getViz()->getMediumFont();
            borderColor = ofColor(0, 0, 0, 255 * 0.05f);
            setGridColorX(getTheme().TypographyPrimaryColor_withAlpha(1));
            setGridColorY(getTheme().TypographyPrimaryColor_withAlpha(1));
        }
    };
}