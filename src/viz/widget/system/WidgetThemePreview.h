
#pragma once
#include "../Widget.h"

namespace Aquamarine
{
    class WidgetThemePreview : public Widget
    {

    public:
        WidgetThemePreview(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
        {
            initWidget();
        }

        virtual ~WidgetThemePreview()
        {
        }

        void update(WidgetContext context) override
        {
        }

        void draw(WidgetContext context) override
        {
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {

            string event = args.getFullEventName();

            if (event == "OK." + WIDGET_EVENT::CLICK)
            {
                closeThisWidget();
            }
            else if (event == "CANCEL." + WIDGET_EVENT::CLICK)
            {
                closeThisWidget();
            }
        };

    private:
        bool loaded = false;

        WidgetElmTextbox *TXT_USERNAME;
        WidgetElmTextbox *TXT_PASSWORD;
        WidgetElmTextbox *TXT_LICENCE;
        WidgetElmProgressBar *PGS_VALIDATION_RESULT;
        WidgetTable *THEME_PREVIEW_TABLE;

        void initWidget() override
        {
            if (loaded)
                return;

            if (!loaded)
            {
                WidgetManager::loadWidgetFromFileToExisting("ui/widgets/theme-preview.xml", *this);
                loaded = true;
            }

            TXT_USERNAME = getTextbox("TXT_USERNAME");
            TXT_PASSWORD = getTextbox("TXT_PASSWORD");
            TXT_LICENCE = getTextbox("TXT_LICENCE");
            PGS_VALIDATION_RESULT = getProgressBar("PGS_VALIDATION_RESULT");

            if (getChildWidgetByPersistentId("THEME_PREVIEW_TABLE"))
            {
                THEME_PREVIEW_TABLE = dynamic_cast<WidgetTable *>(getChildWidgetByPersistentId("THEME_PREVIEW_TABLE"));
            }

            setIsAutoUpdateWhenActive(true);
        }
    };

}