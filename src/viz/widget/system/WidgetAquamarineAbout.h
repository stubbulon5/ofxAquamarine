
#pragma once
#include "../Widget.h"

namespace Aquamarine
{
    class WidgetAquamarineAbout : public Widget
    {

    public:
        WidgetAquamarineAbout(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            Widget::loadState(widgetXML);

            ofxXmlSettings widgetSettings = ofxXmlSettings();
            if (!widgetSettings.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            return true;
        }

        virtual void onChildWidgetsLoaded() override
        {
            Widget *w = getChildWidgetByPersistentId("ABOUT_TAB_TEXT_HOLDER");
            if (w)
            {
                WidgetElmTextarea *TXT_ABOUT = w->getTextarea("TXT_ABOUT");
                if (TXT_ABOUT)
                {
                    string replaceText = TXT_ABOUT->getValue();
                    ofStringReplace(replaceText, "${FRAMEWORK_NAME}", WidgetManager::getFrameworkName());
                    ofStringReplace(replaceText, "${FRAMEWORK_VER}", WidgetManager::getFrameworkVersion());
                    TXT_ABOUT->setValue(replaceText);
                }
            }

            if (!loaded)
            {
                ABOUT_TABS = dynamic_cast<WidgetMenuCollection *>(WidgetManager::getWidgetByPersistentId(this, "ABOUT_TABS"));
                ABOUT_TAB_ATTRIBUTIONS = dynamic_cast<WidgetTable *>(WidgetManager::getWidgetByPersistentId(this, "ABOUT_TAB_ATTRIBUTIONS"));
                ABOUT_TAB_CONTACT_MENU = dynamic_cast<WidgetMenu *>(WidgetManager::getWidgetByPersistentId(this, "ABOUT_TAB_CONTACT_MENU"));

                // Respond to when user selected an option
                ABOUT_TAB_CONTACT_MENU->setMenuItemSelectedCallback([&, this](WidgetMenuItemArgs args)
                {
                    if (args.menuItem->tag == "bug")
                    {
                        ofLaunchBrowser("https://github.com/stubbulon5/ofxAquamarine/issues/new");
                    }
                    else if (args.menuItem->tag == "feature")
                    {
                        ofLaunchBrowser("https://github.com/stubbulon5/ofxAquamarine/discussions");
                    }
                    else if (args.menuItem->tag == "contact")
                    {
                        ofLaunchBrowser("https://github.com/stubbulon5");
                    } 
                });

                loaded = true;
            }
        }

        virtual ~WidgetAquamarineAbout()
        {
            // ofRemoveListener(ABOUT_TABS->widgetEventReceived, this, &WidgetAquamarineAbout::onAboutTabsEventReceived);
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
            string parentWidgetId = args.sender.getParentPersistentId();
            string event = args.getFullEventName();

            if (args.sender.matchesWidgetOrParent(ABOUT_TAB_ATTRIBUTIONS) && args.eventName == WIDGET_EVENT::TABLE_CELL_CLICKED)
            {

                WidgetTableRow *eventRow = dynamic_cast<WidgetTable *>(&args.sender)->getTableRow(args.eventXML);
                WidgetTableCell *eventCell = dynamic_cast<WidgetTable *>(&args.sender)->getTableCell(args.eventXML);

                ofLaunchBrowser(eventRow->key);
            }
            else if (args.sender.matchesWidgetOrParent(ABOUT_TAB_CONTACT_MENU))
            {

                cout << "zzz";
            }
            else if (args.sender.matchesWidgetOrChildOf(this))
            {
                if (event == "OK." + WIDGET_EVENT::CLICK)
                {
                    closeThisWidget();
                }
            }
        }

    private:
        WidgetMenuCollection *ABOUT_TABS;
        WidgetTable *ABOUT_TAB_ATTRIBUTIONS;
        WidgetMenu *ABOUT_TAB_CONTACT_MENU;

        bool loaded = false;

        void initWidget() override
        {
        }
    };
}
