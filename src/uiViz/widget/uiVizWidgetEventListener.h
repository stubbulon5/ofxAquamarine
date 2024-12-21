
#pragma once
#include "uiVizWidget.h"
#include "uiVizWidgetTable.h"
#include "uiVizWidgetMenuCollection.h"
#include "uiVizWidgetWidgetList.h"

namespace Aquamarine
{
    class uiVizWidgetEventListener : public uiVizWidget
    {

    public:
        uiVizWidgetEventListener(string persistentId, string widgetXML) : uiVizWidget(persistentId, widgetXML)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual ~uiVizWidgetEventListener()
        {

            ofRemoveListener(widgetList->widgetEventReceived, this, &uiVizWidgetEventListener::onWidgetListEventReceived);
            ofRemoveListener(eventListenLog->widgetEventReceived, this, &uiVizWidgetEventListener::onEventListenLogEventReceived);

            eventListenLog->deleteThisWidget();
            eventsTableSource->deleteThisWidget();
            eventsTableTarget->deleteThisWidget();
            tabs->deleteThisWidget();

            if (contextMenu)
                ofRemoveListener(contextMenu->menuItemSelected, this, &uiVizWidgetEventListener::onMenuItemSelected);

            // do more cleanup here
        }

        virtual void update(uiVizWidgetContext context) override
        {
        }

        virtual void onWidgetWasJustInitialized() override
        {
            populateEventListenerTable();
        }

        virtual void draw(uiVizWidgetContext context) override
        {
            if (btnAdd != nullptr)
                btnAdd->setIsVisible(tabs->getSelectedTag() == "LISTENER_CONFIG");
        }

        virtual void onMenuItemSelected(uiVizWidgetMenuItemArgs &args)
        {
            uiVizWidget *owner = getOwnerWidget();
            if (!owner)
                return;

            switch (args.activeMenuTabId)
            {

            case MENU_GROUP_1::MENU_TAB_SETTINGS:

                if (args.menuItem->uniqueID == 5)
                {
                    // SAVE AS PNG ...

                    uiVizWidgetManager::showSaveDialog("", extractFileSystemSafeName(owner->getTitle() + ".png"),
                                                       [&, owner](string filePath)
                                                       {
                                                           owner->saveWidgetContentsToImageFile(filePath, uiVizWidgetContext::SCREEN);
                                                       });
                }
                else if (args.menuItem->uniqueID == 6)
                {
                    // SAVE AS XML ...
                    uiVizWidgetManager::showSaveDialog("", extractFileSystemSafeName(owner->getTitle() + ".xml"),
                                                       [&, owner](string filePath)
                                                       {
                                                           owner->saveWidgetXMLToFile(filePath);
                                                       });
                }

                setNeedsUpdate(true);
                setWidgetNeedsUpdate(true);
                break;
            }
        }

        virtual void addToEventListenLog(string source, string eventName, string redirectEventName)
        {
            uiVizWidget *owner = getOwnerWidget();
            if (!owner)
                return;

            getViz()->addToWidgetEventListenMap(
                owner->getPersistentId(),
                source,
                eventName,
                redirectEventName,
                true);

            populateEventListenerTable();
        }

        virtual void removeFromEventListenLog(string source, string eventName, string redirectEventName)
        {
            uiVizWidget *owner = getOwnerWidget();
            if (!owner)
                return;

            getViz()->removeFromWidgetEventListenMap(
                owner->getPersistentId(),
                source,
                eventName,
                redirectEventName);

            populateEventListenerTable();
        }

        uiVizWidget *getOwnerWidget()
        {
            uiVizWidget *w = uiVizWidgetManager::getWidget(getOwnerWidgetId());
            return w;
        }

        virtual void populateEventListenerTable()
        {
            uiVizWidget *owner = getOwnerWidget();
            if (!owner)
                return;

            eventListenLog->clearTableRows();

            vector<uiViz::WidgetEvent> listeningEvents = getViz()->getListeningEvents(owner->getPersistentId());
            for (uiViz::WidgetEvent e : listeningEvents)
            {
                eventListenLog->addRow(
                    uiVizWidgetTableRow("HEADER", {uiVizWidgetTableCell(
                                                       e.eventSenderId,
                                                       e.eventSenderId),
                                                   uiVizWidgetTableCell(
                                                       "Event Name",
                                                       e.eventName),
                                                   uiVizWidgetTableCell(
                                                       "Receive to Event",
                                                       e.redirectEventName),
                                                   uiVizWidgetTableCell(
                                                       "DELETE",
                                                       "DELETE",
                                                       60)}));
            }
            eventListenLog->setNeedsUpdate(true);
        }

