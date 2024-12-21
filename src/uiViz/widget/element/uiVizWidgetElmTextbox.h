#pragma once

#include "../uiVizWidget.h"

namespace Aquamarine
{
    class uiVizWidgetElmTextbox : public uiVizWidgetElm
    {

    public:
        enum TextType
        {
            SINGLE_LINE,
            MULTI_LINE,
            NUMERIC,
            PASSWORD,
            FORMATTED
        };

        string getTextTypeString(TextType val)
        {
            switch (val)
            {
            case TextType::MULTI_LINE:
                return "MULTI_LINE";
            case TextType::NUMERIC:
                return "NUMERIC";
            case TextType::PASSWORD:
                return "PASSWORD";
            case TextType::FORMATTED:
                return "FORMATTED";
            default:
                return "SINGLE_LINE";
            }
        };

        TextType getTextTypeEnum(string val)
        {
            val = ofToUpper(val);
            if (val == "MULTI_LINE")
                return TextType::MULTI_LINE;
            if (val == "NUMERIC")
                return TextType::NUMERIC;
            if (val == "PASSWORD")
                return TextType::PASSWORD;
            if (val == "FORMATTED")
                return TextType::FORMATTED;
            return TextType::SINGLE_LINE;
        };

        TextType getTextType()
        {
            return mType;
        }

        void setTextType(TextType val)
        {
            mType = val;
            configureBehaviourForType();
        }

        uiVizWidgetElmTextbox(string persistentId, string widgetXML, uiVizWidgetBase *parent) : uiVizWidgetElm(persistentId, widgetXML, parent)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            uiVizWidgetElm::loadState(widgetXML);
            ofxXmlSettings mWidgetXML = ofxXmlSettings();

            if (!mWidgetXML.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            // const string& tag, const string& attribute, int defaultValue, int which
            __LoadedValueFloat = mWidgetXML.getAttribute("element", "value", 1234567890.777f, 0);
            __LoadedValueInt = mWidgetXML.getAttribute("element", "value", 1234567777, 0);
            __LoadedValueString = mWidgetXML.getAttribute("element", "value", "", 0);

            if (getAllSiblingElementsLoaded())
            {
                // Firing events should ONLY ever happen once all sibling elements are loaded.
                // This is to ensure relationship consistency and prevent eventReceived events
                // firing on widgets before all elements are loaded...
                onAllSiblingElementsLoaded();
            }

            setTextType(getTextTypeEnum(mWidgetXML.getAttribute("element", "type", getTextTypeString(getTextType()), 0)));
            mWidgetXML.pushTag("element");
            // gget stuff
            mWidgetXML.popTag();

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings mWidgetXML = uiVizWidgetElm::saveState();
            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::TEXTBOX, 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual void onAllSiblingElementsLoaded() override
        {

            if (getTextType() == TextType::NUMERIC)
            {
                if (__LoadedValueFloat != 1234567890.777f)
                {
                    setValue(__LoadedValueFloat, true);
                }
                else if (__LoadedValueInt != 1234567777)
                {
                    setValue(__LoadedValueInt, true);
                }
            }
            else
            {
                setValue(__LoadedValueString, true);
            }
        }

        virtual ~uiVizWidgetElmTextbox()
        {
        }

        int getMaxLength()
        {
            return mMaxLength;
        }

        void setMaxLength(int val)
        {
            mMaxLength = val;
        }

        string getValue()
        {
            return mValue;
        }

        void setValue(string val, bool fireEvent)
        {

            bool valChanged = (mValue != val);

            if (getTextType() == TextType::NUMERIC)
            {
                if ((val != "" && val != "0" && val.substr(0, 1) != "0"))
                {
                    if (std::atof(val.c_str()) == 0)
                        return;
                }
            }

            if (val.size() > getMaxLength())
            {
                mValue = val.substr(0, getMaxLength());
            }
            else
            {
                mValue = val;
            }

            if (fireEvent && valChanged)
            {
                string eventVal = uiVizShared::encodeForXML(ofToString(mValue));
                if (parent())
                    parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::VALUE_CHANGED, "<event><value>" + eventVal + "</value></event>", *this);
            }

            setNeedsUpdate(true);
            setWidgetNeedsUpdate(true);
        }

