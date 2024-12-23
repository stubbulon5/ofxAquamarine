
#pragma once
#include "WidgetMenu.h"

namespace Aquamarine
{
    class WidgetClipboardMenu : public Widget
    {

    public:
        WidgetClipboardMenu(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
        {
            initWidget();
        }

        virtual ~WidgetClipboardMenu()
        {
        }

        void update(WidgetContext context) override
        {
        }

        void draw(WidgetContext context) override
        {
        }

        void showClipboardMenu(int x, int y, string clipboardOperationTargetWidgetId)
        {
            setShowingAsPopout(true);
            setIsVisible(true);
            clipboardMenu->setShowingAsPopout(true);
            if (clipboardMenu->getIsVisible())
                return;
            setClipboardOperationTargetWidgetId(clipboardOperationTargetWidgetId);
            clipboardMenu->clearSelectedMenuItems(IconCache::IconTag::WIDGET_CLIPBOARD);

            clipboardMenu->setMenuItemEnabled(IconCache::IconTag::WIDGET_CLIPBOARD, 0, getAllowCut());
            clipboardMenu->setMenuItemEnabled(IconCache::IconTag::WIDGET_CLIPBOARD, 1, getAllowCopy());
            clipboardMenu->setMenuItemEnabled(IconCache::IconTag::WIDGET_CLIPBOARD, 2, getAllowPaste());

            clipboardMenu->setWidgetPosition(x, y, false);
            clipboardMenu->setWidgetNeedsUpdate(true);
            clipboardMenu->setIsVisible(true);
            clipboardMenu->setActiveWidget();
        }

        void hideClipboardMenu(bool force)
        {
            WidgetManager::moveWidgetToBack(*this);
            if (!force && !mMenuHasBeenHovered)
                return;
            clipboardMenu->setIsVisible(false);

            WidgetBase *target = WidgetManager::getWidgetBase(getClipboardOperationTargetWidgetId());
            if (target != nullptr)
            {
                target->showClipboardForThisWidget(false);
            }
            setIsVisible(false);
        }

        WidgetMenu *getMenu()
        {
            return clipboardMenu;
        }

        void onMenuItemSelected(WidgetMenuItemArgs &args)
        {

            WidgetBase *target = WidgetManager::getWidgetBase(getClipboardOperationTargetWidgetId());
            if (target == nullptr)
                return;

            hideClipboardMenu(true);

            switch (args.activeMenuTabId)
            {

            case IconCache::IconTag::WIDGET_CLIPBOARD:
                switch (args.menuItem->uniqueID)
                {
                case 0: // Cut
                    break;
                case 1: // Copy
                    ofSetClipboardString(target->getSelectionDataForClipboard());
                    break;
                case 2: // Paste
                    string clipboard = Shared::encodeForXML(ofGetClipboardString());

                    WidgetElm *elm = dynamic_cast<WidgetElm *>(target);
                    if (elm && elm->parent())
                    {
                        Widget *w = dynamic_cast<Widget *>(elm->parent());
                        if (w)
                        {
                            w->focusWidgetElement(*elm);
                            w->setActiveWidget();
                        }
                    }
                    else
                    {
                        target->setActiveWidget();
                    }

                    target->eventTransmit(getPersistentId(), WIDGET_EVENT::CLIPBOARD_PASTE, "<event>" + clipboard + "</event>", *this);
                    break;
                }
                break;
            }
        }

        void onMenuLostFocus(WidgetEventArgs &args)
        {
            hideClipboardMenu(false);
            mMenuHasBeenHovered = false;
        }

        void onMenuHovered(WidgetEventArgs &args)
        {
            mMenuHasBeenHovered = true;
        }

        void setClipboardOperationTargetWidgetId(string val)
        {
            mClipboardOperationTargetWidgetId = val;
        }

        string getClipboardOperationTargetWidgetId()
        {
            return mClipboardOperationTargetWidgetId;
        }

        void setAllowCut(bool val)
        {
            mAllowCut = val;
        }

        bool getAllowCut()
        {
            return mAllowCut;
        }

        void setAllowCopy(bool val)
        {
            mAllowCopy = val;
        }

        bool getAllowCopy()
        {
            return mAllowCopy;
        }

        void setAllowPaste(bool val)
        {
            mAllowPaste = val;
        }

        bool getAllowPaste()
        {
            return mAllowPaste;
        }

    private:
        WidgetMenu *clipboardMenu = nullptr;
        string mClipboardOperationTargetWidgetId = "";
        bool mMenuHasBeenHovered = false;
        bool mAllowCut = true;
        bool mAllowCopy = true;
        bool mAllowPaste = true;

        void initWidget() override
        {

            if (clipboardMenu == nullptr)
            {
                clipboardMenu = new WidgetMenu("CLIPBOARD_MENU", "<widget isSystemWidget='1'><bounds width='100' height='100'/></widget>", getWidgetId(), WidgetMenu::PreferredPopoutDirection::DOWN,
                                                    {
                                                        // ----------------------------------------------------------------------------
                                                        // Clipboard...
                                                        // ----------------------------------------------------------------------------
                                                        WidgetMenuTab("Clipboard", IconCache::getIcon("MED_WIDGET_CLIPBOARD"), IconCache::IconTag::WIDGET_CLIPBOARD, {WidgetMenuItem("Cut", 0), WidgetMenuItem("Copy", 1), WidgetMenuItem("Paste", 2)}),
                                                    });

                ofAddListener(dynamic_cast<WidgetMenu *>(clipboardMenu)->menuItemSelected, this, &WidgetClipboardMenu::onMenuItemSelected);
                clipboardMenu->setIsVisible(false);
                clipboardMenu->setIsRoundedRectangle(false);

                clipboardMenu->setIsDraggable(false);
                clipboardMenu->setIsResizable(false);
                clipboardMenu->setIsUnhoverLoseFocus(false);

                ofAddListener(clipboardMenu->widgetLostFocus, this, &WidgetClipboardMenu::onMenuLostFocus);
                ofAddListener(clipboardMenu->widgetHovered, this, &WidgetClipboardMenu::onMenuHovered);

                addChildWidget(*clipboardMenu);
                clipboardMenu->setIsShadow(true);
            }
        }
    };

}