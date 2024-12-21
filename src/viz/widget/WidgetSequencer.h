#pragma once
#include "WidgetSequencerBase.h"

namespace Aquamarine
{
    class WidgetSequencer : public WidgetSequencerBase
    {
    public:
        WidgetSequencer(string persistentId, string widgetXML) : WidgetSequencerBase(persistentId, widgetXML)
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

            vector<WidgetTableCell> headerCells = vector<WidgetTableCell>();
            headerCells.push_back(
                WidgetTableCell(
                    "TRACK_KEY",
                    "",
                    240));

            setHeaderRow(WidgetTableRow("TRACK_KEY", headerCells));

            vector<string> tracks = {"Track 1", "Track 2", "Track 3"};

            vector<WidgetTableRow> rows = vector<WidgetTableRow>();

            for (string track : tracks)
            {

                WidgetTableRow row = WidgetTableRow(track, {WidgetTableCell(track, track)});

                rows.push_back(
                    row);
            }
            setTableRows(rows);

            for (int j = 1; j <= 100; j++)
            {
                addCol(WidgetTableCell("", ofToString(j)));
            }

            // TODO
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

        virtual ~WidgetSequencer() {
        };

        virtual void drawCellHeaderCol(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, WidgetTableRow &row, WidgetTableCell &cell, int rowIndex)
        {

            Elm vizElm_cell;
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
        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
            WidgetSequencerBase::onWidgetEventReceived(args);
        }

    private:
        shared_ptr<ofxSmartFont> font;

        void initWidget() override
        {
            font = getViz()->getMediumFont();
        }
    };
}