
#pragma once
#include "uiVizWidget.h"

namespace Aquamarine
{
    class uiVizWidgetMenuItem
    {
    public:
        string label = "";
        string tag = "";
        int uniqueID = 0;
        bool isSelected = false;
        bool isToggle = false;

        uiVizWidgetMenuItem(string label, int uniqueID) : label(label), tag(""), uniqueID(uniqueID) {};
        uiVizWidgetMenuItem(string label, string tag, int uniqueID) : label(label), tag(tag), uniqueID(uniqueID) {};
        uiVizWidgetMenuItem(string label, int uniqueID, bool isToggle, bool isSelected) : label(label), tag(""), uniqueID(uniqueID), isToggle(isToggle), isSelected(isSelected) {}
        uiVizWidgetMenuItem(string label, string tag, int uniqueID, bool isToggle, bool isSelected) : label(label), tag(tag), uniqueID(uniqueID), isToggle(isToggle), isSelected(isSelected) {};

        ~uiVizWidgetMenuItem()
        {
        }
    };

    class uiVizWidgetMenuTab
    {
    public:
        string label = "";
        int uniqueID = 0;
        bool isSelected = false;
        vector<uiVizWidgetMenuItem> menuItems;
        uiVizWidget *tabWidget = NULL;
        uiVizCoord::vizBounds tabWidgetOriginalBounds = uiVizCoord::vizBounds(0, 0, 0, 0);
        bool isTabWidgetBoundsInitialized = false;
        bool isVisible = true;

        uiVizIcon icon = uiVizIcon("", uiVizCoord::vizBounds(0, 0, 0, 0), 1.0f, uiVizIcon::IconSize::REGULAR, ofColor::white, 0);

        uiVizWidgetMenuTab(string label, uiVizIcon icon, int uniqueID) : label(label), icon(icon), uniqueID(uniqueID) {};

        uiVizWidgetMenuTab(string label, int uniqueID, vector<uiVizWidgetMenuItem> menuItems) : label(label), uniqueID(uniqueID), menuItems(menuItems) {};

        uiVizWidgetMenuTab(string label, uiVizIcon icon, int uniqueID, vector<uiVizWidgetMenuItem> menuItems) : label(label), icon(icon), uniqueID(uniqueID), menuItems(menuItems) {};

        uiVizWidgetMenuTab(string label, uiVizIcon icon, int uniqueID, uiVizWidget *tabWidget) : label(label), icon(icon), uniqueID(uniqueID), tabWidget(tabWidget)
        {
            if (tabWidget)
                tabWidgetOriginalBounds = uiVizCoord::vizBounds(tabWidget->getX(), tabWidget->getY(), tabWidget->getWidth(), tabWidget->getHeight());
        };

        ~uiVizWidgetMenuTab()
        {
        }
    };

    class uiVizWidgetMenuItemArgs : public ofEventArgs
    {
    public:
        uiVizWidgetMenuItemArgs(int activeMenuTabId, uiVizWidgetMenuItem *menuItem) : activeMenuTabId(activeMenuTabId), menuItem(menuItem) {}
        int activeMenuTabId;
        uiVizWidgetMenuItem *menuItem;
    };

    class uiVizWidgetMenuTabArgs : public ofEventArgs
    {
    public:
        uiVizWidgetMenuTabArgs(int activeMenuTabIndex, int activeMenuTabId, uiVizWidgetMenuTab *menuTab) : activeMenuTabIndex(activeMenuTabIndex), activeMenuTabId(activeMenuTabId), menuTab(menuTab) {}
        int activeMenuTabIndex;
        int activeMenuTabId;
        uiVizWidgetMenuTab *menuTab;
    };

    class uiVizWidgetMenuSlice
    {
    public:
        uiVizWidgetMenuItem menuItem = uiVizWidgetMenuItem("", 0);
        uiVizCoord::vizPoint labelPoint = uiVizCoord::vizPoint(0, 0);
        uiVizCoord::vizBounds bounds = uiVizCoord::vizBounds(0, 0, 0, 0);
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

        uiVizWidgetMenuSlice() {};

        ~uiVizWidgetMenuSlice()
        {
        }
    };

    class uiVizWidgetTabSlice
    {
    public:
        uiVizWidgetMenuTab menuTab = uiVizWidgetMenuTab("", 0, vector<uiVizWidgetMenuItem>());
        uiVizCoord::vizPoint labelPoint = uiVizCoord::vizPoint(0, 0);
        uiVizCoord::vizBounds bounds = uiVizCoord::vizBounds(0, 0, 0, 0);
        ofColor regularColor;
        ofColor selectedColor;
        ofColor hoveredColor;
        ofColor regularFontColor;
        ofColor selectedFontColor;
        ofColor hoveredFontColor;
        float currentAlpha = 0.0f;
        int scrollOffsetX = 0;
        int scrollOffsetY = 0;

        bool isHovered()
        {
            int mX = ofGetMouseX();
            int mY = ofGetMouseY();
            return mX > bounds.x && mY > bounds.y && mX < bounds.x + bounds.width && mY < bounds.y + bounds.height;
        }

        uiVizWidgetTabSlice() {};

        ~uiVizWidgetTabSlice()
        {
        }
    };

    class uiVizWidgetMenu : public uiVizWidget
    {
    public:
        // Events
        ofEvent<uiVizWidgetMenuItemArgs> menuItemSelected;
        uiVizWidgetMenuItemArgs *args;

        ofEvent<uiVizWidgetMenuTabArgs> menuTabSelected;
        uiVizWidgetMenuTabArgs *tabArgs;

        uiVizWidgetMenuItem *DefaultMenuItem;
        uiVizWidgetMenuTab *DefaultMenuTab;

        enum class PreferredPopoutDirection
        {
            UP,
            RIGHT,
            DOWN,
            LEFT
        };

        enum class TabLocation
        {
            TOP,
            RIGHT,
            BOTTOM,
            LEFT
        };

        uiVizWidgetMenu(string persistentId, string widgetXML, string ownerWidgetId, PreferredPopoutDirection preferredDirection, vector<uiVizWidgetMenuItem> menuItems) : uiVizWidget(persistentId, widgetXML, ownerWidgetId)
        {
            mMenuItems = menuItems;
            initWidget();
        }

        uiVizWidgetMenu(string persistentId, string widgetXML, string ownerWidgetId, PreferredPopoutDirection preferredDirection, vector<uiVizWidgetMenuTab> menuTabs) : uiVizWidget(persistentId, widgetXML, ownerWidgetId)
        {
            mMenuTabs = menuTabs;
            if (mMenuTabs.size() > 0)
                setActiveMenuTabIndex(0);
            initWidget();
        }

