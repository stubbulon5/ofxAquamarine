//
//  uiVizWidgetTable.h
//  bloom-macos
//
//
#pragma once
#include "../uiVizWidgetTable.h"

namespace Aquamarine
{
    class uiVizWidgetWidgetList : public uiVizWidgetTable
    {
    public:
        uiVizWidgetWidgetList(string persistentId, string widgetXML) : uiVizWidgetTable(persistentId, widgetXML)
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

            uiVizWidgetTableRow headerRow;
            std::vector<uiVizWidgetTableRow> tableRows;

            if (getLoadedWidgetsDisplayMode())
            {

                headerRow = uiVizWidgetTableRow("Header", {uiVizWidgetTableCell("widget", "Widget Id", 300),
                                                           uiVizWidgetTableCell("type", "Type")});

                for (uiVizWidget &widget : uiVizWidgetManager::getWidgets())
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
                            uiVizWidgetTableRow(widget.getWidgetId(), {uiVizWidgetTableCell(widget.getPersistentId(), widget.getPersistentId()),
                                                                       uiVizWidgetTableCell("", widget.getWidgetClassType())}));
                    }
                }
            }
            else
            {

                headerRow = uiVizWidgetTableRow("Header", {uiVizWidgetTableCell("widget", "Widget", 300),
                                                           uiVizWidgetTableCell("type", "Type")});

                uiVizWidgetManager::widget_map_type widgetMap = uiVizWidgetManager::getWidgetMap();

                for (auto it = widgetMap.begin(); it != widgetMap.end(); it++)
                {
                    string widgetClass = it->first;
                    string widgetName = widgetClass;
                    ofStringReplace(widgetName, "uiVizWidget", "");
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
                            uiVizWidgetTableRow(widgetClass, {uiVizWidgetTableCell("", widgetName),
                                                              uiVizWidgetTableCell("", "Widget")}));
                    }
                }

                uiVizWidget *w = uiVizWidgetManager::loadWidget(uiVizWidgetManager::WIDGET_CLASS, "tempWidget", "<widget><bounds width='100' height='100'/></widget>");
                w->mapElements();
                element_map_type elmClassMapTemp = w->elmClassMap;
                delete w;

                for (auto it = elmClassMapTemp.begin(); it != elmClassMapTemp.end(); it++)
                {
                    string elementClass = it->first;
                    string elementName = elementClass;
                    ofStringReplace(elementName, "uiVizWidget", "");
                    if (elementName == "")
                        elementName = "Element";

                    tableRows.push_back(
                        uiVizWidgetTableRow(elementClass, {uiVizWidgetTableCell("", elementName, 300),
                                                           uiVizWidgetTableCell("", "Element")}));
                }
            }

            setHeaderRow(headerRow);
            setTableRows(tableRows);
            return true;
        }

        virtual void setIsVisible(bool value) override
        {
            uiVizWidgetTable::setIsVisible(value);
            if (value)
            {
                setLoadedWidgetsDisplayMode(getLoadedWidgetsDisplayMode());
            }
        }

        virtual ~uiVizWidgetWidgetList() {
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