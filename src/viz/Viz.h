
#pragma once
#include "Poco/RegularExpression.h"
#include "ofxSmartFont.h"
// ^^ ofxSmartFont.h to be replaced by :
#include "ofxFontStash2.h"

#include "Icon.h"
#include "Lang.h"
#include "Coord.h"
#include "ofxOsc.h"
#include <fstream>
#include "core/nlohmann/json.hpp"
using json = nlohmann::json;

namespace Aquamarine
{
    using Poco::RegularExpression;

#pragma once

#define MIN_WIDTH_8K 7680
#define MIN_HEIGHT_8K 4320

#define MIN_WIDTH_4K 3840
#define MIN_HEIGHT_4K 2160

#define MIN_WIDTH_RETINA 2560
#define MIN_HEIGHT_RETINA 1600

#define VIZ_KEY_NUMBER_1 49
#define VIZ_KEY_NUMBER_2 50
#define VIZ_KEY_NUMBER_3 51
#define VIZ_KEY_NUMBER_4 52
#define VIZ_KEY_NUMBER_5 53

    //------------------------------ WIDGET THEME  --------------------------------
    class WidgetTheme
    {

    public:
        WidgetTheme()
        {
        }

        bool loadForWidget_DEPRECATE(WidgetTheme &fallback, string themeXML)
        {
            ofxXmlSettings settings;
            if (settings.loadFromBuffer(themeXML.c_str()))
            {
                if (settings.pushTag("widget"))
                {
                    RoundedRectangle = settings.getAttribute("appearance", "roundedRectangle", RoundedRectangle, 0);
                    if (settings.pushTag("appearance"))
                    {
                        populateWidgetTheme_DEPRECATE(fallback, settings);
                        settings.popTag(); // appearance
                    }
                    settings.popTag(); // widget
                }
                return true;
            }
            return false;
        }

        bool loadForElement_DEPRECATE(WidgetTheme &fallback, string themeXML)
        {
            ofxXmlSettings settings;
            if (settings.loadFromBuffer(themeXML.c_str()))
            {
                if (settings.pushTag("element"))
                {
                    ElementRoundedRectangle = settings.getAttribute("appearance", "roundedRectangle", ElementRoundedRectangle, 0);
                    if (settings.pushTag("appearance"))
                    {
                        populateElementTheme_DEPRECATE(fallback, settings);
                        settings.popTag(); // appearance
                    }
                    settings.popTag(); // widget
                }
                return true;
            }
            return false;
        }

        bool load_DEPRECATE(string themeXML)
        {

            ofxXmlSettings settings;

            if (settings.loadFromBuffer(themeXML.c_str()))
            {

                Name = settings.getAttribute("theme", "name", "Unknown");
                string boolString = ofToLower(settings.getAttribute("theme", "isDark", "false"));
                IsDark = (boolString == "true" || boolString == "1" || boolString == "yes" || boolString == "on");

                settings.pushTag("theme");

                // Widgets
                if (settings.pushTag("widgets"))
                {
                    RoundedRectangle = settings.getAttribute("appearance", "roundedRectangle", RoundedRectangle, 0);
                    populateWidgetTheme_DEPRECATE(*this, settings);
                    settings.popTag(); // widgets
                }

                // Elements
                if (settings.pushTag("elements"))
                {
                    ElementRoundedRectangle = settings.getAttribute("appearance", "roundedRectangle", ElementRoundedRectangle, 0);
                    populateElementTheme_DEPRECATE(*this, settings);
                    settings.popTag(); // elemets
                }

                settings.popTag(); // theme
                setMainMenuColor(settings.getAttribute("theme", "mainMenuColor", ""));
                setPopoutMenuColor(settings.getAttribute("theme", "popoutMenuColor", ""));

                return true;
            }
            return false;
        }

        void populateWidgetTheme_DEPRECATE(WidgetTheme &fallback, ofxXmlSettings &settings)
        {

            setWidgetColor(settings.getAttribute("widgetColor", "color", ""));

            setWidgetContentColor(settings.getAttribute("widgetContentColor", "color", ""));

            setWidgetAccent1Color(settings.getAttribute("widgetAccentColor", "color1", ""));
            setWidgetAccent2Color(settings.getAttribute("widgetAccentColor", "color2", ""));
            setWidgetAccent3Color(settings.getAttribute("widgetAccentColor", "color3", ""));
            setWidgetAccent4Color(settings.getAttribute("widgetAccentColor", "color4", ""));

            setBackgroundColor1(settings.getAttribute("backgroundColor", "color1", ""));
            setBackgroundColor2(settings.getAttribute("backgroundColor", "color2", ""));
            setBackgroundColor3(settings.getAttribute("backgroundColor", "color3", ""));
            setBackgroundColor4(settings.getAttribute("backgroundColor", "color4", ""));

            setWidgetAlternating1Color(settings.getAttribute("widgetAlternatingColor", "color1", ""));
            setWidgetAlternating2Color(settings.getAttribute("widgetAlternatingColor", "color2", ""));

            setWidgetModalBackgroundColor(settings.getAttribute("widgetModalBackgroundColor", "color", ""));

            HoveredWidgetAlpha = settings.getAttribute("widgetAlpha", "hovered", fallback.HoveredWidgetAlpha);
            UnhoveredWidgetAlpha = settings.getAttribute("widgetAlpha", "unhovered", fallback.UnhoveredWidgetAlpha);
            ContentChromeAlpha = settings.getAttribute("widgetAlpha", "contentChrome", fallback.ContentChromeAlpha);

            setTitleFontColor(settings.getAttribute("titleFontColor", "color", ""));

            HoveredTitleFontAlpha = settings.getAttribute("titleFontAlpha", "hovered", fallback.HoveredTitleFontAlpha);
            UnhoveredTitleFontAlpha = settings.getAttribute("titleFontAlpha", "unhovered", fallback.UnhoveredTitleFontAlpha);

            setTitleColor(settings.getAttribute("titleColor", "color", ""));

            HoveredTitleAlpha = settings.getAttribute("titleAlpha", "hovered", fallback.HoveredTitleAlpha);
            UnhoveredTitleAlpha = settings.getAttribute("titleAlpha", "unhovered", fallback.UnhoveredTitleAlpha);

            setTitleGradientColors(
                settings.getAttribute("titleGradientColor", "color1", fallback.getTitleGradientColor1Hex()),
                settings.getAttribute("titleGradientColor", "color2", fallback.getTitleGradientColor2Hex()),
                settings.getAttribute("titleGradientColor", "color3", fallback.getTitleGradientColor3Hex()),
                settings.getAttribute("titleGradientColor", "color4", fallback.getTitleGradientColor4Hex()));

            setTypographyColors(
                settings.getAttribute("typographyColor", "primary", fallback.getTypographyPrimaryColorHex()),
                settings.getAttribute("typographyColor", "secondary", fallback.getTypographySecondaryColorHex()),
                settings.getAttribute("typographyColor", "tertiary", fallback.getTypographyTertiaryColorHex()),
                settings.getAttribute("typographyColor", "quaternary", fallback.getTypographyQuaternaryColorHex()));

            setSelectionColors(
                settings.getAttribute("selection", "foreground", fallback.getSelectionForegroundColorHex()),
                settings.getAttribute("selection", "background", fallback.getSelectionBackgroundColorHex()));

            setHoverColors(
                settings.getAttribute("hover", "foreground", fallback.getHoverBackgroundColorHex()),
                settings.getAttribute("hover", "background", fallback.getHoverBackgroundColorHex()));
        }

        void populateElementTheme_DEPRECATE(WidgetTheme &fallback, ofxXmlSettings &settings)
        {

            setElementForegroundColor(settings.getAttribute("foregroundColor", "color", ""));

            ElementHoveredForegroundAlpha = settings.getAttribute("foregroundAlpha", "hovered", fallback.ElementHoveredForegroundAlpha);
            ElementUnhoveredForegroundAlpha = settings.getAttribute("foregroundAlpha", "unhovered", fallback.ElementUnhoveredForegroundAlpha);

            setElementBackgroundColor(settings.getAttribute("backgroundColor", "color", ""));

            ElementHoveredBackgroundAlpha = settings.getAttribute("backgroundAlpha", "hovered", fallback.ElementHoveredBackgroundAlpha);
            ElementUnhoveredBackgroundAlpha = settings.getAttribute("backgroundAlpha", "unhovered", fallback.ElementUnhoveredBackgroundAlpha);
        }

        /******** NEW */

        bool loadForWidgetJson(WidgetTheme &fallback, string widgetJson)
        {
            json doc;
            try
            {
                doc = json::parse(widgetJson);
                doc["widget"]["appearance"]["roundedRectangle"].get_to(RoundedRectangle);
                return populateWidgetThemeJson(fallback, doc);
            }
            catch (exception &error)
            {
                std::cerr << "\nTheme JSON error for Widget: " << error.what() << std::endl;
            }
            return false;
        }

        bool loadForElementJson(WidgetTheme &fallback, string elementJson)
        {
            json doc;
            try
            {
                doc = json::parse(elementJson);
                doc["element"]["appearance"]["roundedRectangle"].get_to(ElementRoundedRectangle);
                return populateElementThemeJson(fallback, doc);
            }
            catch (exception &error)
            {
                std::cerr << "\nTheme JSON error for Element: " << error.what() << std::endl;
            }
            return false;
        }

        bool loadJson(string themeJson)
        {
            json doc;
            try
            {
                doc = json::parse(themeJson);

                Name = "Unknown";
                doc["name"].get_to(Name);

                doc["isDark"].get_to(IsDark);

                // Widgets
                populateWidgetThemeJson(*this, doc);

                // Elements
                populateElementThemeJson(*this, doc);

                string strMainMenuColor = "";
                string strPopoutMenuColor = "";
                doc["mainMenuColor"].get_to(strMainMenuColor);
                doc["popoutMenuColor"].get_to(strPopoutMenuColor);

                setMainMenuColor(strMainMenuColor);
                setPopoutMenuColor(strPopoutMenuColor);

                return true;
            }
            catch (exception &error)
            {
                std::cerr << "\nTheme JSON error: " << error.what() << std::endl;
                return false;
            }
            return false;
        }

