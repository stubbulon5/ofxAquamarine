
#pragma once
#include "WidgetTable.h"

namespace Aquamarine
{
    class WidgetMatrix : public WidgetTable
    {
    public:
        WidgetMatrix(string persistentId, string widgetXML) : WidgetTable(persistentId, widgetXML)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            WidgetTable::loadState(widgetXML);

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
            ofxXmlSettings settings = Widget::saveState();
            settings.setAttribute("widget", "class", WidgetManager::WIDGET_CLASS_MATRIX, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~WidgetMatrix() {
        };

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
            WidgetTable::onWidgetEventReceived(args);
        }

    private:
        shared_ptr<ofxSmartFont> font;

        void initWidget() override
        {
            setTableUseFbo(true);
        }
    };
}