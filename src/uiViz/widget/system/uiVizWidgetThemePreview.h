//
//  uiVizMainMenu.h
//  bloom-macos
//
//  Created by Stuart Barnes on 12/04/2019.
//
#pragma once
#include "uiVizWidget.h"

namespace Aquamarine
{
    class uiVizWidgetThemePreview : public uiVizWidget
    {

    public:
        uiVizWidgetThemePreview(string persistentId, string widgetXML) : uiVizWidget(persistentId, widgetXML)
        {
            initWidget();
        }

        virtual ~uiVizWidgetThemePreview()
        {
        }

        void update(uiVizWidgetContext context) override
        {
        }

        void draw(uiVizWidgetContext context) override
        {
        }

        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
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

        uiVizWidgetElmTextbox *TXT_USERNAME;
        uiVizWidgetElmTextbox *TXT_PASSWORD;
        uiVizWidgetElmTextbox *TXT_LICENCE;
        uiVizWidgetElmProgressBar *PGS_VALIDATION_RESULT;
        uiVizWidgetTable *THEME_PREVIEW_TABLE;

        void initWidget() override
        {
            if (loaded)
                return;

            if (!loaded)
            {
                uiVizWidgetManager::loadWidgetFromFileToExisting("ui/widgets/theme-preview.xml", *this);
                loaded = true;
            }

            TXT_USERNAME = getTextbox("TXT_USERNAME");
            TXT_PASSWORD = getTextbox("TXT_PASSWORD");
            TXT_LICENCE = getTextbox("TXT_LICENCE");
            PGS_VALIDATION_RESULT = getProgressBar("PGS_VALIDATION_RESULT");

            if (getChildWidgetByPersistentId("THEME_PREVIEW_TABLE"))
            {
                THEME_PREVIEW_TABLE = dynamic_cast<uiVizWidgetTable *>(getChildWidgetByPersistentId("THEME_PREVIEW_TABLE"));
            }

            setIsAutoUpdateWhenActive(true);
        }
    };

}