        bool populateWidgetThemeJson(WidgetTheme &fallback, json &doc)
        {
            doc["widgets"]["roundedRectangle"].get_to(RoundedRectangle);

            string tmp_str = "";
            doc["widgets"]["widgetColor"]["color"].get_to(tmp_str);
            setWidgetColor(tmp_str);

            tmp_str = "";
            doc["widgets"]["widgetContentColor"]["color"].get_to(tmp_str);
            setWidgetContentColor(tmp_str);

            tmp_str = "";
            doc["widgets"]["widgetAccentColor"]["color1"].get_to(tmp_str);
            setWidgetAccent1Color(tmp_str);

            tmp_str = "";
            doc["widgets"]["widgetAccentColor"]["color2"].get_to(tmp_str);
            setWidgetAccent2Color(tmp_str);

            tmp_str = "";
            doc["widgets"]["widgetAccentColor"]["color3"].get_to(tmp_str);
            setWidgetAccent3Color(tmp_str);

            tmp_str = "";
            doc["widgets"]["widgetAccentColor"]["color4"].get_to(tmp_str);
            setWidgetAccent4Color(tmp_str);

            tmp_str = "";
            doc["widgets"]["backgroundColor"]["color1"].get_to(tmp_str);
            setBackgroundColor1(tmp_str);

            tmp_str = "";
            doc["widgets"]["backgroundColor"]["color2"].get_to(tmp_str);
            setBackgroundColor2(tmp_str);

            tmp_str = "";
            doc["widgets"]["backgroundColor"]["color3"].get_to(tmp_str);
            setBackgroundColor3(tmp_str);

            tmp_str = "";
            doc["widgets"]["backgroundColor"]["color4"].get_to(tmp_str);
            setBackgroundColor4(tmp_str);

            tmp_str = "";
            doc["widgets"]["widgetAlternatingColor"]["color1"].get_to(tmp_str);
            setWidgetAlternating1Color(tmp_str);

            tmp_str = "";
            doc["widgets"]["widgetAlternatingColor"]["color2"].get_to(tmp_str);
            setWidgetAlternating2Color(tmp_str);

            tmp_str = "";
            doc["widgets"]["widgetModalBackgroundColor"]["color"].get_to(tmp_str);
            setWidgetModalBackgroundColor(tmp_str);

            doc["widgets"]["widgetAlpha"]["hovered"].get_to(HoveredWidgetAlpha);
            doc["widgets"]["widgetAlpha"]["unhovered"].get_to(UnhoveredWidgetAlpha);
            doc["widgets"]["widgetAlpha"]["contentChrome"].get_to(ContentChromeAlpha);

            tmp_str = "";
            doc["widgets"]["titleFontColor"]["color"].get_to(tmp_str);
            setTitleFontColor(tmp_str);

            doc["widgets"]["titleFontAlpha"]["hovered"].get_to(HoveredTitleFontAlpha);
            doc["widgets"]["titleFontAlpha"]["unhovered"].get_to(UnhoveredTitleFontAlpha);

            tmp_str = "";
            doc["widgets"]["titleColor"]["color"].get_to(tmp_str);
            setTitleColor(tmp_str);

            doc["widgets"]["titleAlpha"]["hovered"].get_to(HoveredTitleAlpha);
            doc["widgets"]["titleAlpha"]["unhovered"].get_to(UnhoveredTitleAlpha);

            string titleGradientColor1 = fallback.getTitleGradientColor1Hex();
            doc["widgets"]["titleGradientColor"]["color1"].get_to(titleGradientColor1);

            string titleGradientColor2 = fallback.getTitleGradientColor2Hex();
            doc["widgets"]["titleGradientColor"]["color2"].get_to(titleGradientColor2);

            string titleGradientColor3 = fallback.getTitleGradientColor3Hex();
            doc["widgets"]["titleGradientColor"]["color3"].get_to(titleGradientColor3);

            string titleGradientColor4 = fallback.getTitleGradientColor4Hex();
            doc["widgets"]["titleGradientColor"]["color4"].get_to(titleGradientColor4);

            setTitleGradientColors(titleGradientColor1, titleGradientColor2, titleGradientColor3, titleGradientColor4);

            string typographyColor1 = fallback.getTypographyPrimaryColorHex();
            doc["widgets"]["typographyColor"]["primary"].get_to(typographyColor1);

            string typographyColor2 = fallback.getTypographySecondaryColorHex();
            doc["widgets"]["typographyColor"]["secondary"].get_to(typographyColor2);

            string typographyColor3 = fallback.getTypographyTertiaryColorHex();
            doc["widgets"]["typographyColor"]["tertiary"].get_to(typographyColor3);

            string typographyColor4 = fallback.getTypographyQuaternaryColorHex();
            doc["widgets"]["typographyColor"]["quaternary"].get_to(typographyColor4);

            setTypographyColors(typographyColor1, typographyColor2, typographyColor3, typographyColor4);

            string selectionColorsForeground = "";
            doc["widgets"]["selection"]["foreground"].get_to(selectionColorsForeground);

            string selectionColorsBackground = "";
            doc["widgets"]["selection"]["background"].get_to(selectionColorsBackground);

            setSelectionColors(selectionColorsForeground, selectionColorsBackground);

            string hoverColorsForeground = "";
            doc["widgets"]["hover"]["foreground"].get_to(hoverColorsForeground);

            string hoverColorsBackground = "";
            doc["widgets"]["hover"]["background"].get_to(hoverColorsBackground);

            setHoverColors(hoverColorsForeground, hoverColorsBackground);

            return true;
        }

        bool populateElementThemeJson(WidgetTheme &fallback, json &doc)
        {
            doc["elements"]["roundedRectangle"].get_to(ElementRoundedRectangle);

            string tmp_str = "";
            doc["elements"]["foregroundColor"]["color"].get_to(tmp_str);
            setElementForegroundColor(tmp_str);

            doc["elements"]["foregroundAlpha"]["hovered"].get_to(ElementHoveredForegroundAlpha);
            doc["elements"]["foregroundAlpha"]["unhovered"].get_to(ElementUnhoveredForegroundAlpha);

            tmp_str = "";
            doc["elements"]["backgroundColor"]["color"].get_to(tmp_str);
            setElementBackgroundColor(tmp_str);

            doc["elements"]["backgroundAlpha"]["hovered"].get_to(ElementHoveredBackgroundAlpha);
            doc["elements"]["backgroundAlpha"]["unhovered"].get_to(ElementUnhoveredBackgroundAlpha);

            return true;
        }

        bool isDefaultLight()
        {
            return (Name == "System");
        }

        bool isDefaultDark()
        {
            return (Name == "SystemDark");
        }

        bool isDefault()
        {
            return (isDefaultLight() || isDefaultDark());
        }

        ofColor TitleFontColor_withAlpha(float alpha)
        {
            return ofColor(TitleFontColor.r, TitleFontColor.g, TitleFontColor.b, TitleFontColor.a * alpha);
        }

        string getTitleFontColorHex()
        {
            return "#" + getHexFromInt(TitleFontColor.r) + getHexFromInt(TitleFontColor.g) + getHexFromInt(TitleFontColor.b) + getHexFromInt(TitleFontColor.a);
        }

        void setTitleFontColor(ofColor val)
        {
            setColorFromSource(TitleFontColor, val);
        }

        void setTitleFontColor(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setTitleFontColor(val);
        }

        ofColor TitleColor_withAlpha(float alpha)
        {
            return ofColor(TitleColor.r, TitleColor.g, TitleColor.b, TitleColor.a * alpha);
        }

        string getTitleColorHex()
        {
            return "#" + getHexFromInt(TitleColor.r) + getHexFromInt(TitleColor.g) + getHexFromInt(TitleColor.b) + getHexFromInt(TitleColor.a);
        }

        void setTitleColor(ofColor val)
        {
            setColorFromSource(TitleColor, val);
        }

        void setTitleColor(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setTitleColor(val);
        }

        ofColor TitleGradientColor1_withAlpha(float alpha)
        {
            return ofColor(TitleGradientColor1.r, TitleGradientColor1.g, TitleGradientColor1.b, TitleGradientColor1.a * alpha);
        }

        string getTitleGradientColor1Hex()
        {
            return "#" + getHexFromInt(TitleGradientColor1.r) + getHexFromInt(TitleGradientColor1.g) + getHexFromInt(TitleGradientColor1.b) + getHexFromInt(TitleGradientColor1.a);
        }

        ofColor TitleGradientColor2_withAlpha(float alpha)
        {
            return ofColor(TitleGradientColor2.r, TitleGradientColor2.g, TitleGradientColor2.b, TitleGradientColor2.a * alpha);
        }

        string getTitleGradientColor2Hex()
        {
            return "#" + getHexFromInt(TitleGradientColor2.r) + getHexFromInt(TitleGradientColor2.g) + getHexFromInt(TitleGradientColor2.b) + getHexFromInt(TitleGradientColor2.a);
        }

        ofColor TitleGradientColor3_withAlpha(float alpha)
        {
            return ofColor(TitleGradientColor3.r, TitleGradientColor3.g, TitleGradientColor3.b, TitleGradientColor3.a * alpha);
        }

        string getTitleGradientColor3Hex()
        {
            return "#" + getHexFromInt(TitleGradientColor3.r) + getHexFromInt(TitleGradientColor3.g) + getHexFromInt(TitleGradientColor3.b) + getHexFromInt(TitleGradientColor3.a);
        }

        ofColor TitleGradientColor4_withAlpha(float alpha)
        {
            return ofColor(TitleGradientColor4.r, TitleGradientColor4.g, TitleGradientColor4.b, TitleGradientColor4.a * alpha);
        }

        string getTitleGradientColor4Hex()
        {
            return "#" + getHexFromInt(TitleGradientColor4.r) + getHexFromInt(TitleGradientColor4.g) + getHexFromInt(TitleGradientColor4.b) + getHexFromInt(TitleGradientColor4.a);
        }

        void setTitleGradientColors(ofColor color1, ofColor color2, ofColor color3, ofColor color4)
        {
            TitleGradientColor1 = color1;
            TitleGradientColor2 = color2;
            TitleGradientColor3 = color3;
            TitleGradientColor4 = color4;
            mTitleGradientSet = true;
        }

        void setTitleGradientColors(string RGBAHexColor1, string RGBAHexColor2, string RGBAHexColor3, string RGBAHexColor4)
        {
            if (RGBAHexColor1 == "" && RGBAHexColor2 == "" && RGBAHexColor3 == "" && RGBAHexColor4 == "")
                return;
            ofColor color1 = getColor_fromRGBAHex(RGBAHexColor1);
            ofColor color2 = getColor_fromRGBAHex(RGBAHexColor2);
            ofColor color3 = getColor_fromRGBAHex(RGBAHexColor3);
            ofColor color4 = getColor_fromRGBAHex(RGBAHexColor4);
            setTitleGradientColors(color1, color2, color3, color4);
        }

        bool getTitleGradientSet()
        {
            return mTitleGradientSet;
        }

        ofColor TypographyPrimaryColor_withAlpha(float alpha)
        {
            return ofColor(TypographyPrimaryColor.r, TypographyPrimaryColor.g, TypographyPrimaryColor.b, TypographyPrimaryColor.a * alpha);
        }

        string getTypographyPrimaryColorHex()
        {
            return "#" + getHexFromInt(TypographyPrimaryColor.r) + getHexFromInt(TypographyPrimaryColor.g) + getHexFromInt(TypographyPrimaryColor.b) + getHexFromInt(TypographyPrimaryColor.a);
        }

        ofColor TypographySecondaryColor_withAlpha(float alpha)
        {
            return ofColor(TypographySecondaryColor.r, TypographySecondaryColor.g, TypographySecondaryColor.b, TypographySecondaryColor.a * alpha);
        }

        string getTypographySecondaryColorHex()
        {
            return "#" + getHexFromInt(TypographySecondaryColor.r) + getHexFromInt(TypographySecondaryColor.g) + getHexFromInt(TypographySecondaryColor.b) + getHexFromInt(TypographySecondaryColor.a);
        }

        ofColor TypographyTertiaryColor_withAlpha(float alpha)
        {
            return ofColor(TypographyTertiaryColor.r, TypographyTertiaryColor.g, TypographyTertiaryColor.b, TypographyTertiaryColor.a * alpha);
        }

        string getTypographyTertiaryColorHex()
        {
            return "#" + getHexFromInt(TypographyTertiaryColor.r) + getHexFromInt(TypographyTertiaryColor.g) + getHexFromInt(TypographyTertiaryColor.b) + getHexFromInt(TypographyTertiaryColor.a);
        }

        ofColor TypographyQuaternaryColor_withAlpha(float alpha)
        {
            return ofColor(TypographyQuaternaryColor.r, TypographyQuaternaryColor.g, TypographyQuaternaryColor.b, TypographyQuaternaryColor.a * alpha);
        }

        string getTypographyQuaternaryColorHex()
        {
            return "#" + getHexFromInt(TypographyQuaternaryColor.r) + getHexFromInt(TypographyQuaternaryColor.g) + getHexFromInt(TypographyQuaternaryColor.b) + getHexFromInt(TypographyQuaternaryColor.a);
        }

        void setTypographyColors(ofColor primary, ofColor secondary, ofColor tertiary, ofColor quaternary)
        {
            TypographyPrimaryColor = primary;
            TypographySecondaryColor = secondary;
            TypographyTertiaryColor = tertiary;
            TypographyQuaternaryColor = quaternary;
        }

        void setTypographyColors(string RGBAHexPrimary, string RGBAHexSecondary, string RGBAHexTertiary, string RGBAHexQuaternary)
        {
            ofColor primary = getColor_fromRGBAHex(RGBAHexPrimary);
            ofColor secondary = getColor_fromRGBAHex(RGBAHexSecondary);
            ofColor tertiary = getColor_fromRGBAHex(RGBAHexTertiary);
            ofColor quaternary = getColor_fromRGBAHex(RGBAHexQuaternary);
            setTypographyColors(primary, secondary, tertiary, quaternary);
        }

        ofColor SelectionForegroundColor_withAlpha(float alpha)
        {
            return ofColor(SelectionForegroundColor.r, SelectionForegroundColor.g, SelectionForegroundColor.b, SelectionForegroundColor.a * alpha);
        }

        string getSelectionForegroundColorHex()
        {
            return "#" + getHexFromInt(SelectionForegroundColor.r) + getHexFromInt(SelectionForegroundColor.g) + getHexFromInt(SelectionForegroundColor.b) + getHexFromInt(SelectionForegroundColor.a);
        }

        ofColor SelectionBackgroundColor_withAlpha(float alpha)
        {
            return ofColor(SelectionBackgroundColor.r, SelectionBackgroundColor.g, SelectionBackgroundColor.b, SelectionBackgroundColor.a * alpha);
        }

