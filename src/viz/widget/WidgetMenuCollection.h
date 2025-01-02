
#pragma once
#include "Widget.h"
#include "WidgetMenu.h"

namespace Aquamarine
{

    class WidgetMenuCollectionItem
    {
    public:
        string tag = "";
        Widget *menu;
        string widgetPersistentId = "";
        string label = "";
        Icon icon;
        Widget::DisplayOptions displayOptions =
            Widget::DisplayOptions(
                Widget::DisplayType::MENU,
                Widget::HorizontalAlign::LEFT,
                Widget::VerticalAlign::CENTER,
                Widget::NonScaledPadding(0, getDefaultNonScaledPadding(), 0, getDefaultNonScaledPadding()));

        bool isSelected = false;
        bool requiresAttention = false;
        bool isVisible = true;

        // Ideally, use an instance of WidgetMenu :)
        WidgetMenuCollectionItem(string tag, Widget *menu, string label, Icon icon) : tag(tag), menu(menu), label(label), icon(icon) { init(); }
        WidgetMenuCollectionItem(string tag, Widget *menu, string label) : tag(tag), menu(menu), label(label) { init(); }
        WidgetMenuCollectionItem(string tag, Widget *menu, Icon icon) : tag(tag), menu(menu), icon(icon) { init(); }

        WidgetMenuCollectionItem(string tag, Widget *menu, string label, Icon icon, Widget::DisplayOptions displayOptions) : tag(tag), menu(menu), label(label), icon(icon), displayOptions(displayOptions) { init(); }
        WidgetMenuCollectionItem(string tag, Widget *menu, string label, Widget::DisplayOptions displayOptions) : tag(tag), menu(menu), label(label), displayOptions(displayOptions) { init(); }
        WidgetMenuCollectionItem(string tag, Widget *menu, Icon icon, Widget::DisplayOptions displayOptions) : tag(tag), menu(menu), icon(icon), displayOptions(displayOptions) { init(); }

        int getDefaultNonScaledPadding()
        {
            return 10;
        }

        void init()
        {
            if (displayOptions.nonScaledPadding.left == 0)
                displayOptions.nonScaledPadding.left = getDefaultNonScaledPadding();
            if (displayOptions.nonScaledPadding.right == 0)
                displayOptions.nonScaledPadding.right = getDefaultNonScaledPadding();
        }

        ~WidgetMenuCollectionItem()
        {
        }
    };

    class WidgetMenuCollectionItemSlice
    {
    public:
        WidgetMenuCollectionItem collectionItem = WidgetMenuCollectionItem("", nullptr, "");
        Coord::vizPoint labelPoint = Coord::vizPoint(0, 0);
        Coord::vizBounds bounds = Coord::vizBounds(0, 0, 0, 0);
        ofColor regularColor;
        ofColor selectedColor;
        ofColor hoveredColor;
        ofColor regularFontColor;
        ofColor selectedFontColor;
        ofColor hoveredFontColor;
        ofColor disabledFontColor;
        bool isEnabled = true;

        float currentAlpha = 0.0f;

        bool isHovered()
        {
            int mX = ofGetMouseX();
            int mY = ofGetMouseY();
            return mX > bounds.x && mY > bounds.y && mX < bounds.x + bounds.width && mY < bounds.y + bounds.height;
        }

        WidgetMenuCollectionItemSlice() {};

        ~WidgetMenuCollectionItemSlice()
        {
        }
    };

    class WidgetMenuCollectionArgs : public ofEventArgs
    {
    public:
        WidgetMenuCollectionArgs(string menuTag, WidgetMenu *menu, int activeMenuTabId, WidgetMenuItem *menuItem) : menuTag(menuTag), menu(menu), activeMenuTabId(activeMenuTabId), menuItem(menuItem) {}
        string menuTag = "";
        WidgetMenu *menu;
        int activeMenuTabId;
        WidgetMenuItem *menuItem;
    };

    class WidgetMenuCollection : public Widget
    {
    public:
        ofEvent<WidgetMenuCollectionArgs> menuItemSelected;
        ofEvent<WidgetMenuCollectionArgs> menuItemUnselected;