        virtual void populateEventsTableSource()
        {
            eventsTableSource->clearTableRows();

            for (string e : mSourceEventsList)
            {
                eventsTableSource->addRow(
                    uiVizWidgetTableRow("HEADER", {uiVizWidgetTableCell(
                                                      e,
                                                      e)}));
            }

            eventsTableSource->setNeedsUpdate(true);
            setNeedsUpdate(true);
        }

        virtual void populateEventsTableTarget()
        {
            eventsTableTarget->clearTableRows();

            for (string e : mTargetEventsList)
            {
                eventsTableTarget->addRow(
                    uiVizWidgetTableRow("HEADER", {uiVizWidgetTableCell(
                                                      e,
                                                      e)}));
            }
            eventsTableTarget->setNeedsUpdate(true);
            setNeedsUpdate(true);
        }

        virtual void onWidgetListEventReceived(uiVizWidgetEventArgs &args)
        {

            if (args.eventName == WIDGET_EVENT::TABLE_CELL_CLICKED)
            {

                uiVizWidgetTableRow *eventRow = widgetList->getTableRow(args.eventXML);
                uiVizWidgetTableCell *eventCell = widgetList->getTableCell(args.eventXML);

                string selectedRootPath = eventRow->key;
                string selectedRootTitle = eventCell->label;

                addToEventListenLog(eventCell->key, "...", "");

                widgetList->setIsVisible(false);
                uiVizWidgetManager::clearAllWidgetHighlights();
            }
            else if (args.eventName == WIDGET_EVENT::TABLE_CELL_HOVERED)
            {

                uiVizWidgetTableRow *eventRow = widgetList->getTableRow(args.eventXML);
                uiVizWidgetTableCell *eventCell = widgetList->getTableCell(args.eventXML);

                uiVizWidgetManager::highlightWidget(eventRow->cells[0].key, true, true);
            }
        }

        virtual void onEventListenLogEventReceived(uiVizWidgetEventArgs &args)
        {
            if (args.eventName == WIDGET_EVENT::TABLE_CELL_CLICKED)
            {

                uiVizWidgetTableRow *eventRow = eventListenLog->getTableRow(args.eventXML);
                uiVizWidgetTableCell *eventCell = eventListenLog->getTableCell(args.eventXML);

                string selectedRootPath = eventRow->key;
                string selectedRootTitle = eventCell->label;

                if (eventCell->key == "DELETE")
                {
                    removeFromEventListenLog(eventRow->cells[0].label, eventRow->cells[1].label, eventRow->cells[2].label);
                }
            }
        }

        virtual void setSourceEventsList(vector<string> eventNames)
        {
            mSourceEventsList = eventNames;
            populateEventsTableSource();
        }

        virtual vector<string> getSourceEventsList()
        {
            return mSourceEventsList;
        }

        virtual void setTargetEventsList(vector<string> eventNames)
        {
            mTargetEventsList = eventNames;
            populateEventsTableTarget();
        }

        virtual vector<string> getTargetEventsList()
        {
            return mTargetEventsList;
        }

        static vector<string> getAllWidgetEvents()
        {
            return {
                WIDGET_EVENT::RESIZED,
                WIDGET_EVENT::DRAGGED,
                WIDGET_EVENT::HOVERED,
                WIDGET_EVENT::UNHOVERED,
                WIDGET_EVENT::CLICK,
                WIDGET_EVENT::CLIPBOARD_CUT,
                WIDGET_EVENT::CLIPBOARD_COPY,
                WIDGET_EVENT::CLIPBOARD_PASTE,
                WIDGET_EVENT::FOCUS_ELEMENT,
                WIDGET_EVENT::FOCUS_NEXT_ELEMENT,
                WIDGET_EVENT::VALUE_CHANGED,
                WIDGET_EVENT::TABLE_CELL_WIDGET_INITIALIZED,
                WIDGET_EVENT::TABLE_CELL_CLICKED,
                WIDGET_EVENT::TABLE_CELL_SELECTED,
                WIDGET_EVENT::TABLE_CELL_UNSELECTED,
                WIDGET_EVENT::TABLE_CELL_HOVERED,
                WIDGET_EVENT::RECEIVE_DRAG_AND_DROP_DATA,
                WIDGET_EVENT::DRAG_AND_DROP_DATA_SET};
        }