        string getSelectionBackgroundColorHex()
        {
            return "#" + getHexFromInt(SelectionBackgroundColor.r) + getHexFromInt(SelectionBackgroundColor.g) + getHexFromInt(SelectionBackgroundColor.b) + getHexFromInt(SelectionBackgroundColor.a);
        }

        void setSelectionColors(ofColor foreground, ofColor background)
        {
            SelectionForegroundColor = foreground;
            SelectionBackgroundColor = background;
        }

        void setSelectionColors(string RGBAHexForeground, string RGBAHexBackground)
        {
            ofColor foreground = getColor_fromRGBAHex(RGBAHexForeground);
            ofColor background = getColor_fromRGBAHex(RGBAHexBackground);
            setSelectionColors(foreground, background);
        }

        ofColor HoverForegroundColor_withAlpha(float alpha)
        {
            return ofColor(HoverForegroundColor.r, HoverForegroundColor.g, HoverForegroundColor.b, HoverForegroundColor.a * alpha);
        }

        string getHoverForegroundColorHex()
        {
            return "#" + getHexFromInt(HoverForegroundColor.r) + getHexFromInt(HoverForegroundColor.g) + getHexFromInt(HoverForegroundColor.b) + getHexFromInt(HoverForegroundColor.a);
        }

        ofColor HoverBackgroundColor_withAlpha(float alpha)
        {
            return ofColor(HoverBackgroundColor.r, HoverBackgroundColor.g, HoverBackgroundColor.b, HoverBackgroundColor.a * alpha);
        }

        string getHoverBackgroundColorHex()
        {
            return "#" + getHexFromInt(HoverBackgroundColor.r) + getHexFromInt(HoverBackgroundColor.g) + getHexFromInt(HoverBackgroundColor.b) + getHexFromInt(HoverBackgroundColor.a);
        }

        void setHoverColors(ofColor foreground, ofColor background)
        {
            HoverForegroundColor = foreground;
            HoverBackgroundColor = background;
        }

        void setHoverColors(string RGBAHexForeground, string RGBAHexBackground)
        {
            ofColor foreground = getColor_fromRGBAHex(RGBAHexForeground);
            ofColor background = getColor_fromRGBAHex(RGBAHexBackground);
            setHoverColors(foreground, background);
        }

        ofColor WidgetColor_withAlpha(float alpha)
        {
            return ofColor(WidgetColor.r, WidgetColor.g, WidgetColor.b, WidgetColor.a * alpha);
        }

        string getWidgetColorHex()
        {
            return "#" + getHexFromInt(WidgetColor.r) + getHexFromInt(WidgetColor.g) + getHexFromInt(WidgetColor.b) + getHexFromInt(WidgetColor.a);
        }

        void setWidgetColor(ofColor val)
        {
            setColorFromSource(WidgetColor, val);
        }

        void setWidgetColor(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setWidgetColor(val);
        }

        string getMainMenuColorHex()
        {
            return "#" + getHexFromInt(MainMenuColor.r) + getHexFromInt(MainMenuColor.g) + getHexFromInt(MainMenuColor.b) + getHexFromInt(MainMenuColor.a);
        }

        void setMainMenuColor(ofColor val)
        {
            setColorFromSource(MainMenuColor, val);
        }

        void setMainMenuColor(string RGBAHex)
        {
            if (RGBAHex == "")
            {
                setMainMenuColor(WidgetColor);
                return;
            }
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setMainMenuColor(val);
        }

        string getPopoutMenuColorHex()
        {
            return "#" + getHexFromInt(PopoutMenuColor.r) + getHexFromInt(PopoutMenuColor.g) + getHexFromInt(PopoutMenuColor.b) + getHexFromInt(PopoutMenuColor.a);
        }

        void setPopoutMenuColor(ofColor val)
        {
            setColorFromSource(PopoutMenuColor, val);
        }

        void setPopoutMenuColor(string RGBAHex)
        {
            if (RGBAHex == "")
            {
                setPopoutMenuColor(WidgetColor);
                return;
            }
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setPopoutMenuColor(val);
        }

        ofColor WidgetContentColor_withAlpha(float alpha)
        {
            return ofColor(WidgetContentColor.r, WidgetContentColor.g, WidgetContentColor.b, WidgetContentColor.a * alpha);
        }

        string getWidgetContentColorHex()
        {
            return "#" + getHexFromInt(WidgetContentColor.r) + getHexFromInt(WidgetContentColor.g) + getHexFromInt(WidgetContentColor.b) + getHexFromInt(WidgetContentColor.a);
        }

        void setWidgetContentColor(ofColor val)
        {
            setColorFromSource(WidgetContentColor, val);
        }

        void setWidgetContentColor(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setWidgetContentColor(val);
        }

        ofColor WidgetAccent1Color_withAlpha(float alpha)
        {
            return ofColor(WidgetAccent1Color.r, WidgetAccent1Color.g, WidgetAccent1Color.b, WidgetAccent1Color.a * alpha);
        }

        string getWidgetAccent1ColorHex()
        {
            return "#" + getHexFromInt(WidgetAccent1Color.r) + getHexFromInt(WidgetAccent1Color.g) + getHexFromInt(WidgetAccent1Color.b) + getHexFromInt(WidgetAccent1Color.a);
        }

        void setWidgetAccent1Color(ofColor val)
        {
            setColorFromSource(WidgetAccent1Color, val);
        }

        void setWidgetAccent1Color(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setWidgetAccent1Color(val);
        }

        ofColor WidgetAccent2Color_withAlpha(float alpha)
        {
            return ofColor(WidgetAccent2Color.r, WidgetAccent2Color.g, WidgetAccent2Color.b, WidgetAccent2Color.a * alpha);
        }

        string getWidgetAccent2ColorHex()
        {
            return "#" + getHexFromInt(WidgetAccent2Color.r) + getHexFromInt(WidgetAccent2Color.g) + getHexFromInt(WidgetAccent2Color.b) + getHexFromInt(WidgetAccent2Color.a);
        }

        void setWidgetAccent2Color(ofColor val)
        {
            setColorFromSource(WidgetAccent2Color, val);
        }

        void setWidgetAccent2Color(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setWidgetAccent2Color(val);
        }

        ofColor WidgetAccent3Color_withAlpha(float alpha)
        {
            return ofColor(WidgetAccent3Color.r, WidgetAccent3Color.g, WidgetAccent3Color.b, WidgetAccent3Color.a * alpha);
        }

        string getWidgetAccent3ColorHex()
        {
            return "#" + getHexFromInt(WidgetAccent3Color.r) + getHexFromInt(WidgetAccent3Color.g) + getHexFromInt(WidgetAccent3Color.b) + getHexFromInt(WidgetAccent3Color.a);
        }

        void setWidgetAccent3Color(ofColor val)
        {
            setColorFromSource(WidgetAccent3Color, val);
        }

        void setWidgetAccent3Color(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setWidgetAccent3Color(val);
        }

        ofColor WidgetAccent4Color_withAlpha(float alpha)
        {
            return ofColor(WidgetAccent4Color.r, WidgetAccent4Color.g, WidgetAccent4Color.b, WidgetAccent4Color.a * alpha);
        }

        string getWidgetAccent4ColorHex()
        {
            return "#" + getHexFromInt(WidgetAccent4Color.r) + getHexFromInt(WidgetAccent4Color.g) + getHexFromInt(WidgetAccent4Color.b) + getHexFromInt(WidgetAccent4Color.a);
        }

        void setWidgetAccent4Color(ofColor val)
        {
            setColorFromSource(WidgetAccent4Color, val);
        }

        void setWidgetAccent4Color(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setWidgetAccent4Color(val);
        }
        // Background
        ofColor BackgroundColor1_withAlpha(float alpha)
        {
            return ofColor(BackgroundColor1.r, BackgroundColor1.g, BackgroundColor1.b, BackgroundColor1.a * alpha);
        }

        string getBackgroundColor1Hex()
        {
            return "#" + getHexFromInt(BackgroundColor1.r) + getHexFromInt(BackgroundColor1.g) + getHexFromInt(BackgroundColor1.b) + getHexFromInt(BackgroundColor1.a);
        }

        void setBackgroundColor1(ofColor val)
        {
            setColorFromSource(BackgroundColor1, val);
        }

        void setBackgroundColor1(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setBackgroundColor1(val);
        }

        ofColor BackgroundColor2_withAlpha(float alpha)
        {
            return ofColor(BackgroundColor2.r, BackgroundColor2.g, BackgroundColor2.b, BackgroundColor2.a * alpha);
        }

        string getBackgroundColor2Hex()
        {
            return "#" + getHexFromInt(BackgroundColor2.r) + getHexFromInt(BackgroundColor2.g) + getHexFromInt(BackgroundColor2.b) + getHexFromInt(BackgroundColor2.a);
        }

        void setBackgroundColor2(ofColor val)
        {
            setColorFromSource(BackgroundColor2, val);
        }

        void setBackgroundColor2(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setBackgroundColor2(val);
        }

        ofColor BackgroundColor3_withAlpha(float alpha)
        {
            return ofColor(BackgroundColor3.r, BackgroundColor3.g, BackgroundColor3.b, BackgroundColor3.a * alpha);
        }

        string getBackgroundColor3Hex()
        {
            return "#" + getHexFromInt(BackgroundColor3.r) + getHexFromInt(BackgroundColor3.g) + getHexFromInt(BackgroundColor3.b) + getHexFromInt(BackgroundColor3.a);
        }

        void setBackgroundColor3(ofColor val)
        {
            setColorFromSource(BackgroundColor3, val);
        }

        void setBackgroundColor3(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setBackgroundColor3(val);
        }

        ofColor BackgroundColor4_withAlpha(float alpha)
        {
            return ofColor(BackgroundColor4.r, BackgroundColor4.g, BackgroundColor4.b, BackgroundColor4.a * alpha);
        }

        string getBackgroundColor4Hex()
        {
            return "#" + getHexFromInt(BackgroundColor4.r) + getHexFromInt(BackgroundColor4.g) + getHexFromInt(BackgroundColor4.b) + getHexFromInt(BackgroundColor4.a);
        }

        void setBackgroundColor4(ofColor val)
        {
            setColorFromSource(BackgroundColor4, val);
        }

        void setBackgroundColor4(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setBackgroundColor4(val);
        }

        // Eg text
        ofColor WidgetAccentHighContrast1Color_withAlpha(float alpha)
        {
            return WidgetAccent1Color_withAlpha(alpha);
        }

        // Eg text
        ofColor WidgetAccentHighContrast2Color_withAlpha(float alpha)
        {
            return WidgetAccent2Color_withAlpha(alpha);
        }

        // Eg highlight BEHIND text
        ofColor WidgetAccentLowContrast1Color_withAlpha(float alpha)
        {
            return WidgetAccent3Color_withAlpha(alpha);
        }

        // Eg highlight BEHIND text
        ofColor WidgetAccentLowContrast2Color_withAlpha(float alpha)
        {
            return WidgetAccent4Color_withAlpha(alpha);
        }

        ofColor WidgetAlternating1Color_withAlpha(float alpha)
        {
            return ofColor(WidgetAlternating1Color.r, WidgetAlternating1Color.g, WidgetAlternating1Color.b, WidgetAlternating1Color.a * alpha);
        }

        string getWidgetAlternating1ColorHex()
        {
            return "#" + getHexFromInt(WidgetAlternating1Color.r) + getHexFromInt(WidgetAlternating1Color.g) + getHexFromInt(WidgetAlternating1Color.b) + getHexFromInt(WidgetAlternating1Color.a);
        }

        void setWidgetAlternating1Color(ofColor val)
        {
            setColorFromSource(WidgetAlternating1Color, val);
        }

        void setWidgetAlternating1Color(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setWidgetAlternating1Color(val);
        }

        ofColor WidgetAlternating2Color_withAlpha(float alpha)
        {
            return ofColor(WidgetAlternating2Color.r, WidgetAlternating2Color.g, WidgetAlternating2Color.b, WidgetAlternating2Color.a * alpha);
        }

        string getWidgetAlternating2ColorHex()
        {
            return "#" + getHexFromInt(WidgetAlternating2Color.r) + getHexFromInt(WidgetAlternating2Color.g) + getHexFromInt(WidgetAlternating2Color.b) + getHexFromInt(WidgetAlternating2Color.a);
        }

        void setWidgetAlternating2Color(ofColor val)
        {
            setColorFromSource(WidgetAlternating2Color, val);
        }