        WidgetMenuCollection(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
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
            setMenuType(getMenuTypeEnum(settings.getAttribute("properties", "menuType", "MENU")));

            if (getMenuType() == MenuType::TAB)
            {
                setItemXExpression(settings.getAttribute("properties", "itemXExpression", getItemXExpression()));
                setItemYExpression(settings.getAttribute("properties", "itemYExpression", getItemYExpression()));
                setItemWidthExpression(settings.getAttribute("properties", "itemWidthExpression", getItemWidthExpression()));
                setItemHeightExpression(settings.getAttribute("properties", "itemHeightExpression", getItemHeightExpression()));
            }

            settings.pushTag("properties");

            vector<WidgetMenuCollectionItem> items = vector<WidgetMenuCollectionItem>();
            if (settings.pushTag("items"))
            {
                int numItems = settings.getNumTags("item");
                for (int i = 0; i < numItems; i++)
                {
                    string tag = settings.getAttribute("item", "tag", "TAG_" + ofToString(i), i);
                    string label = settings.getAttribute("item", "label", "", i);
                    string icon = settings.getAttribute("item", "icon", "", i);
                    bool isSelected = settings.getAttribute("item", "isSelected", false, i);
                    bool requiresAttention = settings.getAttribute("item", "requiresAttention", false, i);
                    bool isVisible = settings.getAttribute("item", "isVisible", true, i);

                    WidgetMenuCollectionItem item(tag, nullptr, label);
                    item.isSelected = isSelected;
                    item.requiresAttention = requiresAttention;
                    item.isVisible = isVisible;

                    if (icon != "")
                    {
                        float iconSizeRegScale = IconCache::getIconSizeRegScale(Shared::getViz()->getScale());
                        int iconSizeRegScaledSize = iconSizeRegScaledSize = IconCache::getIconSizeReg(Shared::getViz()->getScale());

                        item.icon = Icon(
                            icon,
                            Coord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), // Consider adding w,h options on the item tag...
                            iconSizeRegScale,
                            Icon::IconSize::REGULAR, ofColor::white, -1);
                    }

                    // Child widget for item...
                    string itemWidgetXML = Shared::getXMLAtTag(&settings, "widget:properties:items:item", i);
                    if (itemWidgetXML != "")
                    {
                        Widget *w = WidgetManager::loadWidget(itemWidgetXML);
                        if (w)
                        {
                            w->setIsVisible(false);
                            item.widgetPersistentId = w->getPersistentId();
                        }
                        item.menu = w;
                    }

                    items.push_back(item);
                }
                settings.popTag(); // items
            }

            settings.popTag(); // properties
            settings.popTag(); // widget

            if (items.size() > 0)
            {
                setMenuCollection(items);
            }

            return true;
        }

        enum class MenuType
        {
            MENU,
            TAB
        };

        MenuType getMenuTypeEnum(string val)
        {
            val = ofToUpper(val);
            if (val == "MENU")
                return MenuType::MENU;
            if (val == "TAB")
                return MenuType::TAB;
            return MenuType::MENU;
        };

        MenuType getMenuType()
        {
            return mMenuType;
        }

        void setMenuType(MenuType val)
        {
            mMenuType = val;
            initializeMenuSlices();
            setWidgetNeedsUpdate(true);
        }

        Widget *getWidgetForTag(string tag)
        {
            for (WidgetMenuCollectionItemSlice &slice : mMenuCollectionSliceData)
            {
                if (slice.collectionItem.tag == tag)
                {
                    return slice.collectionItem.menu;
                }
            }
            return nullptr;
        }

        WidgetMenu *getMenuForTag(string tag)
        {
            Widget *w = getWidgetForTag(tag);
            if (w && dynamic_cast<WidgetMenu *>(w))
            {
                return dynamic_cast<WidgetMenu *>(w);
            }
            return nullptr;
        }

        void setMenuItemSelectedVal(string menuTag, int tabMenuId, int menuItemId, bool selected, bool fireEvent)
        {
            WidgetMenu *m = getMenuForTag(menuTag);
            if (m)
            {
                m->setMenuItemSelectedVal(tabMenuId, menuItemId, selected, fireEvent);
            }
        }

        void selectMenuItem(string menuTag, int tabMenuId, int menuItemId, bool fireEvent)
        {
            setMenuItemSelectedVal(menuTag, tabMenuId, menuItemId, true, fireEvent);
        }

        void deselectMenuItem(string menuTag, int tabMenuId, int menuItemId, bool fireEvent)
        {
            setMenuItemSelectedVal(menuTag, tabMenuId, menuItemId, false, fireEvent);
        }

        void updateMenuCollectionItem(WidgetMenuCollectionItem item, bool addIfNotFound)
        {
            for (WidgetMenuCollectionItem &collectionItem : mMenuCollection)
            {
                if (collectionItem.tag == item.tag)
                {
                    updateMenuSlice(item);
                    // Update the slice
                    for (WidgetMenuCollectionItemSlice &slice : mMenuCollectionSliceData)
                    {
                        if (slice.collectionItem.tag == item.tag)
                        {
                            slice.collectionItem = item;
                            return;
                        }
                    }
                }
            }

            // Wasn't found add if needed
            if (addIfNotFound)
            {
                updateMenuSlice(item);
                WidgetMenuCollectionItemSlice slice;
                slice.collectionItem = item;
                mMenuCollectionSliceData.push_back(slice);
            }
            initSliceColors();
        }

