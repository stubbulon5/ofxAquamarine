
#pragma once
#include "../WidgetTable.h"

namespace Aquamarine
{
    class WidgetWidgetList : public WidgetTable
    {
    public:
        WidgetWidgetList(string persistentId, string widgetXML) : WidgetTable(persistentId, widgetXML)
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

            WidgetTableRow headerRow;
            std::vector<WidgetTableRow> tableRows;

            if (getLoadedWidgetsDisplayMode())
            {

                headerRow = WidgetTableRow("Header", {WidgetTableCell("widget", "Widget Id", 300),
                                                           WidgetTableCell("type", "Type")});

                for (Widget &widget : WidgetManager::getWidgets())
                {
                    bool found = false;
                    for (string widgetType : getAllowedWidgetClassTypes())
                    {
                        if (widgetType == widget.getWidgetClassType())
                        {
                            found = true;
                            break;
                        }
                    }

                    if (found || getAllowedWidgetClassTypes().size() == 0)
                    {
                        tableRows.push_back(
                            WidgetTableRow(widget.getWidgetId(), {WidgetTableCell(widget.getPersistentId(), widget.getPersistentId()),
                                                                       WidgetTableCell("", widget.getWidgetClassType())}));
                    }
                }
            }
            else
            {

                headerRow = WidgetTableRow("Header", {WidgetTableCell("widget", "Widget", 300),
                                                           WidgetTableCell("type", "Type")});

                WidgetManager::widget_map_type widgetMap = WidgetManager::getWidgetMap();

                for (auto it = widgetMap.begin(); it != widgetMap.end(); it++)
                {
                    string widgetClass = it->first;
                    string widgetName = widgetClass;
                    ofStringReplace(widgetName, "Widget", "");
                    if (widgetName == "")
                        widgetName = "Widget";

                    bool found = false;
                    for (string widgetType : getAllowedWidgetClassTypes())
                    {
                        if (widgetType == widgetClass)
                        {
                            found = true;
                            break;
                        }
                    }

                    if (found || getAllowedWidgetClassTypes().size() == 0)
                    {
                        tableRows.push_back(
                            WidgetTableRow(widgetClass, {WidgetTableCell("", widgetName),
                                                              WidgetTableCell("", "Widget")}));
                    }
                }

                Widget *w = WidgetManager::loadWidget(WidgetManager::WIDGET_CLASS, "tempWidget", "<widget><bounds width='100' height='100'/></widget>");
                w->mapElements();
                element_map_type elmClassMapTemp = w->elmClassMap;
                delete w;

                for (auto it = elmClassMapTemp.begin(); it != elmClassMapTemp.end(); it++)
                {
                    string elementClass = it->first;
                    string elementName = elementClass;
                    ofStringReplace(elementName, "Widget", "");
                    if (elementName == "")
                        elementName = "Element";

                    tableRows.push_back(
                        WidgetTableRow(elementClass, {WidgetTableCell("", elementName, 300),
                                                           WidgetTableCell("", "Element")}));
                }
            }

            setHeaderRow(headerRow);
            setTableRows(tableRows);
            return true;
        }

        virtual void setIsVisible(bool value) override
        {
            WidgetTable::setIsVisible(value);
            if (value)
            {
                setLoadedWidgetsDisplayMode(getLoadedWidgetsDisplayMode());
            }
        }

        virtual ~WidgetWidgetList() {
        };

        bool getLoadedWidgetsDisplayMode()
        {
            return mLoadedWidgetsDisplayMode;
        }

        void setLoadedWidgetsDisplayMode(bool val)
        {
            mLoadedWidgetsDisplayMode = val;
            loadState(getXML());
            setNeedsUpdate(true);
        }

        vector<string> getAllowedWidgetClassTypes()
        {
            return mAllowedWidgetClassTypes;
        }

        void setAllowedWidgetClassTypes(vector<string> value)
        {
            mAllowedWidgetClassTypes = value;
            loadState(getXML());
            setNeedsUpdate(true);
        }

    private:
        bool mLoadedWidgetsDisplayMode = false;
        vector<string> mAllowedWidgetClassTypes;

        void initWidget() override
        {
            setIsAutoResizeToContentsWidth(true);
            setIsAutoResizeToContentsHeight(false);
            setHeight(400);
            setWidth(400);
        }
    };
}