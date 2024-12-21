#pragma once
#include "uiVizWidgetSequencerBase.h"

namespace Aquamarine
{
    class uiVizWidgetSequencer : public uiVizWidgetSequencerBase
    {
    public:
        uiVizWidgetSequencer(string persistentId, string widgetXML) : uiVizWidgetSequencerBase(persistentId, widgetXML)
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

            setRowHeight(settings.getAttribute("properties", "rowHeight", 100));
            setFixedColWidth(settings.getAttribute("properties", "fixedColWidth", 4 * getRowHeight()));

            vector<uiVizWidgetTableCell> headerCells = vector<uiVizWidgetTableCell>();
            headerCells.push_back(
                uiVizWidgetTableCell(
                    "TRACK_KEY",
                    "",
                    240));

            setHeaderRow(uiVizWidgetTableRow("TRACK_KEY", headerCells));

            vector<string> tracks = {"Track 1", "Track 2", "Track 3"};

            vector<uiVizWidgetTableRow> rows = vector<uiVizWidgetTableRow>();

            for (string track : tracks)
            {

                uiVizWidgetTableRow row = uiVizWidgetTableRow(track, {uiVizWidgetTableCell(track, track)});

                rows.push_back(
                    row);
            }
            setTableRows(rows);

            for (int j = 1; j <= 100; j++)
            {
                addCol(uiVizWidgetTableCell("", ofToString(j)));
            }

            // TODO
            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = uiVizWidget::saveState();
            settings.setAttribute("widget", "class", uiVizWidgetManager::WIDGET_CLASS_SEQUENCER, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~uiVizWidgetSequencer() {
        };

        virtual void drawCellHeaderCol(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex)
        {

            uiVizElm vizElm_cell;
            vizElm_cell.setRectangle(
                absoluteScaledX,
                absoluteScaledY,
                scaledWidth,
                scaledHeight,
                getTheme().TitleColor_withAlpha(getStrongChromeAlpha()));
            vizElm_cell.draw();

            ofPushStyle();
            ofSetColor((isRowSelected || isRowHovered) ? getTheme().TypographyPrimaryColor_withAlpha(getTheme().HoveredTitleAlpha) : getTheme().TypographyPrimaryColor_withAlpha(getTheme().UnhoveredTitleAlpha));
            font->draw(cell.label, absoluteScaledLabelX, absoluteScaledLabelY);
            ofPopStyle();
        }
        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {
            uiVizWidgetSequencerBase::onWidgetEventReceived(args);
        }

    private:
        shared_ptr<ofxSmartFont> font;

        void initWidget() override
        {
            font = getViz()->getMediumFont();
        }
    };
}