        void setWidgetAlternating2Color(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setWidgetAlternating2Color(val);
        }

        ofColor getWidgetModalBackgroundColor()
        {
            return ofColor(WidgetModalBackgroundColor.r, WidgetModalBackgroundColor.g, WidgetModalBackgroundColor.b, WidgetModalBackgroundColor.a);
        }

        string getWidgetModalBackgroundColorHex()
        {
            return "#" + getHexFromInt(WidgetModalBackgroundColor.r) + getHexFromInt(WidgetModalBackgroundColor.g) + getHexFromInt(WidgetModalBackgroundColor.b) + getHexFromInt(WidgetModalBackgroundColor.a);
        }

        void setWidgetModalBackgroundColor(ofColor val)
        {
            setColorFromSource(WidgetModalBackgroundColor, val);
        }

        void setWidgetModalBackgroundColor(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setWidgetModalBackgroundColor(val);
        }

        ofColor ElementForegroundColor_withAlpha(float alpha)
        {
            return ofColor(ElementForegroundColor.r, ElementForegroundColor.g, ElementForegroundColor.b, ElementForegroundColor.a * alpha);
        }

        string getElementForegroundColorHex()
        {
            return "#" + getHexFromInt(ElementForegroundColor.r) + getHexFromInt(ElementForegroundColor.g) + getHexFromInt(ElementForegroundColor.b) + getHexFromInt(ElementForegroundColor.a);
        }

        void setElementForegroundColor(ofColor val)
        {
            setColorFromSource(ElementForegroundColor, val);
        }

        void setElementForegroundColor(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setElementForegroundColor(val);
        }

        ofColor ElementBackgroundColor_withAlpha(float alpha)
        {
            return ofColor(ElementBackgroundColor.r, ElementBackgroundColor.g, ElementBackgroundColor.b, ElementBackgroundColor.a * alpha);
        }

        string getElementBackgroundColorHex()
        {
            return "#" + getHexFromInt(ElementBackgroundColor.r) + getHexFromInt(ElementBackgroundColor.g) + getHexFromInt(ElementBackgroundColor.b) + getHexFromInt(ElementBackgroundColor.a);
        }

        static string getHexFromColor(ofColor c)
        {
            return "#" + getHexFromInt(c.r) + getHexFromInt(c.g) + getHexFromInt(c.b) + getHexFromInt(c.a);
        }

        static string getHexFromColor(ofColor c, float alpha)
        {
            return "#" + getHexFromInt(c.r) + getHexFromInt(c.g) + getHexFromInt(c.b) + getHexFromInt(255 * alpha);
        }

        void setElementBackgroundColor(ofColor val)
        {
            setColorFromSource(ElementBackgroundColor, val);
        }

        void setElementBackgroundColor(string RGBAHex)
        {
            if (RGBAHex == "")
                return;
            ofColor val = getColor_fromRGBAHex(RGBAHex);
            setElementBackgroundColor(val);
        }

        void setWidgetAlphas(int titleFontColor_a, int titleColor_a, int widgetColor_a)
        {
            TitleFontColor.a = titleFontColor_a;
            TitleColor.a = titleColor_a;
            WidgetColor.a = widgetColor_a;
        }

        void setColorFromSource(ofColor &targetColor, ofColor sourceColor)
        {
            targetColor.r = sourceColor.r;
            targetColor.g = sourceColor.g;
            targetColor.b = sourceColor.b;
            targetColor.a = sourceColor.a;
        }

        static int getR_fromRGBAHex(string RGBAHex)
        {
            if (RGBAHex.size() >= 2)
            {
                return min((int)ofHexToInt(RGBAHex.substr(0, 2)), 255);
            }
            return 0;
        }

        static int getG_fromRGBAHex(string RGBAHex)
        {
            if (RGBAHex.size() >= 4)
            {
                return min((int)ofHexToInt(RGBAHex.substr(2, 2)), 255);
            }
            return 0;
        }

        static int getB_fromRGBAHex(string RGBAHex)
        {
            if (RGBAHex.size() >= 6)
            {
                return min((int)ofHexToInt(RGBAHex.substr(4, 2)), 255);
            }
            return 0;
        }

        static int getA_fromRGBAHex(string RGBAHex)
        {
            if (RGBAHex.size() >= 8)
            {
                return min((int)ofHexToInt(RGBAHex.substr(6, 2)), 255);
            }
            return 255;
        }

        static ofColor getColor_fromRGBAHex(string RGBAHex)
        {
            ofStringReplace(RGBAHex, "#", "");
            return ofColor(
                getR_fromRGBAHex(RGBAHex),
                getG_fromRGBAHex(RGBAHex),
                getB_fromRGBAHex(RGBAHex),
                getA_fromRGBAHex(RGBAHex));
        }

        static string getHexFromInt(unsigned char c)
        {
            // Not necessarily the most efficient approach,
            // creating a new stringstream each time.
            // It'll do, though.
            std::stringstream ss;

            // Set stream modes
            ss << std::uppercase << std::setw(2) << std::setfill('0') << std::hex;

            // Stream in the character's ASCII code
            // (using `+` for promotion to `int`)
            ss << +c;

            // Return resultant string content
            return ss.str();
        }

        string Name = "";
        bool IsDark = false;

        // Widget
        bool RoundedRectangle = true;
        float UnhoveredWidgetAlpha = 0.9f;
        float HoveredWidgetAlpha = 0.95f;
        float ContentChromeAlpha = 0.8f;

        float UnhoveredTitleAlpha = 0.78f;
        float HoveredTitleAlpha = 0.95f;

        float UnhoveredTitleFontAlpha = 0.7f;
        float HoveredTitleFontAlpha = 0.9f;

        // These are not persisted
        float GenericHoveredAlpha = 0.5f;
        float GenericSelectedAlpha = 0.6f;

        ofColor TitleFontColor = ofColor(0, 0, 0, 238);
        ofColor TitleColor = ofColor(254, 254, 254, 255);

        ofColor TitleGradientColor1 = ofColor(255, 255, 255, 255);
        ofColor TitleGradientColor2 = ofColor(254, 254, 254, 255);
        ofColor TitleGradientColor3 = ofColor(254, 254, 254, 255);
        ofColor TitleGradientColor4 = ofColor(255, 255, 255, 255);

        ofColor TypographyPrimaryColor = ofColor(51, 51, 51, 255);
        ofColor TypographySecondaryColor = ofColor(85, 85, 85, 255);
        ofColor TypographyTertiaryColor = ofColor(170, 170, 170, 255);
        ofColor TypographyQuaternaryColor = ofColor(238, 238, 238, 255);

        ofColor SelectionForegroundColor = ofColor(255, 255, 255, 255);
        ofColor SelectionBackgroundColor = ofColor(153, 153, 153, 255);
        ofColor HoverForegroundColor = ofColor(255, 255, 255, 255);
        ofColor HoverBackgroundColor = ofColor(170, 170, 170, 255);

        ofColor MainMenuColor = ofColor(255, 255, 255, 255);
        ofColor PopoutMenuColor = ofColor(255, 255, 255, 255);
        ofColor WidgetColor = ofColor(255, 255, 255, 255);
        ofColor WidgetContentColor = ofColor(255, 255, 255, 238);

        ofColor WidgetAccent1Color = ofColor(0, 206, 209, 255);
        ofColor WidgetAccent2Color = ofColor(95, 158, 160, 255);
        ofColor WidgetAccent3Color = ofColor(0, 135, 147, 255);
        ofColor WidgetAccent4Color = ofColor(0, 77, 122, 255);

        ofColor BackgroundColor1 = ofColor(0, 206, 209, 255);
        ofColor BackgroundColor2 = ofColor(95, 158, 160, 255);
        ofColor BackgroundColor3 = ofColor(0, 137, 147, 255);
        ofColor BackgroundColor4 = ofColor(0, 77, 122, 255);

        ofColor WidgetAlternating1Color = ofColor(213, 213, 213, 255);
        ofColor WidgetAlternating2Color = ofColor(221, 221, 221, 255);
        ofColor WidgetModalBackgroundColor = ofColor(0, 0, 0, 180);

        // Element
        bool ElementRoundedRectangle = true;

        ofColor ElementForegroundColor = ofColor(51, 51, 51, 221);
        ofColor ElementBackgroundColor = ofColor(204, 204, 204, 255);

        float ElementHoveredForegroundAlpha = 0.95f;
        float ElementHoveredBackgroundAlpha = 0.95f;
        float ElementHoveredAccentAlpha = 0.95f;

        float ElementUnhoveredForegroundAlpha = 0.90f;
        float ElementUnhoveredBackgroundAlpha = 0.90f;

    private:
        bool mTitleGradientSet = false;
    };

    class WidgetThemeManager
    {

    public:
        WidgetThemeManager()
        {
        }

        WidgetThemeManager(ofxXmlSettings themesXML)
        {
            load_DEPRECATE(themesXML);
        }

        WidgetThemeManager(string themesXML)
        {
            load_DEPRECATE(themesXML);
        }

        bool load_DEPRECATE(ofxXmlSettings settings)
        {
            mThemes.clear();
            TiXmlElement *themesElm = settings.doc.RootElement(); // themes
            // TiXmlElement* themesElm = rootElm->FirstChildElement("themes");

            if (themesElm != NULL)
            {
                for (TiXmlNode *themeElm = themesElm->FirstChild(); themeElm; themeElm = themeElm->NextSibling())
                {
                    TiXmlPrinter printer;
                    themeElm->Accept(&printer);
                    string themeXML = printer.CStr();

                    WidgetTheme currTheme = WidgetTheme();
                    if (currTheme.load_DEPRECATE(themeXML))
                    {
                        mThemes.push_back(currTheme);
                        if (currTheme.Name == "System")
                            mSystemTheme = currTheme;
                        if (currTheme.Name == "SystemDark")
                            mSystemThemeDark = currTheme;
                    }
                }
                return true;
            }
            return false;
        }

        bool load_DEPRECATE(string themesXML)
        {
            ofxXmlSettings settings;
            if (settings.loadFromBuffer(themesXML.c_str()))
            {
                return load_DEPRECATE(settings);
            }
            return false;
        }

        bool loadThemesFromFile_DEPRECATE(string fileName)
        {
            ofxXmlSettings settings;
            if (settings.loadFile(fileName))
            {
                return load_DEPRECATE(settings);
            }
            else
            {
                return false;
            }
        }

        bool load(json &doc)
        {
            mThemes.clear();

            int themesLoaded = 0;
            for (auto theme : doc["themes"])
            {
                // cout << "SJON  " << theme.dump();

                WidgetTheme currTheme = WidgetTheme();
                if (currTheme.loadJson(theme.dump()))
                {
                    mThemes.push_back(currTheme);
                    if (currTheme.Name == "System")
                        mSystemTheme = currTheme;
                    if (currTheme.Name == "SystemDark")
                        mSystemThemeDark = currTheme;
                }

                themesLoaded++;
            }

            return themesLoaded > 0;
        }

        bool load(string json_string)
        {
            try
            {
                json doc = json::parse(json_string);
                return load(doc);
            }
            catch (exception &error)
            {
                std::cerr << "\nTheme JSON error: " << error.what() << " near ";
                return false;
            }
        }

        bool loadThemesFromFile(string fileName)
        {
            try
            {
                std::ifstream ifs(fileName);
                std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                return load(data);
            }
            catch (exception &error)
            {
                std::cerr << "\nError loading Theme file: `" << fileName << "` : " << error.what() << std::endl;
                return false;
            }
        }

        std::vector<WidgetTheme> getThemes()
        {
            return mThemes;
        }

        WidgetTheme getThemeByName(string themeName)
        {
            for (WidgetTheme theme : mThemes)
            {
                if (ofToLower(theme.Name) == ofToLower(themeName))
                {
                    return theme;
                }
            }
            return getDefaultTheme();
        }

        WidgetTheme getThemeByName(string themeName, bool lessOpacity)
        {
            WidgetTheme theme = getThemeByName(themeName);
            if (lessOpacity)
            {
                theme = getLessOpacity(theme, false);
            }
            return theme;
        }

        WidgetTheme getDefaultTheme()
        {
            return mDefaultTheme;
        }

        void setDefaultTheme(WidgetTheme theme)
        {
            mDefaultTheme = theme;
        }