        void addMenuCollectionItem(WidgetMenuCollectionItem item)
        {
            updateMenuCollectionItem(item, true);
        }

        void setMenuCollection(vector<WidgetMenuCollectionItem> menuCollection)
        {
            mMenuCollection = menuCollection;
            if (getMenuType() == MenuType::TAB && !isAnySliceSelected() && mMenuCollectionSliceData.size() > 0)
            {
                setSelectedSlice(mMenuCollectionSliceData[0], false);
            }
            initializeMenuSlices();
        }

        void setTabMenuCollection(Widget *owner, vector<WidgetMenuCollectionItem> menuCollection)
        {
            if (owner)
                setOwnerWidgetId(owner->getWidgetId());
            mMenuCollection = menuCollection;
            if (getMenuType() == MenuType::TAB && !isAnySliceSelected() && mMenuCollectionSliceData.size() > 0)
            {
                setSelectedSlice(mMenuCollectionSliceData[0], false);
            }
            mMenuType = MenuType::TAB;
            initializeMenuSlices();
        }

        string getItemXExpression()
        {
            return mItemXExpression;
        }

        void setItemXExpression(string val)
        {
            mItemXExpression = val;
        }

        string getItemYExpression()
        {
            return mItemYExpression;
        }

        void setItemYExpression(string val)
        {
            mItemYExpression = val;
        }

        string getItemWidthExpression()
        {
            return mItemWidthExpression;
        }

        void setItemWidthExpression(string val)
        {
            mItemWidthExpression = val;
        }

        string getItemHeightExpression()
        {
            return mItemHeightExpression;
        }

        void setItemHeightExpression(string val)
        {
            mItemHeightExpression = val;
        }

        WidgetMenuCollectionItemSlice *getSelectedSlice()
        {
            for (WidgetMenuCollectionItemSlice &slice : mMenuCollectionSliceData)
            {
                if (slice.collectionItem.isSelected)
                    return &slice;
            }
            return nullptr;
        }

        int getSelectedSliceIndex()
        {
            int i = 0;
            for (WidgetMenuCollectionItemSlice slice : mMenuCollectionSliceData)
            {
                if (slice.collectionItem.isSelected)
                    return i;
                i++;
            }
            return -1;
        }

        void clearSelectedSlice()
        {
            mSelectedMenuTag = "";
            for (WidgetMenuCollectionItemSlice &slice : mMenuCollectionSliceData)
            {
                slice.collectionItem.isSelected = false;
                if (slice.collectionItem.menu)
                {
                    WidgetManager::moveWidgetToBack(*slice.collectionItem.menu);
                    slice.collectionItem.menu->setIsVisible(false);
                }
            }
            setNeedsUpdate(true);
        }

        void setMenuItem_RequiresAttention(string menuTag, bool val)
        {
            WidgetMenuCollectionItem *item = getMenuItemRef(menuTag);
            if (item)
            {
                item->requiresAttention = val;
                initSliceColors();
                setNeedsUpdate(true);
            }
        }

        void setMenuItem_setMetadata(string menuTag, string key, string val)
        {
        }

        void setMenuItem_Visibility(string menuTag, bool visible)
        {
            WidgetMenuCollectionItem *item = getMenuItemRef(menuTag);
            if (item)
            {
                item->isVisible = visible;
                mHasJustChangedItemVisibility = true;
                initSliceColors();
                clearSelectedSlice();
                setNeedsUpdate(true);
            }
        }

        WidgetMenuCollectionItem *getMenuItemRef(string menuTag)
        {
            for (WidgetMenuCollectionItemSlice &slice : mMenuCollectionSliceData)
            {
                if (slice.collectionItem.tag == menuTag)
                {
                    return &slice.collectionItem;
                }
            }
            return nullptr;
        }

        bool isAnySliceSelected()
        {
            for (WidgetMenuCollectionItemSlice &slice : mMenuCollectionSliceData)
            {
                if (slice.collectionItem.isSelected)
                    return true;
            }
            return false;
        }

        bool isAnyMenuVisible()
        {
            for (WidgetMenuCollectionItemSlice &slice : mMenuCollectionSliceData)
            {
                if (slice.collectionItem.menu && slice.collectionItem.menu->getIsVisible())
                    return true;
            }
            return false;
        }

        void setSelectedTag(string tag, bool fireEvent)
        {
            for (WidgetMenuCollectionItemSlice &slice : mMenuCollectionSliceData)
            {
                if (tag == slice.collectionItem.tag)
                {
                    setSelectedSlice(slice, fireEvent);
                    return;
                }
            }
        }

        string getSelectedTag()
        {
            return mSelectedMenuTag;
        }

