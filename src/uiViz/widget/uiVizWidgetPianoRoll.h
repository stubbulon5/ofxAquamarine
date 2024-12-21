
#pragma once
#include "uiVizWidgetSequencerBase.h"

namespace Aquamarine
{
    class uiVizWidgetPianoRoll : public uiVizWidgetSequencerBase
    {
    public:
        uiVizWidgetPianoRoll(string persistentId, string widgetXML) : uiVizWidgetSequencerBase(persistentId, widgetXML)
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

            setFixedHeaderRowHeight(settings.getAttribute("properties", "pianoKeyWidth", 30));

            vector<uiVizWidgetTableCell> headerCells = vector<uiVizWidgetTableCell>();
            headerCells.push_back(
                uiVizWidgetTableCell(
                    "PIANO_KEY",
                    "",
                    150));

            setHeaderRow(uiVizWidgetTableRow("PIANO_KEY", headerCells));

            vector<string> notes = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

            vector<uiVizWidgetTableRow> rows = vector<uiVizWidgetTableRow>();
            int octave = 0;
            for (int keys = 0; keys <= 87; keys++)
            {
                int keyNoteNum = keys % 12;
                string keyNote = notes[keyNoteNum];
                if (keyNote == "C")
                    octave++;

                uiVizWidgetTableRow row = uiVizWidgetTableRow(keyNote, {uiVizWidgetTableCell(keyNote + ofToString(octave), keyNote + ofToString(octave))});
                row.rowSlice.regularColor = ofColor::fromHsb(getDefaultScaleDegreeHueChromatic()[keyNoteNum], 255, 255);

                rows.push_back(
                    row);
            }
            setTableRows(rows);

            for (int j = 1; j <= 100; j++)
            {
                addCol(uiVizWidgetTableCell("", ofToString(j)));
            }

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        static vector<float> getDefaultScaleDegreeHueChromatic()
        {
            return {
                // Take from chromatic scale colors derived off hue circle degress
                /* C */ 0, /* C# */ 21.25, /* D */ 42.5, /* D# */ 63.75,
                /* E */ 85, /* F */ 106.25, /* F# */ 127.5, /* G */ 148.75,
                /* G# */ 170, /* A */ 191.25, /* A# */ 212.5, /* B */ 233.75};
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = uiVizWidget::saveState();
            settings.setAttribute("widget", "class", uiVizWidgetManager::WIDGET_CLASS_PIANO_ROLL, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~uiVizWidgetPianoRoll() {
        };

        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {
            uiVizWidgetSequencerBase::onWidgetEventReceived(args);
        }

        virtual void drawCellHeaderCol(bool isRowSelected, bool isRowHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex)
        {

            bool isBlackKey = !(row.key.size() == 1);

            ofColor keyColor = isBlackKey ? ofColor::black : ofColor::white;

            if (isRowHovered)
                keyColor = row.rowSlice.regularColor;

            uiVizElm vizElm_cell;
            vizElm_cell.setRectangle(
                absoluteScaledX,
                absoluteScaledY,
                isBlackKey ? scaledWidth * 0.76f : scaledWidth,
                isBlackKey ? scaledHeight : scaledHeight * 1.5f,
                keyColor);
            vizElm_cell.draw();

            if (isBlackKey)
            {
                // Little filler blocks...
                vizElm_cell.setRectangle(
                    absoluteScaledX + scaledWidth * 0.76f,
                    absoluteScaledY,
                    scaledWidth * 0.24f,
                    scaledHeight * 0.5f,
                    ofColor::white // or prev white key color
                );
                vizElm_cell.draw();

                vizElm_cell.setRectangle(
                    absoluteScaledX + scaledWidth * 0.76f,
                    absoluteScaledY + scaledHeight * 0.5f,
                    scaledWidth * 0.24f,
                    scaledHeight * 0.5f,
                    ofColor::white // or next white key color
                );
                vizElm_cell.draw();
            }

            if (row.key == "B" || row.key == "E")
            { // Border between keys
                vizElm_cell.setRectangle(
                    absoluteScaledX,
                    absoluteScaledY + scaledHeight - scale(2),
                    scaledWidth - scale(2),
                    scale(2),
                    borderColor);
                vizElm_cell.draw();
            }

            ofColor oppositeColor = isBlackKey ? ofColor::white : ofColor::black;
            float alpha = ((isRowSelected || isRowHovered) ? getTheme().HoveredTitleAlpha : getTheme().UnhoveredTitleAlpha) * 255.0f;
            ofColor color = ofColor(oppositeColor.r, oppositeColor.g, oppositeColor.b, alpha);

            ofPushStyle();
            ofSetColor(color);
            font->draw(cell.label, absoluteScaledLabelX, absoluteScaledLabelY);
            ofPopStyle();
        }

        /********* Override this to draw your own content *********/
        virtual void drawCellContent(bool isRowSelected, bool isRowHovered, bool isCellHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex, int colIndex) override
        {
            uiVizWidgetSequencerBase::drawCellContent(isRowSelected, isRowHovered, isCellHovered, absoluteScaledX, absoluteScaledY, absoluteScaledLabelX, absoluteScaledLabelY, scaledWidth, scaledHeight, row, cell, rowIndex, colIndex);
        }

        virtual void afterRowsDrawn()
        {
            // Vertical border line

            uiVizElm vizElm_cell;
            uiVizCoord::vizBounds headerColBounds = getHeaderColBounds();

            vizElm_cell.setRectangle(
                headerColBounds.x + headerColBounds.width - scale(2),
                headerColBounds.y,
                scale(2),
                headerColBounds.height,
                borderColor);
            vizElm_cell.draw();
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
            font = getViz()->getMediumFont();
            setTableUseFbo(true);
            borderColor = ofColor(0, 0, 0, 255 * 0.05f);
        }
    };
}