        WidgetTheme getSystemTheme(bool lessOpacity)
        {
            WidgetTheme theme = (getDefaultTheme().IsDark ? mSystemThemeDark : mSystemTheme);
            if (lessOpacity)
            {
                theme = getLessOpacity(theme, false);
            }
            return theme;
        }

        WidgetTheme getSystemThemeLight(bool lessOpacity)
        {
            WidgetTheme theme = mSystemTheme;
            if (lessOpacity)
            {
                theme = getLessOpacity(theme, false);
            }
            return theme;
        }

        WidgetTheme getSystemThemeDark(bool lessOpacity)
        {
            WidgetTheme theme = mSystemThemeDark;
            if (lessOpacity)
            {
                theme = getLessOpacity(theme, false);
            }
            return theme;
        }

        WidgetTheme getThemeForMainMenu()
        {
            WidgetTheme theme = getDefaultTheme();
            theme.setWidgetColor(theme.MainMenuColor);
            theme.setTitleGradientColors(theme.MainMenuColor, theme.MainMenuColor, theme.MainMenuColor, theme.MainMenuColor);
            theme.setTitleColor(theme.MainMenuColor);
            return theme;
        }

        WidgetTheme getThemeForPopout()
        {
            WidgetTheme theme = getDefaultTheme();
            theme.setWidgetColor(theme.PopoutMenuColor);
            theme.setTitleGradientColors(theme.PopoutMenuColor, theme.PopoutMenuColor, theme.PopoutMenuColor, theme.PopoutMenuColor);
            theme.setTitleColor(theme.PopoutMenuColor);
            return theme;
        }

        WidgetTheme getLessOpacity(WidgetTheme theme, bool includingElements)
        {
            theme = getLessOpacity(theme, includingElements, 0.97f);
            return theme;
        }

        WidgetTheme getLessOpacity(WidgetTheme theme, bool includingElements, float perc)
        {
            theme.HoveredWidgetAlpha *= perc;
            theme.UnhoveredWidgetAlpha *= perc;
            theme.HoveredTitleAlpha *= perc;
            theme.UnhoveredTitleAlpha *= perc;

            // Element
            if (includingElements)
            {
                theme.ElementForegroundColor.a *= perc;
                theme.ElementBackgroundColor.a *= perc;
                theme.ElementHoveredAccentAlpha *= perc;
                theme.ElementUnhoveredForegroundAlpha *= perc;
                theme.ElementUnhoveredBackgroundAlpha *= perc;
            }

            return theme;
        }

        WidgetTheme getContrastingSystemTheme(WidgetTheme theme, bool lessOpacity)
        {
            return (theme.IsDark ? getSystemThemeLight(lessOpacity) : getSystemThemeDark(lessOpacity));
        }

        WidgetTheme getMatchingSystemTheme(WidgetTheme theme, bool lessOpacity)
        {
            return (theme.IsDark ? getSystemThemeDark(lessOpacity) : getSystemThemeLight(lessOpacity));
        }

    private:
        std::vector<WidgetTheme> mThemes = std::vector<WidgetTheme>();
        WidgetTheme mDefaultTheme = WidgetTheme();
        WidgetTheme mSystemTheme = WidgetTheme();
        WidgetTheme mSystemThemeDark = WidgetTheme();
    };

    //------------------------------ UI VIZ CLASS --------------------------------
    class Viz
    {

    public:
        // Events
        ofEvent<float> scaleChanged;

        Viz()
        {
            Viz(1.0f, "English", 60, true, false, "", "");
        }

        int MAX_FPS = 60;
        int MIN_FPS = 10;
        int MIN_FPS_FOR_ANIMATION = 20;
        Lang mLang = Lang();
        Lang mLangHelp = Lang();

        Viz(float SETTING_User_Scale, string SETTING_language, int SETTING_userExperience, bool SETTING_useFbo = true, bool SETTING_showFps = false, string SETTING_absoluteFontPath = "", bool SETTING_autoLoadMostRecentProject = true, string SETTING_themeName = "")
        {
            mSETTING_User_Scale = SETTING_User_Scale;
            mSETTING_language = SETTING_language;
            setUserExperience(SETTING_userExperience, true);
            mSETTING_useFbo = !ofFbo::checkGLSupport() ? false : SETTING_useFbo;
            mSETTING_showFps = SETTING_showFps;
            mSETTING_SETTING_autoLoadMostRecentProject = SETTING_autoLoadMostRecentProject;
            mSETTING_font_path = "fonts/";
            mSETTING_font_file = "Raleway-Bold.ttf";
            mSETTING_absolute_font_path = "";

            mFonts.setup(true); /// bool is for debug (set this to false eventually)
            mFonts.addFont("Default", "fonts/Raleway-Bold.ttf");
            mFonts.addFont("Monospace", "fonts/DejaVuSansMono-Bold.ttf");

            mLang.populateFromFile(SETTING_language);
            mLangHelp.populateFromHelpFile(SETTING_language);

            IconCache::bootstrapIconMapDefault(getScale());

            ofFile *f;

            if (SETTING_absoluteFontPath != "")
            {
                f = new ofFile(ofToDataPath(SETTING_absoluteFontPath));
            }
            else
            {
                f = new ofFile(mSETTING_font_path + mSETTING_font_file);
            }

            if (f->exists())
            {
                mSETTING_absolute_font_path = f->getAbsolutePath();
            }
            else
            {
                ofSystemAlertDialog("Error: Unable to find required font : " + f->getAbsolutePath());
                mSETTING_absolute_font_path = getUserSpecifiedFont();
                if (mSETTING_absolute_font_path == "")
                {
                    ofExit();
                }
            }

            loadTheme(SETTING_themeName, false);

            init();
        }

        void loadTheme(string themeName, bool reInit = true)
        {
            // themeManager.loadThemesFromFile_DEPRECATE("themes.xml");
            themeManager.loadThemesFromFile(ofToDataPath("themes.json"));
            WidgetTheme theme = themeManager.getThemeByName(themeName);
            themeManager.setDefaultTheme(theme);

            if (reInit)
                init();
        }

        void init()
        {

            setResolutionMultiplier();

            IconCache::scale(getScale());

            scaleFonts();

            cacheFontStyles(getThemeManager()->getDefaultTheme());

            isInitialConfigure = false;
        }

        void langLoadXML(string xml)
        {
            mLang.populate(xml);
        }

        void langLoadFile(string filename)
        {
            mLang.populateFromFile(filename);
        }

        string lang(string tag)
        {
            return mLang.get(tag);
        }

        void langHelpLoadXML(string xml)
        {
            mLangHelp.populate(xml);
        }

        void populateFromHelpFile(string filename)
        {
            mLangHelp.populateFromHelpFile(filename);
        }

        string langHelp(string tag)
        {
            return mLangHelp.get(tag);
        }

        void allocateFbo()
        {
            bool useFbo = true;
            if (useFbo)
            {
                fbo.allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGBA, 16); // GL_RGBA32F, 32);
                fbo.begin();
                ofClear(255, 255, 255);
                fbo.end();
            }
        }
        void fboBegin()
        {
            fbo.begin();
        }

        void fboEnd()
        {
            fbo.end();
        }

        void fboDraw()
        {
            fbo.draw(0, 0);
        }

        void clearFbo()
        {
            fbo.begin();
            ofClear(255, 255, 255);
            fbo.end();
        }

        string getUserSpecifiedFont()
        {
            ofFileDialogResult r = ofSystemLoadDialog("Please locate TrueType font (.ttf file)", false, ofDirectory("").getAbsolutePath());
            RegularExpression rex(".*.[ttf|TTF]");

            if (r.getPath() != "" && rex.match(r.getPath()))
            {
                mSETTING_absolute_font_path = r.getPath();
                return mSETTING_absolute_font_path;
            }
            else
            {
                return "";
            }
        }

        int get_MAX_FPS()
        {
            return MAX_FPS;
        }

        int get_MIN_FPS_FOR_ANIMATION()
        {
            return MIN_FPS_FOR_ANIMATION;
        }

        int getUserExperience()
        {
            return mSETTING_userExperience;
        }

        bool isBoostingUserExperience()
        {
            return mIsBoostingUserExperience;
        }

        bool isThrottlingUserExperience()
        {
            return mIsThrottlingUserExperience;
        }

        void throttleUserExperience(bool overrideAnimationThrottling)
        {
            if (!isThrottlingUserExperience() && (overrideAnimationThrottling || getUserExperience() >= get_MIN_FPS_FOR_ANIMATION()))
            {
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
                cout << "[THROTTLING USER EXPERIENCE: " << mSETTING_userExperience << "->" << MIN_FPS << "]" << endl;
#endif
                setUserExperience(MIN_FPS);
                mIsThrottlingUserExperience = true;
            }
        }

        void revertThrottleUserExperience()
        {
            if (mSETTING_userExperience_prev > mSETTING_userExperience)
            {
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
                cout << "[REVERTING THROTTLING OF USER EXPERIENCE: " << mSETTING_userExperience << "->" << mSETTING_userExperience_prev << "]" << endl;
#endif
                setUserExperience(mSETTING_userExperience_prev);
                mIsThrottlingUserExperience = false;
            }
        }

        void boostUserExperience(bool overrideAnimationThrottling)
        {
            if (!isBoostingUserExperience() && (overrideAnimationThrottling || getUserExperience() >= get_MIN_FPS_FOR_ANIMATION()))
            {
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
                cout << "[BOOSTING USER EXPERIENCE: " << mSETTING_userExperience << "->" << MAX_FPS << "]" << endl;
#endif
                setUserExperience(MAX_FPS);
                mIsBoostingUserExperience = true;
            }
        }

        void revertBoostUserExperience()
        {
            if (mSETTING_userExperience_prev < mSETTING_userExperience)
            {
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
                cout << "[REVERTING USER EXPERIENCE: " << mSETTING_userExperience << "->" << mSETTING_userExperience_prev << "]" << endl;
#endif
                setUserExperience(mSETTING_userExperience_prev);
                mIsBoostingUserExperience = false;
            }
        }

        void setUserExperience(int SETTING_userExperience)
        {
            setUserExperience(SETTING_userExperience, false);
        }

        void setUserExperience(int SETTING_userExperience, bool isInitializing)
        {

            if (!isThrottlingUserExperience() && !isBoostingUserExperience())
            {
                mSETTING_userExperience_prev = (mSETTING_userExperience_prev == 0) ? SETTING_userExperience : mSETTING_userExperience;
            }

            mSETTING_userExperience = SETTING_userExperience;

            if (mSETTING_userExperience > MAX_FPS)
            {
                mSETTING_userExperience = MAX_FPS;
            }

            if (mSETTING_userExperience < MIN_FPS)
            {
                mSETTING_userExperience = MIN_FPS;
            }

            if (isInitializing)
            {

                if (mSETTING_userExperience <= MIN_FPS)
                {
                    ofDisableAntiAliasing();
                    ofDisableSmoothing();
                }
                else
                {
                    ofEnableAntiAliasing();
                    ofEnableSmoothing();
                }
            }
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
            cout << "[SETTING USER EXPERIENCE: " << mSETTING_userExperience << "]" << endl;
#endif

            ofSetFrameRate(mSETTING_userExperience);
        }

        float getResolutionMultiplier()
        {
            return mResolutionMultiplier;
        }

        void setResolutionMultiplier()
        {

// Don't want iPad to change resolution when switching orientation
#ifdef TARGET_OF_IOS
            mResolutionMultiplier = 2;
            return;
#endif

            // Call this when the window changes from one monitor to another  (ie different DPI's)

            // DONT FORGET TO SET 'High Resolution Capable' = YES everytime a project is updated in XCode since it wipes the setting!
            int screenWidth = ofGetScreenWidth();
            int screenHeight = ofGetScreenHeight();

#if VIZ_DEBUG_LEVEL >= 1 && VIZ_DEBUG_LEVEL < 2
            cout << "Current ResolutionMultiplier is " << mResolutionMultiplier << "]" << screenWidth << "x" << screenHeight << endl;
#endif

            // int windowScale = (((ofAppGLFWWindow*)ofGetWindowPtr()))->getPixelScreenCoordScale();
            bool highDipEnabled = true; // windowScale > 1;

            if (screenWidth >= MIN_WIDTH_8K && screenHeight >= MIN_HEIGHT_8K)
            {
                mResolutionMultiplier = 4;
            }
            else if (screenWidth >= MIN_WIDTH_4K && screenHeight >= MIN_HEIGHT_4K)
            {
                mResolutionMultiplier = 2.5f;
            }
            else if (screenWidth >= MIN_WIDTH_RETINA && screenHeight >= MIN_HEIGHT_RETINA)
            {
                if (highDipEnabled)
                {
                    mResolutionMultiplier = 2;
                }
                else
                {
                    mResolutionMultiplier = 1;
                }
            }
            else
            {
                mResolutionMultiplier = 1;
            }

#if VIZ_DEBUG_LEVEL >= 1 && VIZ_DEBUG_LEVEL < 2
            cout << "SET ResolutionMultiplier to " << mResolutionMultiplier << "]" << endl;
#endif
        }