        void setSelectedSlice(WidgetMenuCollectionItemSlice &slice, bool fireEvent)
        {

            clearSelectedSlice();

            slice.collectionItem.isSelected = true;
            mSelectedMenuTag = slice.collectionItem.tag;

            Widget *w;
            if (getMenuType() == MenuType::TAB)
            {
                Widget *owner = getOwnerWidget();
                if (owner)
                    w = owner->getChildWidgetByPersistentId(slice.collectionItem.widgetPersistentId);
                if (!w)
                    w = slice.collectionItem.menu;
            }
            else
            {
                w = slice.collectionItem.menu;
            }

            WidgetManager::recalculateWidgetExpressions(*this);

            if (w)
            {
                WidgetMenu *m;

                if (getMenuType() == MenuType::MENU && dynamic_cast<WidgetMenu *>(w))
                {
                    WidgetMenu *m = dynamic_cast<WidgetMenu *>(w);
                }

                w->setWasJustInitialized(true);
                w->setIsDraggable(false);
                w->setCloseAction(CloseAction::HIDE);

                if (getMenuType() == MenuType::TAB)
                {

                    w->showChildWithLayoutExpressions(
                        getItemXExpression(),
                        getItemYExpression(),
                        getItemWidthExpression(),
                        getItemHeightExpression(),
                        false);

                    WidgetManager::recalculateWidgetExpressions(*w);
                }
                else
                {
                    w->setTitleStyle(TitleStyle::BOTTOM_STATIC);
                    w->getTheme().UnhoveredWidgetAlpha = 0.95f;
                    w->getTheme().HoveredWidgetAlpha = 0.95f;

                    if (slice.collectionItem.displayOptions.displayType == Widget::DisplayType::REGULAR)
                    {
                        w->setWidgetSize(
                            w->getWidth(),
                            w->getHeight(),
                            false);
                        w->popoutWithoutArrow(
                            getUsableX(),
                            getY() + getHeight());
                    }
                    else if (slice.collectionItem.displayOptions.displayType == Widget::DisplayType::POPOUT)
                    {
                        w->popoutFrom(
                            deScale(slice.bounds.x + slice.bounds.width / 2),
                            deScale(slice.bounds.y + slice.bounds.height) + getScaledPadding() * 1.5f,
                            PopoutDirection::DOWN);
                    }
                    else if (slice.collectionItem.displayOptions.displayType == Widget::DisplayType::MENU)
                    {
                        w->popoutWithoutArrow(
                            deScale(slice.bounds.x),
                            getY() + getHeight());
                    }

                    if (w->getIsVisible())
                    {
                        WidgetManager::moveWidgetToFront(*w);
                    }
                }

                if (w->getIsVisible())
                {
                    w->setWidgetNeedsUpdate(true);
                    WidgetManager::recalculateWidgetExpressions(*w);
                }
            }

            if (fireEvent)
            {
                WidgetMenuCollectionArgs args(slice.collectionItem.tag, nullptr, NULL, NULL);
                ofNotifyEvent(menuItemSelected, args);
                onMenuItemSelected(args);
            }

            setNeedsUpdate(true);
        }

        virtual ~WidgetMenuCollection()
        {
            Widget *owner = getOwnerWidget();

            for (int i = 0; i < mMenuCollectionSliceData.size(); i++)
            {
                WidgetMenuCollectionItemSlice &slice = mMenuCollectionSliceData[i];
                Widget *w;
                if (getMenuType() == MenuType::TAB)
                {
                    if (owner)
                        w = owner->getChildWidgetByPersistentId(slice.collectionItem.widgetPersistentId);
                }
                else
                {
                    w = slice.collectionItem.menu;
                }

                if (w)
                {
                    if (dynamic_cast<WidgetMenu *>(w))
                    {
                        WidgetMenu *m = dynamic_cast<WidgetMenu *>(w);
                        ofRemoveListener(m->menuItemSelected, this, &WidgetMenuCollection::onCoreMenuItemSelected);
                    }
                    if (getMenuType() == MenuType::TAB)
                    {
                        if (owner)
                            owner->removeChildWidgetByPersistentId(w->getPersistentId());
                    }
                    else
                    {
                        w->setOwnerWidgetId("");
                        w->deleteThisWidget();
                    }
                }
            }
        };

        virtual void onWidgetAfterAllWidgetsLoaded(std::vector<std::reference_wrapper<Widget>> widgets)
        {
            // Fires when WidgetManager has loaded all widgets
            if (mMenuCollectionSliceData.size() == 0)
                initializeMenuSlices();
        }

        Widget *getOwnerWidget()
        {
            if (parentWidget())
            {
                return parentWidget();
            }
            else if (getOwnerWidgetId() != "")
            {
                // Add the widget to the children of the widget hosting this Tab Menu!
                Widget *w = WidgetManager::getWidgetByPersistentId(getOwnerWidgetId());
                return w;
            }
            return nullptr;
        }