        uiVizWidgetMenu(string persistentId, string widgetXML) : uiVizWidget(persistentId, widgetXML)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            ofxXmlSettings settings = ofxXmlSettings();
            ofxXmlSettings tabItemSettings = ofxXmlSettings();
            if (!settings.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            settings.pushTag("widget");
            settings.pushTag("properties");

            string tabsXML = uiVizShared::getXMLAtTag(&settings, "widget:properties", 0);
            bool tabsXMLLoaded = tabItemSettings.loadFromBuffer(tabsXML.c_str()) && tabItemSettings.pushTag("tabs");

            if (settings.pushTag("tabs"))
            {

                vector<uiVizWidgetMenuTab> menuTabs = vector<uiVizWidgetMenuTab>();
                int numTabs = settings.getNumTags("tab");
                for (int i = 0; i < numTabs; i++)
                {
                    string tag = settings.getAttribute("tab", "tag", "TAG_" + ofToString(i), i);
                    string label = settings.getAttribute("tab", "label", "", i);
                    string icon = settings.getAttribute("tab", "icon", "", i);
                    int id = settings.getAttribute("tab", "id", 0, i);

                    // Create a tab!
                    uiVizWidgetMenuTab tab(label, id, vector<uiVizWidgetMenuItem>());
                    if (icon != "")
                    {
                        float iconSizeRegScale = uiVizIconCache::getIconSizeRegScale(uiVizShared::getViz()->getScale());
                        int iconSizeRegScaledSize = iconSizeRegScaledSize = uiVizIconCache::getIconSizeReg(uiVizShared::getViz()->getScale());

                        tab.icon = uiVizIcon(
                            icon,
                            uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), // Consider adding w,h options on the item tag...
                            iconSizeRegScale,
                            uiVizIcon::IconSize::REGULAR, ofColor::white, -1);
                    }

                    // Items for the Tab!
                    if (tabsXMLLoaded && tabItemSettings.pushTag("tab", i))
                    {
                        int numItems = tabItemSettings.getNumTags("item");
                        bool isWidget = tabItemSettings.getNumTags("widget") > 0;

                        if (isWidget)
                        {
                            string tabWidgetXML = uiVizShared::getXMLAtTag(&tabItemSettings, "tabs:tab", i);
                            // cout << "TAB " << ofToString(i) << " WIDGET XML:\n" << tabWidgetXML;
                            uiVizWidget *w = uiVizWidgetManager::loadWidget(tabWidgetXML);
                            tab.tabWidget = w;
                        }
                        else
                        {
                            tab.menuItems.clear();
                            for (int j = 0; j < numItems; j++)
                            {
                                string itemTag = tabItemSettings.getAttribute("item", "tag", "TAG_" + ofToString(j), j);
                                string itemLabel = tabItemSettings.getAttribute("item", "label", "", j);
                                int itemId = tabItemSettings.getAttribute("item", "id", 0, j);
                                bool isToggle = tabItemSettings.getAttribute("item", "isToggle", false, j);
                                bool isSelected = tabItemSettings.getAttribute("item", "isSelected", false, j);
                                uiVizWidgetMenuItem menuItem = uiVizWidgetMenuItem(itemLabel, itemTag, itemId, isToggle, isSelected);
                                tab.menuItems.push_back(menuItem);
                            }
                        }
                        tabItemSettings.popTag(); // tab
                    }
                    menuTabs.push_back(tab);
                }
                settings.popTag(); // tabs
                setMenuTabs(menuTabs);
            }
            settings.popTag(); // properties
            settings.popTag(); // widget
            return true;
        }

        virtual ~uiVizWidgetMenu()
        {

            for (int i = 0; i < tabSlices.size(); i++)
            {
                uiVizWidgetTabSlice *slice = &tabSlices[i];
                // Stop listening to events...
                if (slice->menuTab.tabWidget != NULL)
                {
                    ofRemoveListener(slice->menuTab.tabWidget->widgetEventReceived, this, &uiVizWidgetMenu::onTabWidgetEventReceived);
                }
            }

            delete DefaultMenuItem;
            delete args;
            delete DefaultMenuTab;
            delete tabArgs;
        };

        void setMenuTabs(vector<uiVizWidgetMenuTab> menuTabs)
        {
            mMenuSlicesInitialized = false;
            mMenuTabs = menuTabs;
            if (mMenuTabs.size() > 0)
                setActiveMenuTabIndex(0);
            initWidget();
        }

        void initializeMenuSlices(bool isVeryFirstInit)
        {
            tabSlices.clear();
            menuSliceData.clear();
            clearChildWidgets();

            for (int i = 0; i < mMenuTabs.size(); i++)
            {
                uiVizWidgetTabSlice tabSlice;
                tabSlice.menuTab = mMenuTabs[i];
                tabSlices.push_back(tabSlice);

                vector<uiVizWidgetMenuSlice> menuSlices = vector<uiVizWidgetMenuSlice>();

                for (int j = 0; j < tabSlice.menuTab.menuItems.size(); j++)
                {
                    uiVizWidgetMenuSlice menuItemSlice;
                    menuItemSlice.menuItem = tabSlice.menuTab.menuItems[j];
                    menuSlices.push_back(menuItemSlice);
                }

                if (tabSlice.menuTab.tabWidget != NULL)
                {
                    if (isVeryFirstInit)
                    {
                        tabSlice.menuTab.tabWidget->setTitleStyle(TitleStyle::NONE);
                        tabSlice.menuTab.tabWidget->setDrawWidgetTitle(false);
                    }
                    tabSlice.menuTab.tabWidget->setIsVisible(false);
                    addChildWidget(*tabSlice.menuTab.tabWidget);
                    ofAddListener(tabSlice.menuTab.tabWidget->widgetEventReceived, this, &uiVizWidgetMenu::onTabWidgetEventReceived);
                }

                menuSliceData.push_back(menuSlices);
            }
        }