        void setUseFbo(int SETTING_useFbo)
        {
            mSETTING_useFbo = SETTING_useFbo;
        }

        bool getUseFbo()
        {
            return mSETTING_useFbo;
        }

        void setShowFps(int SETTING_showFps)
        {
            mSETTING_showFps = SETTING_showFps;
        }

        bool getShowFps()
        {
            return mSETTING_showFps;
        }

        void setUserScale(float val)
        {
            mSETTING_User_Scale = val;
            init();
            ofNotifyEvent(scaleChanged, val);
        }

        float getUserScale()
        {
            return mSETTING_User_Scale;
        }

        void setAutoLoadMostRecentProject(int SETTING_SETTING_autoLoadMostRecentProject)
        {
            mSETTING_SETTING_autoLoadMostRecentProject = SETTING_SETTING_autoLoadMostRecentProject;
        }

        bool getAutoLoadMostRecentProject()
        {
            return mSETTING_SETTING_autoLoadMostRecentProject;
        }

        void setUserFont(string absolutePath)
        {
            mSETTING_absolute_font_path = absolutePath;
            scaleFonts();
            cacheFontStyles(getThemeManager()->getDefaultTheme()); // delete above ^^

            mFonts.setup(true); /// bool is for debug (set this to false eventually)
            mFonts.addFont("Default", absolutePath);
        }

        float scale(float val)
        {
            return (float)val * getScale();
        }

        float deScale(float val)
        {
            // Useful for matching mouse coords (exact pixels) with coordinate non-scaled system
            return ((float)val / ((float)mSETTING_User_Scale * (float)getResolutionMultiplier()));
        }

        float getScale()
        {
            return (float)mSETTING_User_Scale * (float)getResolutionMultiplier();
        }

        float scaleAnimation(string widgetId, string animationName, float currVal, float targetVal, float secondsDuration)
        {
            return scaleForUserExperience(widgetId, animationName, currVal, targetVal, secondsDuration, false);
        }

        float scaleAnimationForUI(string widgetId, string animationName, float currVal, float targetVal, float secondsDuration)
        {
            return scaleForUserExperience(widgetId, animationName, currVal, targetVal, secondsDuration, true);
        }

        struct AnimationData
        {
            string widgetId;
            string animationName;
            float startVal;
            float targetVal;
            float secondsDuration;
            AnimationData(string widgetId, string animationName, float startVal, float targetVal, float secondsDuration) : widgetId(widgetId), animationName(animationName), startVal(startVal), targetVal(targetVal), secondsDuration(secondsDuration) {};
        };

        vector<AnimationData> getAnimations()
        {
            return mAnimations;
        }

        bool isWidgetAnimating(string widgetId)
        {
            for (auto animation : mAnimations)
            {
                if (animation.widgetId == widgetId)
                {
                    return true;
                }
            }
            return false;
        }

        void removeAnimation(string animationName)
        {
            mAnimations.erase(std::remove_if(
                                  mAnimations.begin(), mAnimations.end(),
                                  [&](AnimationData const &animation)
                                  {
                                      return animation.animationName == animationName;
                                  }),
                              mAnimations.end());
        }

        float getNonScaledPadding()
        {
            return 5;
        }

        float getScaledPadding()
        {
            return scale(5);
        }

        void uncacheFonts()
        {
#if VIZ_DEBUG_LEVEL >= 1 && VIZ_DEBUG_LEVEL <= 2
            cout << "flushing fonts..." << endl;
#endif
            mTinyFont.reset();
            mSmallFont.reset();
            mMediumFont.reset();
            mLargeFont.reset();
            mXLargeFont.reset();
            mXXLargeFont.reset();
            mXXXLargeFont.reset();
        }

        /* TODO ---------------- PORT THIS OVER TO WidgetManager class  BEGIN  ------ */

        void setIsWidgetDragging(bool value)
        {
            mIsWidgetDragging = value;
        }

        bool isWidgetDragging()
        {
            return mIsWidgetDragging;
        }

        void setIsWidgetResizing(bool value)
        {
            mIsWidgetResizing = value;
        }

        bool isWidgetResizing()
        {
            return mIsWidgetResizing;
        }

        vector<string> getWidgetIds()
        {
            return mWidgetIds;
        }

        void addWidgetId(string widgetId)
        {
            mWidgetIds.push_back(widgetId);
        }

        void removeWidgetId(string widgetId)
        {

            auto itr = std::find(mWidgetIds.begin(), mWidgetIds.end(), widgetId);
            if (itr != mWidgetIds.end())
                mWidgetIds.erase(itr);
        }

        void setActiveWidgetId(string widgetId)
        {
            if (mActiveWidgetId != widgetId && mActiveWidgetId != "")
            {
                mPriorActiveWidgetId = mActiveWidgetId;
            }
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
            cout << "SETTING ACTIVE WIDGET : " << widgetId << "\n";
#endif
            mActiveWidgetId = widgetId;
        }

        string getActiveWidgetId()
        {
            return mActiveWidgetId;
        }

        void setTopmostActiveRootWidgetId(string widgetId)
        {
            if (mTopmostActiveRootWidgetId != widgetId && mTopmostActiveRootWidgetId != "")
            {
                mPriorTopmostActiveRootWidgetId = mTopmostActiveRootWidgetId;
            }
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
            cout << "SETTING ROOT ACTIVE WIDGET : " << widgetId << "\n";
#endif
            mTopmostActiveRootWidgetId = widgetId;
        }

        string getTopmostActiveRootWidgetId()
        {
            return mTopmostActiveRootWidgetId;
        }

        string getPriorTopmostActiveRootWidgetId()
        {
            return mPriorTopmostActiveRootWidgetId;
        }

        string getTargetDropWidgetId()
        {
            return mTargetDropWidgetId;
        }

        void setTargetDropWidgetId(string widgetId)
        {
            mTargetDropWidgetId = widgetId;
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
            cout << "SETTING DROP TARGET WIDGET : " << widgetId << "\n";
#endif
        }

        void clearTargetDropWidgetId()
        {
            mTargetDropWidgetId = "";
        }

        string getPriorActiveWidgetId()
        {
            return mPriorActiveWidgetId;
        }

        bool isActiveWidgetId(string widgetId)
        {
            return mActiveWidgetId == widgetId;
        }

        void clearActiveWidgetId()
        {
            /*
                If getting unexpected EXC_BAD_ACCESS errors make sure to check that ALL widgets are callling
                ofRemoveListener for ALL corresponding ofAddListener in ther destructors!
            */

            if (mActiveWidgetId != "")
            {
                mPriorActiveWidgetId = mActiveWidgetId;
            }
            mActiveWidgetId = "";
        }

        void clearPriorActiveWidgetId()
        {
            mPriorActiveWidgetId = "";
        }

        void setActiveChildWidgetId(string widgetId)
        {
            if (mActiveChildWidgetId != widgetId && mActiveChildWidgetId != "")
            {
                mPriorActiveChildWidgetId = mActiveChildWidgetId;
            }
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
            cout << "SETTING ACTIVE CHILD WIDGET : " << widgetId << "\n";
#endif
            mActiveChildWidgetId = widgetId;
        }

        string getActiveChildWidgetId()
        {
            return mActiveChildWidgetId;
        }

        string getPriorActiveChildWidgetId()
        {
            return mPriorActiveChildWidgetId;
        }

        void setNextInLineForFocusWidgetId(string widgetId)
        {
            mNextInLineForFocusWidgetId = widgetId;
        }

        string getNextInLineForFocusWidgetId()
        {
            return mNextInLineForFocusWidgetId;
        }

        string getModalWidgetId()
        {
            return mModalWidgetId;
        }

        bool getIsPopoutVisible()
        {
            return mVisiblePopouts.size() > 0;
        }

        void setIsPopoutVisible(string popoutWidgetId, bool val)
        {
            auto itr = std::find(mVisiblePopouts.begin(), mVisiblePopouts.end(), popoutWidgetId);
            if (itr != mVisiblePopouts.end())
                mVisiblePopouts.erase(itr);
            if (val)
                mVisiblePopouts.push_back(popoutWidgetId);
        }

        string getTopmostPopoutWidgetId()
        {
            if (mVisiblePopouts.size() > 0)
            {
                return mVisiblePopouts[mVisiblePopouts.size() - 1];
            }
            return "";
        }

        vector<string> getPopoutWidgetIds()
        {
            return mVisiblePopouts;
        }

        void clearPopoutWidgetIds()
        {
            mVisiblePopouts.clear();
        }

        void setModalWidgetId(string widgetId)
        {
            if (widgetId != mModalWidgetId)
                mPriorModalWidgetId = mModalWidgetId;
            mModalWidgetId = widgetId;
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
            cout << "SETTING MODAL WIDGET : " << widgetId << "\n";
#endif
        }

        bool isModalWidgetId(string widgetId)
        {
            return mModalWidgetId == widgetId;
        }

        string getPriorModalWidgetId()
        {
            return mPriorModalWidgetId;
        }

        void clearModalWidgetId()
        {
            mModalWidgetId = "";
        }

        void setIsAnyWidgetDraggingOrResizing(bool value)
        {
            mIsAnyWidgetDraggingOrResizing = value;
        }

        bool getIsAnyWidgetDraggingOrResizing()
        {
            return mIsAnyWidgetDraggingOrResizing;
        }

        bool mIsWidgetDragging = false;
        bool mIsWidgetResizing = false;

        vector<string> mWidgetIds = vector<string>();
        vector<string> mVisiblePopouts = vector<string>();

        string mActiveWidgetId = "";
        string mTopmostActiveRootWidgetId = "";
        string mPriorActiveWidgetId = "";
        string mPriorTopmostActiveRootWidgetId = "";

        string mActiveChildWidgetId = "";
        string mNextInLineForFocusWidgetId = "";
        string mPriorActiveChildWidgetId = "";

        string mModalWidgetId = "";
        string mPriorModalWidgetId = "";
        string mTargetDropWidgetId = "";
        bool mIsAnyWidgetDraggingOrResizing = false;

        /* TODO ---------------- PORT THIS OVER TO WidgetManager class  END  ------ */

        // todo set up some constants

        void cacheFontStyles(WidgetTheme theme)
        {

            int resolutionMultiplier = getResolutionMultiplier();
            int fontScale = int((float)resolutionMultiplier * (float)mSETTING_User_Scale);
            if (fontScale <= 0)
                fontScale = 1;

            ofColor defaultColor = theme.TypographyPrimaryColor_withAlpha(1);

            ofxFontStash2::Style tiny;
            tiny.fontID = "Default";
            tiny.fontSize = 4 * fontScale;
            tiny.color = defaultColor;
            mFonts.addStyle("tiny", tiny);

            ofxFontStash2::Style small;
            small.fontID = "Default";
            small.fontSize = 9 * fontScale;
            small.color = defaultColor;
            mFonts.addStyle("small", small);

            ofxFontStash2::Style medium;
            medium.fontID = "Default";
            medium.fontSize = 11 * fontScale;
            medium.color = defaultColor;
            mFonts.addStyle("medium", medium);

            ofxFontStash2::Style large;
            large.fontID = "Default";
            large.fontSize = 14 * fontScale;
            large.color = defaultColor;
            mFonts.addStyle("large", large);

            ofxFontStash2::Style xlarge;
            xlarge.fontID = "Default";
            xlarge.fontSize = 20 * fontScale;
            xlarge.color = defaultColor;
            mFonts.addStyle("xlarge", xlarge);

            ofxFontStash2::Style xxlarge;
            xxlarge.fontID = "Default";
            xxlarge.fontSize = 24 * fontScale;
            xxlarge.color = defaultColor;
            mFonts.addStyle("xxlarge", xxlarge);

            ofxFontStash2::Style xxxlarge;
            xxxlarge.fontID = "Default";
            xxxlarge.fontSize = 24 * fontScale;
            xxxlarge.color = defaultColor;
            mFonts.addStyle("xxxlarge", xxxlarge);

            // Mono

            ofxFontStash2::Style smallMono;
            smallMono.fontID = "Monospace";
            smallMono.fontSize = 13 * fontScale;
            smallMono.color = defaultColor;
            mFonts.addStyle("small-mono", smallMono);

            ofxFontStash2::Style mediumMono;
            mediumMono.fontID = "Monospace";
            mediumMono.fontSize = 15 * fontScale;
            mediumMono.color = defaultColor;
            mFonts.addStyle("medium-mono", mediumMono);

            ofxFontStash2::Style largeMono;
            largeMono.fontID = "Monospace";
            largeMono.fontSize = 18 * fontScale;
            largeMono.color = defaultColor;
            mFonts.addStyle("large-mono", largeMono);

            mFonts.setDefaultStyle("xlarge");
        }

