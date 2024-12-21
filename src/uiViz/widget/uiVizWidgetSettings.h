
#pragma once
#include "uiVizWidgetMenu.h"

namespace Aquamarine
{
    class uiVizWidgetSettings : public uiVizWidget
    {

    public:
        uiVizWidgetSettings(string persistentId, string widgetXML) : uiVizWidget(persistentId, widgetXML)
        {
            initWidget();
        }

        virtual ~uiVizWidgetSettings()
        {
            ofRemoveListener(settingsMenu->menuTabSelected, this, &uiVizWidgetSettings::onMenuTabSelected);
            ofRemoveListener(settingsMenu->menuItemSelected, this, &uiVizWidgetSettings::onMenuItemSelected);
            ofRemoveListener(settingsMenu->widgetUnhovered, this, &uiVizWidgetSettings::onMenuUnhovered);
        }

        void update(uiVizWidgetContext context) override
        {
        }

        void draw(uiVizWidgetContext context) override
        {
            drawWidgetContentBlock();
        }

        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {

            string event = args.getFullEventName();

            uiVizWidgetElmSlider *SLD_UI_SCALE = getSlider("SLD_UI_SCALE");
            uiVizWidgetElmSlider *SLD_EXPERIENCE = getSlider("SLD_EXPERIENCE");
            uiVizWidgetElmCheckbox *CHK_USE_GPU = getCheckbox("CHK_USE_GPU");
            uiVizWidgetElmCheckbox *CHK_SHOW_FPS = getCheckbox("CHK_SHOW_FPS");
            uiVizWidgetElmCheckbox *CHK_AUTOLOAD_RECENT = getCheckbox("CHK_AUTOLOAD_RECENT");

            uiVizWidgetElmCheckbox *CHK_OSC_SEND = getCheckbox("CHK_OSC_SEND");
            uiVizWidgetElmCheckbox *CHK_OSC_RECEIVE = getCheckbox("CHK_OSC_RECEIVE");

            if (event == "OK." + WIDGET_EVENT::CLICK)
            {
                uiVizShared::setUserScale(SLD_UI_SCALE->getValue());
                uiVizShared::setUserExperience(SLD_EXPERIENCE->getValue(), true);
                uiVizShared::setUseFbo(CHK_USE_GPU->getValue());
                uiVizShared::setShowFps(CHK_SHOW_FPS->getValue());

                if (CHK_OSC_SEND->getValue())
                {
                    uiVizWidgetElmTextbox *TXT_OSC_SEND_HOST = getTextbox("TXT_OSC_SEND_HOST");
                    uiVizWidgetElmTextbox *TXT_OSC_SEND_PORT = getTextbox("TXT_OSC_SEND_PORT");

                    string host = TXT_OSC_SEND_HOST->getValue();
                    int port = TXT_OSC_SEND_PORT->getValue(-1);
                    if (host != "" && port > -1)
                    {
                        getViz()->setupOscSender(host, port);
                    }
                }

                if (CHK_OSC_RECEIVE->getValue())
                {
                    uiVizWidgetElmTextbox *TXT_OSC_RECEIVE_PORT = getTextbox("TXT_OSC_RECEIVE_PORT");
                    int port = TXT_OSC_RECEIVE_PORT->getValue(-1);
                    if (port > -1)
                    {
                        getViz()->setupOscReceiver(port);
                    }
                }

                saveSettings();
                closeThisWidget();
            }

            if (event == "CANCEL." + WIDGET_EVENT::CLICK)
            {
                closeThisWidget();
            }
        }

        virtual void afterLoad(ofxXmlSettings settings)
        { // Override in descendant for after load hook
        }

        static ofxXmlSettings getSettings()
        {
            ofxXmlSettings settings;
            string settingsFile = uiVizShared::getSettingsFileFullPath();
            if (!settings.loadFile(settingsFile))
            {
                string exceptionStr = "Invalid Settings file. Set it using 'uiVizShared::initSettingsFile' Terminating...";
                throw std::runtime_error(exceptionStr.c_str());
                OF_EXIT_APP(1);
            }
            return settings;
        }