        void update(uiVizWidgetContext context) override
        {
            if (menuSliceData.size() == 0)
                initializeMenuSlices(false);

            font = getViz()->getMediumFont();
            ofRectangle rectLabel = font->rect("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

            int maxItemWidth = 0;
            int maxItemHeight = 0;

            int tabWidthAppend = 0;
            int tabHeightAppend = 0;

            clearContentBoundsScaled();

            for (int i = 0; i < tabSlices.size(); i++)
            {
                uiVizWidgetTabSlice *tabSlice = &tabSlices[i];

                tabSlice->labelPoint = uiVizCoord::vizPoint(
                    scale(uiVizWidget::getUsableX(false)),
                    scale(uiVizWidget::getUsableY(false)) + (i + 1) * scale(getTabHeight()) * 2 - getScaledPadding() * 2);

                tabSlice->bounds = uiVizCoord::vizBounds(
                    scale(uiVizWidget::getUsableX(false)),
                    scale(uiVizWidget::getUsableY(false)) + i * scale(getTabHeight()),
                    scale(getTabWidth()),
                    scale(getTabHeight()));

                tabWidthAppend += scale(getTabWidth());
                tabHeightAppend += scale(getTabHeight());

                if (tabSlice->menuTab.icon.imageLoaded())
                {

                    tabSlice->menuTab.icon.setScaledPos(
                        tabSlice->bounds.x + (scale(getTabWidth()) - tabSlice->menuTab.icon.getScaledBounds().width) / 2,
                        tabSlice->bounds.y + (scale(getTabHeight()) - tabSlice->menuTab.icon.getScaledBounds().height) / 2);
                }
            }

            if (tabSlices.size() > 0)
            {
                setMinHeight(tabSlices.size() * getTabHeight() + getNonScaledPadding() + ((isTitleTop() || isTitleBottom()) ? getUnhoveredTitleBarSize(false) : 0));
            }

            vector<uiVizWidgetMenuItem> activeMenuItems = getActiveMenu();

            if (menuSliceData.size() > 0)
            {
                for (int i = 0; i < menuSliceData[mActiveMenuTabIndex].size(); i++)
                {
                    uiVizWidgetMenuSlice *slice = &menuSliceData[mActiveMenuTabIndex][i];

                    float cellHeight = rectLabel.height * 1.75f + getScaledPadding();
                    float cellTop = scale(getUsableY(true)) + i * cellHeight;

                    slice->bounds = uiVizCoord::vizBounds(scale(getUsableX(true)), cellTop, scale(getUsableWidth()), cellHeight);
                    slice->labelPoint = uiVizCoord::vizPoint(scale(getUsableX(true)) + getScaledPadding(), cellTop + (rectLabel.height + cellHeight) / 2.25f);

                    appendContentBoundsScaled(slice->bounds);
                    if (i == 0)
                        appendContentBoundsScaled(slice->bounds); // Add one extra item height

                    // Record the max width's and height of content items
                    ofRectangle rectLabelWidth = font->rect(slice->menuItem.label);
                    maxItemWidth = std::max((float)(rectLabelWidth.width + getScaledPadding() * 2), (float)maxItemWidth);
                    maxItemHeight = std::max((float)(rectLabel.height), (float)maxItemHeight);
                }
            }

            // MAX BOUNDS CHECK
            setContentBoundsScaled(uiVizCoord::vizBounds(getContentBoundsScaled().x,
                                                         getContentBoundsScaled().y,
                                                         maxItemWidth,
                                                         getContentBoundsScaled().height < tabHeightAppend ? tabHeightAppend : getContentBoundsScaled().height));

            uiVizWidgetTabSlice *tabSlice = &tabSlices[mActiveMenuTabIndex];
            if (tabSlice != nullptr)
            {
                tabSlice->menuTab.isSelected = true;
                setScrollOffsetX(tabSlice->scrollOffsetX, true);
                setScrollOffsetY(tabSlice->scrollOffsetY, true);
            }

            // AUTO RESIZING STUFF
            bool skipAutosize = false;
            if (tabSlice != nullptr && tabSlice->menuTab.tabWidget && (!mUpdatedSinceMenuItemChangedWidth || !mUpdatedSinceMenuItemChangedHeight))
            {

                tabSlice->menuTab.tabWidget->setX_Expr("${" + getPersistentId() + ".ABSOLUTE_USABLE_X}");
                tabSlice->menuTab.tabWidget->setY_Expr("${" + getPersistentId() + ".ABSOLUTE_USABLE_Y}");
                uiVizWidgetManager::recalculateWidgetExpressions(*(tabSlice->menuTab.tabWidget));

                // tabSlice->menuTab.tabWidget->setWidgetSize(
                //     tabSlice->menuTab.tabWidgetOriginalBounds.width, tabSlice->menuTab.tabWidgetOriginalBounds.height, false);

                tabSlice->menuTab.tabWidget->setIsVisible(true);
                tabSlice->menuTab.tabWidget->setChildWidgetsVisibility(true);
                tabSlice->menuTab.tabWidget->setWidgetNeedsUpdate(true);
                tabSlice->menuTab.tabWidget->setNeedsUpdate(true);

                setMinWidth(tabSlice->menuTab.tabWidget->getMinWidth());

                updateWidget();
                updateWidgetPopoutArrow();
                mUpdatedSinceMenuItemChangedWidth = true;
                mUpdatedSinceMenuItemChangedHeight = true;
                skipAutosize = true;
            }

            if (!skipAutosize)
            {

                if (getIsAutoResizeToContentsWidth() && !mUpdatedSinceMenuItemChangedWidth)
                {
                    setSize(deScale(getContentBoundsScaled().width) + getWidth() - getUsableWidth(), getHeight());
                    mUpdatedSinceMenuItemChangedWidth = true;
                    setWidgetNeedsUpdate(true);
                }

                if (getIsAutoResizeToContentsHeight() && !mUpdatedSinceMenuItemChangedHeight)
                {
                    setSize(getWidth(), deScale(getContentBoundsScaled().height) + deScale(getHoveredTitleBarSize(true)));
                    mUpdatedSinceMenuItemChangedHeight = true;
                    setWidgetNeedsUpdate(true);
                }

                updateWidget();
                updateWidgetPopoutArrow();
            }

            setDrawWidgetTitle(true);

            // Is this JUST a tab menu (with no child menu items / tabWidget?)
            if (tabSlices.size() > 0 && menuSliceData[mActiveMenuTabIndex].size() == 0 && tabSlice->menuTab.tabWidget == NULL)
            {
                setWidth(getTabWidth() + getNonScaledPadding() * 2);
                setDrawWidgetTitle(false);

                if (parent())
                {
                    parent()->setWidgetNeedsUpdate(true);
                    parent()->setNeedsUpdate(true);
                    parent()->setExpressionRecalculationRequired(true);
                    parent()->updateWidgetPopoutArrow();
                }
                return;
            }
        }

        void hideTabWidgets()
        {
            for (int i = 0; i < tabSlices.size(); i++)
            {
                uiVizWidgetTabSlice *slice = &tabSlices[i];
                // Hide any visible tabWidget initially
                if (!slice->menuTab.isSelected && slice->menuTab.tabWidget != NULL)
                {
                    slice->menuTab.tabWidget->uiVizWidgetBase::setIsVisible(false);
                }
            }
        }

        void draw(uiVizWidgetContext context) override
        {

            setEnableScrollYPositive(true);
            setEnableScrollYNegative(true);

            uiVizWidgetTabSlice *tabSlice = &tabSlices[mActiveMenuTabIndex];
            if (tabSlice != nullptr && tabSlice->menuTab.tabWidget)
            {
                int extraWidth = getWidth() - getUsableWidth();
                setMinWidth(extraWidth + tabSlice->menuTab.tabWidget->getMinWidth());
                if (getUsableWidth() < tabSlice->menuTab.tabWidget->getWidth())
                {
                    resizeToTabWidget(tabSlice->menuTab.tabWidget->getWidth(), tabSlice->menuTab.tabWidget->getHeight());
                }

                // int extraWidth = getWidth() - getUsableWidth();
            }
            else
            {
                setMinWidth(150);
            }

            if (getHeight() < getMinHeight())
            {
                setHeight(getMinHeight());
                setWidgetNeedsUpdate(true);
                setNeedsUpdate(true);
                setPosition(getX(), getY());
            }

            /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
             Menu Tabs
             *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
            if (getIsTabbedMenu())
            {

                hideTabWidgets();

                float targetTabHoveredAlpha = targetTabHoveredAlpha = 0.5f;

                for (int i = 0; i < tabSlices.size(); i++)
                {

                    uiVizWidgetTabSlice *slice = &tabSlices[i];

                    slice->menuTab.icon.setColor(getTheme().TitleFontColor_withAlpha(0.9f));

                    bool sliceHovered = slice->isHovered() && !getIsDragging();
                    if (slice->menuTab.isSelected || sliceHovered || slice->currentAlpha > 0)
                    {

                        slice->currentAlpha = !slice->isHovered() ? scaleAnimation(getWidgetId() + "_tabhovered_" + ofToString(i), slice->currentAlpha, 0, 0.3f) : scaleAnimation(getWidgetId() + "_tabhovered_" + ofToString(i), slice->currentAlpha, targetTabHoveredAlpha, 0.05f);

                        if (slice->menuTab.isSelected || (sliceHovered && getIsMousePressedOverWidget(true)))
                        {
                            slice->currentAlpha = targetTabHoveredAlpha;
                        }

                        /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                         Bound box
                         *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
                        vizElm_tabBoundBox.setRectangle(scale(getUsableX(false)),
                                                        scale(getUsableY(false)) - getScaledPadding(),
                                                        scale(getUsableWidth()) + getScaledPadding() * 2,
                                                        scale(getUsableHeight()) + getScaledPadding() * 2,
                                                        getTheme().WidgetColor_withAlpha(getTheme().GenericSelectedAlpha));

                        vizElm_tabBoundBox.draw();

                        /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                         Tab box
                         *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
                        vizElm_tabHighlight.setRectangle(slice->bounds.x - getScaledPadding(),
                                                         slice->bounds.y,
                                                         slice->bounds.width + getScaledPadding() * 2,
                                                         slice->bounds.height,
                                                         getTheme().TitleFontColor_withAlpha(slice->currentAlpha));

                        vizElm_tabHighlight.draw();
                    }

                    ofPushStyle();
                    if (sliceHovered)
                    {
                        ofSetColor(getTheme().TitleFontColor_withAlpha(getTheme().GenericHoveredAlpha));
                        setTitle(slice->menuTab.label);

                        if (getIsMousePressedAndReleasedOverWidget(false))
                        {
                            setActiveMenuTabIndex(i);

                            // args->activeMenuTabIndex = i;
                            args->activeMenuTabId = slice->menuTab.uniqueID;
                            tabArgs->menuTab = &slice->menuTab;
                            ofNotifyEvent(menuTabSelected, *tabArgs);

                            // Does this menuTab have a tabWidget?
                            if (slice->menuTab.tabWidget)
                            {
                                slice->menuTab.tabWidget->setIsVisible(true);
                                setEnableScrollYPositive(false);
                                setEnableScrollYNegative(false);
                                setWidth(150);

                                slice->menuTab.tabWidget->setDrawWidgetChrome(false);
                                slice->menuTab.tabWidget->setIsResizable(false);
                                slice->menuTab.tabWidget->setIsDraggable(false);
                                // slice->menuTab.tabWidget->setDrawWidgetTitle(false);
                                // slice->menuTab.tabWidget->setTitleStyle(TitleStyle::NONE);
                                slice->menuTab.tabWidget->setChildWidgetsVisibility(true);

                                if (!slice->menuTab.isTabWidgetBoundsInitialized)
                                {
                                    slice->menuTab.tabWidget->setX_Expr("${" + getPersistentId() + ".ABSOLUTE_USABLE_X}");
                                    slice->menuTab.tabWidget->setY_Expr("${" + getPersistentId() + ".ABSOLUTE_USABLE_Y}");
                                    uiVizWidgetManager::recalculateWidgetExpressions(*(slice->menuTab.tabWidget));
                                    resizeToTabWidget(slice->menuTab.tabWidget->getWidth(), slice->menuTab.tabWidget->getHeight());
                                    slice->menuTab.tabWidget->setWidgetNeedsUpdate(true);
                                    slice->menuTab.tabWidget->setNeedsUpdate(true);
                                    slice->menuTab.isTabWidgetBoundsInitialized = true;
                                }
                                else
                                {
                                    resizeToTabWidget(slice->menuTab.tabWidget->getWidth(), slice->menuTab.tabWidget->getHeight());
                                }
                            }
                        }
                    }
                    else
                    {
                        ofSetColor(ofColor::white);
                    }

                    /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
                     Tab box ICON
                     *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
                    if (slice->menuTab.icon.imageLoaded())
                    {
                        slice->menuTab.icon.drawSvg();
                    }

                    ofPopStyle();
                }
            }

            /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
             Menu Items for selected Tab
             *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
            if (menuSliceData.size() > 0)
            {

                for (int i = 0; i < menuSliceData[mActiveMenuTabIndex].size(); i++)
                {

                    uiVizWidgetMenuSlice *slice = &menuSliceData[mActiveMenuTabIndex][i];
                    uiVizWidgetMenuItem *menuItem = &slice->menuItem;

                    if (isInsideWidgetBounds(deScale(slice->bounds.x), deScale(slice->bounds.y), deScale(slice->bounds.width), deScale(slice->bounds.height)))
                    {

                        bool sliceHovered = slice->isHovered() && !getIsDragging();
                        bool sliceEnabled = slice->isEnabled;

                        if (sliceEnabled && (menuItem->isSelected || sliceHovered || slice->currentAlpha > getTheme().GenericSelectedAlpha))
                        {

                            slice->currentAlpha = !menuItem->isSelected && !slice->isHovered() && slice->currentAlpha > getTheme().GenericSelectedAlpha ? scaleAnimation(getWidgetId() + "_hovered_" + ofToString(i), slice->currentAlpha, getTheme().GenericSelectedAlpha, 0.3f) : scaleAnimation(getWidgetId() + "_hovered_" + ofToString(i), slice->currentAlpha, getTheme().GenericHoveredAlpha, 0.05f);

                            if (sliceHovered && getIsMousePressedOverWidget(true))
                                slice->currentAlpha = getTheme().GenericHoveredAlpha;
                            if (menuItem->isSelected)
                            {
                                slice->currentAlpha = getTheme().GenericHoveredAlpha;
                            }

                            vizElm_menuHighlight.setRectangle(slice->bounds.x,
                                                              slice->bounds.y,
                                                              slice->bounds.width + getScaledPadding() * 2,
                                                              slice->bounds.height,
                                                              getTheme().WidgetAccentHighContrast1Color_withAlpha(slice->currentAlpha)
                                                              // getTheme().WidgetAccentHighContrast1Color_withAlpha(0.8)
                            );

                            vizElm_menuHighlight.draw();
                        }

                        ofPushStyle();
                        if (sliceEnabled && sliceHovered)
                        {
                            ofSetColor(getTheme().TitleFontColor_withAlpha(1));
                            if (getIsMousePressedAndReleasedOverWidget(false))
                            {
                                if (menuItem->isToggle)
                                {
                                    toggleMenuItem(getIdForTabMenuIndex(mActiveMenuTabIndex), menuItem->uniqueID, true);
                                }
                                else
                                {
                                    selectMenuItem(getIdForTabMenuIndex(mActiveMenuTabIndex), menuItem->uniqueID, true);
                                }
                            }
                        }
                        else
                        {
                            ofSetColor(sliceEnabled ? getTheme().TitleFontColor_withAlpha(0.9f) : getTheme().TitleFontColor_withAlpha(0.8f));
                        }
                        font->draw(menuItem->label, slice->labelPoint.x, slice->labelPoint.y);
                        ofPopStyle();
                    }
                }
            }
        }

        void resizeToTabWidget(int width, int height)
        {
            int extraWidth = getWidth() - getUsableWidth();
            int extraHeight = getHeight() - getUsableHeight();
            int targetWidth = width + extraWidth;
            int targetHeight = height + extraHeight;
            //  setWidgetSize(targetWidth, max(getHeight(), targetHeight), false);

            uiVizWidgetTabSlice *tabSlice = &tabSlices[mActiveMenuTabIndex];
            if (tabSlice != nullptr && tabSlice->menuTab.tabWidget)
            {
                int tabWidgetMinWidth = tabSlice->menuTab.tabWidget->getMinWidth() + extraWidth;
                int tabWidgetMinHeight = tabSlice->menuTab.tabWidget->getMinHeight() + extraHeight;
                setMinWidth(tabWidgetMinWidth);
                setMinHeight(tabWidgetMinHeight);

                if (targetWidth < tabWidgetMinWidth)
                    targetWidth = tabWidgetMinWidth;
                if (targetHeight < tabWidgetMinHeight)
                    targetHeight = tabWidgetMinHeight;
            }

            setWidth(targetWidth);
            setHeight(max(getHeight(), targetHeight));

            setWidgetPosition(getX(), getY() - 1, false);
            setWidgetPosition(getX(), getY() + 1, false);
            setWidgetSize(targetWidth, max(getHeight(), targetHeight), false);

            setWidgetNeedsUpdate(true);
        }

        void onTabWidgetEventReceived(uiVizWidgetEventArgs &args)
        {
            // Resize the Menu bounds if the tabWidget bounds have changed
            if (args.eventName == WIDGET_EVENT::RESIZED)
            {
                mTabWidgetWasResized = true;

                uiVizWidgetTabSlice *tabSlice = &tabSlices[mActiveMenuTabIndex];
                if (tabSlice != nullptr && tabSlice->menuTab.tabWidget)
                {
                    tabSlice->menuTab.tabWidgetOriginalBounds = uiVizCoord::vizBounds(0, 0, args.sender.getWidth(), args.sender.getHeight());
                }

                resizeToTabWidget(args.sender.getWidth(), args.sender.getHeight());
            }
        }

        // virtual bool setWidgetSize_ShouldPrevent(int width, int height, bool enableDockingLogic) {
        //     uiVizWidgetTabSlice* tabSlice = &tabSlices[mActiveMenuTabIndex];
        //     if (tabSlice != nullptr && tabSlice->menuTab.tabWidget)  {
        //         if (width <= tabSlice->menuTab.tabWidget->getMinWidth() || height <= tabSlice->menuTab.tabWidget->getMinHeight()) {
        //             // Prevent resize operation
        //             return true;
        //         }
        //     }
        //     return false;
        // }

        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {
            // Resize the TAB WIDGET menu bounds have changed
            if (args.eventName == WIDGET_EVENT::RESIZED)
            {

                if (mTabWidgetWasResized == true)
                {
                    mTabWidgetWasResized = false;
                    return;
                }

                uiVizWidgetTabSlice *tabSlice = &tabSlices[mActiveMenuTabIndex];
                if (tabSlice != nullptr && tabSlice->menuTab.tabWidget)
                {
                    //   tabSlice->menuTab.tabWidget->setX_Expr("${" + getPersistentId() + ".ABSOLUTE_USABLE_X}");
                    //   tabSlice->menuTab.tabWidget->setY_Expr("${" + getPersistentId() + ".ABSOLUTE_USABLE_Y}");
                    //   uiVizWidgetManager::recalculateWidgetExpressions(*(tabSlice->menuTab.tabWidget));

                    // if (args.sender.getWidth() <= tabSlice->menuTab.tabWidget->getMinWidth() || args.sender.getHeight() <= tabSlice->menuTab.tabWidget->getMinHeight()) {
                    //     // Prevent resize operation
                    //     return;
                    // }

                    tabSlice->menuTab.tabWidget->setWidgetSize(getUsableWidth(), getUsableHeight(), false);
                    tabSlice->menuTab.tabWidget->setWidgetNeedsUpdate(true);
                    tabSlice->menuTab.tabWidget->setNeedsUpdate(true);
                    updateWidget();
                    mUpdatedSinceMenuItemChangedWidth = true;
                    mUpdatedSinceMenuItemChangedHeight = true;
                    // resizeToTabWidget(args.sender.getWidth(), args.sender.getHeight());

                    // when tabIndex clicked fixes itself! why ??
                    // add to chord wheel - clicked - add for fretboard
                }
            }
        }

        virtual vector<uiVizIcon> getWidgetTitleIcons() override
        {
            // Override in descendant to modify...
            if (!getShowTitleIcons())
                return vector<uiVizIcon>();

            if (getShowMenuPin())
            {
                return {
                    uiVizIconCache::getIcon("REG_WIDGET_CLOSE"),
                    getIsMenuPinned() ? uiVizIconCache::getIcon("REG_REG_RADIO_BUTTON_CHECKED") : uiVizIconCache::getIcon("REG_REG_RADIO_BUTTON_UNCHECKED")};
            }
            else
            {
                return {
                    uiVizIconCache::getIcon("REG_WIDGET_CLOSE")};
            }
        }

        void closeMenu()
        {
            for (int i = 0; i < tabSlices.size(); i++)
            {
                uiVizWidgetTabSlice *slice = &tabSlices[i];
                // Hide any visible tabWidget too...
                if (slice->menuTab.tabWidget != NULL)
                {
                    slice->menuTab.tabWidget->uiVizWidgetBase::setIsVisible(false);
                    slice->menuTab.tabWidget->setChildWidgetsVisibility(false);
                }
            }
            setIsVisible(false);

            if (parent() != nullptr)
            {
                parent()->setActiveWidget();
            }

            mUpdatedSinceMenuItemChangedWidth = false;
            mUpdatedSinceMenuItemChangedHeight = false;
        }

        virtual void titleIconClicked(int iconTag, uiVizCoord::vizBounds deScaledClickBounds) override
        {
            if (iconTag == uiVizIconCache::IconTag::WIDGET_CLOSE)
            {
                closeMenu();
                return;
            }
            else if (iconTag == uiVizIconCache::IconTag::WIDGET_PIN)
            {
                setIsMenuPinned(false);
            }
            else if (iconTag == uiVizIconCache::IconTag::WIDGET_UNPIN)
            {
                setIsMenuPinned(true);
            }
            uiVizWidget::titleIconClicked(iconTag, deScaledClickBounds);
        }

        void setMenuItemSelectedVal(int tabMenuId, string menuItemTag, bool selected, bool fireEvent)
        {
            int tabIndex = getIndexForTabMenu(tabMenuId);
            vector<uiVizWidgetMenuSlice> *menuSlices = getMenuTabSlice(tabMenuId);
            if (menuSlices == nullptr)
                return;
            vector<uiVizWidgetMenuSlice> &menuSlicesRef = *menuSlices;

            for (int i = 0; i < menuSlicesRef.size(); i++)
            {
                if (menuSlicesRef[i].menuItem.tag == menuItemTag)
                {
                    menuSlicesRef[i].menuItem.isSelected = selected;

                    if (fireEvent)
                    {
                        uiVizWidgetMenuItemArgs args(tabMenuId, &menuSlicesRef[i].menuItem);
                        ofNotifyEvent(menuItemSelected, args);
                        hideMenuAfterSelection();
                    }
                }
            }
        }

        void selectMenuItem(int tabMenuId, string menuItemTag, bool fireEvent)
        {
            setMenuItemSelectedVal(tabMenuId, menuItemTag, true, fireEvent);
        }

        void deselectMenuItem(int tabMenuId, string menuItemTag, bool fireEvent)
        {
            setMenuItemSelectedVal(tabMenuId, menuItemTag, false, fireEvent);
        }

        void setMenuItemSelectedValByLabel(int tabMenuId, string menuItemLabel, bool selected, bool fireEvent)
        {

            int tabIndex = getIndexForTabMenu(tabMenuId);
            vector<uiVizWidgetMenuSlice> *menuSlices = getMenuTabSlice(tabMenuId);
            if (menuSlices == nullptr)
                return;
            vector<uiVizWidgetMenuSlice> &menuSlicesRef = *menuSlices;

            for (int i = 0; i < menuSlicesRef.size(); i++)
            {
                if (menuSlicesRef[i].menuItem.label == menuItemLabel)
                {
                    menuSlicesRef[i].menuItem.isSelected = selected;

                    if (fireEvent)
                    {
                        uiVizWidgetMenuItemArgs args(tabMenuId, &menuSlicesRef[i].menuItem);
                        ofNotifyEvent(menuItemSelected, args);
                        hideMenuAfterSelection();
                    }
                }
            }
        }

        void selectMenuItemByLabel(int tabMenuId, string menuItemLabel, bool fireEvent)
        {
            setMenuItemSelectedValByLabel(tabMenuId, menuItemLabel, true, fireEvent);
        }

        void deselecMenuItemByLabel(int tabMenuId, string menuItemLabel, bool fireEvent)
        {
            setMenuItemSelectedValByLabel(tabMenuId, menuItemLabel, false, fireEvent);
        }

        void hideMenuAfterSelection()
        {
            if (getIsMenuPinned())
                return;
            closeMenu();
        }

        void setMenuItemSelectedVal(int tabMenuId, int menuItemId, bool selected, bool fireEvent)
        {

            int tabIndex = getIndexForTabMenu(tabMenuId);
            int menuItemIndex = getIndexForTabMenuItem(tabIndex, menuItemId);

            if (tabIndex == -1 || menuItemIndex == -1)
                return;

            uiVizWidgetMenuSlice *slice = &menuSliceData[tabIndex][menuItemIndex];
            uiVizWidgetMenuItem *menuItem = &slice->menuItem;

            if (menuItem->isToggle)
            {
                menuItem->isSelected = selected;
            }
            else
            {
                // Clear the others
                for (int j = 0; j < menuSliceData[tabIndex].size(); j++)
                {
                    uiVizWidgetMenuSlice *unselectSlice = &menuSliceData[tabIndex][j];
                    if (!unselectSlice->menuItem.isToggle)
                        unselectSlice->menuItem.isSelected = false;
                }
                menuItem->isSelected = selected;
            }

            if (fireEvent)
            {
                uiVizWidgetMenuItemArgs args(tabMenuId, menuItem);
                ofNotifyEvent(menuItemSelected, args);
                hideMenuAfterSelection();
            }
        }

        void selectMenuItem(int tabMenuId, int menuItemId, bool fireEvent)
        {
            setMenuItemSelectedVal(tabMenuId, menuItemId, true, fireEvent);
        }

        void deselectMenuItem(int tabMenuId, int menuItemId, bool fireEvent)
        {
            setMenuItemSelectedVal(tabMenuId, menuItemId, false, fireEvent);
        }

        void setMenuItemEnabled(int tabMenuId, int menuItemId, bool enabled)
        {
            int tabIndex = getIndexForTabMenu(tabMenuId);
            vector<uiVizWidgetMenuSlice> *menuSlices = getMenuTabSlice(tabMenuId);
            if (menuSlices == nullptr)
                return;
            vector<uiVizWidgetMenuSlice> &menuSlicesRef = *menuSlices;

            for (int i = 0; i < menuSlicesRef.size(); i++)
            {
                if (menuSlicesRef[i].menuItem.uniqueID == menuItemId)
                {
                    menuSlicesRef[i].isEnabled = enabled;
                    return;
                }
            }
        }

        void addOrUpdateMenuTab(uiVizWidgetMenuTab tab)
        {
            int tabIndex = getIndexForTabMenu(tab.uniqueID);
            if (tabIndex == -1)
            { // Adding
                mMenuTabs.push_back(tab);
                tabIndex = mMenuTabs.size() - 1;
            }
            else
            { // Updating
                mMenuTabs[tabIndex] = tab;
            }
            // initializeMenuSlices(false);

            // for (int i = 0; i < mMenuTabs.size(); i++) {
            uiVizWidgetTabSlice *tabSlice = &tabSlices[tabIndex];
            if (!tabSlice || tabSlice == nullptr)
                return;
            tabSlice->menuTab = tab;

            vector<uiVizWidgetMenuSlice> menuSlices = vector<uiVizWidgetMenuSlice>();

            for (int j = 0; j < tabSlice->menuTab.menuItems.size(); j++)
            {
                uiVizWidgetMenuSlice menuItemSlice;
                menuItemSlice.menuItem = tabSlice->menuTab.menuItems[j];
                menuSlices.push_back(menuItemSlice);
            }

            if (tabSlice->menuTab.tabWidget != NULL)
            {
                // if (isVeryFirstInit) {
                tabSlice->menuTab.tabWidget->setTitleStyle(TitleStyle::NONE);
                tabSlice->menuTab.tabWidget->setDrawWidgetTitle(false);
                // }
                tabSlice->menuTab.tabWidget->setIsVisible(false);
                addChildWidget(*tabSlice->menuTab.tabWidget);
                ofAddListener(tabSlice->menuTab.tabWidget->widgetEventReceived, this, &uiVizWidgetMenu::onTabWidgetEventReceived);
            }

            menuSliceData.push_back(menuSlices);
            // }
        }

        void clearSelectedMenuItems(int tabMenuId)
        {

            int tabIndex = getIndexForTabMenu(tabMenuId);

            if (tabIndex == -1)
                return;

            for (int j = 0; j < menuSliceData[tabIndex].size(); j++)
            {
                uiVizWidgetMenuSlice *unselectSlice = &menuSliceData[tabIndex][j];
                unselectSlice->menuItem.isSelected = false;
            }
        }

        vector<uiVizWidgetMenuSlice> *getMenuTabSlice(int tabMenuId)
        {
            int tabIndex = getIndexForTabMenu(tabMenuId);
            if (tabIndex == -1)
                return nullptr;
            return &menuSliceData[tabIndex];
        }

        void updateMinimized() override
        {
        }

        void drawMinimized() override
        {
        }

        void onWidgetMouseScrolled(ofMouseEventArgs &e) override
        {
            uiVizWidgetTabSlice *tabSlice = &tabSlices[mActiveMenuTabIndex];
            if (tabSlice != nullptr)
            {
                tabSlice->scrollOffsetX = getScrollOffsetX();
                tabSlice->scrollOffsetY = getScrollOffsetY();
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

        TabLocation getTabLocation()
        {
            return mTabLocation;
        }

        void setTabLocation(TabLocation val)
        {
            mTabLocation = val;
        }

        int getUsableX(bool inclScrollOffset) override
        {
            return (getTabLocation() == TabLocation::LEFT) ? uiVizWidget::getUsableX(inclScrollOffset) + getTabWidth() + getNonScaledPadding() : uiVizWidget::getUsableX(inclScrollOffset);
        }

        int getUsableY(bool inclScrollOffset) override
        {
            return (getTabLocation() == TabLocation::TOP) ? uiVizWidget::getUsableY(inclScrollOffset) + getTabHeight() + getNonScaledPadding() : uiVizWidget::getUsableY(inclScrollOffset);
        }

        int getUsableWidth() override
        {
            return (getTabLocation() == TabLocation::LEFT || getTabLocation() == TabLocation::RIGHT) ? uiVizWidget::getUsableWidth() - getTabWidth() - getNonScaledPadding() : uiVizWidget::getUsableWidth();
        }

        int getUsableHeight() override
        {
            return (getTabLocation() == TabLocation::TOP || getTabLocation() == TabLocation::BOTTOM) ? uiVizWidget::getUsableHeight() - getTabHeight() - getNonScaledPadding() : uiVizWidget::getUsableHeight();
        }

        bool getIsTabbedMenu()
        {
            return mMenuTabs.size() > 0;
        }

        vector<uiVizWidgetMenuTab> getMenuTabs()
        {
            return mMenuTabs;
        }

        int getIndexForTabMenu(int tabMenuId)
        {
            for (int i = 0; i < tabSlices.size(); i++)
            {
                uiVizWidgetTabSlice *slice = &tabSlices[i];
                if (slice != nullptr && tabMenuId == slice->menuTab.uniqueID)
                    return i;
            }
            return -1;
        }

        int getIdForTabMenuIndex(int tabMenuIndex)
        {
            for (int i = 0; i < tabSlices.size(); i++)
            {
                uiVizWidgetTabSlice *slice = &tabSlices[i];
                if (slice != nullptr && tabMenuIndex == i)
                    return slice->menuTab.uniqueID;
            }
            return -1;
        }

        int getIndexForTabMenuItem(int tabIndex, int menuItemId)
        {
            int idForTabMenu = getIdForTabMenuIndex(tabIndex);
            if (idForTabMenu == -1)
                return -1;
            if (mMenuTabs[tabIndex].menuItems.size() == 0)
                return -1;

            for (int i = 0; i < mMenuTabs[tabIndex].menuItems.size(); i++)
            {
                if (menuItemId == mMenuTabs[tabIndex].menuItems[i].uniqueID)
                    return i;
            }
            return -1;
        }

        vector<uiVizWidgetMenuItem> getActiveMenu()
        {
            if (mMenuTabs.size() > 0 && mMenuTabs.size() - 1 >= mActiveMenuTabIndex)
            {
                return mMenuTabs[mActiveMenuTabIndex].menuItems;
            }
            return vector<uiVizWidgetMenuItem>();
        }

        int getActiveMenuTabIndex()
        {
            return mActiveMenuTabIndex;
        }

        void setActiveMenuTabId(int tabMenuId)
        {
            int index = getIndexForTabMenu(tabMenuId);
            setActiveMenuTabIndex(index);
        }

        void setActiveMenuTabIndex(int index)
        {

            if (!getIsTabbedMenu())
                return;

            index = std::max((int)0, (int)index);
            index = std::min((int)mMenuTabs.size() - 1, (int)index);
            mActiveMenuTabIndex = index;
            mMenuItems = mMenuTabs[mActiveMenuTabIndex].menuItems;
            setTitle(mMenuTabs[mActiveMenuTabIndex].label);

            // Clear the others
            for (int j = 0; j < tabSlices.size(); j++)
            {
                uiVizWidgetTabSlice *unselectSlice = &tabSlices[j];
                unselectSlice->menuTab.isSelected = false;
            }

            mUpdatedSinceMenuItemChangedWidth = false;
            mUpdatedSinceMenuItemChangedHeight = false;

            setNeedsUpdate(true);
            setWidgetNeedsUpdate(true);
        }

        int getMenuTabsSize()
        {
            return mMenuTabs.size();
        }

        int getMenuItemsSize()
        {
            return mMenuItems.size();
        }

        bool getIsMenuPinned()
        {
            return mIsMenuPinned;
        }

        void setIsMenuPinned(bool val)
        {
            mIsMenuPinned = val;
        }

        bool getShowMenuPin()
        {
            return mShowMenuPin;
        }

        void setShowMenuPin(bool val)
        {
            mShowMenuPin = val;
        }

    private:
        PreferredPopoutDirection mPreferredDirection;
        vector<uiVizWidgetMenuItem> mMenuItems = vector<uiVizWidgetMenuItem>();
        vector<uiVizWidgetMenuTab> mMenuTabs = vector<uiVizWidgetMenuTab>(); // TODO -> set mMenuItems -> mMenuTabs[activeIndex]

        vector<uiVizWidgetTabSlice> tabSlices = vector<uiVizWidgetTabSlice>();

        // Initialize this ONCE and then use ...
        // Manipulation of this vector should be done only by public methods
        // which re sync menuSliceData with the menuItems data model.
        vector<vector<uiVizWidgetMenuSlice>> menuSliceData;

        uiVizElm vizElm_tabBoundBox;
        uiVizElm vizElm_menuHighlight;
        uiVizElm vizElm_tabHighlight;
        shared_ptr<ofxSmartFont> font;
        float mTabWidth = 40;
        float mTabHeight = 40;
        TabLocation mTabLocation = TabLocation::LEFT;
        int mActiveMenuTabIndex = 0;
        int mActiveMenuTabIndexPrev = 0;
        bool mUpdatedSinceMenuItemChangedWidth = false;
        bool mUpdatedSinceMenuItemChangedHeight = false;

        bool mIsMenuPinned = false;
        bool mShowMenuPin = true;
        bool mTabWidgetWasResized = false;
        bool mMenuSlicesInitialized = false;

        /* Todo
        virtual void initWidgetScale(float scale) override {
            for (uiVizWidgetMenuTab& menuTab:mMenuTabs) {
                uiVizIconCache::scale(menuTab.icon, scale);
            }
        }
        */

        void initWidget() override
        {
            setIsVisible(false);
            hideTabWidgets();
            setIsAutoUpdateWhenActive(true);
            DefaultMenuItem = new uiVizWidgetMenuItem("", -1);
            args = new uiVizWidgetMenuItemArgs(0, DefaultMenuItem);
            setTitleStyle(TitleStyle::BOTTOM_STATIC);
            setScrollAction(ScrollAction::SCROLL);

            setIsAutoResizeToContentsWidth(true);
            setIsAutoResizeToContentsHeight(true);

            setEnableScrollYPositive(true);
            setEnableScrollYNegative(true);
            setEnableScrollXPositive(false);
            setEnableScrollXNegative(false);

            setMinWidth(150);
            // setMaxHeight(300);

            DefaultMenuTab = new uiVizWidgetMenuTab("", -1, mMenuItems);
            tabArgs = new uiVizWidgetMenuTabArgs(0, 0, DefaultMenuTab);
            if (!mMenuSlicesInitialized)
            {
                initializeMenuSlices(true);
                mMenuSlicesInitialized = true;
            }
        }

        void toggleMenuItem(int tabMenuId, string menuItemTag, bool fireEvent)
        {
            int tabIndex = getIndexForTabMenu(tabMenuId);
            vector<uiVizWidgetMenuSlice> *menuSlices = getMenuTabSlice(tabMenuId);
            if (menuSlices == nullptr)
                return;
            vector<uiVizWidgetMenuSlice> &menuSlicesRef = *menuSlices;

            for (int i = 0; i < menuSlicesRef.size(); i++)
            {
                if (menuSlicesRef[i].menuItem.tag == menuItemTag)
                {
                    bool newVal = !menuSlicesRef[i].menuItem.isSelected;
                    setMenuItemSelectedVal(tabMenuId, menuItemTag, newVal, fireEvent);

                    if (fireEvent)
                    {
                        uiVizWidgetMenuItemArgs args(tabMenuId, &menuSlicesRef[i].menuItem);
                        ofNotifyEvent(menuItemSelected, args);
                        hideMenuAfterSelection();
                    }
                }
            }
        }

        void toggleMenuItem(int tabMenuId, int menuItemId, bool fireEvent)
        {

            int tabIndex = getIndexForTabMenu(tabMenuId);
            int menuItemIndex = getIndexForTabMenuItem(tabIndex, menuItemId);

            if (tabIndex == -1 || menuItemIndex == -1)
                return;

            uiVizWidgetMenuSlice *slice = &menuSliceData[tabIndex][menuItemIndex];
            uiVizWidgetMenuItem *menuItem = &slice->menuItem;

            bool newVal = !slice->menuItem.isSelected;
            setMenuItemSelectedVal(tabMenuId, menuItemId, newVal, fireEvent);
        }
    };

}