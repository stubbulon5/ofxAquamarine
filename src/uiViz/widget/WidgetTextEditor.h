
#pragma once
#include "Widget.h"

namespace Aquamarine
{
// #ifdef VIZ_DISABLE_GLEDITOR

    class WidgetTextEditor : public Widget
    {
    public:
        WidgetTextEditor(string persistentId, string widgetXML) : Widget(persistentId, widgetXML) {}
        virtual bool loadFile(string path) { return false; }
        virtual bool saveFile(string path, bool transmitEvent) { return false; }
        virtual void setText(string text) {}
        virtual string getText() { return ""; }
        bool getAllowSave() { return false; }
        void setAllowSave(bool val) {}
        bool getDoesRespondToFileDrop() { return false; }
        void setDoesRespondToFileDrop(bool val) {}
    };

// #else

// #include "ofxGLEditor.h"

//     class WidgetTextEditor : public Widget, public ofxGLEditorListener
//     {
//     public:
//         WidgetTextEditor(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
//         {
//             initWidget();
//             loadState(widgetXML);
//         }

//         virtual bool loadState(string widgetXML) override
//         {

//             ofxXmlSettings settings = ofxXmlSettings();
//             if (!settings.loadFromBuffer(widgetXML.c_str()))
//             {
//                 return false;
//             }

//             settings.pushTag("widget");
//             if (!loadFile(settings.getAttribute("properties", "file", "")))
//             {
//                 settings.pushTag("properties");
//                 setText(settings.getValue("text", ""));
//                 settings.popTag();
//             }

//             ofxEditor::loadFont("fonts/DejaVuSansMono.ttf", settings.getAttribute("properties", "fontSize", scale(20)));

//             setDoesRespondToFileDrop(settings.getAttribute("properties", "doesRespondToFileDrop", true));
//             setLineWrap(settings.getAttribute("properties", "lineWrap", false));
//             setLineNumbers(settings.getAttribute("properties", "lineNumbers", false));
//             settings.pushTag("properties");
//             settings.popTag(); // properties
//             settings.popTag(); // widget

//             return true;
//         }

//         virtual ofxXmlSettings saveState() override
//         {
//             ofxXmlSettings settings = Widget::saveState();
//             settings.setAttribute("widget", "class", WidgetManager::WIDGET_CLASS_TEXT_EDITOR, 0);

//             settings.pushTag("widget");
//             settings.setAttribute("properties", "doesRespondToFileDrop", getDoesRespondToFileDrop(), 0);
//             settings.setAttribute("properties", "lineWrap", getLineWrap(), 0);
//             settings.setAttribute("properties", "lineNumbers", getLineNumbers(), 0);
//             settings.pushTag("properties");

//             if (mLoadedFile != "")
//             {
//                 saveFile(mLoadedFile, true);
//             }
//             else
//             {
//                 settings.addTag("text");
//                 settings.setValue("text", getText(), 0);
//                 settings.popTag();
//             }

//             // TODO

//             settings.popTag(); // properties
//             settings.popTag(); // widget

//             return settings;
//         }

//         virtual ~WidgetTextEditor() {
//         };

//         virtual void onWidgetEventReceived(WidgetEventArgs &args) override
//         {

//             string event = args.getFullEventName();

//             if (args.eventName == WIDGET_EVENT::RECEIVE_DRAG_AND_DROP_DATA)
//             {
//                 if (getDoesRespondToFileDrop())
//                 {
//                     string path = args.eventXML.getValue("event", "");
//                     if (ofFile::doesFileExist(path, false))
//                     {
//                         editor.openFile(path, 1);
//                     }
//                 }
//             }
//         }

//         virtual void update(WidgetContext context) override
//         {
//             editor.resize(scale(getUsableWidth()), scale(getUsableHeight()) - ofxEditor::getCharHeight());
//         }

//         virtual void draw(WidgetContext context) override
//         {
//             ofTranslate(scale(getUsableX()), scale(getUsableY()));
//             editor.draw();

//             // draw current editor info using the same font as the editor
//             if (!editor.isHidden() && editor.getCurrentEditor() > 0)
//             {

//                 int bottom = ofGetHeight() - ofxEditor::getCharHeight();
//                 int right = ofGetWidth() - ofxEditor::getCharWidth() * 7; // should be enough room
//                 ofPushStyle();
//                 ofSetColor(ofColor::gray);

//                 // draw the current editor num
//                 editor.drawString("Editor: " + ofToString(editor.getCurrentEditor()),
//                                   0, bottom);

//                 // draw the current line & line pos
//                 editor.drawString(ofToString(editor.getCurrentLine() + 1) + "," +
//                                       ofToString(editor.getCurrentLinePos()),
//                                   right, bottom);
//                 ofPopStyle();
//             }
//         }

//         virtual void onWidgetKeyPressed(ofKeyEventArgs &e) override
//         {

//             // note: when using CTRL as the modifier key, CTRL + key might be an ascii
//             // control char so check for both the char 'f' and 6, the ascii value for
//             // CTRL + f, see also: http://ascii-table.com/control-chars.php
//             bool modifier = ofGetKeyPressed(ofxEditor::getSuperAsModifier() ? OF_KEY_SUPER : OF_KEY_CONTROL);
//             if (modifier)
//             {
//                 switch (e.key)
//                 {
//                 case 's':
//                 case 19:
//                     saveFile(mLoadedFile, true);
//                     return;
//                 case 'f':
//                 case 6:
//                     ofToggleFullscreen();
//                     return;
//                 case 'l':
//                 case 12:
//                     editor.setLineWrapping(!editor.getLineWrapping());
//                     return;
//                 case 'n':
//                 case 14:
//                     editor.setLineNumbers(!editor.getLineNumbers());
//                     return;
//                 case 'k':
//                 case 26:
//                     editor.setAutoFocus(!editor.getAutoFocus());
//                     return;
//                 }
//             }

//             // see ofxGLEditor.h for key commands
//             editor.keyPressed(e.key);
//             setNeedsUpdate(true);
//         }

//         virtual bool loadFile(string path)
//         {
//             if (path == "")
//                 return false;
//             bool loaded = editor.openFile(path, 1);
//             setNeedsUpdate(true);
//             if (loaded)
//                 mLoadedFile = path;
//             return loaded;
//         }

//         virtual bool saveFile(string path, bool transmitEvent)
//         {
//             if (path == "")
//                 return false;
//             bool saved = editor.saveFile(path);
//             setNeedsUpdate(true);
//             if (saved)
//             {
//                 mLoadedFile = path;
//                 if (transmitEvent)
//                     eventTransmit(getPersistentId(), WIDGET_EVENT::SAVED, "<event>" + Shared::encodeForXML(mLoadedFile) + "</event>", *this);
//             }
//             return saved;
//         }

//         virtual void setText(string text)
//         {
//             editor.setText(text, 1);
//         }

//         virtual string getText()
//         {
//             return editor.getText(1);
//         }

//         bool getAllowSave()
//         {
//             return mAllowSave;
//         }

//         void setAllowSave(bool val)
//         {
//             mAllowSave = false;
//         }

//         bool getDoesRespondToFileDrop()
//         {
//             return mDoesRespondToFileDrop;
//         }

//         void setDoesRespondToFileDrop(bool val)
//         {
//             mDoesRespondToFileDrop = val;
//         }

//         bool getLineWrap()
//         {
//             return mLineWrap;
//         }

//         void setLineWrap(bool val)
//         {
//             mLineWrap = val;
//             editor.setLineWrapping(val);
//             setNeedsUpdate(true);
//         }

//         bool getLineNumbers()
//         {
//             return mLineNumbers;
//         }

//         void setLineNumbers(bool val)
//         {
//             mLineNumbers = val;
//             editor.setLineNumbers(val);
//             setNeedsUpdate(true);
//         }

//     private:
//         ofxGLEditor editor;
//         bool mDoesRespondToFileDrop = true;
//         bool mAllowSave = false;
//         string mLoadedFile = "";
//         bool mLineWrap = false;
//         bool mLineNumbers = false;

//         void initWidget() override
//         {

//             int resolutionMultiplier = getViz()->getResolutionMultiplier();
//             int fontScale = int((float)resolutionMultiplier * getViz()->getUserScale());
//             int fontSize = 15 * fontScale;

//             setScrollAction(ScrollAction::SCROLL);
//             // setup the global editor font before you do anything!
//             ofxEditor::loadFont("fonts/DejaVuSansMono-Bold.ttf", fontSize);

//             ofxEditor::setAutoFocusMinScale(1);
//             ofxEditor::setAutoFocusMaxScale(1);

//             // set a custom Repl banner & prompt (do this before setup())
//             ofxRepl::setReplBanner("Hello, welcome to ofxGLEditor!");
//             ofxRepl::setReplPrompt("prompt> ");

//             // setup editor with event listener
//             editor.setup(this);

//             // change multi editor settings, see ofxEditorSettings.h for details
//             editor.getSettings().setTextColor(getTheme().TypographyPrimaryColor);                   // main text color
//             editor.getSettings().setCursorColor(getTheme().TypographyPrimaryColor_withAlpha(0.9f)); // current pos cursor
//             // editor.getSettings().setAlpha(0.5); // main text, cursor, & highlight alpha

//             // other settings
//             // editor.setLineWrapping(true);
//             // editor.setLineNumbers(true);
//             // editor.setAutoFocus(true);

//             // move the cursor to a specific line
//             // editor.setCurrentLine(4);
//             // ofLogNotice() << "current line: " << editor.getCurrentLine();
//         }
//     };

// #endif
}