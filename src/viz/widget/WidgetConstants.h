#pragma once
#include <string>

//------------------------------ UI VIZ WIDGET CONSTANTS --------------------------------
namespace Aquamarine
{
    class WIDGET_EVENT
    {
    public:
        static const std::string RESIZED;
        static const std::string DRAGGED;
        static const std::string HOVERED;
        static const std::string UNHOVERED;
        static const std::string CLICK;
        static const std::string CLIPBOARD_CUT;
        static const std::string CLIPBOARD_COPY;
        static const std::string CLIPBOARD_PASTE;
        static const std::string FOCUS_ELEMENT;
        static const std::string FOCUS_NEXT_ELEMENT;
        static const std::string VALUE_CHANGED;
        static const std::string TABLE_CELL_WIDGET_INITIALIZED;
        static const std::string TABLE_CELL_DRAW_CONTENT;
        static const std::string TABLE_CELL_CLICKED;
        static const std::string TABLE_CELL_SELECTED;
        static const std::string TABLE_CELL_UNSELECTED;
        static const std::string TABLE_CELL_HOVERED;
        static const std::string RECEIVE_DRAG_AND_DROP_DATA;
        static const std::string DRAG_AND_DROP_DATA_SET;
        static const std::string SAVED;
        static const std::string LOADED;
    };

    class WIDGET_ELM_CLASS
    {
    public:
        static const std::string BASE;
        static const std::string TEXTBOX;
        static const std::string TEXTAREA;
        static const std::string LABEL;
        static const std::string BUTTON;
        static const std::string SLIDER;
        static const std::string CHECKBOX;
        static const std::string DROPDOWN;
        static const std::string BREADCRUMB;
        static const std::string IMAGE;
        static const std::string PROGRESS_BAR;
        static const std::string XY_PAD;
    };
}