        void loadSettings()
        {
            ofxXmlSettings settings = getSettings();

            uiVizWidgetElmSlider *SLD_UI_SCALE = getSlider("SLD_UI_SCALE");
            uiVizWidgetElmSlider *SLD_EXPERIENCE = getSlider("SLD_EXPERIENCE");
            uiVizWidgetElmCheckbox *CHK_USE_GPU = getCheckbox("CHK_USE_GPU");
            uiVizWidgetElmCheckbox *CHK_SHOW_FPS = getCheckbox("CHK_SHOW_FPS");
            uiVizWidgetElmCheckbox *CHK_AUTOLOAD_RECENT = getCheckbox("CHK_AUTOLOAD_RECENT");

            SLD_UI_SCALE->setValue(settings.getValue("settings:userInterfaceScaling", 1.0), true);
            SLD_EXPERIENCE->setValue(settings.getValue("settings:userExperience", 60), true);
            CHK_USE_GPU->setValue(settings.getValue("settings:useFbo", true), true, false);
            CHK_SHOW_FPS->setValue(settings.getValue("settings:showFps", false), true, false);
            CHK_AUTOLOAD_RECENT->setValue(settings.getValue("settings:autoLoadMostRecentProject", true), true, false);

            string themeName = settings.getValue("settings:themeName", "");
            settingsMenu->selectMenuItemByLabel(uiVizIconCache::IconTag::WIDGET_THEMES, themeName, false);

            afterLoad(settings);
        }

        virtual void beforeSave(ofxXmlSettings settings)
        { // Override in descendant for save hook
        }

        void saveSettings()
        {
            ofxXmlSettings settings = getSettings();

            uiVizWidgetElmSlider *SLD_UI_SCALE = getSlider("SLD_UI_SCALE");
            uiVizWidgetElmSlider *SLD_EXPERIENCE = getSlider("SLD_EXPERIENCE");
            uiVizWidgetElmCheckbox *CHK_USE_GPU = getCheckbox("CHK_USE_GPU");
            uiVizWidgetElmCheckbox *CHK_SHOW_FPS = getCheckbox("CHK_SHOW_FPS");
            uiVizWidgetElmCheckbox *CHK_AUTOLOAD_RECENT = getCheckbox("CHK_AUTOLOAD_RECENT");

            settings.setValue("settings:userExperience", SLD_EXPERIENCE->getValue());
            settings.setValue("settings:useFbo", CHK_USE_GPU->getValue());
            settings.setValue("settings:showFps", CHK_SHOW_FPS->getValue());
            settings.setValue("settings:language", "english");
            settings.setValue("settings:fontPath", "fonts/Verdana.ttf");
            settings.setValue("settings:userInterfaceScaling", SLD_UI_SCALE->getValue());
            settings.setValue("settings:autoLoadMostRecentProject", CHK_AUTOLOAD_RECENT->getValue());
            settings.setValue("settings:themeName", selectedTheme.Name);

            beforeSave(settings);
            settings.saveFile(uiVizShared::getSettingsFileFullPath()); // puts settings.xml file in the bin/data folder
        }

        virtual void onMenuItemSelected(uiVizWidgetMenuItemArgs &args)
        {

            switch (args.activeMenuTabId)
            {

            case uiVizIconCache::IconTag::WIDGET_SETTINGS:
                switch (args.menuItem->uniqueID)
                {
                case 0:
                    break;
                }
                break;

            case uiVizIconCache::IconTag::WIDGET_THEMES:
            {
                selectedTheme = uiVizShared::getViz()->getThemeManager()->getThemeByName(args.menuItem->label);
                uiVizShared::getViz()->loadTheme(selectedTheme.Name, true);
                uiVizWidgetManager::setTheme(selectedTheme);
            }
            break;
            }
        }

        void onMenuTabSelected(uiVizWidgetMenuTabArgs &args)
        {
            setWidgetNeedsUpdate(true);
            setNeedsUpdate(true);
            setExpressionRecalculationRequired(true);
        }