        ofxFontStash2::Style getScaledFontStyle(string styleName)
        {
            return mFonts.getStyle(styleName);
        }

        ofxFontStash2::Fonts *getFonts(WidgetTheme theme)
        {

            int resolutionMultiplier = getResolutionMultiplier();
            int fontScale = int((float)resolutionMultiplier * (float)mSETTING_User_Scale);
            if (fontScale <= 0)
                fontScale = 1;

            ofColor defaultColor = theme.TypographyPrimaryColor_withAlpha(1);

            mFonts.getStyles()["tiny"].color = defaultColor;
            mFonts.getStyles()["small"].color = defaultColor;
            mFonts.getStyles()["medium"].color = defaultColor;
            mFonts.getStyles()["large"].color = defaultColor;
            mFonts.getStyles()["xlarge"].color = defaultColor;
            mFonts.getStyles()["xxlarge"].color = defaultColor;
            mFonts.getStyles()["xxlarge"].color = defaultColor;

            return &mFonts;
        }

        ofxFontStash2::Fonts *getFonts()
        {
            return &mFonts;
        }

        ofRectangle getTextBounds(string text, ofxFontStash2::Style fontStyle, int x, int y)
        {
            if (text.size() > 1)
            {
                text[0] = 'X'; // Char with uppercase
                text[1] = 'y'; // Char with "a tail"
                return getFonts()->getTextBounds(text, fontStyle, x, y);
            }
            else
            {
                ofRectangle r1 = getFonts()->getTextBounds("X", fontStyle, x, y);
                ofRectangle r2 = getFonts()->getTextBounds("y", fontStyle, x, y);
                return ofRectangle(x, y, r1.width, r2.height);
            }
        }

        ofxFontStash2::Style getSmallFontStyleSizedForDimensions(int width, int height)
        {
            if (width >= 800 && height >= 800)
            {
                return getXXXLargeFontStyle();
            }
            else if (width >= 700 && height >= 700)
            {
                return getXXLargeFontStyle();
            }
            else if (width >= 650 && height >= 650)
            {
                return getXLargeFontStyle();
            }
            else if (width >= 400 && height >= 400)
            {
                return getLargeFontStyle();
            }
            else if (width >= 300 && height >= 300)
            {
                return getMediumFontStyle();
            }
            else if (width >= 200 && height >= 200)
            {
                return getSmallFontStyle();
            }
            else
            {
                return getTinyFontStyle();
            }
        }

        ofxFontStash2::Style getMediumFontStyleSizedForDimensions(int width, int height)
        {
            if (width >= 600 && height >= 600)
            {
                return getXXXLargeFontStyle();
            }
            else if (width >= 500 && height >= 500)
            {
                return getXXLargeFontStyle();
            }
            else if (width >= 450 && height >= 450)
            {
                return getXLargeFontStyle();
            }
            else if (width >= 400 && height >= 400)
            {
                return getLargeFontStyle();
            }
            else if (width >= 200 && height >= 200)
            {
                return getMediumFontStyle();
            }
            else if (width >= 150 && height >= 150)
            {
                return getMediumFontStyle();
            }
            else
            {
                return getSmallFontStyle();
            }
        }

        ofxFontStash2::Style getFontStyleSizedForDimensions(int width, int height)
        {
            if (width >= 500 && height >= 500)
            {
                return getXXXLargeFontStyle();
            }
            else if (width >= 400 && height >= 400)
            {
                return getXXLargeFontStyle();
            }
            else if (width >= 300 && height >= 300)
            {
                return getXLargeFontStyle();
            }
            else if (width >= 250 && height >= 250)
            {
                return getLargeFontStyle();
            }
            else if (width >= 150 && height >= 150)
            {
                return getMediumFontStyle();
            }
            else if (width >= 100 && height >= 100)
            {
                return getSmallFontStyle();
            }
            else
            {
                return getTinyFontStyle();
            }
        }

        ofxFontStash2::Style getLargeStyleFontSizedForDimensions(int width, int height)
        {
            if (width >= 250 && height >= 250)
            {
                return getXXXLargeFontStyle();
            }
            else if (width >= 200 && height >= 200)
            {
                return getXXLargeFontStyle();
            }
            else if (width >= 150 && height >= 150)
            {
                return getXLargeFontStyle();
            }
            else if (width >= 100 && height >= 100)
            {
                return getLargeFontStyle();
            }
            else if (width >= 50 && height >= 50)
            {
                return getMediumFontStyle();
            }
            else if (width >= 25 && height >= 25)
            {
                return getSmallFontStyle();
            }
            else
            {
                return getTinyFontStyle();
            }
        }

        ofxFontStash2::Style getTinyFontStyle()
        {
            return mFonts.getStyle("tiny");
        }

        ofxFontStash2::Style getSmallFontStyle()
        {
            return mFonts.getStyle("small");
        }

        ofxFontStash2::Style getMediumFontStyle()
        {
            return mFonts.getStyle("medium");
        }

        ofxFontStash2::Style getLargeFontStyle()
        {
            return mFonts.getStyle("large");
        }

        ofxFontStash2::Style getXLargeFontStyle()
        {
            return mFonts.getStyle("xlarge");
        }

        ofxFontStash2::Style getXXLargeFontStyle()
        {
            return mFonts.getStyle("xxlarge");
        }

        ofxFontStash2::Style getXXXLargeFontStyle()
        {
            return mFonts.getStyle("xxxlarge");
        }

        ofxFontStash2::Style getSmallFontStyleMono()
        {
            return mFonts.getStyle("small-mono");
        }

        ofxFontStash2::Style getMediumFontStyleMono()
        {
            return mFonts.getStyle("medium-mono");
        }

        ofxFontStash2::Style getLargeFontStyleMono()
        {
            return mFonts.getStyle("large-mono");
        }

        shared_ptr<ofxSmartFont> getScaledFont(string fontName)
        {
            // USE ONE OF: (tiny | small | medium | large | xlarge)
            int resolutionMultiplier = getResolutionMultiplier();

            int fontScale = int((float)resolutionMultiplier * (float)mSETTING_User_Scale);

            if (fontScale <= 0)
            {
                fontScale = 1;
            }

            /*
            if (fontScale > 180) {
                fontScale = 180;
            }
            */
            ofxSmartFont::add(mSETTING_absolute_font_path, fontScale * 4, "tiny_x" + ofToString(fontScale));
            ofxSmartFont::add(mSETTING_absolute_font_path, fontScale * 9, "small_x" + ofToString(fontScale));
            ofxSmartFont::add(mSETTING_absolute_font_path, fontScale * 11, "medium_x" + ofToString(fontScale));
            ofxSmartFont::add(mSETTING_absolute_font_path, fontScale * 14, "large_x" + ofToString(fontScale));
            ofxSmartFont::add(mSETTING_absolute_font_path, fontScale * 20, "xlarge_x" + ofToString(fontScale));
            ofxSmartFont::add(mSETTING_absolute_font_path, fontScale * 24, "xxlarge_x" + ofToString(fontScale));
            ofxSmartFont::add(mSETTING_absolute_font_path, fontScale * 30, "xxxlarge_x" + ofToString(fontScale));

            shared_ptr<ofxSmartFont> theFont = ofxSmartFont::get(fontName + "_x" + ofToString(fontScale));
            if (theFont == nullptr)
            {
                cout << "fatal: could NOT find cached font '" << fontName + "_x" + ofToString(fontScale) << "'. Does it exist on the file system and was name specified as one of : (tiny | small | medium | large | xlarge) " << endl;
            }
            return theFont;
        }

        shared_ptr<ofxSmartFont> getSmallFontSizedForDimensions(int width, int height)
        {
            if (width >= 700 && height >= 700)
            {
                return mXXXLargeFont;
            }
            else if (width >= 600 && height >= 600)
            {
                return mXXLargeFont;
            }
            else if (width >= 500 && height >= 500)
            {
                return mXLargeFont;
            }
            else if (width >= 400 && height >= 400)
            {
                return mLargeFont;
            }
            else if (width >= 300 && height >= 300)
            {
                return mMediumFont;
            }
            else if (width >= 200 && height >= 200)
            {
                return mSmallFont;
            }
            else
            {
                return mTinyFont;
            }
        }

        shared_ptr<ofxSmartFont> getMediumFontSizedForDimensions(int width, int height)
        {
            if (width >= 600 && height >= 600)
            {
                return mXXXLargeFont;
            }
            else if (width >= 500 && height >= 500)
            {
                return mXXLargeFont;
            }
            else if (width >= 450 && height >= 450)
            {
                return mXLargeFont;
            }
            else if (width >= 400 && height >= 400)
            {
                return mLargeFont;
            }
            else if (width >= 200 && height >= 200)
            {
                return mMediumFont;
            }
            else if (width >= 150 && height >= 150)
            {
                return mSmallFont;
            }
            else
            {
                return mSmallFont;
            }
        }

        shared_ptr<ofxSmartFont> getFontSizedForDimensions(int width, int height)
        {
            if (width >= 500 && height >= 500)
            {
                return mXXXLargeFont;
            }
            else if (width >= 400 && height >= 400)
            {
                return mXXLargeFont;
            }
            else if (width >= 300 && height >= 300)
            {
                return mXLargeFont;
            }
            else if (width >= 250 && height >= 250)
            {
                return mLargeFont;
            }
            else if (width >= 150 && height >= 150)
            {
                return mMediumFont;
            }
            else if (width >= 100 && height >= 100)
            {
                return mSmallFont;
            }
            else
            {
                return mTinyFont;
            }
        }

        shared_ptr<ofxSmartFont> getLargeFontSizedForDimensions(int width, int height)
        {
            if (width >= 250 && height >= 250)
            {
                return mXXXLargeFont;
            }
            else if (width >= 200 && height >= 200)
            {
                return mXXLargeFont;
            }
            else if (width >= 150 && height >= 150)
            {
                return mXLargeFont;
            }
            else if (width >= 100 && height >= 100)
            {
                return mLargeFont;
            }
            else if (width >= 50 && height >= 50)
            {
                return mMediumFont;
            }
            else if (width >= 25 && height >= 25)
            {
                return mSmallFont;
            }
            else
            {
                return mTinyFont;
            }
        }

        shared_ptr<ofxSmartFont> getTinyFont()
        {
            return mTinyFont;
        }

        shared_ptr<ofxSmartFont> getSmallFont()
        {
            return mSmallFont;
        }

        shared_ptr<ofxSmartFont> getMediumFont()
        {
            return mMediumFont;
        }

        shared_ptr<ofxSmartFont> getLargeFont()
        {
            return mLargeFont;
        }

        shared_ptr<ofxSmartFont> getXLargeFont()
        {
            return mXLargeFont;
        }

        shared_ptr<ofxSmartFont> getXXLargeFont()
        {
            return mXXLargeFont;
        }

        shared_ptr<ofxSmartFont> getXXXLargeFont()
        {
            return mXXXLargeFont;
        }

        WidgetThemeManager *getThemeManager()
        {
            return &themeManager;
        }

        struct WidgetEvent
        {
            string address;
            string eventSenderType;
            string eventSenderId;
            string eventTargetId;
            string eventName;
            string redirectEventName;
            string eventXML;
            WidgetEvent();

            WidgetEvent(string eventSenderId, string eventTargetId, string eventName, string redirectEventName = "") : eventSenderId(eventSenderId), eventTargetId(eventTargetId), eventName(eventName), redirectEventName(redirectEventName) {};

            WidgetEvent(string address, string eventSenderType, string eventSenderId, string eventTargetId, string eventName, string redirectEventName, string eventXML) : address(address), eventSenderType(eventSenderType), eventSenderId(eventSenderId), eventTargetId(eventTargetId), eventName(eventName), redirectEventName(redirectEventName), eventXML(eventXML) {};
        };