        int getValue(int defaultVal)
        {
            if (mValue != "" && mValue != "0")
            {
                int intVal = std::atoi(mValue.c_str());
                return (intVal != 0) ? intVal : defaultVal;
            }
            return defaultVal;
        }

        void setValue(int val, bool fireEvent)
        {
            setTextType(TextType::NUMERIC);
            setValue(ofToString(val), fireEvent);
        }

        float getValue(float defaultVal)
        {
            if (mValue != "" && mValue != "0")
            {
                float floatVal = std::atof(mValue.c_str());
                return (floatVal != 0) ? floatVal : defaultVal;
            }
            return defaultVal;
        }

        void setValue(float val, bool fireEvent)
        {
            setTextType(TextType::NUMERIC);
            setValue(ofToString(val), fireEvent);
            // todo : bug when enterin text at end of line (when full and needs to scroll) sometimes exception
        }

        void onWidgetMouseScrolled(ofMouseEventArgs &e) override
        {
            setCursorIndex(mCursorIndex);
        }

        virtual void update(uiVizWidgetContext context) override
        {

            getIsRoundedRectangle() ? vizElm_TEXTBOX.setRectRounded(scale(getX()), scale(getY()), scale(getWidth()), scale(getHeight()), scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_TEXTBOX.setRectangle(scale(getX()), scale(getY()), scale(getWidth()), scale(getHeight()), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));

            calculateTextBounds();
            setCursorIndex(mCursorIndex);
        }

        virtual void draw(uiVizWidgetContext context) override
        {

            vizElm_TEXTBOX.setColor(getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));
            vizElm_TEXTBOX.draw();

            ofPushStyle();
            ofSetColor(getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha));

            /*
            ofRectangle r = getFonts(getTheme())->drawFormattedColumn(
                getTextOrTitle(),
                scale(getUsableX()),
                scale(getUsableY()), // why isn't padding being applied ?
                scale(getUsableWidth() - getNonScaledPadding()*2.0f)
                );
            */