        void onMenuUnhovered(uiVizWidgetEventArgs &args)
        {
        }

        // Correctly calculate the usable x and width for child elements to use
        int getUsableX() override
        {
            return uiVizWidget::getUsableX(false) + settingsMenu->getWidth();
        }

        int getUsableWidth() override
        {
            return uiVizWidget::getUsableWidth() - settingsMenu->getWidth();
        }

        uiVizWidgetMenu *getSettingsMenu()
        {
            return settingsMenu;
        }

        vector<uiVizWidgetMenuItem> getThemesMenuItems()
        {
            vector<uiVizWidgetMenuItem> menuItems = vector<uiVizWidgetMenuItem>();
            int i = 0;
            for (uiVizWidgetTheme theme : uiVizShared::getViz()->getThemeManager()->getThemes())
            {
                menuItems.push_back(uiVizWidgetMenuItem(theme.Name, i++));
            }
            return menuItems;
        }

    private:
        uiVizWidgetMenu *settingsMenu;
        uiVizWidgetTheme selectedTheme;
        bool loaded = false;

        void initWidget() override
        {

            uiVizWidgetManager::removeWidget(*settingsMenu);

            settingsMenu = new uiVizWidgetMenu("APP_SETTINGS_MENU", "<widget><bounds width='100' height='100'/></widget>", getWidgetId(), uiVizWidgetMenu::PreferredPopoutDirection::DOWN, {// ----------------------------------------------------------------------------
                                                                                                                                                                                            // General settings...
                                                                                                                                                                                            // ----------------------------------------------------------------------------
                                                                                                                                                                                            uiVizWidgetMenuTab("Settings", uiVizIconCache::getIcon("REG_WIDGET_SETTINGS"), uiVizIconCache::IconTag::WIDGET_SETTINGS, {}),

                                                                                                                                                                                            uiVizWidgetMenuTab("Color", uiVizIconCache::getIcon("MED_CONTENT_THEORY_COLOR"), uiVizIconCache::IconTag::WIDGET_THEMES, {getThemesMenuItems()})

                                                                                                                                                                                           });

            ofAddListener(settingsMenu->menuTabSelected, this, &uiVizWidgetSettings::onMenuTabSelected);
            ofAddListener(settingsMenu->menuItemSelected, this, &uiVizWidgetSettings::onMenuItemSelected);
            ofAddListener(settingsMenu->widgetUnhovered, this, &uiVizWidgetSettings::onMenuUnhovered);

            setTitleStyle(uiVizWidget::TitleStyle::TOP_STATIC);

            settingsMenu->setIsVisible(true);
            settingsMenu->getTheme().HoveredWidgetAlpha = 0.7f;
            settingsMenu->getTheme().HoveredTitleAlpha = 0.8f;
            settingsMenu->getTheme().UnhoveredWidgetAlpha = 0.65f;
            settingsMenu->getTheme().UnhoveredTitleAlpha = 0.75f;
            settingsMenu->setIsAutoResizeToContentsHeight(false);

            settingsMenu->setIsDraggable(false);
            settingsMenu->setIsResizable(false);
            // settingsMenu->setIsUnhoverLoseFocus(true);
            settingsMenu->setIsRoundedRectangle(false);
            settingsMenu->setIsMenuPinned(true);
            settingsMenu->setTitleStyle(TitleStyle::NONE);
            setMinHeight(200);

            settingsMenu->setX_Expr("${" + getPersistentId() + ".ABSOLUTE_USABLE_X_FIXED}-${PADDING}");
            settingsMenu->setY_Expr("${" + getPersistentId() + ".ABSOLUTE_USABLE_Y_FIXED}");
            settingsMenu->setHeight_Expr("${" + getPersistentId() + ".USABLE_HEIGHT}");
            addChildWidget(*settingsMenu);

            if (!loaded)
            {
                uiVizWidgetManager::loadWidgetFromFileToExisting("ui/widgets/settings.xml", *this);
                loaded = true;
                loadSettings();
            }
        }
    };

}