
#pragma once
#include "uiVizWidgetTable.h"

namespace Aquamarine
{
    class uiVizWidgetMatrix : public uiVizWidgetTable
    {
    public:
        uiVizWidgetMatrix(string persistentId, string widgetXML) : uiVizWidgetTable(persistentId, widgetXML)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            uiVizWidgetTable::loadState(widgetXML);

            ofxXmlSettings settings = ofxXmlSettings();
            if (!settings.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            settings.pushTag("widget");

            setRowHeight(settings.getAttribute("properties", "rowHeight", 50));
            setFreezeHeaderRow(settings.getAttribute("properties", "freezeHeaderRow", true));
            setFreezeFirstColumn(settings.getAttribute("properties", "freezeFirstColumn", true));
            setAllowMultiSelect(settings.getAttribute("properties", "allowMultiSelect", true));
            setShowColumnFilters(settings.getAttribute("properties", "showColumnFilters", false), false);
            setSelectionMode(settings.getAttribute("properties", "selectionMode", "CELL"));
            setEnableColSnapScrolling(settings.getAttribute("properties", "enableColSnapScrolling", false));
            setFixedColWidth(settings.getAttribute("properties", "fixedColWidth", 50));
            setFixedHeaderRowHeight(settings.getAttribute("properties", "fixedHeaderRowHeight", 0));

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = uiVizWidget::saveState();
            settings.setAttribute("widget", "class", uiVizWidgetManager::WIDGET_CLASS_MATRIX, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~uiVizWidgetMatrix() {
        };

        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {
            uiVizWidgetTable::onWidgetEventReceived(args);
        }

    private:
        shared_ptr<ofxSmartFont> font;

        void initWidget() override
        {
            setTableUseFbo(true);
        }
    };
}