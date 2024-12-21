
#pragma once
#include "../Widget.h"
#include "WidgetFileExplorer.h"
#include <future>
#include <thread>

namespace Aquamarine
{
    class WidgetThemeEditor : public Widget
    {
    public:
        WidgetThemeEditor(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
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

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = Widget::saveState();
            settings.setAttribute("widget", "class", WidgetManager::WIDGET_CLASS_THEME_EDITOR, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~WidgetThemeEditor()
        {
            ofRemoveListener(textEditor->widgetEventReceived, this, &WidgetThemeEditor::onTextEditorEventReceived);
        };

        void saveThemeFile()
        {
            string themePath = ofToDataPath("themes.xml", true);
            textEditor->saveFile(themePath, false);
            Shared::getViz()->getThemeManager()->loadThemesFromFile(themePath);

            WidgetTheme selectedTheme = Shared::getViz()->getThemeManager()->getThemeByName(Shared::getViz()->getThemeManager()->getDefaultTheme().Name);
            WidgetManager::setTheme(selectedTheme);
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
            string event = args.getFullEventName();
            if (args.eventName == WIDGET_EVENT::CLICK && saveButton && args.sender.getPersistentId() == saveButton->getPersistentId())
            {
                saveThemeFile();
            }
            else if (args.eventName == WIDGET_EVENT::CLICK && cancelButton && args.sender.getPersistentId() == cancelButton->getPersistentId())
            {
                closeThisWidget();
            }
        }

        void onTextEditorEventReceived(WidgetEventArgs &args)
        {
            string event = args.getFullEventName();
            if (textEditor && args.sender.getPersistentId() == textEditor->getPersistentId())
            {

                if (args.eventName == WIDGET_EVENT::SAVED)
                {
                    saveThemeFile();
                }
            }
        }

    private:
        WidgetElmButton *saveButton = nullptr;
        WidgetElmButton *cancelButton = nullptr;
        WidgetTextEditor *textEditor = nullptr;
        string mPath = "";

        void initWidget() override
        {

            string saveButtonId = getPersistentId() + "_SAVE_BUTTON";
            string cancelButtonId = getPersistentId() + "_CANCEL_BUTTON";
            string textEditorId = getPersistentId() + "_TEXT_EDITOR";

            if (!saveButton)
            {
                saveButton = dynamic_cast<WidgetElmButton *>(addOrLoadWidgetElement(saveButton, WIDGET_ELM_CLASS::BUTTON, saveButtonId, R"(
                <element>
                </element>
                )"));

                saveButton->setX_Expr("${PARENT.RIGHT}");
                saveButton->setY_Expr("${PARENT.ABSOLUTE_Y}+${PADDING}");
                saveButton->setHeight_Expr("30");
                saveButton->setWidth_Expr("100");
                saveButton->setTitle("Save");
                saveButton->setIsEnabled(false);
            }

            if (!cancelButton)
            {
                cancelButton = dynamic_cast<WidgetElmButton *>(addOrLoadWidgetElement(cancelButton, WIDGET_ELM_CLASS::BUTTON, cancelButtonId, R"(
                <element>
                </element>
                )"));

                cancelButton->setX_Expr("${PARENT.LEFT}");
                cancelButton->setY_Expr("${PARENT.ABSOLUTE_Y}+${PADDING}");
                cancelButton->setHeight_Expr("30");
                cancelButton->setWidth_Expr("100");
                cancelButton->setTitle("Cancel");
            }

            if (!textEditor)
            {
                textEditor = new WidgetTextEditor(textEditorId, R"(
                <widget></widget>
                )");

                textEditor->setX_Expr("${PARENT.ABSOLUTE_USABLE_X}");
                textEditor->setY_Expr("${PARENT.ABSOLUTE_USABLE_Y}");
                textEditor->setHeight_Expr("${PARENT.USABLE_HEIGHT}");
                textEditor->setWidth_Expr("${PARENT.USABLE_WIDTH}");
                textEditor->setIsRoundedRectangle(false);
                addChildWidget(*textEditor, true);
                ofAddListener(textEditor->widgetEventReceived, this, &WidgetThemeEditor::onTextEditorEventReceived);
                string themeFile = ofToDataPath("themes.xml", true);
                textEditor->loadFile(themeFile);
            }

            setTitleStyle(TitleStyle::TOP_STATIC);

            setStaticTopTitleBarSize(40);
            setShowTitleIcons(false);
            setTitle("");
        }
    };
}