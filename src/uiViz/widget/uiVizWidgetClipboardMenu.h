
#pragma once
#include "uiVizWidgetMenu.h"

namespace Aquamarine
{
    class uiVizWidgetClipboardMenu : public uiVizWidget
    {

    public:
        uiVizWidgetClipboardMenu(string persistentId, string widgetXML) : uiVizWidget(persistentId, widgetXML)
        {
            initWidget();
        }

        virtual ~uiVizWidgetClipboardMenu()
        {
        }

        void update(uiVizWidgetContext context) override
        {
        }

        void draw(uiVizWidgetContext context) override
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
            clipboardMenu->clearSelectedMenuItems(uiVizIconCache::IconTag::WIDGET_CLIPBOARD);

            clipboardMenu->setMenuItemEnabled(uiVizIconCache::IconTag::WIDGET_CLIPBOARD, 0, getAllowCut());
            clipboardMenu->setMenuItemEnabled(uiVizIconCache::IconTag::WIDGET_CLIPBOARD, 1, getAllowCopy());
            clipboardMenu->setMenuItemEnabled(uiVizIconCache::IconTag::WIDGET_CLIPBOARD, 2, getAllowPaste());

            clipboardMenu->setWidgetPosition(x, y, false);
            clipboardMenu->setWidgetNeedsUpdate(true);
            clipboardMenu->setIsVisible(true);
            clipboardMenu->setActiveWidget();
        }

        void hideClipboardMenu(bool force)
        {
            uiVizWidgetManager::moveWidgetToBack(*this);
            if (!force && !mMenuHasBeenHovered)
                return;
            clipboardMenu->setIsVisible(false);

            uiVizWidgetBase *target = uiVizWidgetManager::getWidgetBase(getClipboardOperationTargetWidgetId());
            if (target != nullptr)
            {
                target->showClipboardForThisWidget(false);
            }
            setIsVisible(false);
        }

        uiVizWidgetMenu *getMenu()
        {
            return clipboardMenu;
        }

        void onMenuItemSelected(uiVizWidgetMenuItemArgs &args)
        {

            uiVizWidgetBase *target = uiVizWidgetManager::getWidgetBase(getClipboardOperationTargetWidgetId());
            if (target == nullptr)
                return;

            hideClipboardMenu(true);

            switch (args.activeMenuTabId)
            {

            case uiVizIconCache::IconTag::WIDGET_CLIPBOARD:
                switch (args.menuItem->uniqueID)
                {
                case 0: // Cut
                    break;
                case 1: // Copy
                    ofSetClipboardString(target->getSelectionDataForClipboard());
                    break;
                case 2: // Paste
                    string clipboard = uiVizShared::encodeForXML(ofGetClipboardString());

                    uiVizWidgetElm *elm = dynamic_cast<uiVizWidgetElm *>(target);
                    if (elm && elm->parent())
                    {
                        uiVizWidget *w = dynamic_cast<uiVizWidget *>(elm->parent());
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

        void onMenuLostFocus(uiVizWidgetEventArgs &args)
        {
            hideClipboardMenu(false);
            mMenuHasBeenHovered = false;
        }

        void onMenuHovered(uiVizWidgetEventArgs &args)
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
        uiVizWidgetMenu *clipboardMenu = nullptr;
        string mClipboardOperationTargetWidgetId = "";
        bool mMenuHasBeenHovered = false;
        bool mAllowCut = true;
        bool mAllowCopy = true;
        bool mAllowPaste = true;

        void initWidget() override
        {

            if (!clipboardMenu)
            {
                clipboardMenu = new uiVizWidgetMenu("CLIPBOARD_MENU", "<widget isSystemWidget='1'><bounds width='100' height='100'/></widget>", getWidgetId(), uiVizWidgetMenu::PreferredPopoutDirection::DOWN,
                                                    {
                                                        // ----------------------------------------------------------------------------
                                                        // Clipboard...
                                                        // ----------------------------------------------------------------------------
                                                        uiVizWidgetMenuTab("Clipboard", uiVizIconCache::getIcon("MED_WIDGET_CLIPBOARD"), uiVizIconCache::IconTag::WIDGET_CLIPBOARD, {uiVizWidgetMenuItem("Cut", 0), uiVizWidgetMenuItem("Copy", 1), uiVizWidgetMenuItem("Paste", 2)}),
                                                    });

                ofAddListener(dynamic_cast<uiVizWidgetMenu *>(clipboardMenu)->menuItemSelected, this, &uiVizWidgetClipboardMenu::onMenuItemSelected);
                clipboardMenu->setIsVisible(false);
                clipboardMenu->setIsRoundedRectangle(false);

                clipboardMenu->setIsDraggable(false);
                clipboardMenu->setIsResizable(false);
                clipboardMenu->setIsUnhoverLoseFocus(false);

                ofAddListener(clipboardMenu->widgetLostFocus, this, &uiVizWidgetClipboardMenu::onMenuLostFocus);
                ofAddListener(clipboardMenu->widgetHovered, this, &uiVizWidgetClipboardMenu::onMenuHovered);

                addChildWidget(*clipboardMenu);
                clipboardMenu->setIsShadow(true);
            }
        }
    };

}