        void initializeMenuSlices()
        {
            mMenuCollectionSliceData.clear();
            for (WidgetMenuCollectionItem &collectionItem : mMenuCollection)
            {
                updateMenuSlice(collectionItem);
                WidgetMenuCollectionItemSlice slice;
                slice.collectionItem = collectionItem;
                mMenuCollectionSliceData.push_back(slice);
            }
            initSliceColors();
        }

        void initSliceColors()
        {
            for (WidgetMenuCollectionItemSlice &slice : mMenuCollectionSliceData)
            {
                if (slice.collectionItem.requiresAttention)
                {
                    slice.selectedFontColor = getTheme().WidgetAccentHighContrast2Color_withAlpha(1);
                    slice.regularFontColor = getTheme().WidgetAccentHighContrast2Color_withAlpha(0.95f);
                }
                else
                {
                    slice.selectedFontColor = getTheme().TitleFontColor_withAlpha(1);
                    slice.regularFontColor = getTheme().TitleFontColor_withAlpha(0.95f);
                }
            }
        }
        virtual void onWidgetThemeChanged(WidgetEventArgs args) override
        {
            initSliceColors();
        }

        virtual void onMenuItemSelected(WidgetMenuCollectionArgs &args)
        {
            // Override this to hangle menu events
        }

        virtual void onMenuItemUnselected(WidgetMenuCollectionArgs &args)
        {
            // Override this to hangle menu events
        }

        virtual void onWidgetOwnedWidgetClosed(WidgetBase *closedWidget) override
        {
            if (getMenuType() != MenuType::MENU)
                return;
            clearSelectedSlice();
        }

        void update(WidgetContext context) override
        {
            if (mMenuCollectionSliceData.size() == 0)
                initializeMenuSlices();

            // Are we in an initial state (TAB) and have no default selected Tab ?
            if (getMenuType() == MenuType::TAB && !isAnySliceSelected() && mMenuCollectionSliceData.size() > 0)
            {
                if (getSelectedTag() != "")
                {
                    setSelectedTag(getSelectedTag(), true);
                }
                else
                {
                    setSelectedSlice(mMenuCollectionSliceData[0], true);
                }
            }

            setHeight(getTabHeight() + getNonScaledPadding());

            int tabWidthAppend = 0;
            int tabWidthAppendRight = 0;
            int tabWidthAppendLeft = 0;

            font = Shared::getViz()->getMediumFont();
            ofRectangle fontRectHeight = font->rect("Bmaj"); // Magic text...

            for (int i = 0; i < mMenuCollectionSliceData.size(); i++)
            {

                int tabWidth = 0;
                WidgetMenuCollectionItemSlice &slice = mMenuCollectionSliceData[i];

                // Don't do anything for invisible slices...
                if (!slice.collectionItem.isVisible)
                {
                    continue;
                }

                bool rightAligned = slice.collectionItem.displayOptions.horizontalAlign == Widget::HorizontalAlign::RIGHT;

                if (rightAligned)
                {
                    tabWidthAppend = scale(getUsableWidth()) - tabWidthAppendRight;

                    tabWidth += scale(slice.collectionItem.displayOptions.nonScaledPadding.right);

                    /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                    Label
                    *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
                    if (slice.collectionItem.label != "")
                    {
                        ofRectangle fontRect = font->rect(slice.collectionItem.label);
                        slice.labelPoint = Coord::vizPoint(
                            scale(Widget::getUsableX(false)) - fontRect.width + tabWidthAppend - tabWidth - getScaledPadding(),
                            scale(Widget::getUsableY(false)) + (scale(getTabHeight()) + (fontRectHeight.height * 0.25f)) / 2.0f);
                        tabWidth += fontRect.width;
                    }

                    /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                    Icon
                    *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
                    if (slice.collectionItem.icon.imageLoaded())
                    {
                        Coord::vizBounds iconBounds = slice.collectionItem.icon.getScaledBounds();
                        slice.collectionItem.icon.setScaledPos(
                            scale(Widget::getUsableX(false)) - iconBounds.width + tabWidthAppend - tabWidth - (slice.collectionItem.label != "" ? getScaledPadding() * 2.0f : getScaledPadding()),
                            scale(Widget::getUsableY(false)) + ((scale(getTabHeight()) - iconBounds.height) / 2) - getScaledPadding() / 2.0f);
                        tabWidth += iconBounds.width + (slice.collectionItem.label != "" ? getScaledPadding() : 0);
                    }

                    tabWidth += scale(slice.collectionItem.displayOptions.nonScaledPadding.left);
                }
                else
                {
                    tabWidthAppend = tabWidthAppendLeft;

                    tabWidth += scale(slice.collectionItem.displayOptions.nonScaledPadding.left);
                    /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                    Icon
                    *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
                    if (slice.collectionItem.icon.imageLoaded())
                    {
                        Coord::vizBounds iconBounds = slice.collectionItem.icon.getScaledBounds();
                        slice.collectionItem.icon.setScaledPos(
                            scale(Widget::getUsableX(false)) + tabWidthAppend + tabWidth + getScaledPadding(),
                            scale(Widget::getUsableY(false)) + ((scale(getTabHeight()) - iconBounds.height) / 2) - getScaledPadding() / 2.0f);
                        tabWidth += iconBounds.width + (slice.collectionItem.label != "" ? getScaledPadding() : 0);
                    }

                    /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                    Label
                    *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
                    if (slice.collectionItem.label != "")
                    {
                        ofRectangle fontRect = font->rect(slice.collectionItem.label);
                        slice.labelPoint = Coord::vizPoint(
                            scale(Widget::getUsableX(false)) + tabWidthAppend + tabWidth + getScaledPadding(),
                            scale(Widget::getUsableY(false)) + (scale(getTabHeight()) + (fontRectHeight.height * 0.25f)) / 2.0f);
                        tabWidth += fontRect.width;
                    }
                    tabWidth += scale(slice.collectionItem.displayOptions.nonScaledPadding.right);
                }
                tabWidth += getScaledPadding() * 2.0f;

                /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                Bounds
                *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
                slice.bounds = Coord::vizBounds(
                    scale(Widget::getUsableX(false)) + tabWidthAppend - (rightAligned ? tabWidth : 0),
                    scale(Widget::getUsableY(false)),
                    tabWidth,
                    scale(getTabHeight()));

                // Append total width usage
                if (rightAligned)
                {
                    tabWidthAppendRight += tabWidth;
                }
                else
                {
                    tabWidthAppendLeft += tabWidth;
                }
            }
        }