        void setAllowedWidgetClassTypes(vector<string> value)
        {
            if (widgetList != nullptr)
            {
                widgetList->setAllowedWidgetClassTypes(value);
            }
        }

        uiVizWidgetMenuCollection *getTabsComponent()
        {
            return tabs;
        }

        uiVizWidgetMenu *getSettingsMenu()
        {
            return contextMenu;
        }

    private:
        uiVizWidgetMenuCollection *tabs;
        uiVizWidgetTable *eventListenLog = nullptr;
        uiVizWidgetWidgetList *widgetList = nullptr;
        uiVizWidgetElmButton *btnAdd = nullptr;
        uiVizIcon iconClose, iconAdd;
        uiVizWidgetTable *sourceEvent = nullptr;
        uiVizWidgetTable *targetEvent = nullptr;
        vector<string> mSourceEventsList, mTargetEventsList;
        uiVizWidgetTable *eventsTableSource = nullptr;
        uiVizWidgetTable *eventsTableTarget = nullptr;
        uiVizWidgetMenu *contextMenu = nullptr;

        enum MENU_GROUP_1
        {
            MENU_TAB_SETTINGS = 8,
        };

        bool loaded = false;

        void handleEventSelection(uiVizWidgetTableArgs &args)
        {
            vector<uiVizWidgetTableRow> selectedRows = eventListenLog->getSelectedRows();
            vector<int> rowIndexes = eventListenLog->getSelectedRowIndexes();

            if (selectedRows.size() > 0)
            {
                uiVizWidgetTableRow row = selectedRows[0];
                for (int i = 0; i < row.cells.size(); i++)
                {
                    uiVizWidgetTableCell cell = row.cells.at(i);
                    if (cell.isSelected)
                    {

                        uiVizWidgetTableCell cell(args.cell.key, args.cell.key);
                        uiVizWidgetTableRow updatedRow = eventListenLog->updateCell(rowIndexes[0], i, cell);

                        uiVizWidget *owner = getOwnerWidget();
                        if (!owner)
                            return;

                        getViz()->updateWidgetEventListenMap(
                            owner->getPersistentId(),
                            updatedRow.cells[0].label,
                            updatedRow.cells[1].label,
                            updatedRow.cells[2].label,
                            // Old vals ...
                            row.cells[1].label,
                            row.cells[2].label);
                    }
                }
            }

            eventListenLog->clearSelectedRows();
            args.sender.closeThisWidget();
        }

        static string extractFileSystemSafeName(string theoryArtifactName)
        {
            string illegalChars = "\\/:?\"<>|[]";
            for (auto it = theoryArtifactName.begin(); it < theoryArtifactName.end(); ++it)
            {
                bool found = illegalChars.find(*it) != string::npos;
                if (found)
                {
                    *it = ' ';
                }
            }
            return theoryArtifactName;
        }