            defaultStyle.color = (getValue() != "") ? getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha) : getTheme().TypographyPrimaryColor_withAlpha(0.5f);

            /*
            fbo.begin();
            ofClear(0, 0, 0, 0);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

            ofPushMatrix();
            ofTranslate(-1*scale(getWidgetStateX()), -1*scale(getWidgetStateY()));
            ofPushStyle();
     */

            ofRectangle r = getViz()->getFonts(getTheme())->drawColumn(getTextOrTitle(), defaultStyle, scale(getUsableX()), scale(getUsableY()), scale(getUsableWidth() - getNonScaledPadding() * 2.0f), ofAlignHorz::OF_ALIGN_HORZ_LEFT, false);
            ofPopStyle();

            /*
            ofPopStyle();
            ofPopMatrix();
            fbo.end();
            */

            if (getIsFocused(true))
            {
                int timer = getTimerMillis();
                if (timer >= 1000)
                    resetTimer();
                if (timer <= 500)
                {
                    vizElm_CURSOR.setColor(getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha));
                    vizElm_CURSOR.draw();
                }
            }
        }

        virtual void onWidgetMousePressed(ofMouseEventArgs &e) override
        {
            if (e.button == OF_MOUSE_BUTTON_LEFT)
            {
                parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::FOCUS_ELEMENT, "<event></event>", *this);
                setCursorIndex(mCursorIndex);
                parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::CLICK, "<event></event>", *this);
            }
            else if (e.button == OF_MOUSE_BUTTON_RIGHT)
            {
                showClipboardForThisWidget(true);
            }
        }

        virtual void onWidgetKeyPressed(ofKeyEventArgs &e) override
        {

            if (!getIsFocused(true))
                return;

            int key = e.key;

            if (!keyIsValid(key, e))
                return;

            if (key != OF_KEY_BACKSPACE && key != OF_KEY_LEFT && key != OF_KEY_RIGHT &&
                key != OF_KEY_UP && key != OF_KEY_DOWN && !moreCharactersAllowed())
                return;
            if (mHighlightText)
            {
                // if key is printable or delete
                if ((key >= 32 && key <= 255) || key == OF_KEY_BACKSPACE || key == OF_KEY_DEL)
                {
                    setValue("", true);
                    setCursorIndex(0);
                }
            }
            if (key == OF_KEY_BACKSPACE)
            {
                // delete character at cursor position //
                if (mCursorIndex > 0)
                {
                    setValue(getValue().substr(0, mCursorIndex - 1) + getValue().substr(mCursorIndex), true);
                    setCursorIndex(mCursorIndex - 1);
                }
            }
            else if (key == OF_KEY_LEFT)
            {
                setCursorIndex(max((int)mCursorIndex - 1, 0));
            }
            else if (key == OF_KEY_RIGHT)
            {
                setCursorIndex(std::min((int)mCursorIndex + 1, (int)getValue().size()));
            }
            else if (key == OF_KEY_UP && (getTextType() == TextType::MULTI_LINE || getTextType() == TextType::FORMATTED))
            {
                setCursorIndex(getIndexForLine(mCursorIndex, "UP"));
            }
            else if (key == OF_KEY_DOWN && (getTextType() == TextType::MULTI_LINE || getTextType() == TextType::FORMATTED))
            {
                setCursorIndex(getIndexForLine(mCursorIndex, "DOWN"));
            }
            else
            {
                // insert character at cursor position //
                if (key == OF_KEY_UP || key == OF_KEY_DOWN)
                    return;
                string text = getValue();
                setValue(text.substr(0, mCursorIndex) + (char)key + text.substr(mCursorIndex), true);
                setCursorIndex(mCursorIndex + 1);
            }
            mHighlightText = false;

            setNeedsUpdate(true);
        }

        virtual void onWidgetEventReceived(uiVizWidgetEventArgs &args) override
        {

            string event = args.getFullEventName();

            if (event == "CLIPBOARD.paste")
            {
                string clipboard = args.eventXML.getValue("event", "");
                pasteAtIndex(mCursorIndex, clipboard, true);
            }
        }

        virtual string getSelectionDataForClipboard() override
        {

            return getValue(); // this isn't right -- need to copy only selection... TODO...
        }

        void pasteAtIndex(int index, string paste, bool updateCursor)
        {

            if (getTextType() == TextType::NUMERIC)
            {
                float numericVal = std::atof(paste.c_str());
                paste = ofToString(numericVal);
            }

            setValue(getValue().substr(0, index) + paste + getValue().substr(index), true);
            if (updateCursor)
                setCursorIndex(index + paste.size());
            calculateTextBounds();
            ensureCursorIsInView();
            setWidgetNeedsUpdate(true);
            setNeedsUpdate(true);
        }

        vector<string> getLines()
        {

            float columnWidth = scale(getUsableWidth() - getNonScaledPadding() * 2.0f);
            vector<ofxFontStash2::StyledText> parsed;
            getViz()->getFonts(getTheme())->parseStyledText(getTextOrTitle(), parsed);
            vector<ofxFontStash2::StyledLine> laidOutLines;

            // plain
            getViz()->getFonts(getTheme())->layoutLines({{getTextOrTitle(), defaultStyle}}, columnWidth, laidOutLines);

            // formatted
            // getFonts(getTheme())->layoutLines(parsed, columnWidth, laidOutLines);

            vector<string> textLines = {};
            for (int line = 0; line < laidOutLines.size(); line++)
            {
                string textLine = "";
                for (int lineElm = 0; lineElm < laidOutLines[line].elements.size(); lineElm++)
                {
                    textLine += laidOutLines[line].elements[lineElm].content.styledText.text;
                }
                textLines.push_back(textLine);
            }
            return textLines;
        }

        string getLines(int fromLine, int toLine)
        {
            vector<string> textLines = getLines();
            if (fromLine < 0)
                fromLine = 0;
            fromLine = std::min((int)textLines.size() - 1, fromLine);
            toLine = std::min((int)textLines.size() - 1, toLine);
            string text = "";
            for (int line = fromLine; line <= toLine; line++)
            {
                text += textLines[line];
            }
            return text;
        }

        int getIndexForLine(int currIndex, string mode)
        {
            vector<string> textLines = getLines();
            int retVal = 0;
            if (mode == "DOWN" && mCursorLineIndex < textLines.size() + 1)
            {
                if (textLines.size() == 1)
                    return textLines[0].size() - 1; // jump to end of line (only 1 line of text!)
                string textToPoint = getLines(0, std::max(0, mCursorLineIndex - 1));
                retVal = textToPoint.size() + 1 + mCursorIndexForCurrentLine;
            }
            else if (mode == "UP" && mCursorLineIndex > 0)
            {
                string textToPoint = getLines(0, std::max(0, mCursorLineIndex - 3));
                retVal = textToPoint.size() - 1 + mCursorIndexForCurrentLine;
            }

            retVal = std::min(retVal, (int)getTextOrTitle().size() - 1);
            retVal = std::max(retVal, 0);

            return retVal;
        }

        void calculateTextBounds()
        {

            vector<ofxFontStash2::StyledLine> laidOutLines;
            float columnWidth = scale(getUsableWidth() - getNonScaledPadding() * 2.0f);

            // plain
            getViz()->getFonts(getTheme())->layoutLines({{getTextOrTitle(), defaultStyle}}, columnWidth, laidOutLines);
            // Draw offscreen during update...
            ofRectangle bbox = getViz()->getFonts(getTheme())->drawLines(laidOutLines, 0 - scale(getUsableX() + columnWidth * 2), scale(getUsableY()));

            setContentBoundsScaled(uiVizCoord::vizBounds(getContentBoundsScaled().x,
                                                         getContentBoundsScaled().y,
                                                         bbox.width,
                                                         bbox.height));

            calculateUsableY();
        }

        void calculateUsableY()
        {
            vector<ofxFontStash2::StyledLine> laidOutLines;
            float columnWidth = scale(getUsableWidth() - getNonScaledPadding() * 2.0f);

            // plain
            getViz()->getFonts(getTheme())->layoutLines({{getTextOrTitle(), defaultStyle}}, columnWidth, laidOutLines);
            ofRectangle bbox = getViz()->getFonts(getTheme())->drawLines(laidOutLines, 0 - scale(getUsableX() + columnWidth * 2), scale(getUsableY()));

            setContentBoundsScaled(uiVizCoord::vizBounds(getContentBoundsScaled().x,
                                                         getContentBoundsScaled().y,
                                                         bbox.width,
                                                         bbox.height));

            mUsableY = uiVizWidgetBase::getUsableY() + getNonScaledPadding() + (laidOutLines.size() > 0 ? deScale(laidOutLines[0].lineH) : 0);
        }

        virtual int getUsableY() override
        {
            return mUsableY;
        }

        void ensureCursorIsInView()
        {

            if (!getIsFocused(true))
                return;

            int maxCursorX = scale(getUsableX() + getUsableWidth() - mSingleCharWidth);
            if (mCursorX >= maxCursorX)
            {
                scrollX(getScrollOffsetX() - (mSingleCharWidth * 2 + getScaledPadding()));
                mCursorX_ExceededBounds = true;
            }
            else
            {
                mCursorX_ExceededBounds = false;
                mOldCursorX = mCursorX;
            }

            // fix below and then move onto mouse click to set cursor!!
            int minCursorX = scale(getUsableX());
            if (mCursorX < minCursorX)
            {
                scrollX(getScrollOffsetX() + scale(mCursorWidth));
                mCursorX_ExceededBounds = true;
            }
            else
            {
                mCursorX_ExceededBounds = false;
                mOldCursorX = mCursorX;
            }

            int maxCursorY = scale(getUsableY() + getUsableHeight() - mCursorHeight);
            if (mCursorY > maxCursorY)
            {
                setScrollOffsetY(maxCursorY, false);
                setWidgetNeedsUpdate(true);
                setNeedsUpdate(true);
                mCursorY_ExceededBounds = true;
            }
            else
            {
                mCursorY_ExceededBounds = false;
                mOldCursorY = mCursorY;
            }
        }

        void setCursorIndex(int index)
        {

            if (!getIsFocused(true))
                return;

            string textRange = getValue().substr(0, index);
            if (getValue().length() > 0 && getTextType() == TextType::PASSWORD)
            {
                textRange = getTextOrTitle().substr(0, index);
            }

            float columnWidth = scale(getUsableWidth() - getNonScaledPadding() * 2.0f);
            vector<ofxFontStash2::StyledLine> laidOutLines;

            // plain
            getViz()->getFonts(getTheme())->layoutLines({{textRange, defaultStyle}}, columnWidth, laidOutLines);
            ofRectangle bbox = getViz()->getFonts(getTheme())->drawLines(laidOutLines, 0 - scale(getUsableX() + columnWidth * 2), scale(getUsableY()));
            ofRectangle singleChar = getViz()->getFonts(getTheme())->drawColumn("#", defaultStyle, -100, -100, 1000, ofAlignHorz::OF_ALIGN_HORZ_LEFT, false);
            mSingleCharWidth = singleChar.width;
            // formatted
            /*
            vector<ofxFontStash2::StyledText> parsed;
            getFonts(getTheme())->parseStyledText(textRange, parsed);
            getFonts(getTheme())->layoutLines(parsed, columnWidth, laidOutLines);
            ofRectangle bbox = getFonts(getTheme())->drawLines(laidOutLines, scale(getUsableX()), scale(getUsableY()));
            ofRectangle singleChar = getFonts(getTheme())->drawFormattedColumn("#", -100, -100, 1000);
             */

            mCursorHeight = deScale(laidOutLines.size() > 0 ? laidOutLines[laidOutLines.size() - 1].lineH : singleChar.height);

            mCursorX = scale(getUsableX()) + (laidOutLines.size() > 0 ? laidOutLines[laidOutLines.size() - 1].lineW : 0);
            mCursorY = scale(getUsableY()) + bbox.height -
                       (laidOutLines.size() > 0 ? scale(mCursorHeight) * 2.0f : getTextOrTitle().size() > 0 ? scale(mCursorHeight)
                                                                                                            : 0) +
                       scale(mCursorHeight) * 0.22f;
            mCursorLineIndex = laidOutLines.size();
            ensureCursorIsInView();

            if (mCursorX_ExceededBounds)
                mCursorX = mOldCursorX;
            if (mCursorY_ExceededBounds)
                mCursorY = mOldCursorY;
            if (mCursorX_ExceededBounds || mCursorY_ExceededBounds)
                return;

            vizElm_CURSOR.setRectangle(mCursorX, mCursorY, scale(mCursorWidth), scale(mCursorHeight), ofColor::red);

            // Set the cursor index (for the contect of the entire text)
            mCursorIndex = std::min(index, (int)getValue().size());

            vector<string> textLines = getLines();
            string textBuilder = "";
            for (int line = 0; line < textLines.size(); line++)
            {
                string lineBuilder = "";
                for (int charIdx = 0; charIdx < textLines[line].size(); charIdx++)
                {
                    lineBuilder += textLines[line].at(charIdx);
                    textBuilder += textLines[line].at(charIdx);
                    if (textBuilder == textRange)
                    {
                        // Set the cursor index (for the contect of the current line)
                        mCursorIndexForCurrentLine = charIdx;
                        return;
                    }
                }
            }
        }

    protected:
        bool keyIsValid(int key, ofKeyEventArgs &e)
        {

            bool cmdOrCtrlModifier = (e.hasModifier(OF_KEY_CONTROL) || e.hasModifier(OF_KEY_COMMAND));

            bool isPasteCommand = (cmdOrCtrlModifier && key == 118 /*v*/);
            if (isPasteCommand)
            {
                pasteAtIndex(mCursorIndex, ofGetClipboardString(), true);
                return false;
            }

            bool isCopyCommand = (cmdOrCtrlModifier && key == 99 /*c*/);
            if (isCopyCommand)
            {
                ofSetClipboardString(getSelectionDataForClipboard());
                return false;
            }

            // TODO: support multiline and rick / formatted (see commented blocks) text copy, scrolling, up and down not working correctly..., windows too..., above commands not working

            if (key == OF_KEY_BACKSPACE || key == OF_KEY_LEFT || key == OF_KEY_RIGHT || key == OF_KEY_UP || key == OF_KEY_DOWN)
            {
                return true;
            }
            else if (getTextType() == TextType::NUMERIC)
            {
                // allow dot (.) //
                if (key == 46)
                {
                    return true;
                    // allow numbers 0-9 //
                }
                else if (key >= 48 && key <= 57)
                {
                    return true;
                }
                else
                {
                    // an invalid key was entered //
                    return false;
                }
            }
            else if (getTextType() == TextType::SINGLE_LINE || getTextType() == TextType::PASSWORD ||
                     getTextType() == TextType::MULTI_LINE || getTextType() == TextType::FORMATTED)
            {
                if (key >= 32 && key <= 255)
                {
                    return true;
                }
                else
                {
                    // an invalid key was entered //
                    return false;
                }
            }
            else
            {
                // invalid textfield type //
                return false;
            }
            return false;
        }

        bool moreCharactersAllowed()
        {
            return mValue.size() < getMaxLength();
        }

    private:
        string getTextOrTitle()
        {
            return (mValue != "") ? ((getTextType() == TextType::PASSWORD) ? std::string(mValue.size(), '*') : mValue) : getTitle();
        }

        void configureBehaviourForType()
        {

            setEnableScrollYPositive(false);
            setEnableScrollYNegative(false);

            switch (getTextType())
            {
            case TextType::SINGLE_LINE:
                break;
            case TextType::MULTI_LINE:
                setEnableScrollYPositive(true);
                setEnableScrollYNegative(true);
                break;
            case TextType::NUMERIC:
                break;
            case TextType::PASSWORD:
                break;
            case TextType::FORMATTED:
                setEnableScrollYPositive(true);
                setEnableScrollYNegative(true);
                break;
            }

            setNeedsUpdate(true);
        }

        float __LoadedValueFloat = 1234567890.777f;
        int __LoadedValueInt = 1234567777;
        string __LoadedValueString = "";

        string mValue = "";
        TextType mType = TextType::SINGLE_LINE;
        int mMaxLength = 100000000;
        int mCursorIndex = 0;
        int mCursorLineIndex = 0;
        int mCursorIndexForCurrentLine = 0;
        bool mHighlightText = false;
        int mFirstLineHeight = 0;
        int mUsableY = -1;

        int mCursorX = 0;
        int mCursorY = 0;
        int mOldCursorX = 0;
        int mOldCursorY = 0;
        bool mCursorX_ExceededBounds = false;
        bool mCursorY_ExceededBounds = false;
        int mCursorWidth = 2;
        int mCursorHeight = 5;
        int mSingleCharWidth = 0;

        uiVizElm vizElm_TEXTBOX;
        uiVizElm vizElm_CURSOR;
        ofxFontStash2::Style defaultStyle;

        void initWidget() override
        {

            /*
             todo :
                fix the bugs around paste (not reflected until hover ie. FIX MAINTAIN FOCUS)
                invalid index, cursor doesnt show,
                shift to select, set cursor index using mouse!)
             */

            setWidth(100);
            setHeight(30);
            defaultStyle = getViz()->getMediumFontStyleMono();
            setScrollAction(ScrollAction::SCROLL);
            setEnableScrollYPositive(true);
            setEnableScrollYNegative(true);
            setEnableScrollXPositive(true);
            setEnableScrollXNegative(true);
            configureBehaviourForType();
        }
    };

}