        void addToEventTransmitQueue(string address, string eventSenderType, string eventSenderId, string eventTargetId, string eventName, string eventXML, bool osc, bool dispatchToLocalListeners)
        {

            if (dispatchToLocalListeners)
            {

                for (WidgetEvent e : mEventListenMap[eventSenderId])
                {
                    string eventNameToUse = (e.redirectEventName != "" ? e.redirectEventName : e.eventName);
                    mLocalListenTransmitEvents.push_back(WidgetEvent(address, eventSenderType, eventSenderId, e.eventTargetId, eventNameToUse, "", eventXML));
                }
            }

            if (osc && getIsSendingOsc())
            {
                mOscTransmitEvents.push_back(WidgetEvent(address, eventSenderType, eventSenderId, eventTargetId, eventName, "", eventXML));
            }
        }

        void addToEventTransmitQueue(string address, string eventSenderType, string eventSenderId, string eventTargetId, string eventName, string eventXML, bool osc)
        {
            addToEventTransmitQueue(address, eventSenderType, eventSenderId, eventTargetId, eventName, eventXML, osc, true);
        }

        void addToEventTransmitQueue(string address, string eventSenderType, string eventSenderId, string eventName, string eventXML, bool osc)
        {
            addToEventTransmitQueue(address, eventSenderType, eventSenderId, "", eventName, eventXML, osc, true);
        }

        bool hasLocalListenEventsInTransmitQueue()
        {
            return mLocalListenTransmitEvents.size() > 0;
        }

        WidgetEvent getNextLocalListenMessageToTransmit()
        {
            if (mLocalListenTransmitEvents.size() > 0)
            {
                WidgetEvent e = mLocalListenTransmitEvents[mLocalListenTransmitEvents.size() - 1];
                mLocalListenTransmitEvents.pop_back();
                return e;
            }
            return WidgetEvent("", "", "");
        }

        bool hasOscEventsInTransmitQueue()
        {
            return getIsSendingOsc() && mOscTransmitEvents.size() > 0;
        }

        void getNextOscMessageToTransmit(ofxOscMessage &m)
        {
            if (mOscTransmitEvents.size() > 0)
            {
                WidgetEvent e = mOscTransmitEvents[mOscTransmitEvents.size() - 1];
                m.setAddress(e.address);
                m.addStringArg(e.eventSenderType);
                m.addStringArg(e.eventSenderId);
                m.addStringArg(e.eventTargetId);
                m.addStringArg(e.eventName);
                m.addStringArg(e.redirectEventName);
                m.addStringArg(e.eventXML);
                mOscTransmitEvents.pop_back();
            }
        }

        bool hasOscEventsInReceiveQueue()
        {
            return getIsReceivingOsc() && osc_Receiver.hasWaitingMessages();
        }

        void getNextOscMessage(ofxOscMessage &m)
        {
            osc_Receiver.getNextMessage(&m);
        }

        void sendOscMessage(ofxOscMessage &m)
        {
            osc_Sender.sendMessage(m);
        }

        bool doesWidgetHaveListeners(string eventSourceId, string eventName)
        {
            for (WidgetEvent e : mEventListenMap[eventSourceId])
            {
                if (e.eventName == eventName)
                {
                    return true;
                }
            }
            return false;
        }

        void addToWidgetEventListenMap(string eventListenerId, string eventSourceId, string eventName, string redirectEventName = "", bool allowDuplicates = false)
        {
            bool found = false;
            if (eventListenerId == eventSourceId || eventListenerId == "" || eventSourceId == "" || eventName == "")
                return;
            if (!allowDuplicates)
            {
                for (WidgetEvent e : mEventListenMap[eventSourceId])
                {
                    if (e.eventTargetId == eventListenerId && e.eventName == eventName && e.redirectEventName == redirectEventName)
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (!found)
            {
                mEventListenMap[eventSourceId].push_back(WidgetEvent(eventSourceId, eventListenerId, eventName, redirectEventName));
            }
        }

        void updateWidgetEventListenMap(string eventListenerId, string eventSourceId, string eventName, string redirectEventName = "", string old_eventName = "", string old_redirectEventName = "")
        {
            for (WidgetEvent &e : mEventListenMap[eventSourceId])
            {
                if (e.eventTargetId == eventListenerId && e.eventName == old_eventName && e.redirectEventName == old_redirectEventName)
                {
                    e.eventName = eventName;
                    e.redirectEventName = redirectEventName;
                }
            }
        }

        void removeFromWidgetEventListenMap(string eventListenerId, string eventSourceId, string eventName, string redirectEventName = "")
        {
            vector<WidgetEvent> e = mEventListenMap[eventSourceId];
            mEventListenMap[eventSourceId].erase(std::remove_if(
                                                     mEventListenMap[eventSourceId].begin(), mEventListenMap[eventSourceId].end(),
                                                     [&](WidgetEvent const &ei)
                                                     {
                                                         return ei.eventTargetId == eventListenerId && ei.eventName == eventName && ei.redirectEventName == redirectEventName;
                                                     }),
                                                 mEventListenMap[eventSourceId].end());
        }

        vector<WidgetEvent> getListeningEvents(string eventListenerId)
        { // The list of events this widget is listening to
            vector<WidgetEvent> events;
            std::map<std::string, vector<WidgetEvent>>::iterator it = mEventListenMap.begin();
            while (it != mEventListenMap.end())
            {
                string currentSourceWidgetPersistentId = it->first;
                for (WidgetEvent e : mEventListenMap[currentSourceWidgetPersistentId])
                {
                    if (e.eventTargetId == eventListenerId)
                    {
                        events.push_back(WidgetEvent(currentSourceWidgetPersistentId, eventListenerId, e.eventName, e.redirectEventName));
                    }
                }
                it++;
            }
            return events;
        }

        void removeListeningEvents(string eventListenerId)
        { // The list of events this widget is listening to
            vector<Viz::WidgetEvent> listeningEvents = getListeningEvents(eventListenerId);
            for (Viz::WidgetEvent e : listeningEvents)
            {
                removeFromWidgetEventListenMap(e.eventTargetId, e.eventSenderId, e.eventName, e.redirectEventName);
            }
        }

        vector<WidgetEvent> getListenEvents(string eventSourceId)
        { // List of events listening to this widget
            return mEventListenMap[eventSourceId];
        }

        void setupOscSender(string host, int sendPort)
        {
            osc_Sender.setup(host, sendPort);
            setIsSendingOsc(true);
        }

        void setupOscReceiver(int receivePort)
        {
            osc_Receiver.setup(receivePort);
            setIsReceivingOsc(true);
        }

        bool getIsSendingOsc()
        {
            return mIsSendingOsc;
        }

        void setIsSendingOsc(bool val)
        {
            mIsSendingOsc = val;
        }

        bool getIsReceivingOsc()
        {
            return mIsReceivingOsc;
        }

        void setIsReceivingOsc(bool val)
        {
            mIsReceivingOsc = val;
        }

    private:
        ofFbo fbo;
        bool isInitialConfigure = true;
        float mSETTING_User_Scale = 1.0;
        string mSETTING_language;
        int mSETTING_userExperience = MAX_FPS;
        int mSETTING_userExperience_prev = 0;
        string mSETTING_font_path = "fonts/";
        string mSETTING_font_file = "Raleway-Regular.ttf";
        string mSETTING_absolute_font_path = "";
        bool mSETTING_useFbo = true;
        bool mSETTING_showFps = false;
        bool mIsBoostingUserExperience = false;
        bool mIsThrottlingUserExperience = false;
        bool mSETTING_SETTING_autoLoadMostRecentProject = true;
        WidgetThemeManager themeManager = WidgetThemeManager();
        // v2 fonts:
        ofxFontStash2::Fonts mFonts;
        ofxFontStash2::Style mFontStyle;

        // OSC
        ofxOscSender osc_Sender;
        ofxOscReceiver osc_Receiver;
        bool mIsSendingOsc = false;
        bool mIsReceivingOsc = false;

        vector<WidgetEvent> mLocalListenTransmitEvents = vector<WidgetEvent>();
        vector<WidgetEvent> mOscTransmitEvents = vector<WidgetEvent>();
        std::map<std::string, vector<WidgetEvent>> mEventListenMap;

        int mResolutionMultiplier = 1;

        shared_ptr<ofxSmartFont> mTinyFont;
        shared_ptr<ofxSmartFont> mSmallFont;
        shared_ptr<ofxSmartFont> mMediumFont;
        shared_ptr<ofxSmartFont> mLargeFont;
        shared_ptr<ofxSmartFont> mXLargeFont;
        shared_ptr<ofxSmartFont> mXXLargeFont;
        shared_ptr<ofxSmartFont> mXXXLargeFont;

        void scaleFonts()
        {
            uncacheFonts();
            mTinyFont = getScaledFont("tiny");
            mSmallFont = getScaledFont("small");
            mMediumFont = getScaledFont("medium");
            mLargeFont = getScaledFont("large");
            mXLargeFont = getScaledFont("xlarge");
            mXXLargeFont = getScaledFont("xxlarge");
            mXXXLargeFont = getScaledFont("xxxlarge");
        }

        vector<AnimationData> mAnimations = vector<AnimationData>();

        AnimationData getAnimation(string name)
        {
            for (auto animation : mAnimations)
            {
                if (animation.animationName == name)
                    return animation;
            }
            AnimationData notFound = AnimationData("NOT_FOUND", "NOT_FOUND", 0.0f, 0.0f, 0.0f);
            return notFound;
        }

        void addAnimation(AnimationData &animation)
        {
            mAnimations.push_back(animation);
        }

        float scaleForUserExperience(string widgetId, string animationName, float currVal, float targetVal, float secondsDuration, bool includeResolutionMultiplier)
        {
            /*
             This only wirks with positive values at the moment
             */

            if (getUserExperience() <= get_MIN_FPS_FOR_ANIMATION())
            {
                removeAnimation(animationName);
                return targetVal;
            }

            bool isDecreasing = false;
            AnimationData animation = getAnimation(animationName);
            if (animation.animationName == "NOT_FOUND")
            {
                animation = AnimationData(widgetId, animationName, currVal, targetVal, secondsDuration);
                addAnimation(animation);
            }
            else
            {
                isDecreasing = animation.targetVal < animation.startVal;
                if ((isDecreasing && currVal <= targetVal) || (!isDecreasing && currVal >= animation.targetVal))
                {
                    removeAnimation(animation.animationName);
                    return targetVal;
                }
            }

            // Original
            // float newVal = ((isDecreasing ? animation.startVal : animation.targetVal) / (float)(includeResolutionMultiplier ? scale(animation.secondsDuration) : animation.secondsDuration)) / (float)getUserExperience() * (isDecreasing ? -1.0f : 1.0f);
            // float retVal = currVal + (includeResolutionMultiplier ? scale(newVal) : newVal);

            // Decrease simply dows not work....
            float newVal;
            float retVal;

            if (isDecreasing)
            {
                float zzz = 0;
                zzz += (animation.startVal > 0 ? animation.startVal : animation.startVal * -1.0f);
                zzz += (animation.targetVal > 0 ? animation.targetVal : animation.targetVal * -1.0f);

                newVal = (zzz / (float)(includeResolutionMultiplier ? scale(animation.secondsDuration) : animation.secondsDuration)) / (float)getUserExperience() * (isDecreasing ? -1.0f : 1.0f);
                retVal = currVal - (includeResolutionMultiplier ? scale(newVal) : newVal);
            }
            else
            {
                newVal = (animation.targetVal / ((float)(includeResolutionMultiplier ? scale(animation.secondsDuration) : animation.secondsDuration))) / (float)getUserExperience();
                retVal = currVal + (includeResolutionMultiplier ? scale(newVal) : newVal);
            }

            if ((isDecreasing && retVal <= targetVal) || (!isDecreasing && retVal >= targetVal))
            {
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
                cout << "[ANIMATION: " << animation.animationName << "] reached targetVal " << targetVal << endl;
#endif
                retVal = targetVal;
                removeAnimation(animation.animationName);
            }
#if VIZ_DEBUG_LEVEL >= 0 && VIZ_DEBUG_LEVEL < 1
            isDecreasing ? cout << "[ANIMATION: " << animation.animationName << "] decreasing " << currVal << " + " << newVal << endl : cout << "[ANIMATION: " << animation.animationName << "] INC " << currVal << " + " << newVal << endl;
#endif
            return retVal;
        }
    };

}