        void draw(WidgetContext context) override
        {

            bool anySliceSelected = isAnySliceSelected();

            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            // For some unknown / bizzare reason, the very first call of draw() in below loop
            // has the wrong alpha. As a workaround, draw an offscreen 1x1 pixel to mitigate...
            //*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            Elm vizElm_cell;
            vizElm_cell.setRectangle(-1, -1, 1, 1, getTheme().TitleColor_withAlpha(0.1f));
            vizElm_cell.draw();

            /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
            Bound box
            *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
            for (int i = 0; i < mMenuCollectionSliceData.size(); i++)
            {
                WidgetMenuCollectionItemSlice &slice = mMenuCollectionSliceData[i];

                // Don't do anything for invisible slices...
                if (!slice.collectionItem.isVisible)
                {
                    continue;
                }

                bool sliceHovered = slice.isHovered() && !getIsDragging();

                slice.collectionItem.icon.setColor((sliceHovered || slice.collectionItem.isSelected ? slice.selectedFontColor : slice.regularFontColor));

                if (slice.collectionItem.isSelected || sliceHovered)
                {

                    /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                    Tab box
                    *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
                    vizElm_tabHighlight.setRectangle(
                        slice.bounds.x,
                        slice.bounds.y - getScaledPadding(),
                        slice.bounds.width,
                        slice.bounds.height + getScaledPadding(),
                        // getTheme().WidgetColor_withAlpha(getTheme().ContentChromeAlpha)
                        getTheme().WidgetAccentHighContrast1Color_withAlpha(getTheme().GenericSelectedAlpha));

                    /*

                    float relativeX_Alpha=getTheme().ContentChromeAlpha;
                    float relativeY_Alpha=getTheme().ContentChromeAlpha;

                    if (sliceHovered) {
                        float x = (((float)(ofGetMouseX() - (float)slice.bounds.x)-getScaledPadding()) / ((float)(ofGetMouseX() - (float)slice.bounds.x + (float)slice.bounds.width)))*2.0f-0.5f; // zero = mid point
                        float y = (((float)(ofGetMouseY() - (float)slice.bounds.y)) / ((float)(ofGetMouseY() - (float)slice.bounds.y + (float)slice.bounds.height)))*2.0f-0.5f;  // zero = mid point
                        relativeX_Alpha=1.0f+((x*x)*-1.0f);
                        relativeY_Alpha=1.0f+((y*y)*-1.0f);
                    }
                   */
                    ofColor c = getTheme().WidgetAccentHighContrast1Color_withAlpha(getTheme().GenericSelectedAlpha);
                    vizElm_tabHighlight.setRectGradient(c, c, c, c);

                    // slice.currentAlpha = relativeX_Alpha;
                }
                else
                {
                    // ofColor c = getTheme().WidgetColor_withAlpha(0);
                    ofColor c = getTheme().WidgetAccentHighContrast1Color_withAlpha(0);
                    vizElm_tabHighlight.setRectGradient(c, c, c, c);
                }
                vizElm_tabHighlight.draw();

                if (sliceHovered)
                {
                    bool pressedAndReleased = getIsMousePressedAndReleasedOverWidget(false);
                    if (!mHasJustChangedItemVisibility)
                    {

                        if ((anySliceSelected && !slice.collectionItem.isSelected) || pressedAndReleased)
                        {

                            if (slice.collectionItem.isSelected && getMenuType() == MenuType::MENU)
                            { // Don't want to toggle with tabs...
                                WidgetMenuCollectionArgs args(slice.collectionItem.tag, nullptr, NULL, NULL);
                                ofNotifyEvent(menuItemUnselected, args);
                                onMenuItemUnselected(args);
                                clearSelectedSlice();
                            }
                            else
                            {
                                if ((getMenuType() == MenuType::MENU) || (getMenuType() == MenuType::TAB && pressedAndReleased))
                                {
                                    setSelectedSlice(slice, true);
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (getMenuType() == MenuType::TAB && slice.collectionItem.menu && slice.collectionItem.isSelected != slice.collectionItem.menu->getIsVisible())
                    {
                        slice.collectionItem.menu->setIsVisible(slice.collectionItem.isSelected);
                    }
                }

                ofPushStyle();
                ofSetColor((sliceHovered || slice.collectionItem.isSelected ? slice.selectedFontColor : slice.regularFontColor));
                font->draw(slice.collectionItem.label, slice.labelPoint.x, slice.labelPoint.y);

                /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                Tab box ICON
                *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
                if (slice.collectionItem.icon.imageLoaded())
                {
                    slice.collectionItem.icon.draw();
                }
                ofPopStyle();
            }

            mHasJustChangedItemVisibility = false;
        }

        virtual void onCoreWidgetMouseReleased(ofMouseEventArgs &e) override
        {
            // ONLY override this method if you want to override core widget behaviour. We need to do it: determine any wayward
            // clicks, and close any opened sub menu if necessary. (ie, hide sub menus if click did not originate from owned widget)
            Widget::onCoreWidgetMouseReleased(e);

            if (parentWidget())
                return; // Only want to apply below logic if a root widget!

            if (getSelectedTag() != "")
            {
                for (WidgetMenuCollectionItemSlice &slice : mMenuCollectionSliceData)
                {
                    WidgetMenu *m = dynamic_cast<WidgetMenu *>(slice.collectionItem.menu);
                    if (slice.collectionItem.menu && (slice.collectionItem.menu->hasActiveChildWidgetOrElement(true) ||
                                                      slice.collectionItem.menu->isTitleHovered() ||
                                                      (m && m->getIsMenuPinned())))
                        return;
                }

                if (!getIsActiveWidget())
                {
                    if (getMenuType() != MenuType::MENU)
                        return;
                    clearSelectedSlice();
                }
            }
        }

        virtual void onWidgetUnhovered(WidgetEventArgs &e) override
        {
            if (getMenuType() != MenuType::MENU)
                return;

            if (!isAnyMenuVisible())
            {
                WidgetMenuCollectionItemSlice *slice = getSelectedSlice();
                int sliceIndex = getSelectedSliceIndex();
                clearSelectedSlice();
            }
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
        }

        virtual void onWidgetAddedAsChild(WidgetEventArgs args) override
        {
            if (parentWidget())
            {
                setTheme(parentWidget()->getTheme());
                initSliceColors();
                if (getWidth_Expr() == "" || getWidth_Expr() == "${WINDOW.WIDTH}")
                {
                    setWidth_Expr("${" + parentWidget()->getPersistentId() + ".USABLE_WIDTH}");
                }

                setWidgetNeedsUpdate(true);

                Widget *owner = getOwnerWidget();
                if (owner)
                {
                    if (getItemXExpression() == "")
                        setItemXExpression("${" + owner->getPersistentId() + ".ABSOLUTE_USABLE_X}");
                    if (getItemYExpression() == "")
                        setItemYExpression("${" + getPersistentId() + ".BOTTOM}");
                    if (getItemWidthExpression() == "")
                        setItemWidthExpression("${" + owner->getPersistentId() + ".USABLE_WIDTH}");
                    if (getItemHeightExpression() == "")
                        setItemHeightExpression("${" + getPersistentId() + ".BOTTOM_USABLE_HEIGHT}");

                    if (getMenuType() == MenuType::TAB)
                    {

                        initializeMenuSlices();
                        initSliceColors();

                        if (getMenuType() == MenuType::TAB && !isAnySliceSelected() && mMenuCollectionSliceData.size() > 0)
                        {
                            WidgetMenuCollectionItemSlice &slice = mMenuCollectionSliceData[0];
                            setSelectedSlice(slice, false);
                        }

                        // For reason unknown, this must be called again in order to dynamically loaded TAB widgets
                        // to display their content...
                        WidgetMenuCollectionItemSlice *selectedSlice = getSelectedSlice();
                        if (selectedSlice)
                        {
                            setSelectedSlice(*selectedSlice, true);
                        }
                    }
                }
            }
        }

        float getTabWidth()
        {
            return mTabWidth;
        }

        float getTabHeight()
        {
            return mTabHeight;
        }

        void setTabSize(float valWidth, float valHeight)
        {
            mTabWidth = valWidth;
            mTabHeight = valHeight;
        }

        virtual void setTheme(WidgetTheme val) override
        {
            Widget::setTheme(val);
            setIsRoundedRectangle(false);
        }

    private:
        void updateMenuSlice(WidgetMenuCollectionItem &collectionItem)
        {
            if (collectionItem.menu)
            {
                collectionItem.menu->setIsSystemWidget(true);
                collectionItem.menu->setCloseAction(CloseAction::HIDE);
                collectionItem.menu->setTheme(collectionItem.menu->getTheme());

                if (getMenuType() == MenuType::TAB)
                {
                    Widget *w = getOwnerWidget();
                    if (w)
                    {

                        if (collectionItem.menu->getX_Expr() == "")
                            collectionItem.menu->setX_Expr("${" + w->getPersistentId() + ".ABSOLUTE_USABLE_X}");
                        if (collectionItem.menu->getY_Expr() == "")
                            collectionItem.menu->setY_Expr("${" + getPersistentId() + ".BOTTOM}");
                        if (collectionItem.menu->getWidth_Expr() == "")
                            collectionItem.menu->setWidth_Expr("${" + w->getPersistentId() + ".USABLE_WIDTH}");
                        if (collectionItem.menu->getHeight_Expr() == "")
                            collectionItem.menu->setHeight_Expr("${" + getPersistentId() + ".BOTTOM_USABLE_HEIGHT}");

                        w->addChildWidget(*collectionItem.menu, true);
                        collectionItem.menu->setIsVisible(false);
                    }
                }
                else
                {
                    WidgetManager::addWidget(*collectionItem.menu, false, getWidgetId());
                    WidgetManager::moveWidgetToBack(*collectionItem.menu);
                    collectionItem.menu->setIsVisible(false);
                }

                if (dynamic_cast<WidgetMenu *>(collectionItem.menu))
                {
                    WidgetMenu *m = dynamic_cast<WidgetMenu *>(collectionItem.menu);
                    ofAddListener(m->menuItemSelected, this, &WidgetMenuCollection::onCoreMenuItemSelected);
                }
            }
        }

        MenuType mMenuType = MenuType::MENU;

        string mItemXExpression = "";
        string mItemYExpression = "";
        string mItemWidthExpression = "";
        string mItemHeightExpression = "";

        vector<WidgetMenuCollectionItem> mMenuCollection = vector<WidgetMenuCollectionItem>();
        vector<WidgetMenuCollectionItemSlice> mMenuCollectionSliceData = vector<WidgetMenuCollectionItemSlice>();
        string mSelectedMenuTag = "";
        shared_ptr<ofxSmartFont> font;
        Elm vizElm_tabBoundBox;
        Elm vizElm_tabHighlight;

        float mTabWidth = 150;
        float mTabHeight = 30;
        bool mHasJustChangedItemVisibility = false;

        void onCoreMenuItemSelected(WidgetMenuItemArgs &coreArgs)
        {

            if (getMenuType() != MenuType::MENU)
                return;

            WidgetMenuCollectionItemSlice *slice = getSelectedSlice();
            if (!slice)
                return;
            WidgetMenuCollectionArgs args(slice->collectionItem.tag, dynamic_cast<WidgetMenu *>(slice->collectionItem.menu), coreArgs.activeMenuTabId, coreArgs.menuItem);
            ofNotifyEvent(menuItemSelected, args);
            onMenuItemSelected(args);
            WidgetMenu *m = dynamic_cast<WidgetMenu *>(slice->collectionItem.menu);
            if ((m && m->getIsMenuPinned()))
            {
                // Menu is pinned, don't close...
            }
            else
            {
                clearSelectedSlice();
            }
        }

        void initWidget() override
        {
            setIsPermanentWidget(true);
            setX(0);
            setY(0);

            setDrawWidgetChrome(true);
            setDrawWidgetTitle(false);
            setIsResizable(false);
            setIsDraggable(false);
            setTitleStyle(TitleStyle::NONE);
            setScrollAction(ScrollAction::NONE);
            if (getWidth_Expr() == "")
            {
                setWidth_Expr("${WINDOW.WIDTH}");
            }

            setHeight_Expr(ofToString(getTabHeight()));
        }
    };

}