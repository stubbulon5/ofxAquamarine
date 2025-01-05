
#pragma once
#include "WidgetMenu.h"
#include "core/nlohmann/json.hpp"
using json = nlohmann::json;

namespace Aquamarine
{
    class WidgetSettings : public Widget
    {

    public:
        WidgetSettings(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
        {
            initWidget();
        }

        virtual ~WidgetSettings()
        {
            ofRemoveListener(settingsMenu->menuTabSelected, this, &WidgetSettings::onMenuTabSelected);
            ofRemoveListener(settingsMenu->menuItemSelected, this, &WidgetSettings::onMenuItemSelected);
            ofRemoveListener(settingsMenu->widgetUnhovered, this, &WidgetSettings::onMenuUnhovered);
        }

        void update(WidgetContext context) override
        {
        }

        void draw(WidgetContext context) override
        {
            drawWidgetContentBlock();
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {

            string event = args.getFullEventName();

            WidgetElmSlider *SLD_UI_SCALE = getSlider("SLD_UI_SCALE");
            WidgetElmSlider *SLD_EXPERIENCE = getSlider("SLD_EXPERIENCE");
            WidgetElmCheckbox *CHK_USE_GPU = getCheckbox("CHK_USE_GPU");
            WidgetElmCheckbox *CHK_SHOW_FPS = getCheckbox("CHK_SHOW_FPS");
            WidgetElmCheckbox *CHK_AUTOLOAD_RECENT = getCheckbox("CHK_AUTOLOAD_RECENT");

            WidgetElmCheckbox *CHK_OSC_SEND = getCheckbox("CHK_OSC_SEND");
            WidgetElmCheckbox *CHK_OSC_RECEIVE = getCheckbox("CHK_OSC_RECEIVE");

            if (event == "OK." + WIDGET_EVENT::CLICK)
            {
                Shared::setUserScale(SLD_UI_SCALE->getValue());
                Shared::setUserExperience(SLD_EXPERIENCE->getValue(), true);
                Shared::setUseFbo(CHK_USE_GPU->getValue());
                Shared::setShowFps(CHK_SHOW_FPS->getValue());

                if (CHK_OSC_SEND->getValue())
                {
                    WidgetElmTextbox *TXT_OSC_SEND_HOST = getTextbox("TXT_OSC_SEND_HOST");
                    WidgetElmTextbox *TXT_OSC_SEND_PORT = getTextbox("TXT_OSC_SEND_PORT");

                    string host = TXT_OSC_SEND_HOST->getValue();
                    int port = TXT_OSC_SEND_PORT->getValue(-1);
                    if (host != "" && port > -1)
                    {
                        getViz()->setupOscSender(host, port);
                    }
                }

                if (CHK_OSC_RECEIVE->getValue())
                {
                    WidgetElmTextbox *TXT_OSC_RECEIVE_PORT = getTextbox("TXT_OSC_RECEIVE_PORT");
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

        virtual void afterLoad(json settings)
        { // Override in descendant for after load hook
        }

        static json getSettingsJson()
        {
            json doc;
            try
            {
                std::ifstream ifs(Shared::getSettingsFileFullPath());
                std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                doc = json::parse(data);
                return doc;

            }
            catch (exception &error)
            {
                json settings =
                    Shared::initSettingsFile(
                        App::APPLICATION_SETTINGS_FOLDER(),
                        App::APPLICATION_SETTINGS_FILE());
                return settings;
            }
        }

        void loadSettings()
        {
            json settings = getSettingsJson();

            float userInterfaceScaling = Shared::getDefaultScaling();
            string language = "english";
            int userExperience = Shared::getDefaultFPS();
            bool useFbo = true;
            bool showFps = false;
            string fontPath = "fonts/Verdana.ttf";
            bool autoLoadMostRecentProject = true;
            string themeName = "System";

            settings["settings"]["userInterfaceScaling"].get_to(userInterfaceScaling);
            settings["settings"]["language"].get_to(language);
            settings["settings"]["userExperience"].get_to(userExperience);
            settings["settings"]["useFbo"].get_to(useFbo);
            settings["settings"]["showFps"].get_to(showFps);
            settings["settings"]["fontPath"].get_to(fontPath);
            settings["settings"]["autoLoadMostRecentProject"].get_to(autoLoadMostRecentProject);
            settings["settings"]["themeName"].get_to(themeName);

            WidgetElmSlider *SLD_UI_SCALE = getSlider("SLD_UI_SCALE");
            WidgetElmSlider *SLD_EXPERIENCE = getSlider("SLD_EXPERIENCE");
            WidgetElmCheckbox *CHK_USE_GPU = getCheckbox("CHK_USE_GPU");
            WidgetElmCheckbox *CHK_SHOW_FPS = getCheckbox("CHK_SHOW_FPS");
            WidgetElmCheckbox *CHK_AUTOLOAD_RECENT = getCheckbox("CHK_AUTOLOAD_RECENT");

            SLD_UI_SCALE->setValue(userInterfaceScaling, false);
            SLD_EXPERIENCE->setValue(userExperience, false);
            CHK_USE_GPU->setValue(useFbo, false, false);
            CHK_SHOW_FPS->setValue(showFps, false, false);
            CHK_AUTOLOAD_RECENT->setValue(autoLoadMostRecentProject, false, false);

            settingsMenu->selectMenuItemByLabel(IconCache::IconTag::WIDGET_THEMES, themeName, false);

            afterLoad(settings);
        }

        virtual void beforeSave(json settings)
        { // Override in descendant for save hook
        }

        void saveSettings()
        {
            json settings = getSettingsJson();

            WidgetElmSlider *SLD_UI_SCALE = getSlider("SLD_UI_SCALE");
            WidgetElmSlider *SLD_EXPERIENCE = getSlider("SLD_EXPERIENCE");
            WidgetElmCheckbox *CHK_USE_GPU = getCheckbox("CHK_USE_GPU");
            WidgetElmCheckbox *CHK_SHOW_FPS = getCheckbox("CHK_SHOW_FPS");
            WidgetElmCheckbox *CHK_AUTOLOAD_RECENT = getCheckbox("CHK_AUTOLOAD_RECENT");

            settings["settings"]["userExperience"] = SLD_EXPERIENCE->getValue();
            settings["settings"]["useFbo"] = CHK_USE_GPU->getValue();
            settings["settings"]["showFps"] = CHK_SHOW_FPS->getValue();
            settings["settings"]["language"] = "english";
            settings["settings"]["fontPath"] = "fonts/Verdana.ttf";
            settings["settings"]["userInterfaceScaling"] = SLD_UI_SCALE->getValue();
            settings["settings"]["autoLoadMostRecentProject"] = CHK_AUTOLOAD_RECENT->getValue();
            settings["settings"]["themeName"] = selectedTheme.Name;

            beforeSave(settings);

            // Save the file
            std::ofstream ofs(Shared::getSettingsFileFullPath());
            ofs << settings;
        }

        virtual void onMenuItemSelected(WidgetMenuItemArgs &args)
        {

            switch (args.activeMenuTabId)
            {

            case IconCache::IconTag::WIDGET_SETTINGS:
                switch (args.menuItem->uniqueID)
                {
                case 0:
                    break;
                }
                break;

            case IconCache::IconTag::WIDGET_THEMES:
            {
                selectedTheme = Shared::getViz()->getThemeManager()->getThemeByName(args.menuItem->label);
                Shared::getViz()->loadTheme(selectedTheme.Name, true);
                WidgetManager::setTheme(selectedTheme);
            }
            break;
            }
        }

        void onMenuTabSelected(WidgetMenuTabArgs &args)
        {
            setWidgetNeedsUpdate(true);
            setNeedsUpdate(true);
            setExpressionRecalculationRequired(true);
        }

        void onMenuUnhovered(WidgetEventArgs &args)
        {
        }

        // Correctly calculate the usable x and width for child elements to use
        int getUsableX() override
        {
            return Widget::getUsableX(false) + settingsMenu->getWidth();
        }

        int getUsableWidth() override
        {
            return Widget::getUsableWidth() - settingsMenu->getWidth();
        }

        WidgetMenu *getSettingsMenu()
        {
            return settingsMenu;
        }

        vector<WidgetMenuItem> getThemesMenuItems()
        {
            vector<WidgetMenuItem> menuItems = vector<WidgetMenuItem>();
            int i = 0;
            for (WidgetTheme theme : Shared::getViz()->getThemeManager()->getThemes())
            {
                menuItems.push_back(WidgetMenuItem(theme.Name, i++));
            }
            return menuItems;
        }

    private:
        WidgetMenu *settingsMenu;
        WidgetTheme selectedTheme;
        bool loaded = false;

        void initWidget() override
        {

            WidgetManager::removeWidget(*settingsMenu);

            settingsMenu = new WidgetMenu("APP_SETTINGS_MENU", "<widget><bounds width='100' height='100'/></widget>", getWidgetId(), WidgetMenu::PreferredPopoutDirection::DOWN, {// ----------------------------------------------------------------------------
                                                                                                                                                                                            // General settings...
                                                                                                                                                                                            // ----------------------------------------------------------------------------
                                                                                                                                                                                            WidgetMenuTab("Settings", IconCache::getIcon("REG_WIDGET_SETTINGS"), IconCache::IconTag::WIDGET_SETTINGS, {}),

                                                                                                                                                                                            WidgetMenuTab("Color", IconCache::getIcon("MED_CONTENT_THEORY_COLOR"), IconCache::IconTag::WIDGET_THEMES, {getThemesMenuItems()})

                                                                                                                                                                                           });

            ofAddListener(settingsMenu->menuTabSelected, this, &WidgetSettings::onMenuTabSelected);
            ofAddListener(settingsMenu->menuItemSelected, this, &WidgetSettings::onMenuItemSelected);
            ofAddListener(settingsMenu->widgetUnhovered, this, &WidgetSettings::onMenuUnhovered);

            setTitleStyle(Widget::TitleStyle::TOP_STATIC);

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
                WidgetManager::loadWidgetFromFileToExisting("ui/widgets/settings.xml", *this);
                loaded = true;
                loadSettings();
            }
        }
    };

}