        void initWidget() override
        {

            mSourceEventsList = uiVizWidgetEventListener::getAllWidgetEvents();
            mTargetEventsList = uiVizWidgetEventListener::getAllWidgetEvents();

            iconClose = uiVizIconCache::getIcon("REG_WIDGET_CLOSE");
            iconAdd = uiVizIconCache::getIcon("MED_ADD_CIRCLE");

            if (!loaded)
            {

                eventListenLog = new uiVizWidgetTable("eventListenLog", R"(
                <widget><bounds xExpr="${PARENT.ABSOLUTE_USABLE_X}" yExpr="${PARENT.ABSOLUTE_USABLE_Y}+${TABS.HEIGHT}" widthExpr="${PARENT.USABLE_WIDTH}" heightExpr="${PARENT.USABLE_HEIGHT}-${BTN_ADD.HEIGHT}-${TABS.HEIGHT}-${PADDING}" />
                    <properties autoResizeHeight="0" autoResizeWidth="0">
                        <header>
                            <cell width="40%">Sender</cell><cell width="30%">Event Name</cell><cell width="30%">Receive to Event</cell><cell></cell>
                        </header>
                    </properties>
                </widget>
                )");
                eventListenLog->setIsRoundedRectangle(false);
                ofAddListener(eventListenLog->widgetEventReceived, this, &uiVizWidgetEventListener::onEventListenLogEventReceived);
                eventListenLog->setSelectionMode(uiVizWidgetTable::SelectionMode::CELL);

                // Events table (sourcE)
                eventsTableSource = new uiVizWidgetTable("eventsTableSource", R"(
                <widget isSystemWidget="1"><bounds width="200" height="250" />
                    <properties autoResizeHeight="0" autoResizeWidth="0" showColumnFilters="0">
                        <header>
                            <cell width="100%">Event</cell>
                        </header>
                    </properties>
                </widget>
                )");

                uiVizWidgetManager::addWidgetForPopout(*eventsTableSource, getWidgetId(), true);
                populateEventsTableSource();

                // Events table (target)
                eventsTableTarget = new uiVizWidgetTable("eventsTableTarget", R"(
                <widget isSystemWidget="1"><bounds width="200" height="250" />
                    <properties autoResizeHeight="0" autoResizeWidth="0" showColumnFilters="0">
                        <header>
                            <cell width="100%">Event</cell>
                        </header>
                    </properties>
                </widget>
                )");

                uiVizWidgetManager::addWidgetForPopout(*eventsTableTarget, getWidgetId(), true);
                populateEventsTableTarget();

                eventsTableSource->setFunction_Cell_Selected([this](uiVizWidgetTableArgs &args)
                                                             { handleEventSelection(args); });

                eventsTableTarget->setFunction_Cell_Selected([this](uiVizWidgetTableArgs &args)
                                                             { handleEventSelection(args); });

                eventListenLog->setFunction_Cell_Selected([this](uiVizWidgetTableArgs &args)
                                                          {
                ofPoint popoutPos(deScale(args.cell.cellSlice.bounds.x) + (deScale(args.cell.cellSlice.bounds.width / 2)), deScale(args.cell.cellSlice.bounds.y));
                if (args.cellIndex == 1) {
                    eventsTableSource->popoutFrom(popoutPos.x, popoutPos.y, PopoutDirection::UP);
                } else if (args.cellIndex == 2) {
                    eventsTableTarget->popoutFrom(popoutPos.x, popoutPos.y, PopoutDirection::UP);
                } });

                eventListenLog->setFunction_Cell_Hovered([this](uiVizWidgetTableArgs &args)
                                                         { uiVizWidgetManager::highlightWidget(args.row.cells[0].key, true, true); });

                eventListenLog->setFunction_DrawCellContent([this](bool isRowSelected, bool isRowHovered, bool isCellHovered, int absoluteScaledX, int absoluteScaledY, int absoluteScaledLabelX, int absoluteScaledLabelY, int scaledWidth, int scaledHeight, uiVizWidgetTableRow &row, uiVizWidgetTableCell &cell, int rowIndex, int colIndex)
                                                            {
                
                if (cell.key == "DELETE") {
                    iconClose.setScaledPos(
                        absoluteScaledX+(scaledWidth-iconClose.getScaledBounds().width)/2.0f, 
                        absoluteScaledY+(scaledHeight-iconClose.getScaledBounds().height)/2.0f
                    );
                    uiVizElm vizElm_cell;
                    vizElm_cell.setRectangle(absoluteScaledX, absoluteScaledY, scaledWidth, scaledHeight);
                    if(vizElm_cell.isHovered_Rect()) {
                        vizElm_cell.setColor(ofColor(255, 0, 0, 180));
                        vizElm_cell.draw();                
                        iconClose.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                    } else {             
                        iconClose.setColor(getTheme().TypographyPrimaryColor_withAlpha(0.7f));
                    }
                    iconClose.drawSvg();
                } else if ((colIndex == 1 || colIndex == 2) && isCellHovered) { // select ...
                    iconAdd.setScaledPos(
                        absoluteScaledX+(scaledWidth-iconAdd.getScaledBounds().width)/2.0f, 
                        absoluteScaledY+(scaledHeight-iconAdd.getScaledBounds().height)/2.0f
                    );
                    
                    uiVizElm vizElm_cell;
                    vizElm_cell.setRectangle(absoluteScaledX, absoluteScaledY, scaledWidth, scaledHeight);
                    if(vizElm_cell.isHovered_Rect()) {
                        vizElm_cell.setColor(ofColor(0, 255, 0, 180));
                        vizElm_cell.draw();                
                        iconAdd.setColor(getTheme().TypographyPrimaryColor_withAlpha(1));
                    } else {             
                        iconAdd.setColor(getTheme().TypographyPrimaryColor_withAlpha(0.7f));
                    }
                    iconAdd.drawSvg();
                }
                else {
                    eventListenLog->drawCellContent(isRowSelected, isRowHovered, isCellHovered, absoluteScaledX, absoluteScaledY, absoluteScaledLabelX, absoluteScaledLabelY, scaledWidth, scaledHeight, row, cell, rowIndex, colIndex);                        
                } });

                contextMenu = new uiVizWidgetMenu(getPersistentId() + "_SETTINGS_MENU", "<widget><bounds width='100' height='300'/></widget>", getWidgetId(),
                                                  uiVizWidgetMenu::PreferredPopoutDirection::DOWN, {uiVizWidgetMenuTab(uiVizShared::lang("SETTINGS"), uiVizIconCache::getIcon("MED_CONTENT_SETTINGS"), MENU_GROUP_1::MENU_TAB_SETTINGS, {uiVizWidgetMenuItem(uiVizShared::lang("SOUND"), 1, true, true), uiVizWidgetMenuItem(uiVizShared::lang("LEFT_HAND"), 2, true, false), uiVizWidgetMenuItem(uiVizShared::lang("FLIP_ORIENTATION"), 3, true, false), uiVizWidgetMenuItem(uiVizShared::lang("SWITCH_VIEW"), 4, true, false), uiVizWidgetMenuItem(uiVizShared::lang("EXPORT_PNG"), 5), uiVizWidgetMenuItem(uiVizShared::lang("EXPORT_XML"), 6)})

                                                                                                   });

                contextMenu->setIsMenuPinned(true);

                ofAddListener(dynamic_cast<uiVizWidgetMenu *>(contextMenu)->menuItemSelected, this, &uiVizWidgetEventListener::onMenuItemSelected);

                tabs = new uiVizWidgetMenuCollection("TABS", R"(
                <widget>
                    <bounds xExpr="${PARENT.ABSOLUTE_USABLE_X}" yExpr="${PARENT.ABSOLUTE_USABLE_Y}" widthExpr="${PARENT.USABLE_WIDTH}"  />
                    <properties menuType="TAB" />
                </widget>
            )");

                tabs->setIsRoundedRectangle(false);
                tabs->setTabMenuCollection(this, {uiVizWidgetMenuCollectionItem("SETTINGS", contextMenu, "Settings", uiVizIconCache::getIcon("REG_BUG")),
                                                  uiVizWidgetMenuCollectionItem("LISTENER_CONFIG", eventListenLog, "Listener config", uiVizIconCache::getIcon("REG_BUG"))});

                tabs->setItemHeightExpression("${PARENT.USABLE_HEIGHT}-${BTN_ADD.HEIGHT}-${TABS.HEIGHT}-${PADDING}");
                addChildWidget(*tabs, true);
                tabs->setIsVisible(false);

                widgetList = new uiVizWidgetWidgetList(getPersistentId() + "_WIDGET_LIST", R"(
                <widget>
                    <bounds width="200" height="250" />
                </widget>
                )");

                uiVizWidgetManager::addWidgetForPopout(*widgetList, getWidgetId(), true);
                widgetList->setTheme(getViz()->getThemeManager()->getSystemThemeLight(true));
                widgetList->setRowBGColor(ofColor(0, 0, 0, 0));
                widgetList->setRowAltBGColor(ofColor(0, 0, 0, 0));
                widgetList->setLoadedWidgetsDisplayMode(true);
                ofAddListener(widgetList->widgetEventReceived, this, &uiVizWidgetEventListener::onWidgetListEventReceived);

                btnAdd = dynamic_cast<uiVizWidgetElmButton *>(addOrLoadWidgetElement(btnAdd, WIDGET_ELM_CLASS::BUTTON, "BTN_ADD", R"(
                <element>
                <bounds xExpr="${PARENT.RIGHT}" yExpr="${PARENT.BOTTOM}" />
                </element>
                )"));

                btnAdd->setTitle("Add...");
                btnAdd->setPopoutWidget(widgetList, PopoutDirection::UP);

                setWidth_Expr("${PARENT.USABLE_WIDTH}");
                setHeight_Expr("${PARENT.USABLE_HEIGHT}");
                loaded = true;
            }
        }
    };

}