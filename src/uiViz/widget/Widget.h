
#pragma once
#include "WidgetConstants.h"

#define exprtk_disable_enhanced_features
#define exprtk_disable_string_capabilities
#define exprtk_disable_superscalar_unroll
#define exprtk_disable_rtl_io_file
#define exprtk_disable_rtl_vecops
#define exprtk_disable_caseinsensitivity
#include "../core/exprtk/exprtk.hpp"
#include "../uiViz.h"
#include "../Elm.h"
#include "ofxSvg.h"
#include "ofxXmlSettings.h"

#include "ofxExternalDrop.h"

namespace Aquamarine
{
    struct WidgetCacheData
    {
        string imagePath = "";
        string widgetClassName = "";
        string widgetPrettyName = "";
        WidgetCacheData() {};
        WidgetCacheData(string imagePath, string widgetClassName, string widgetPrettyName) : imagePath(imagePath), widgetClassName(widgetClassName), widgetPrettyName(widgetPrettyName) {};
    };

    struct WidgetContext
    {

        enum DrawContext
        {
            SCREEN,
            IMAGE,
            THUMBNAIL
        };

        DrawContext drawContext = DrawContext::SCREEN;
        bool isInitialDraw = true;
        bool isInitialUpdate = true;

        WidgetContext() {};
    };

    struct WidgetSignature
    {
        string persistentId = "";
        string widgetClass = "";
        string widgetXML = "";
        WidgetSignature() {};
        WidgetSignature(string persistentId, string widgetClass, string widgetXML) : persistentId(persistentId), widgetClass(widgetClass), widgetXML(widgetXML) {};

        static string generateRandomId(string PREFIX)
        {
            string random = ofToString((int)(ofRandom(1.0f) * 100000));
            return PREFIX != "" ? PREFIX + "_" + random : random;
        }
    };

    //------------------------------ FORWARD DECLARATIONS  --------------------------------
    class WidgetBase;
    class Widget;

    //------------------------------ EVENT ARGS  --------------------------------
    class WidgetEventArgs : public ofEventArgs
    {
    public:
        WidgetEventArgs(string widgetId, WidgetBase &sender, WidgetBase *target) : widgetId(widgetId), sender(sender), target(target) {}

        WidgetEventArgs(string widgetId, WidgetBase &sender) : widgetId(widgetId), sender(sender) {}

        WidgetEventArgs(string eventSenderId, string eventName, ofxXmlSettings eventXML, WidgetBase &sender) : widgetId(eventSenderId), eventName(eventName), eventXML(eventXML), sender(sender) {}

        WidgetEventArgs(string eventSenderId, string eventName, ofxXmlSettings eventXML, WidgetBase &sender, WidgetBase *target) : widgetId(eventSenderId), eventName(eventName), eventXML(eventXML), sender(sender), target(target) {}

        string widgetId;
        string eventName = "";
        string getFullEventName()
        {
            return widgetId + "." + eventName;
        }

        string getXMLString(bool stripRootEventTag)
        {
            TiXmlElement *rootElm = eventXML.doc.RootElement();
            if (rootElm)
            {
                if (stripRootEventTag)
                {
                    return Shared::getXMLAtTag(&eventXML, "event", 0);
                }
                else
                {
                    TiXmlPrinter printer;
                    rootElm->Accept(&printer);
                    return printer.CStr();
                }
            }
            return "";
        }

        WidgetBase &sender;
        WidgetBase *target = nullptr;
        ofxXmlSettings eventXML;
    };

    //------------------------------ WidgetBase : derive all barebones widgets off of this class. Keep it LEAN --------------------------------

    class widgetTelemetry
    {
    public:
        void startUpdate()
        {
            mStartUpdate = ofGetSystemTimeMillis();
            mUpdateSampleCount >= 1000 ? mUpdateSampleCount = 0 : mUpdateSampleCount++;
        }

        void endUpdate()
        {
            mEndUpdate = ofGetSystemTimeMillis();
        }

        void startDraw()
        {
            mStartDraw = ofGetSystemTimeMillis();
            mDrawSampleCount >= 1000 ? mDrawSampleCount = 0 : mDrawSampleCount++;
        }

        void endDraw()
        {
            mEndDraw = ofGetSystemTimeMillis();
        }

        uint64_t getUpdateMS()
        {
            return mEndUpdate - mStartUpdate;
        }

        uint64_t getDrawMS()
        {
            return mEndDraw - mStartDraw;
        }

        uint64_t getDrawSampleCount()
        {
            return mDrawSampleCount;
        }

        uint64_t getUpdateSampleCount()
        {
            return mUpdateSampleCount;
        }

    private:
        uint64_t mStartUpdate = 0;
        uint64_t mEndUpdate = 0;
        uint64_t mStartDraw = 0;
        uint64_t mEndDraw = 0;
        uint64_t mDrawSampleCount = 0;
        uint64_t mUpdateSampleCount = 0;
    };

    //!  The base from which all Widget and Widget Elements are derived.
    /*!
      Core events and functionality for positioning, updating, drawing, saving and loading Widgets.
    */
    class WidgetBase : public ofThread
    {
    public:
        // Events
        ofEvent<WidgetEventArgs> widgetHovered;
        ofEvent<WidgetEventArgs> widgetUnhovered;
        ofEvent<WidgetEventArgs> widgetClicked;
        ofEvent<WidgetEventArgs> widgetLostFocus;
        ofEvent<WidgetEventArgs> widgetThemeChanged;
        ofEvent<WidgetEventArgs> widgetEventReceived;
        ofEvent<ofResizeEventArgs> widgetResized;

        typedef exprtk::expression<float> expression_t;
        expression_t expression;

        typedef exprtk::parser<float> calculator_t;
        calculator_t calculator;

        enum ScrollAction
        {
            ZOOM,
            SCROLL,
            BASIC,
            NONE
        };

        enum CloseAction
        {
            HIDE,
            REMOVE
        };

        enum class DockLocation
        {
            NONE,
            TOP,
            BOTTOM,
            LEFT,
            RIGHT
        };

        enum class DisplayType
        {
            REGULAR,
            POPOUT,
            MENU

        };

        enum class HorizontalAlign
        {
            LEFT,
            RIGHT,
            CENTER
        };

        enum class VerticalAlign
        {
            TOP,
            BOTTOM,
            CENTER
        };

        enum class PopoutDirection
        { /* C++ 11 */
          NONE,
          DOWN,
          UP,
          LEFT,
          RIGHT,
        };

        struct NonScaledPadding
        {
            int top = 0;
            int right = 0;
            int bottom = 0;
            int left = 0;
            NonScaledPadding(int top, int right, int bottom, int left) : top(top), right(right), bottom(bottom), left(left) {}
        };

        struct DisplayOptions
        {
            DisplayType displayType = DisplayType::REGULAR;
            HorizontalAlign horizontalAlign = HorizontalAlign::LEFT;
            VerticalAlign verticalAlign = VerticalAlign::CENTER;
            NonScaledPadding nonScaledPadding = NonScaledPadding(0, 0, 0, 0);
            // TODO: Add : MinWidth, MinHeight, Gradient colours?
            DisplayOptions() {}
            DisplayOptions(DisplayType displayType, HorizontalAlign horizontalAlign, VerticalAlign verticalAlign) : displayType(displayType), horizontalAlign(horizontalAlign), verticalAlign(verticalAlign) {}

            DisplayOptions(DisplayType displayType, HorizontalAlign horizontalAlign, VerticalAlign verticalAlign, NonScaledPadding nonScaledPadding) : displayType(displayType), horizontalAlign(horizontalAlign), verticalAlign(verticalAlign), nonScaledPadding(nonScaledPadding) {}

            DisplayOptions fromString(string semiColonDelimitedString)
            { // just like CSS eg a:1;b:2
                DisplayOptions r = DisplayOptions();
                semiColonDelimitedString = ofToUpper(semiColonDelimitedString);
                ofStringReplace(semiColonDelimitedString, " ", "");
                vector<string> pairs = ofSplitString(semiColonDelimitedString, ";");
                for (string pair : pairs)
                {
                    vector<string> keyVal = ofSplitString(pair, ":");
                    if (keyVal.size() < 2)
                        continue;
                    if (keyVal[0] == "DISPLAYTYPE")
                    {
                        if (keyVal[1] == "REGULAR")
                        {
                            r.displayType = DisplayType::REGULAR;
                        }
                        else if (keyVal[1] == "POPOUT")
                        {
                            r.displayType = DisplayType::POPOUT;
                        }
                        else if (keyVal[1] == "MENU")
                        {
                            r.displayType = DisplayType::MENU;
                        }
                    }
                }
                return r;
            }
        };

        WidgetBase(string persistentId, string widgetXML) : mPersistentId(persistentId)
        {
            mPersistentId = ofToUpper(mPersistentId);
            init();
            loadState(widgetXML);
        }

        WidgetBase(string persistentId, string widgetXML, string ownerWidgetId) : mPersistentId(persistentId), mOwnerWidgetId(ownerWidgetId)
        {
            mPersistentId = ofToUpper(mPersistentId);
            init();
            loadState(widgetXML);
        }

        virtual ~WidgetBase()
        {
            // Make sure every event listened to is removed in destructor!
            // NOTE ON VIRTUAL DESTRUCTOR : For this exampe: Base *myObj = new Derived(); delete myObj; If your derived class destructor is NOT virtual then
            // only base class object will get deleted (because pointer is of base class "Base *myObj"). So there will be memory leak for derived object.
            ofRemoveListener(getViz()->scaleChanged, this, &WidgetBase::onCoreWidgetScaleChanged);
            ofRemoveListener(ofEvents().windowResized, this, &WidgetBase::onCoreWidgetWindowResized);
            ofRemoveListener(ofEvents().mouseMoved, this, &WidgetBase::onCoreWidgetMouseMoved);
            ofRemoveListener(ofEvents().mousePressed, this, &WidgetBase::onCoreWidgetMousePressed);
            ofRemoveListener(ofEvents().mouseReleased, this, &WidgetBase::onCoreWidgetMouseReleased);
            ofRemoveListener(ofEvents().mouseScrolled, this, &WidgetBase::onCoreWidgetMouseScrolled);
            ofRemoveListener(ofEvents().fileDragEvent, this, &WidgetBase::onCoreWidgetFileDragEvent);
            ofRemoveListener(ofEvents().mouseDragged, this, &WidgetBase::onCoreWidgetMouseDragged);
            ofRemoveListener(ofEvents().mouseEntered, this, &WidgetBase::onCoreWindowMouseEntered);
            ofRemoveListener(ofEvents().mouseExited, this, &WidgetBase::onCoreWindowMouseExited);
            ofRemoveListener(ofEvents().keyPressed, this, &WidgetBase::onCoreWidgetKeyPressed);
            ofRemoveListener(ofEvents().keyReleased, this, &WidgetBase::onCoreWidgetKeyReleased);

            // Touch events
            ofRemoveListener(ofEvents().touchDown, this, &WidgetBase::onCoreWidgetTouchDown);
            ofRemoveListener(ofEvents().touchUp, this, &WidgetBase::onCoreWidgetTouchUp);
            ofRemoveListener(widgetEventReceived, this, &WidgetBase::onCoreWidgetEventReceived);
            ofRemoveListener(widgetResized, this, &WidgetBase::onCoreWidgetResized);

            getViz()->removeListeningEvents(getPersistentId());

#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
            cout << "[" << getPersistentId() << "]~WidgetBase()\n";
#endif
        }

        bool getIsPermanentWidget()
        {
            return mIsPermanentWidget;
        }

        void setIsPermanentWidget(bool val)
        {
            mIsPermanentWidget = val;
        }

        bool getIsHighlighted()
        {
            return mIsHighlighted;
        }

        virtual void setIsHighlighted(bool val)
        {
            mIsHighlighted = val;
        }

        //! Hook before destructor is called. Use it to clear event listeners, etc
        virtual void beforeDelete()
        {
        }

        void deleteThisWidget()
        {

            /*
                If getting unexpected EXC_BAD_ACCESS errors make sure to check that ALL widgets are callling
                ofRemoveListener for ALL corresponding ofAddListener in ther destructors!
            */

            mShouldDeleteThisWidget = true;
            if (getShowingAsPopout())
            {
                getViz()->setIsPopoutVisible(getWidgetId(), false);
            }
            clearActiveWidgetId();
        }

        bool shouldDeleteThisWidget()
        {
            return mShouldDeleteThisWidget;
        }

        void closeThisWidget(string result)
        {
            switch (getCloseAction())
            {
            case CloseAction::HIDE:
            {
                mShouldCloseThisWidget = true;
                break;
            }
            case CloseAction::REMOVE:
            {
                deleteThisWidget();
                break;
            }
            }
            getWidgetClosedCallback()(getPopoutOpenerWidget(), this, result); // call it...

            // Is below line necessary ???
            // if(getPopoutWidget()) {
            //     getPopoutWidget()->getWidgetClosedCallback()(getPopoutOpenerWidget(), this, result);
            // }
        }

        void closeThisWidget()
        {
            closeThisWidget("CLOSED");
        }

        std::function<void(WidgetBase *sender, WidgetBase *closedWidget, const string &)> getWidgetClosedCallback()
        {
            return mWidgetClosedCallback;
        }

        void setWidgetClosedCallback(std::function<void(WidgetBase *sender, WidgetBase *closedWidget, const string &)> widgetClosedCallback)
        {
            mWidgetClosedCallback = widgetClosedCallback;
        }

        bool shouldCloseThisWidget()
        {
            return mShouldCloseThisWidget;
        }

        void setIsEnabled(bool val)
        {
            if (val != mIsEnabled)
            {

                WidgetTheme theme = getViz()->getThemeManager()->getThemeByName(getTheme().Name);
                if (!val)
                    theme = getViz()->getThemeManager()->getLessOpacity(theme, true, 0.85f);
                setTheme(theme);
                setNeedsUpdate(true);
                setWidgetNeedsUpdate(true);
            }
            mIsEnabled = val;
        }

        bool getIsEnabled()
        {
            return mIsEnabled;
        }

        virtual void setWasJustInitialized(bool val)
        {
            mWasJustInitialized = val;
            if (val)
            {
                initWidgetScale(getViz()->getUserScale());
                onWidgetScaleChanged(getViz()->getUserScale());
                setExpressionRecalculationRequired(true);
                onWidgetWasJustInitialized();
            }
        }

        bool getWasJustInitialized()
        {
            return mWasJustInitialized;
        }

        void updateWidgetPopoutArrow()
        {

            if (mPopoutDirection == PopoutDirection::NONE)
                return;

            int bubbleWidth = 15;

            bool isPopingOutFromWidget = mPopoutFromWidget && mPopoutFromWidget != nullptr;

            int rawPopoutX = mRawPopoutX;
            int rawPopoutY = mRawPopoutY;

            if (isPopingOutFromWidget)
            {

                int w = mPopoutFromWidget->getWidgetStateWidth();
                int h = mPopoutFromWidget->getWidgetStateHeight();

                switch (mPopoutDirection)
                {
                case PopoutDirection::DOWN:
                {
                    rawPopoutY = mPopoutFromWidget->getY() + (h + getNonScaledPadding() * 2);
                    rawPopoutX = mPopoutFromWidget->getX() + (w / 2.0f);
                    break;
                }
                case PopoutDirection::UP:
                {
                    rawPopoutY = mPopoutFromWidget->getY() - getHeight() - getNonScaledPadding() * 2;
                    rawPopoutX = mPopoutFromWidget->getX() + (w / 2.0f);
                    break;
                }
                case PopoutDirection::LEFT:
                {
                    rawPopoutY = mPopoutFromWidget->getY() + (h / 2.0f);
                    rawPopoutX = mPopoutFromWidget->getX() - getWidth() - getNonScaledPadding() * 2;
                    break;
                }
                case PopoutDirection::RIGHT:
                {
                    rawPopoutY = mPopoutFromWidget->getY() + (h / 2.0f);
                    rawPopoutX = mPopoutFromWidget->getX() + (w + getNonScaledPadding() * 2);
                    break;
                }
                }
            }
            else
            {
                switch (mPopoutDirection)
                {
                case PopoutDirection::DOWN:
                {
                    break;
                }
                case PopoutDirection::UP:
                {
                    rawPopoutY = rawPopoutY - getHeight() + getNonScaledPadding() - deScale(bubbleWidth / 2.0f);
                    break;
                }
                case PopoutDirection::LEFT:
                {
                    rawPopoutX = rawPopoutX - getWidth() + getNonScaledPadding() - deScale(bubbleWidth / 2.0f);
                    break;
                }
                case PopoutDirection::RIGHT:
                {
                    break;
                }
                }
            }

            int x = rawPopoutX;
            int y = rawPopoutY;

            int bubbleOffset = mPopoutOffset;
            setScreenPadding(5);
            int widgetCenterX = x - (getWidth() / 2.0f);
            int widgetCenterY = y - (getHeight() / 2.0f);

            int bubbleOffsetWidth = bubbleWidth + getScreenPadding() + (getNonScaledPadding() / 2.0f) + scale(1);
            int maxX = (deScale(ofGetWindowWidth()) - bubbleOffset - getWidgetStateWidth());
            int maxY = (deScale(ofGetWindowHeight()) - bubbleOffset - getWidgetStateHeight());

            if (mPopoutDirection == PopoutDirection::RIGHT && x > maxX)
                mPopoutDirection = PopoutDirection::LEFT;
            if (mPopoutDirection == PopoutDirection::DOWN && y > maxY)
                mPopoutDirection = PopoutDirection::UP;

            switch (mPopoutDirection)
            {
            case PopoutDirection::DOWN:
            case PopoutDirection::UP:
                setWidgetPosition(widgetCenterX,
                                  (mPopoutDirection == PopoutDirection::DOWN) ? max(y, bubbleOffset) : min(y, maxY), false);

                mPopoutX = min(x, (int)(deScale(ofGetWindowWidth()) - bubbleOffsetWidth));
                mPopoutX = max(mPopoutX, bubbleOffsetWidth);
                mPopoutY = (mPopoutDirection == PopoutDirection::DOWN) ? max(getY(), bubbleOffset) : max(getY(), getY() + getHeight());
                break;
            case PopoutDirection::LEFT:
            case PopoutDirection::RIGHT:
                setWidgetPosition((mPopoutDirection == PopoutDirection::RIGHT) ? max(x, bubbleOffset) : min(x, maxX),
                                  widgetCenterY, false);

                mPopoutY = min(y, (int)(deScale(ofGetWindowHeight()) - bubbleOffsetWidth));
                mPopoutY = max(mPopoutY, bubbleOffsetWidth);
                mPopoutX = (mPopoutDirection == PopoutDirection::RIGHT) ? max(getX(), bubbleOffset) : max(getX(), getX() + getWidth());
                break;
            }

            mPopoutX = scale(mPopoutX);
            mPopoutY = scale(mPopoutY);
            bubbleWidth = scale(bubbleWidth);
            bubbleOffset = scale(bubbleOffset);

            switch (mPopoutDirection)
            {
            case PopoutDirection::DOWN:
            case PopoutDirection::UP:
                vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE.setTriangle(
                    mPopoutX - bubbleWidth, mPopoutY,
                    mPopoutX, (mPopoutDirection == PopoutDirection::DOWN) ? mPopoutY - bubbleOffset : mPopoutY + bubbleOffset, // center top point
                    mPopoutX + bubbleWidth, mPopoutY);
                break;
            case PopoutDirection::LEFT:
            case PopoutDirection::RIGHT:
                vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE.setTriangle(
                    mPopoutX, mPopoutY - bubbleWidth,
                    (mPopoutDirection == PopoutDirection::RIGHT) ? mPopoutX - bubbleOffset : mPopoutX + bubbleOffset, mPopoutY, // center left point
                    mPopoutX, mPopoutY + bubbleWidth);
                break;
            }
        }

        virtual void popoutFrom(int x, int y, PopoutDirection popoutDirection)
        {
            setIsDraggable(false);
            mRawPopoutX = x;
            mRawPopoutY = y;
            mPopoutDirection = popoutDirection;
            setSize(getWidth(), getHeight());
            updateWidgetPopoutArrow();
            setShowingAsPopout(true);
            setIsVisible(true);
            setActiveWidget();
            // setMinimumWidth(getX()-);
            onWidgetShownAsPopout(nullptr);
            // if (parent()) {
            //     parent()->setExpressionRecalculationRequired(true);
            //     parent()->setWidgetNeedsUpdate(true);
            // }
            // setExpressionRecalculationRequired(true);
            // setWidgetNeedsUpdate(true);
        }

        virtual void popoutFrom(WidgetBase *obj, PopoutDirection popoutDirection, WidgetTheme theme)
        {
            setIsDraggable(false);

            if (theme.Name != "")
            {
                setTheme(theme);
            }

            mPopoutDirection = popoutDirection;
            mPopoutFromWidget = obj;
            if (mPopoutFromWidget)
                setPopoutOpenerWidget(mPopoutFromWidget);

            setSize(getWidth(), getHeight());
            updateWidgetPopoutArrow();
            setShowingAsPopout(true);
            setIsRoundedRectangle(true);
            setIsVisible(true);
            setActiveWidget();
            onWidgetShownAsPopout(obj);
        }

        virtual void popoutFrom(WidgetBase *obj, PopoutDirection popoutDirection, bool ignoreTheme = false)
        {
            WidgetTheme theme = WidgetTheme();
            if (obj && obj->parent())
                theme = getViz()->getThemeManager()->getThemeForPopout();
            popoutFrom(obj, popoutDirection, ignoreTheme ? WidgetTheme() : theme);
        }

        void popoutWithoutArrow(int x, int y)
        {
            setIsDraggable(false);
            setShowingAsPopout(true);
            setWidgetPosition(x, y, false);
            setIsVisible(true);
            setActiveWidget();
            onWidgetShownAsPopout(nullptr);
        }

        WidgetBase *getTargetDropWidget()
        {
            return mTargetDropWidget;
        }

        void setTargetDropWidget(WidgetBase *val)
        {
            mTargetDropWidget = val;
        }

        virtual void showClipboardForThisWidget(bool val)
        {
            mShouldShowClipboardForThisWidget = val;
        }

        bool shouldShowClipboardForThisWidget()
        {
            return mShouldShowClipboardForThisWidget;
        }

        virtual string getSelectionDataForClipboard()
        {
            // Implement
            return "";
        }

        bool getShouldPersist()
        {
            return mShouldPersist;
        }

        void setShouldPersist(bool val)
        {
            mShouldPersist = val;
        }

        bool getShouldPersistTheme()
        {
            return mShouldPersistTheme;
        }

        void setShouldPersistTheme(bool val)
        {
            mShouldPersistTheme = val;
        }

        virtual WidgetTheme &getTheme()
        {
            return mTheme;
        }

        virtual void setTheme(WidgetTheme val)
        {
            bool valChanged = (mTheme.Name != val.Name);
            mTheme = val;
            if (valChanged)
            {
                WidgetEventArgs args = WidgetEventArgs(getWidgetId(), *this);
                setWidgetNeedsUpdate(true);
                setNeedsUpdate(true);
                ofNotifyEvent(widgetThemeChanged, args);
                onWidgetThemeChanged(args);
            }
        }

        float getStrongChromeAlpha()
        {
            return 0.95f;
        }

        /*!
            Load state from XML. Use this to popuate member level variables. Do not call virtual methods here.
        */
        virtual bool loadState(string widgetXML, string parentTag)
        {
            ofxXmlSettings mWidgetXML = ofxXmlSettings();

#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
            cout << "[" << getPersistentId() << "]WidgetBase->loadState()\n";
#endif

            if (!mWidgetXML.loadFromBuffer(widgetXML.c_str()))
            {
#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
                cout << "[" << getPersistentId() << "]->Unable to load XML\n"
                     << widgetXML.c_str() << "\n";
#endif
                return false;
            }

            // if has theme specified, use it, otherwise use getDefaultTheme
            // getDefaultTheme is set by uiViz (loaded at app start)

            /**** Widget Class Type ****/
            mWidgetClassType = mWidgetXML.getAttribute(parentTag, "class", "Widget");
            /**** Theme ****/
            string userThemeName = mWidgetXML.getAttribute(parentTag, "theme", "");

            // If it was defined in XML, then persist it...
            if (userThemeName != "")
            {
                setShouldPersistTheme(true);
            }

            setShouldPersist(mWidgetXML.getAttribute(parentTag, "shouldPersist", false));
            setIsSystemWidget(mWidgetXML.getAttribute(parentTag, "isSystemWidget", false));
            setIsVisualDebugMode(mWidgetXML.getAttribute(parentTag, "visualDebugMode", false));
            string defaultThemeName = getViz()->getThemeManager()->getDefaultTheme().Name;
            setTheme(getViz()->getThemeManager()->getThemeByName((userThemeName != "" ? userThemeName : defaultThemeName), getIsSystemWidget()));
            WidgetTheme theme = getTheme();
            setIgnoreThemeChanges(userThemeName != "" ? true : false);

            mWidgetXML.pushTag(parentTag);
            /**** Title ****/
            setTitle(mWidgetXML.getValue("title", getTitle()));
            setIsEnabled(mWidgetXML.getValue("isEnabled", getIsEnabled()));

            /**** Bounds ****/
            setMaxWidth(mWidgetXML.getAttribute("bounds", "maxWidth", getMaxWidth()));
            setMaxHeight(mWidgetXML.getAttribute("bounds", "maxHeight", getMaxHeight()));
            setMinWidth(mWidgetXML.getAttribute("bounds", "minWidth", getMinWidth()));
            setMinHeight(mWidgetXML.getAttribute("bounds", "minHeight", getMinHeight()));

            /* Expressions or regular value for coords / bounds ? */
            (mWidgetXML.getAttribute("bounds", "widthExpr", "") != "") ? setWidth_Expr(mWidgetXML.getAttribute("bounds", "widthExpr", ofToString(getWidth()))) : setWidth(mWidgetXML.getAttribute("bounds", "width", getWidth()));

            (mWidgetXML.getAttribute("bounds", "heightExpr", "") != "") ? setHeight_Expr(mWidgetXML.getAttribute("bounds", "heightExpr", ofToString(getHeight()))) : setHeight(mWidgetXML.getAttribute("bounds", "height", getHeight()));

            (mWidgetXML.getAttribute("bounds", "xExpr", "") != "") ? setX_Expr(mWidgetXML.getAttribute("bounds", "xExpr", ofToString(getX()))) : setX(mWidgetXML.getAttribute("bounds", "x", getX()));

            (mWidgetXML.getAttribute("bounds", "yExpr", "") != "") ? setY_Expr(mWidgetXML.getAttribute("bounds", "yExpr", ofToString(getY()))) : setY(mWidgetXML.getAttribute("bounds", "y", getY()));

            setIsAutoResizeToContentsWidth(mWidgetXML.getAttribute("bounds", "autoResizeToContentsWidth", getIsAutoResizeToContentsWidth()));
            setIsAutoResizeToContentsHeight(mWidgetXML.getAttribute("bounds", "autoResizeToContentsHeight", getIsAutoResizeToContentsHeight()));

            /**** Behaviour ****/
            setIsDraggable(mWidgetXML.getAttribute("behaviour", "draggable", getIsDraggable()));
            setIsResizable(mWidgetXML.getAttribute("behaviour", "resizable", getIsResizable()));

            /**** Appearance ****/
            setIsShadow(mWidgetXML.getAttribute("appearance", "shadow", getIsShadow()));

            setIsRoundedRectangle(mWidgetXML.getAttribute("appearance", "roundedRectangle",
                                                          ((ofToLower(parentTag) == "element") ? theme.ElementRoundedRectangle : theme.RoundedRectangle), 0));
            setIsVisible(mWidgetXML.getAttribute("appearance", "visible", getIsVisible()));
            setLockAspectRatio(mWidgetXML.getAttribute("appearance", "lockAspectRatio", getLockAspectRatio()));
            setAspectRatio(mWidgetXML.getAttribute("appearance", "aspectRatio", getAspectRatio()));

            if (mWidgetXML.pushTag("listenEvents"))
            {
                for (int i = 0; i < mWidgetXML.getNumTags("event"); i++)
                {
                    getViz()->addToWidgetEventListenMap(
                        getPersistentId(),
                        mWidgetXML.getAttribute("event", "source", "", i),
                        mWidgetXML.getAttribute("event", "eventName", "", i),
                        mWidgetXML.getAttribute("event", "redirectEventName", "", i) // for renaming event to another... eg drag_and_drop_data_set -> receive_drag_and_drop_data
                    );
                }
                mWidgetXML.popTag(); // listenEvents
            }

            mWidgetXML.popTag(); // parentTag
            return true;
        }

        static string getWidgetXMLFromFile(string fileName, bool exceptionIfNotFound)
        {
            ofxXmlSettings widgetFile = ofxXmlSettings();
            if (widgetFile.load(ofToDataPath(fileName)))
            {
                TiXmlElement *rootElm = widgetFile.doc.RootElement();
                TiXmlPrinter printer;
                rootElm->Accept(&printer);
                string widgetXML = printer.CStr();
                return widgetXML;
            }
            if (exceptionIfNotFound)
            {
                string exceptionStr = "[" + fileName + "] file not found or is invalid! Exiting...";
                ofSystemAlertDialog(exceptionStr);
                throw std::runtime_error(exceptionStr.c_str());
                OF_EXIT_APP(1);
            }
            return "";
        }

        static WidgetSignature getWidgetSignatureFromXML(string widgetXML)
        {
            WidgetSignature widget;
            ofxXmlSettings widgetSettings = ofxXmlSettings();
            if (widgetXML != "" && widgetSettings.loadFromBuffer(widgetXML))
            {
                string persistentId = widgetSettings.getAttribute("widget", "id", "");
                string widgetClass = widgetSettings.getAttribute("widget", "class", "Widget");
                widget = WidgetSignature(persistentId, widgetClass, widgetXML);
            }
            return widget;
        }

        static WidgetSignature getWidgetSignatureFromFile(string fileName)
        {
            WidgetSignature widgets;
            string widgetXML = getWidgetXMLFromFile(fileName, false);
            return getWidgetSignatureFromXML(widgetXML);
        }

        bool loadStateFromFile(string fileName)
        {
            string xml = getWidgetXMLFromFile(fileName, false);
            if (xml != "")
            {
                return loadState(xml);
            }
            else
            {
                return false;
            }
        }

        virtual void onWidgetLoaded()
        {
            // Implement any behaviour for when a widget has loaded
        }

        //! Implement any behaviour in derrived class which should handle when this widget changes visibility.
        virtual void onWidgetVisibilityChanged(bool visible)
        {
        }

        //! Implement any behaviour in derrived class which should handle when this widget is initialized.
        virtual void onWidgetWasJustInitialized()
        {
        }

        //! Implement any behaviour in derrived class which should handle when this widget is shown as a popout.
        virtual void onWidgetShownAsPopout(WidgetBase *poppedOutFrom)
        {
        }

        //! Implement any behaviour in derrived class which should handle when this widget changes visibility.
        virtual void onCoreWidgetVisibilityChanged(bool visible)
        {
        }

        virtual bool loadState(string widgetXML)
        {
            bool success = loadState(widgetXML, "widget");
            onWidgetLoaded();
            return success;
        }

        virtual ofxXmlSettings saveState(string parentTag)
        {

            ofxXmlSettings mWidgetXML = ofxXmlSettings();

            string additionalAttrib = "";
            if (getShouldPersistTheme())
            {
                additionalAttrib += (getTheme().isDefault() ? "" : " theme='" + getTheme().Name + "'"); // don't persist theme name if default
            }

            if (getShouldPersist())
            {
                additionalAttrib += " shouldPersist='1'";
            }

            if (getIsVisualDebugMode())
            {
                additionalAttrib += " visualDebugMode='1'";
            }

            string xmlBuff =
                "<" + parentTag + " id='" + getPersistentId() + "' class='Widget'" + additionalAttrib + ">"
                                                                                                             "<title>" +
                getTitle() + "</title>"
                             "<bounds "
                             "x='" +
                ofToString(getX()) + "'  y='" + ofToString(getY()) + "' "
                                                                     "width='" +
                ofToString(getWidth()) + "'  height='" + ofToString(getHeight()) + "' "
                                                                                   "maxWidth='" +
                ofToString(getMaxWidth()) + "'  maxHeight='" + ofToString(getMaxHeight()) + "' "
                                                                                            "minWidth='" +
                ofToString(getMinWidth()) + "'  minHeight='" + ofToString(getMinHeight()) + "' "
                                                                                            "></bounds>"
                                                                                            "<behaviour "
                                                                                            "draggable='" +
                ofToString(getIsDraggable()) + "'  resizable='" + ofToString(getIsResizable()) + "' "
                                                                                                 "></behaviour>"
                                                                                                 "<appearance "
                                                                                                 "visible='" +
                ofToString(getIsVisible()) + "' "
                                             "shadow='" +
                ofToString(getIsShadow()) + "' "
                                            "roundedRectangle='" +
                ofToString(getIsRoundedRectangle()) + "' "
                                                      "lockAspectRatio='" +
                ofToString(getLockAspectRatio()) + "'  aspectRatio='" + ofToString(getAspectRatio()) + "' "
                                                                                                       "></appearance>"
                                                                                                       "<properties/>"
                                                                                                       "</" +
                parentTag + ">";

            // Expression bounds... eg ${MY_GUITAR.RIGHT}-20
            if (mWidgetXML.loadFromBuffer(xmlBuff.c_str()))
            {
                mWidgetXML.pushTag(parentTag);
                if (getX_Expr() != "")
                {
                    mWidgetXML.removeAttribute("bounds", "x");
                    mWidgetXML.setAttribute("bounds", "xExpr", getX_Expr(), 0);
                }
                if (getY_Expr() != "")
                {
                    mWidgetXML.removeAttribute("bounds", "y");
                    mWidgetXML.setAttribute("bounds", "yExpr", getY_Expr(), 0);
                }
                if (getWidth_Expr() != "")
                {
                    mWidgetXML.removeAttribute("bounds", "width");
                    mWidgetXML.setAttribute("bounds", "widthExpr", getWidth_Expr(), 0);
                }
                if (getHeight_Expr() != "")
                {
                    mWidgetXML.removeAttribute("bounds", "height");
                    mWidgetXML.setAttribute("bounds", "heightExpr", getHeight_Expr(), 0);
                }
                if (getIsAutoResizeToContentsWidth())
                {
                    mWidgetXML.setAttribute("bounds", "autoResizeToContentsWidth", true, 0);
                }
                if (getIsAutoResizeToContentsHeight())
                {
                    mWidgetXML.setAttribute("bounds", "autoResizeToContentsHeight", true, 0);
                }

                // Listen events
                vector<uiViz::WidgetEvent> listeningEvents = getViz()->getListeningEvents(getPersistentId());
                if (listeningEvents.size() > 0)
                {
                    mWidgetXML.addTag("listenEvents");
                    mWidgetXML.pushTag("listenEvents");
                    int eventIndex = 0;
                    for (uiViz::WidgetEvent e : listeningEvents)
                    {
                        mWidgetXML.addTag("event");
                        mWidgetXML.setAttribute("event", "source", e.eventSenderId, eventIndex);
                        mWidgetXML.setAttribute("event", "eventName", e.eventName, eventIndex);
                        if (e.redirectEventName != "")
                            mWidgetXML.setAttribute("event", "redirectEventName", e.redirectEventName, eventIndex);
                        eventIndex++;
                    }
                    mWidgetXML.popTag(); // listenEvents
                }

                mWidgetXML.popTag(); // parentTag
            }
            return mWidgetXML;
        }

        virtual string getWidgetPrettyName()
        {
            return Shared::lang("WIDGET_NAME_" + getWidgetClassType()); // eg WIDGET_NAME_Widget in Lanuguage XML file
        }

        string getWidgetClassType()
        {
            return mWidgetClassType;
        }

        void setWidgetClassType(string val)
        {
            mWidgetClassType = val;
        }

        virtual ofxXmlSettings saveState()
        {
            return saveState("widget");
        }

        virtual string getXML()
        {
            ofxXmlSettings mWidgetXML = saveState();
            string XML = "";
            mWidgetXML.copyXmlToString(XML);
            return XML;
        }

        widgetTelemetry *getWidgetTelemetry()
        {
            return &mWidgetTelemetry;
        }

        virtual void onWidgetThemeChanged(WidgetEventArgs args)
        {
            // Implement any behaviour in derrived class which should theme change...
        }

        bool eventListenersExistForThisWidget(string eventName)
        {
            // return getTransmitOscMessages() || getViz()->doesWidgetHaveListeners(getPersistentId(), eventName);
            return getViz()->doesWidgetHaveListeners(getPersistentId(), eventName);
        }

        void eventTransmit(string eventSenderId, string eventName, string eventXML, WidgetBase &sender, WidgetBase *target, bool dispatchToLocalListeners)
        {
            ofxXmlSettings xml = ofxXmlSettings();
            if (!xml.loadFromBuffer(eventXML.c_str()))
            {
                ofLogWarning("Widget") << eventSenderId << "." << eventName << ": failed to parse event XML \n"
                                            << eventXML;
            }

            WidgetEventArgs args = WidgetEventArgs(eventSenderId, eventName, xml, sender, target);
            ofNotifyEvent(widgetEventReceived, args);
            onWidgetEventReceived(args);

            target ? getViz()->addToEventTransmitQueue("/widget/event_transmit", "widget", eventSenderId, target->getPersistentId(), eventName, eventXML, getTransmitOscMessages(), dispatchToLocalListeners) : getViz()->addToEventTransmitQueue("/widget/event_transmit", "widget", eventSenderId, "", eventName, eventXML, getTransmitOscMessages(), dispatchToLocalListeners);
        }

        void eventTransmit(string eventSenderId, string eventName, string eventXML, WidgetBase &sender, WidgetBase *target)
        {
            eventTransmit(eventSenderId, eventName, eventXML, sender, target, true);
        }

        void eventTransmit(string eventSenderId, string eventName, string eventXML, WidgetBase &sender)
        {
            eventTransmit(eventSenderId, eventName, eventXML, sender, nullptr);
        }

        void addToEventTransmitQueue(string eventName, string eventXML)
        {
            if (!eventListenersExistForThisWidget(eventName))
                return;
            getViz()->addToEventTransmitQueue(
                "/widget/event_transmit", "widget", getPersistentId(), "", eventName, eventXML, getTransmitOscMessages(), true);
        }

        virtual void onCoreWidgetEventReceived(WidgetEventArgs &args)
        {
            // Internal...
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args)
        {
            // To be overridden in derived classes
        }

        shared_ptr<ofxSmartFont> getSmallFontSizedForDimensions(int width, int height)
        {
            return getViz()->getSmallFontSizedForDimensions(width, height);
        }

        shared_ptr<ofxSmartFont> getMediumFontSizedForDimensions(int width, int height)
        {
            return getViz()->getMediumFontSizedForDimensions(width, height);
        }

        shared_ptr<ofxSmartFont> getFontSizedForDimensions(int width, int height)
        {
            return getViz()->getFontSizedForDimensions(width, height);
        }

        shared_ptr<ofxSmartFont> getLargeFontSizedForDimensions(int width, int height)
        {
            return getViz()->getLargeFontSizedForDimensions(width, height);
        }

        virtual void setNeedsUpdate(bool needsUpdate)
        {
            mNeedsUpdate = needsUpdate;
        }

        bool needsUpdate()
        {
            return mNeedsUpdate;
        }

        // For content chrome
        void setWidgetNeedsUpdate(bool needsUpdate)
        {
            mWidgetUINeedsUpdate = needsUpdate;

#if VIZ_DEBUG_LEVEL >= 1 && VIZ_DEBUG_LEVEL < 2
            cout << "UPDATING WIDGET..." + getWidgetId() + " [" + getTitle() + "]\n";
#endif
        }

        bool widgetNeedsUpdate()
        {
            return mWidgetUINeedsUpdate;
        }

        //! Set up visual elements in this function which require scaling
        virtual void initWidgetScale(float scale)
        {
        }

        //! set up all the stuff upon widget initialization
        virtual void initWidget()
        {
        }

        virtual void update(WidgetContext context)
        {
            // Implemented by descendant class...
        }

        virtual void draw(WidgetContext context)
        {
            // Implemented by descendant class...
        }

        //! The widget reference Id which gets persisted when saving / serializing
        string getPersistentId()
        {
            return mPersistentId;
        }

        void setPersistentId(string val)
        {
            mPersistentId = ofToUpper(val);
        }

        //! The parent widget reference Id for this widget
        string getParentPersistentId()
        {
            return parent() ? parent()->getPersistentId() : "";
        }

        //!  Raw X position. Excludes padding.
        virtual int getX()
        {
            return mX;
        }

        //!  Raw Y position. Excludes padding.
        virtual int getY()
        {
            return mY;
        }

        //!  Raw Width. Excludes padding.
        virtual int getWidth()
        {
            return mWidth;
        }

        //!  Raw Height. Excludes padding.
        virtual int getHeight()
        {
            return mHeight;
        }

        //!  X position. Includes padding. Includes scroll offset.
        virtual int getUsableX()
        {
            return getUsableX(true);
        }

        //!  Y position. Includes padding. Includes scroll offset.
        virtual int getUsableY()
        {
            return getUsableY(true);
        }

        virtual int getUsableX(bool inclScrollOffset)
        {
            int scrollOffsetX = inclScrollOffset ? getScrollOffsetX() : 0;
            return getX() + scrollOffsetX;
        }

        virtual int getUsableY(bool inclScrollOffset)
        {
            int scrollOffsetY = inclScrollOffset ? getScrollOffsetY() : 0;
            return getY() + scrollOffsetY;
        }

        //!  Usable Width. Includes padding.
        virtual int getUsableWidth()
        {
            return getWidth();
        }

        //!  Usable Height. Includes padding.
        virtual int getUsableHeight()
        {
            return getHeight();
        }

        //!  Usable Width. Includes padding. (same as getUsableWidth() for WidgetBase)
        virtual int getWidgetStateWidth()
        {
            return getUsableWidth();
        }

        //!  Usable Height. Includes padding. (same as getUsableHeight() for WidgetBase)
        virtual int getWidgetStateHeight()
        {
            return getUsableHeight();
        }

        //!  Should this widget resize to it's content width? Required implementation by widget developer
        virtual bool getIsAutoResizeToContentsWidth()
        {
            return mIsAutoResizeToContentsWidth;
        }

        //!  Set wether this widget should resize to it's content width? Required implementation by widget developer
        virtual void setIsAutoResizeToContentsWidth(bool val)
        {
            mIsAutoResizeToContentsWidth = val;
        }

        //!  Should this widget resize to it's content height? Required implementation by widget developer
        virtual bool getIsAutoResizeToContentsHeight()
        {
            return mIsAutoResizeToContentsHeight;
        }

        //!  Set wether this widget should resize to it's content height? Required implementation by widget developer
        virtual void setIsAutoResizeToContentsHeight(bool val)
        {
            mIsAutoResizeToContentsHeight = val;
        }

        int getScreenPadding()
        {
            return mScreenPadding;
        }

        void setScreenPadding(int val)
        {
            mScreenPadding = val;
        }

        string getX_Expr()
        {
            return mX_Expr;
        }

        string getY_Expr()
        {
            return mY_Expr;
        }

        string getWidth_Expr()
        {
            return mWidth_Expr;
        }

        string getHeight_Expr()
        {
            return mHeight_Expr;
        }

        string getTitle()
        {
            return mTitle;
        }

        virtual int getTitleWidth()
        {
            return (getTitle().size() > 0 ? mTitleWidth : 0);
        }

        void setTitleWidth(int val)
        {
            mTitleWidth = val;
        }

        virtual void setTitle(string title)
        {
            mTitle = title;
        }

        bool getExpressionReferencesPersistentId(string persistentId)
        {

            if (!getIsVisible())
                return false;

            return ofIsStringInString(getX_Expr(), "${" + persistentId) ||
                   ofIsStringInString(getY_Expr(), "${" + persistentId) ||
                   ofIsStringInString(getWidth_Expr(), "${" + persistentId) ||
                   ofIsStringInString(getHeight_Expr(), "${" + persistentId);
        }

        virtual bool getExpressionRecalculationRequired()
        {
            return getIsVisible() && mExpressionRecalculationRequired;
        }

        virtual void setExpressionRecalculationRequired(bool val)
        {
            mExpressionRecalculationRequired = val;
        }

        int getMinWidth()
        {
            return mMinWidth;
        }

        int getMinHeight()
        {
            return mMinHeight;
        }

        int getMaxWidth()
        {
            return mMaxWidth;
        }

        int getMaxHeight()
        {
            return mMaxHeight;
        }

        virtual void setX(int x)
        {
            if (mX != x)
                setExpressionRecalculationRequired(true);
            mX = x;
        }

        virtual void setY(int y)
        {
            if (mY != y)
                setExpressionRecalculationRequired(true);
            mY = y;
        }

        virtual void setWidth(int width)
        {
            if (mWidth != width)
            {
                setExpressionRecalculationRequired(true);
                setTitle(getTitle()); // force title text recalculation
            }
            mWidth = width;
        }

        virtual void setHeight(int height)
        {
            if (mHeight != height)
            {
                setExpressionRecalculationRequired(true);
                setTitle(getTitle()); // force title text recalculation
            }
            mHeight = height;
        }

        virtual void setWidgetPosition(float x, float y, bool enableDockingLogic)
        {
            setX(x);
            setY(y);
        }

        virtual Elm::vizRect setSize(int width, int height)
        {
            int clampWidth = width;
            clampWidth = std::max((float)clampWidth, (float)getMinWidth());
            clampWidth = std::min((float)clampWidth, min((float)getMaxWidth(), (float)(deScale(ofGetWindowWidth()) - getScreenPadding() - getX())));
            setWidth(clampWidth);

            int clampHeight = height;
            clampHeight = std::max((float)clampHeight, (float)getMinHeight());
            clampHeight = std::min((float)clampHeight, min((float)getMaxHeight(), (float)(deScale(ofGetWindowHeight()) - getScreenPadding() - getY())));
            setHeight(clampHeight);

            setScrollOffsetY(getScrollOffsetY(), true);
            setScrollOffsetX(getScrollOffsetX(), true);
            return Elm::vizRect(getX(), getY(), clampWidth, clampHeight);
        }

        virtual bool setWidgetSize_ShouldPrevent(int width, int height, bool enableDockingLogic)
        {
            return false;
        }

        virtual void setWidgetSize(int width, int height, bool enableDockingLogic)
        {
            if (getLockAspectRatio())
            {
                height = width * getAspectRatio();
            }

            if (getWidth() != width || getHeight() != height)
            {
                int beforeSetWidth = getWidth();
                int beforeSetHeight = getHeight();

                if (setWidgetSize_ShouldPrevent(width, height, enableDockingLogic))
                {
                    return;
                }

                Elm::vizRect resultRect = setSize(width, height);

                if (resultRect.width != beforeSetWidth || resultRect.height != beforeSetHeight)
                {
                    ofResizeEventArgs args(resultRect.width, resultRect.height);
                    ofNotifyEvent(widgetResized, args);
                    setNeedsUpdate(true);
                }
            }
        }

        void setX_Expr(string expr)
        {
            mX_Expr = (expr == "" ? "" : ofToUpper(expr));
        }

        void setY_Expr(string expr)
        {
            mY_Expr = (expr == "" ? "" : ofToUpper(expr));
        }

        void setWidth_Expr(string expr)
        {
            mWidth_Expr = (expr == "" ? "" : ofToUpper(expr));
        }

        void setHeight_Expr(string expr)
        {
            mHeight_Expr = (expr == "" ? "" : ofToUpper(expr));
        }

        void setIgnoreExpr_Layout(bool val)
        {
            mIgnoreExpr_Layout = val;
        }

        bool getIgnoreExpr_Layout()
        {
            return mIgnoreExpr_Layout;
        }

        void clearWidgetLayoutExpressions()
        {
            setX_Expr("");
            setY_Expr("");
            setWidth_Expr("");
            setHeight_Expr("");
        }

        void setMinWidth(int minWidth)
        {
            mMinWidth = minWidth;
        }

        void setMinHeight(int minHeight)
        {
            mMinHeight = minHeight;
        }

        void setMaxWidth(int maxWidth)
        {
            mMaxWidth = maxWidth;
        }

        void setMaxHeight(int maxHeight)
        {
            mMaxHeight = maxHeight;
        }

        bool getIsShadow()
        {
            return mIsShadow;
        }

        void setIsShadow(bool value)
        {
            mIsShadow = value;
        }

        bool getIsRoundedRectangle()
        {
            return mIsRoundedRectangle;
        }

        void setIsRoundedRectangle(bool value)
        {
            mIsRoundedRectangle = value;
        }

        float getAspectRatio()
        {
            return mAspectRatio;
        }

        void setAspectRatio(float val)
        {
            mAspectRatio = val;
        }

        bool getLockAspectRatio()
        {
            return mLockAspectRatio;
        }

        void setLockAspectRatio(bool val)
        {
            mLockAspectRatio = val;
        }

        virtual bool getIsVisible()
        {
            // A widget should never be visible if it's parent is not!
            WidgetBase *w = parent();
            while (w)
            {
                if (!w->getIsVisible())
                    return false;
                w = w->parent();
            }

            return mIsVisible;
        }

        virtual void setIsVisible(bool value)
        {
            bool visibilityChanged = (getIsVisible() != value);
            mIsVisible = value;
            mShouldCloseThisWidget = false;
            if (getShowingAsPopout())
            {
                getViz()->setIsPopoutVisible(getWidgetId(), value);
                if (!value)
                {
                    // Restore the previously active widget
                    getViz()->setActiveWidgetId(getViz()->getNextInLineForFocusWidgetId());
                }
            }

            if (visibilityChanged)
            {
                onWidgetVisibilityChanged(value);
                onCoreWidgetVisibilityChanged(value);
            }
        }

        Coord::vizBounds getContentBoundsScaled()
        {
            return mContentBoundsScaled;
        }

        void clearContentBoundsScaled()
        {
            mContentBoundsScaled = Coord::vizBounds(0, 0, 0, 0);
        }

        void appendContentBoundsScaled(Coord::vizBounds boundsVal)
        {
            if (boundsVal.x > mContentBoundsScaled.x)
                mContentBoundsScaled.x = boundsVal.x;
            if (boundsVal.y > mContentBoundsScaled.y)
                mContentBoundsScaled.y = boundsVal.y;
            mContentBoundsScaled.width += boundsVal.width;
            mContentBoundsScaled.height += boundsVal.height;
        }

        void setContentBoundsScaled(Coord::vizBounds boundsVal)
        {
            mContentBoundsScaled.x = boundsVal.x;
            mContentBoundsScaled.y = boundsVal.y;
            mContentBoundsScaled.width = boundsVal.width;
            mContentBoundsScaled.height = boundsVal.height;
        }

        void resetTimer()
        {
            mElapsedTimerMillis = 0;
        }

        uint64_t getTimerMillis()
        {
            if (mElapsedTimerMillis == 0)
                mElapsedTimerMillis = ofGetSystemTimeMillis();
            return ofGetSystemTimeMillis() - mElapsedTimerMillis;
        }

        int getScrollingStartedIntervalX()
        {
            return ofGetSystemTimeMillis() - mScrollingStartedMS_X;
        }

        bool getIsScrollingXMouseDown()
        {
            return mIsScrollingXMouseDown;
        }

        void setIsScrollingXMouseDown(bool val)
        {
            mIsScrollingXMouseDown = val;
        }

        bool getIsScrollingYMouseDown()
        {
            return mIsScrollingYMouseDown;
        }

        void setIsScrollingYMouseDown(bool val)
        {
            mIsScrollingYMouseDown = val;
        }

        bool getIsScrollingX()
        {
            if (getIsScrollingXMouseDown())
                return true;
            if (mScrollingStartedMS_X == 0 || getScrollingStartedIntervalX() > 1200)
            {
                mScrollingStartedMS_X = 0;
                return false;
            }
            else
            {
                return true;
            }
        }

        void setIsScrollingX()
        {
            mScrollingStartedMS_X = ofGetSystemTimeMillis();
        }

        int getScrollingStartedIntervalY()
        {
            return ofGetSystemTimeMillis() - mScrollingStartedMS_Y;
        }

        bool getIsScrollingY()
        {
            if (getIsScrollingYMouseDown())
                return true;
            if (mScrollingStartedMS_Y == 0 || getScrollingStartedIntervalY() > 1200)
            {
                mScrollingStartedMS_Y = 0;
                return false;
            }
            else
            {
                return true;
            }
        }

        void setIsScrollingY()
        {
            mScrollingStartedMS_Y = ofGetSystemTimeMillis();
        }

        float getScrollOffsetX()
        {
            return mScrollOffsetX;
        }

        float getScrollOffsetY()
        {
            return mScrollOffsetY;
        }

        virtual void setScrollOffsetX(float val, bool absolute)
        {

            if (val > 0 && getScrollOffsetX() >= 0)
            {
                // we've reached the left of the content
                mScrollOffsetX = 0.0f;
                return;
            }

            if (getUsableWidth() >= deScale(getContentBoundsScaled().width) && mScrollOffsetX != 0)
            {
                // we've reached the right of the content
                mScrollOffsetX = 0.0f;
                setWidgetNeedsUpdate(true);
                return;
            }

            if (val < 0 && getScrollOffsetX() <= (getUsableWidth() - deScale(getContentBoundsScaled().width)))
            {
                // we've reached the end of the content
                mScrollOffsetX = getUsableWidth() - deScale(getContentBoundsScaled().width);
                return;
            }

            mScrollOffsetX = absolute ? val : mScrollOffsetX + val;

            if (absolute && getScrollOffsetX() != val)
            {
                setWidgetNeedsUpdate(true);
            }
        }

        virtual void setScrollOffsetY(float val, bool absolute)
        {

            if (val > 0 && getScrollOffsetY() >= 0)
            {
                // we've reached the top of the content
                mScrollOffsetY = 0.0f;
                return;
            }

            if (getUsableHeight() >= deScale(getContentBoundsScaled().height) && mScrollOffsetY != 0)
            {
                // we've reached the top of the content
                mScrollOffsetY = 0.0f;
                setWidgetNeedsUpdate(true);
                return;
            }

            if (val < 0 && getScrollOffsetY() <= (getUsableHeight() - deScale(getContentBoundsScaled().height)))
            {
                // we've reached the end of the content
                mScrollOffsetY = getUsableHeight() - deScale(getContentBoundsScaled().height);
                return;
            }
            mScrollOffsetY = absolute ? val : mScrollOffsetY + val;

            if (absolute && getScrollOffsetY() != val)
            {
                setWidgetNeedsUpdate(true);
            }
        }

        float getScrollScaleX()
        {
            return mScrollScaleX;
        }

        void setScrollScaleX(float val)
        {
            mScrollScaleX = val;
        }

        float getScrollScaleY()
        {
            return mScrollScaleY;
        }

        void setScrollScaleY(float val)
        {
            mScrollScaleY = val;
        }

        int getEnableScrollYPositive()
        {
            return mEnableScrollYPositive;
        }

        void setEnableScrollYPositive(bool val)
        {
            mEnableScrollYPositive = val;
        }

        int getEnableScrollYNegative()
        {
            return mEnableScrollYNegative;
        }

        void setEnableScrollYNegative(bool val)
        {
            mEnableScrollYNegative = val;
        }

        int getEnableScrollXPositive()
        {
            return mEnableScrollXPositive;
        }

        void setEnableScrollXPositive(bool val)
        {
            mEnableScrollXPositive = val;
        }

        int getEnableScrollXNegative()
        {
            return mEnableScrollXNegative;
        }

        void setEnableScrollXNegative(bool val)
        {
            mEnableScrollXNegative = val;
        }

        void setEnableScroll(bool val)
        {
            setEnableScrollXPositive(val);
            setEnableScrollXNegative(val);
            setEnableScrollYPositive(val);
            setEnableScrollYNegative(val);
        }

        void setScrollAction(ScrollAction val)
        {
            mScrollAction = val;
        }

        ScrollAction getScrollAction()
        {
            return mScrollAction;
        }

        void setCloseAction(CloseAction val)
        {
            mCloseAction = val;
        }

        CloseAction getCloseAction()
        {
            return mCloseAction;
        }

        bool getIsDraggable()
        {
            return mIsDraggable;
        }

        void setIsDraggable(bool value)
        {
            mIsDraggable = value;
            mTempIsDraggable = value;
        }

        bool getIsResizable()
        {
            return mIsResizable;
        }

        void setIsResizable(bool value)
        {
            mIsResizable = value;
            mTempIsResizable = value;
        }

        bool getIsAutoUpdateWhenActive()
        {
            return mIsAutoUpdateWhenActive;
        }

        bool getRespondedToFirstMousePressedEvent()
        {
            return mRespondedToFirstMousePressedEvent;
        }

        void setRespondedToFirstMousePressedEvent(bool value)
        {
            mRespondedToFirstMousePressedEvent = value;
        }

        bool getRespondedToFirstMouseReleasedEvent()
        {
            return mRespondedToFirstMouseReleasedEvent;
        }

        void setRespondedToFirstMouseReleasedEvent(bool value)
        {
            mRespondedToFirstMouseReleasedEvent = value;
        }

        bool getMousePressWasRegisteredWhenCheckingForReleased()
        {
            return mMousePressWasRegisteredWhenCheckingForReleased;
        }

        void setMousePressWasRegisteredWhenCheckingForReleased(bool value)
        {
            mMousePressWasRegisteredWhenCheckingForReleased = value;
        }

        void setIsAutoUpdateWhenActive(bool value)
        {
            mIsAutoUpdateWhenActive = value;
        }

        bool getIsDragging()
        {
            return mIsDragging;
        }

        void setIsDragging(bool value)
        {
            mIsDragging = value;
            if (mIsDragging)
            {
                mContentDragIconFbo = saveWidgetContentsToFbo();
            }
            else
            {
                mContentDragIconFbo.destroy();
            }
        }

        bool getIsResizing()
        {
            return mIsResizing;
        }

        void setIsResizing(bool value)
        {
            mIsResizing = value;
        }

        virtual bool isTransitioningAlpha()
        {
            return false;
        }

        Coord::vizPoint getLastMouseClickCoords()
        {
            return mLastMouseClickCoords;
        }

        void setLastMouseClickCoords(Coord::vizPoint p)
        {
            mLastMouseClickCoords = p;
        }

        bool matchesWidgetId(string widgetId)
        {
            return ofToLower(widgetId) == ofToLower(getWidgetId());
        }

        bool matchesWidget(WidgetBase *w)
        {
            if (!w)
                return false;
            return matchesWidgetId(w->getWidgetId());
        }

        bool matchesWidget(vector<WidgetBase *> w)
        {
            if (w.size() == 0)
                return false;
            bool found = false;
            for (WidgetBase *wb : w)
            {

                if (wb && matchesWidgetId(wb->getWidgetId()))
                {
                    return true;
                }
            }
            return false;
        }

        bool matchesParentWidgetId(string widgetId)
        {
            if (parent())
            {
                return ofToLower(widgetId) == ofToLower(parent()->getWidgetId());
            }
            return false;
        }

        bool matchesParentWidget(WidgetBase *w)
        {
            if (!w)
                return false;
            return matchesParentWidgetId(w->getWidgetId());
        }

        bool matchesWidgetOrParent(WidgetBase *w)
        {
            if (matchesWidget(w))
                return true;
            if (matchesParentWidget(w))
                return true;
            return false;
        }

        bool matchesWidgetOrChildOf(WidgetBase *w)
        {
            if (matchesWidget(w))
                return true;
            if (parent() && w->getWidgetId() == parent()->getWidgetId())
                return true;
            return false;
        }

        bool matchesWidgetOrDescendantOf(WidgetBase *w)
        {
            // TODO : implement
            return false;
        }

        //! The canonical widget reference Id
        string getWidgetId()
        {
            // String representation of this address:
            const void *address = static_cast<const void *>(this);
            std::stringstream ss;
            ss << address;
            return ss.str();
        }

        //! The widget Id of the widget responsible for maintaining this widget lifecycle. If empty, the widgetManager is responsible
        string getOwnerWidgetId()
        {
            return mOwnerWidgetId;
        }

        void setOwnerWidgetId(string val)
        {
            mOwnerWidgetId = val;
        }

        string getActiveWidgetId()
        {
            return getViz()->getActiveWidgetId();
        }

        bool getIsActiveWidget()
        {
            return getViz()->isActiveWidgetId(getWidgetId());
        }

        bool getIsChildOfModal()
        {
            return getIsChildOfModal(this);
        }

        void setActiveWidget()
        {
            if (!getIsVisible())
                return;

            // A widget should never be become active if it's parent is not visible!
            WidgetBase *w = parent();
            while (w)
            {
                if (!w->getIsVisible())
                    return;
                w = w->parent();
            }

            if (getViz()->getModalWidgetId() != "" && !getViz()->getIsPopoutVisible())
            { // Only enforce modal logic if Context menu not visible!
                if (!getIsChildOfModal())
                    return;
            }
            getViz()->setActiveWidgetId(getWidgetId());

            WidgetBase *r = root();
            if (r)
            {
                getViz()->setTopmostActiveRootWidgetId(r->getWidgetId());
            }
        }

        void clearActiveWidgetId()
        {
            /*
                If getting unexpected EXC_BAD_ACCESS errors make sure to check that ALL widgets are callling
                ofRemoveListener for ALL corresponding ofAddListener in ther destructors!
            */

            getViz()->clearActiveWidgetId();
        }

        bool getIsModalWidget()
        {
            return getViz()->isModalWidgetId(getWidgetId());
        }

        void clearIfModal()
        {
            if (getIsModalWidget())
            {
                clearModalWidgetId();
            }
        }

        void setModalWidget()
        {
            setActiveWidget();
            getViz()->setModalWidgetId(getWidgetId());
        }

        void clearModalWidgetId()
        {
            clearActiveWidgetId();
            getViz()->clearModalWidgetId();
        }

        bool getIsRootWidget()
        {
            return parent() == nullptr;
        }

        bool getIsNextInLineForFocus()
        {
            return mIsNextInLineForFocus;
        }

        void setIsNextInLineForFocus(bool val)
        {
            mIsNextInLineForFocus = val;
            if (val)
                getViz()->setNextInLineForFocusWidgetId(getWidgetId());
        }

        //! Override if extending WidgetBase for returning the index of a child element
        virtual int getWidgetElementIndex(string persistentId)
        {
            return 0;
        }

        //! Override if extending WidgetBase for returning the count of child elements
        virtual int getWidgetElementSize()
        {
            return 0;
        }

        string getTargetDropWidgetId()
        {
            return getViz()->getTargetDropWidgetId();
        }

        void setTargetDropWidgetId()
        {
            getViz()->setTargetDropWidgetId(getWidgetId());
        }

        void clearTargetDropWidgetId()
        {
            getViz()->clearTargetDropWidgetId();
        }

        int getUserExperience()
        {
            return getViz()->getUserExperience();
        }

        float scale(float val)
        {
            return getViz()->scale(val);
        }

        float deScale(float val)
        {
            return getViz()->deScale(val);
        }

        float scaleAnimation(string animationName, float currVal, float targetVal, float secondsDuration)
        {
            return getViz()->scaleAnimation(getWidgetId(), getPersistentId() + "_" + animationName, currVal, targetVal, secondsDuration);
        }

        float scaleAnimationForUI(string animationName, float currVal, float targetVal, float secondsDuration)
        {
            return getViz()->scaleAnimationForUI(getWidgetId(), getPersistentId() + "_" + animationName, currVal, targetVal, secondsDuration);
        }

        void removeAnimation(string animationName)
        {
            getViz()->removeAnimation(getPersistentId() + "_" + animationName);
        }

        int getNonScaledPadding()
        {
            return getViz()->getNonScaledPadding();
        }

        int getScaledPadding()
        {
            return getViz()->getScaledPadding();
        }

        bool isDraggingContent()
        {
            return /* getScrollAction() != ScrollAction::SCROLL && */ !getIsScrollingXMouseDown() && !getIsScrollingYMouseDown() && mIsDraggingContent;

            // todo --> convert !getIsScrollingXMouseDown() && !getIsScrollingYMouseDown() to TOUCH
        }

        bool getIsUnhoverLoseFocus()
        {
            return mIsUnhoverLoseFocus;
        }

        void setIsUnhoverLoseFocus(bool val)
        {
            mIsUnhoverLoseFocus = val;
        }

        bool getShouldCloseIfOtherWidgetClicked()
        {
            if (getIsModalWidget())
                return false;
            return mShouldCloseIfOtherWidgetClicked;
        }

        void setShouldCloseIfOtherWidgetClicked(bool val)
        {
            mShouldCloseIfOtherWidgetClicked = val;
        }

        bool getUseFbo()
        {
            return mUseFbo;
        }

        void setUseFbo(bool value)
        {
            mUseFbo = value;
        }

        virtual WidgetBase *parent()
        {
            return nullptr;
        }

        virtual WidgetBase *root()
        {
            WidgetBase *r = this;
            WidgetBase *w = parent();
            while (w)
            {
                r = w;
                w = w->parent();
            }
            return r;
        }

        string getPesistentIdPath()
        {
            string path = getPersistentId();
            WidgetBase *w = parent();
            while (w)
            {
                path = w->getPersistentId() + "->" + path;
                w = w->parent();
            }
            return path;
        }

        virtual ofFbo saveWidgetContentsToFbo()
        {
            // Do something in descendant class
            ofFbo fboCopy;
            return fboCopy;
        }

        virtual ofFbo getContentDragIconFbo()
        {
            return mContentDragIconFbo;
        }

        virtual void drawContentDragIcon()
        {
            if (getContentDragIconFbo().isAllocated())
            {

                float ratio = (float)getWidth() / (float)getHeight();
                if (ratio == 0)
                    ratio = 1;
                int targetWidth = 40;
                int targetHeight = 40;

                if (targetWidth > targetHeight)
                {
                    targetWidth = (float)targetWidth / ratio;
                }
                else
                {
                    targetHeight = (float)targetHeight / ratio;
                }

                getContentDragIconFbo().draw(ofGetMouseX(), ofGetMouseY(), scale(targetWidth), scale(targetHeight));
            }
            else
            {
                // this is a placeholder "catch all" rectangle to indicate contnet being DRAGGED
                ofDrawRectangle(ofGetMouseX(), ofGetMouseY(), scale(30), scale(40));
            }
        }

        virtual void drawPotentiallyDropTargetIndication()
        {
            // this is a placeholder "catch all" rectangle to indicate contnet (potentially) being DROPPED

            ofColor c = (getTheme().IsDark ? ofColor(255, 255, 255, 100) : ofColor(0, 0, 0, 100));

            Elm vizElm_POTENTIAL_DROP_INDICATOR;
            getIsRoundedRectangle() ? vizElm_POTENTIAL_DROP_INDICATOR.setRectRounded(scale(getX()), scale(getY()), scale(getWidth()), scale(getHeight()), getScaledPadding(), c) : vizElm_POTENTIAL_DROP_INDICATOR.setRectangle(scale(getX()), scale(getY()), scale(getWidth()), scale(getHeight()), c);

            // vizElm_POTENTIAL_DROP_INDICATOR.setFilled(false);

            vizElm_POTENTIAL_DROP_INDICATOR.draw();
        }

        virtual bool getIsMousePressedOverWidget(bool respondConstantly)
        {
            if (!getIsActiveWidget())
                return false;

            if (!getIsScrollingY() && !getIsScrollingX() && !getIsDragging() && !getIsResizing() && ofGetMousePressed() && (!getRespondedToFirstMousePressedEvent() || respondConstantly))
            {
                setRespondedToFirstMousePressedEvent(true);
                return true;
            }
            return false;
        }

        bool ofGetTouchDownOrMousePressed()
        {
            return ofGetMousePressed(); // || TODO: istoucheddown
        }

        virtual bool getIsMousePressedAndReleasedOverWidget(bool includeWidgetChromeElements)
        {
            if (!getIsActiveWidget())
                return false;

            if (!getIsDragging() && !getIsResizing() && !ofGetMousePressed() && (getMousePressWasRegisteredWhenCheckingForReleased() && !getRespondedToFirstMouseReleasedEvent()))
            {

                setRespondedToFirstMouseReleasedEvent(true);
                setMousePressWasRegisteredWhenCheckingForReleased(false);

                int mX = deScale(ofGetMouseX());
                int mY = deScale(ofGetMouseY());
                Coord::vizPoint p = getLastMouseClickCoords();

                if (mX > p.x + mMinPixelsMovedToRegisterDrag || mX < p.x - mMinPixelsMovedToRegisterDrag ||
                    mY > p.y + mMinPixelsMovedToRegisterDrag || mY < p.y - mMinPixelsMovedToRegisterDrag)
                {
                    // The mouse moved too much to register a pressed and released event
                    return false;
                }
                else
                {
                    return true;
                }
            }

            return false;
        }

        virtual bool isWidgetHovered()
        {

            // if (!getIsVisible()) return false;

            int mX = deScale(ofGetMouseX());
            int mY = deScale(ofGetMouseY());

            return (mX > getX() && mX < getX() + getWidth() && mY > getY() && mY < getY() + getHeight());
        }

        virtual void onWidgetAfterAllWidgetsLoaded(std::vector<std::reference_wrapper<Widget>> widgets)
        {
            // Fires when WidgetManager has loaded all widgets
        }

        virtual void onCoreWidgetScaleChanged(float &scale)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            onWidgetScaleChanged(scale);
        }

        virtual void onWidgetScaleChanged(float scale)
        {
            // Implement any behaviour in derrived class which should handle scale change...
        }

        //! Implement any behaviour in derrived class which should handle when this widget is added to a parent.
        virtual void onWidgetAddedAsChild(WidgetEventArgs args)
        {
        }

        virtual void onCoreWidgetWindowResized()
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            onWidgetWindowResized();
        }

        virtual void onWidgetWindowResized()
        {
            // Implement any behaviour in derrived class which should handle window resize...
        }

        virtual void onWidgetOwnedWidgetClosed(WidgetBase *closedWidget)
        {
            // Implement any behaviour in derrived class which should handle when owned widget is closed...
        }

        virtual void onCoreWidgetWindowResized(ofResizeEventArgs &e)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            onWidgetWindowResized(e);
        }

        virtual void onCoreWidgetResized(ofResizeEventArgs &e)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            eventTransmit(getPersistentId(), WIDGET_EVENT::RESIZED, "<event width=\"" + ofToString(e.width) + "\" height=\"" + ofToString(e.height) + "\"></event>", *this);
            onWidgetResized(e.width, e.height);
        }

        virtual void onWidgetWindowResized(ofResizeEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle window resize...
        }

        virtual void onCoreWidgetMouseMoved(ofMouseEventArgs &e)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            if (getIsActiveWidget() && getIsEnabled())
            {
                if (getExternalDrop().getIsDragging())
                {
                    getExternalDrop().handleExternalDragAndDropMotionNotify();
                    if (!ofGetMousePressed())
                        getExternalDrop().handleExternalDragAndDropButtonReleaseEvent();
                }
            }
        }

        void handleMousePressed(const std::function<void(ofMouseEventArgs &)> &func)
        {
            m_onCoreWidgetMousePressedFunction = func;
        }

        virtual void onCoreWidgetMousePressed(ofMouseEventArgs &e)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            if (getIsActiveWidget())
            {
                setRespondedToFirstMouseReleasedEvent(false);
                setMousePressWasRegisteredWhenCheckingForReleased(true);
                setLastMouseClickCoords(Coord::vizPoint(deScale(ofGetMouseX()), deScale(ofGetMouseY())));

                if (!isDraggingContent())
                {
                    mPotentiallyStartedDraggingStartX = deScale(ofGetMouseX());
                    mPotentiallyStartedDraggingStartY = deScale(ofGetMouseY());
                    // cout << "registered potential drag start position: " << mPotentiallyStartedDraggingStartX << "," << mPotentiallyStartedDraggingStartY << "\n";
                }

                if (getIsEnabled())
                {
                    onWidgetMousePressed(e);
                    m_onCoreWidgetMousePressedFunction(e);
                } // test this
            }
        }

        virtual void onWidgetMousePressed(ofMouseEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle mouse released...
        }

        void handleMouseReleased(const std::function<void(ofMouseEventArgs &)> &func)
        {
            m_onCoreWidgetMouseReleasedFunction = func;
        }

        virtual void onCoreWidgetMouseReleased(ofMouseEventArgs &e)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            setRespondedToFirstMousePressedEvent(false);

            if (mDraggingFileIn)
            {
                mDraggingFileIn = false;
            }

            if (isDraggingContent())
            {
                // Restore values back what they were before drag operation
                setIsDraggable(mTempIsDraggable);
                setIsResizable(mTempIsResizable);
                getViz()->setIsAnyWidgetDraggingOrResizing(false);
                setIsDragging(false);

                if (!mIsWindowHovered)
                {
                    cout << "Sending the following data EXTERNALLY:\n"
                         << getDragData();
                    onWidgetMouseContentDragReleased_Externally(e, getDragData());
                }

                onWidgetMouseContentDragReleased(e, getDragData());

                if (getIsEnabled())
                {
                    if (getTargetDropWidgetId() != getWidgetId())
                    {
                        WidgetBase *widgetTarget = getTargetDropWidget();
                        if (widgetTarget != nullptr)
                        {
                            widgetTarget->eventTransmit(getPersistentId(), WIDGET_EVENT::RECEIVE_DRAG_AND_DROP_DATA, "<event>" + getDragData() + "</event>", *this, widgetTarget); // Shared::encodeForXML(
                        }
                    }
                    else
                    {
                        eventTransmit(getPersistentId(), WIDGET_EVENT::RECEIVE_DRAG_AND_DROP_DATA, "<event>" + getDragData() + "</event>", *this);
                    }
                }

                mIsDraggingContent = false;
            }

            setIsScrollingXMouseDown(false);
            setIsScrollingYMouseDown(false);

            if (getExternalDrop().getIsDragging())
            {
                getExternalDrop().handleExternalDragAndDropButtonReleaseEvent();
            }

            if (getIsActiveWidget())
            {
                if (getIsEnabled())
                {
                    onWidgetMouseReleased(e);
                    m_onCoreWidgetMouseReleasedFunction(e);
                }
            }
        }

        virtual void onWidgetMouseReleased(ofMouseEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle mouse released...
        }

        virtual void onCoreWidgetTouchDown(ofTouchEventArgs &e)
        {

            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...

            if (getIsActiveWidget())
            {
                if (e.numTouches >= 2)
                {
                    ofMouseEventArgs args(ofMouseEventArgs::Type::Pressed, e.x, e.y, OF_MOUSE_BUTTON_RIGHT, 0);
                    onCoreWidgetMousePressed(args);
                }
            }
        }

        virtual void onWidgetTouchDown(ofTouchEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle mouse released...
        }

        virtual void onCoreWidgetTouchUp(ofTouchEventArgs &e)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...

            if (getIsActiveWidget())
            {
                ofMouseEventArgs args(ofMouseEventArgs::Type::Released, e.x, e.y, OF_MOUSE_BUTTON_LEFT, 0);
                onCoreWidgetMouseReleased(args);
                onWidgetTouchUp(e);

                setNeedsUpdate(true);
                setWidgetNeedsUpdate(true);
            }
        }

        virtual void onWidgetTouchUp(ofTouchEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle mouse released...
        }

        virtual void onWidgetMouseContentDragReleased(ofMouseEventArgs &e, string dragData)
        {
            // Implement any behaviour in derrived class which should handle mouse released...
        }

        virtual void onWidgetMouseContentDragReleased_Externally(ofMouseEventArgs &e, string dragData)
        {
            // Implement any behaviour in derrived class which should handle mouse released (outside of the application window)...
        }

        virtual void onWidgetResized(int width, int height)
        {
            // Hook for resized event
        }

        virtual void onWidgetResizingByUserEnded(int width, int height)
        {
            // Hook for resized event (by user)
        }

        // Draws scrollbar and scrolls to specified amount
        void scrollX(int val)
        {
            ofMouseEventArgs args(ofMouseEventArgs::Type::Scrolled, 0, 0, OF_MOUSE_BUTTON_LEFT, 0);
            args.scrollX = val;
            onCoreWidgetMouseScrolled(args);
        }

        // Draws scrollbar and scrolls to specified amount
        void scrollY(int val)
        {
            ofMouseEventArgs args(ofMouseEventArgs::Type::Scrolled, 0, 0, OF_MOUSE_BUTTON_LEFT, 0);
            args.scrollY = val;
            onCoreWidgetMouseScrolled(args);
        }

        // Draws scrollbar and scrolls to specified amount
        void scroll(int x, int y)
        {
            ofMouseEventArgs args(ofMouseEventArgs::Type::Scrolled, 0, 0, OF_MOUSE_BUTTON_LEFT, 0);
            args.scrollX = x;
            args.scrollY = y;
            onCoreWidgetMouseScrolled(args);
        }

        void scrollX_End()
        {
            if (deScale(getContentBoundsScaled().width) <= getWidth())
            {
                setScrollOffsetX(0, true);
            }
            else
            {
                setScrollOffsetX(getUsableWidth() - deScale(getContentBoundsScaled().width), true);
            }
            setNeedsUpdate(true);
        }

        void scrollY_End()
        {
            if (deScale(getContentBoundsScaled().height) <= getHeight())
            {
                setScrollOffsetY(0, true);
            }
            else
            {
                setScrollOffsetY(getUsableHeight() - deScale(getContentBoundsScaled().height), true);
            }
            setNeedsUpdate(true);
        }

        virtual void onCoreWidgetMouseScrolled(ofMouseEventArgs &e)
        {
            if (getIsActiveWidget())
            {
                switch (getScrollAction())
                {
                case ScrollAction::ZOOM:
                    if (!getIsResizable())
                        return;
                    setSize(getWidth() + e.scrollY * getScrollScaleX(), getHeight() + e.scrollY * getScrollScaleY());
                    onWidgetMouseScrolled(e);
                    break;

                case ScrollAction::SCROLL:
                {
                    // Basic scroll functionality for widgets!
                    int origScrollOffsetX = getScrollOffsetX();
                    int origScrollOffsetY = getScrollOffsetY();

                    if (getUsableWidth() < deScale(getContentBoundsScaled().width))
                    {
                        if (e.scrollX > 0)
                        {
                            // content moving rightwards
                            if (getEnableScrollXPositive())
                                setScrollOffsetX(e.scrollX * getScrollScaleX(), false);
                        }
                        else
                        {
                            if (getEnableScrollXNegative())
                                setScrollOffsetX(e.scrollX * getScrollScaleX(), false);
                        }
                    }

                    if (getUsableHeight() < deScale(getContentBoundsScaled().height))
                    {
                        if (e.scrollY > 0)
                        {
                            // content moving downwards
                            if (getEnableScrollYPositive())
                                setScrollOffsetY(e.scrollY * getScrollScaleY(), false);
                        }
                        else
                        {
                            if (getEnableScrollYNegative())
                                setScrollOffsetY(e.scrollY * getScrollScaleY(), false);
                        }
                    }

                    if (origScrollOffsetX != getScrollOffsetX() || origScrollOffsetY != getScrollOffsetY())
                    {

                        if (origScrollOffsetX != getScrollOffsetX())
                            setIsScrollingX();
                        if (origScrollOffsetY != getScrollOffsetY())
                            setIsScrollingY();

                        setNeedsUpdate(true);
                        onWidgetMouseScrolled(e);
                    }
                }
                break;

                case ScrollAction::BASIC:
                    onWidgetMouseScrolled(e);
                    break;
                }
            }
        }

        void coreDrawWidgetScrollbars(ofColor scrollbarColor)
        {
            if ((getEnableScrollXPositive() || getEnableScrollXNegative()) && deScale(getContentBoundsScaled().width) > getUsableWidth())
            {
                float scrollBarXPerc = std::min((float)1, (float)(getUsableWidth() / (float)deScale(getContentBoundsScaled().width)));

                vizElm_WIDGET_SCROLLBAR_X.setRectRounded(
                    std::max((float)scale(getUsableX(false)), (float)scale(getUsableX(false) + getScrollOffsetX() * -1 * scrollBarXPerc)),
                    scale(getUsableY(false) + getUsableHeight()) - getScaledPadding() * 2,
                    scale(scrollBarXPerc * getUsableWidth()),
                    scale(10),
                    scale(5),
                    ofColor(scrollbarColor));

                int scrollX = getPotentiallyStartedDraggingStartX() - (int)deScale(ofGetMouseX());
                bool isScrollbarHovered = !isDraggingContent() && vizElm_WIDGET_SCROLLBAR_X.isHovered_Rect();
                bool isDraggingToScroll = false; // TODO: ADD 2 FINGER SWIPE TO SCROLLE HERE OLD CODE: getIsMousePressedOverWidget(true) && scrollX != 0;

                if (getIsScrollingX() || isScrollbarHovered || isDraggingToScroll)
                {
                    vizElm_WIDGET_SCROLLBAR_X.draw();
                    if (isScrollbarHovered)
                        setIsScrollingX();

                    if ((!getIsScrollingXMouseDown() && ofGetMousePressed() && isScrollbarHovered) || isDraggingToScroll)
                    {
                        setIsScrollingXMouseDown(true);
                    }

                    if (getIsScrollingXMouseDown() && scrollX != 0)
                    {
                        ofMouseEventArgs args(ofMouseEventArgs::Type::Scrolled, 0, 0, OF_MOUSE_BUTTON_LEFT, 0);
                        args.scrollX = scrollX;
                        onCoreWidgetMouseScrolled(args);
                        setPotentiallyStartedDraggingStartX((int)deScale(ofGetMouseX()));
                    }
                }
            }

            if ((getEnableScrollYPositive() || getEnableScrollYNegative()) && deScale(getContentBoundsScaled().height) > getUsableHeight())
            {
                float scrollBarYPerc = std::min((float)1, (float)(getUsableHeight() / (float)deScale(getContentBoundsScaled().height)));

                vizElm_WIDGET_SCROLLBAR_Y.setRectRounded(
                    // scale(getUsableX(false) + getUsableWidth()) - getScaledPadding() * 2
                    scale(getX() + getWidth()) - getScaledPadding() - scale(10), // come back to this
                    std::max((float)scale(getUsableY(false)), (float)scale(getUsableY(false) + getScrollOffsetY() * -1 * scrollBarYPerc)),
                    scale(10),
                    scale(scrollBarYPerc * getUsableHeight()),
                    scale(5),
                    ofColor(scrollbarColor));

                // int scrollY = getPotentiallyStartedDraggingStartY() - ofGetMouseY();
                //  bool isScrollbarHovered = vizElm_WIDGET_SCROLLBAR_Y.isHovered_Rect();

                int scrollY = getPotentiallyStartedDraggingStartY() - (int)deScale(ofGetMouseY());

                // Show the scrollbar if the right edge of the widget is hovered - WIP
                bool isScrollbarHovered = !isDraggingContent() && (deScale(ofGetMouseX()) >= (getUsableX() + getUsableWidth() - deScale(vizElm_WIDGET_SCROLLBAR_Y.getRectangle().width)) &&
                                                                   deScale(ofGetMouseX()) <= (getUsableX() + getUsableWidth()) &&
                                                                   deScale(ofGetMouseY()) >= (getUsableY()) &&
                                                                   deScale(ofGetMouseY()) <= (getUsableY() + getUsableHeight()));

                bool isDraggingToScroll = false; // // TODO: ADD 2 FINGER SWIPE TO SCROLLE HERE OLD CODE:  getIsMousePressedOverWidget(true) && scrollY != 0;

                if (getIsScrollingY() || isScrollbarHovered || isDraggingToScroll)
                {
                    vizElm_WIDGET_SCROLLBAR_Y.draw();
                    if (isScrollbarHovered)
                        setIsScrollingY();

                    if ((!getIsScrollingYMouseDown() && ofGetMousePressed() && isScrollbarHovered) || isDraggingToScroll)
                    {
                        setIsScrollingYMouseDown(true);
                    }

                    if (getIsScrollingYMouseDown() && scrollY != 0)
                    {
                        ofMouseEventArgs args(ofMouseEventArgs::Type::Scrolled, 0, 0, OF_MOUSE_BUTTON_LEFT, 0);
                        args.scrollY = scrollY;
                        onCoreWidgetMouseScrolled(args);
                        // setPotentiallyStartedDraggingStartY(ofGetMouseY());
                        setPotentiallyStartedDraggingStartY((int)deScale(ofGetMouseY()));
                    }
                }
            }
        }

        virtual void onWidgetMouseScrolled(ofMouseEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle mouse released...
        }

        virtual void onCoreWidgetFileDragEvent(ofDragInfo &dragInfo)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            if (mDraggingFileIn)
            {
                onWidgetFileDragEvent(dragInfo);
                mDraggingFileIn = false;
            }
        }

        virtual void onWidgetFileDragEvent(ofDragInfo &dragInfo)
        {
            if (!getIsEnabled())
                return;
            // Implement any behaviour in derrived class which should handle mouse released...
            // ofSystemAlertDialog("widget " + getWidgetId() + " received file " + dragInfo.files[0]);

            // BUG : currently it's firing for all events!!!! - scope it to only the HOVERED widget

            /*
             if (isWidgetHovered() ) {  // and dragging file in
                 ofSystemAlertDialog("widget " + getWidgetId() + " received file ");
             }
             */
        }

        virtual void onCoreWindowMouseEntered(ofMouseEventArgs &e)
        {
            mDraggingFileIn = true;
            mIsWindowHovered = true;
        }

        virtual void onCoreWindowMouseExited(ofMouseEventArgs &e)
        {
            mIsWindowHovered = false;
        }

        virtual void onCoreWidgetMouseDragged(ofMouseEventArgs &e)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            if (getIsActiveWidget())
            {
                if (getIsEnabled())
                {
                    onWidgetMouseDragged(e);

                    cout << "B_DRAGIN'";
                    if (getExternalDrop().getIsDragging())
                    {
                        cout << "INSIDE_DRAGIN'";
                        getExternalDrop().handleExternalDragAndDropMotionNotify();
                        if (!ofGetMousePressed())
                            getExternalDrop().handleExternalDragAndDropButtonReleaseEvent();
                    }
                    cout << "A_DRAGIN'";
                }
            }
        }

        virtual void onWidgetMouseDragged(ofMouseEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle mouse dragged...
        }

        virtual void onCoreWidgetMouseContentDragged(ofMouseEventArgs &e)
        {
            // ONLY override this method if you want to override core widget beh\aviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...

            if (getIsActiveWidget())
            {
                // We want a small amount of inital mouse movement in order to assert it's a genuine drag opperation
                int mX = deScale(ofGetMouseX());
                int mY = deScale(ofGetMouseY());

                if (mX > mPotentiallyStartedDraggingStartX + mMinPixelsMovedToRegisterDrag ||
                    mX < mPotentiallyStartedDraggingStartX - mMinPixelsMovedToRegisterDrag ||
                    mY > mPotentiallyStartedDraggingStartY + mMinPixelsMovedToRegisterDrag ||
                    mY < mPotentiallyStartedDraggingStartY - mMinPixelsMovedToRegisterDrag)
                {

                    if (!isDraggingContent())
                    {
                        // Save the original states... so we can begin the drag operation
                        mTempIsDraggable = mIsDraggable;
                        mTempIsResizable = mIsResizable;

                        // Prevent the widget chrome from resizing/dragging when we're dragging content outside of it's bounds
                        mIsDraggable = false;
                        mIsResizable = false;
                        getViz()->setIsAnyWidgetDraggingOrResizing(true);
                        setIsDragging(true);
                        setRespondedToFirstMouseReleasedEvent(true);
                        mIsDraggingContent = true;
                    }

                    if (getIsEnabled())
                    {
                        onWidgetMouseContentDragged(e);

                        if (!getExternalDrop().getIsDragging())
                        {
                            // Initiate external Drag and Drop
                            vector<string> dragFiles = getFilesForExternalDropOperation(getDragData());
                            getExternalDrop().externalDragAndDropFileInit(dragFiles);
                        }
                    }
                }
            }
        }

        virtual vector<string> getFilesForExternalDropOperation(string dragData)
        {
            return vector<string>(); // default to no no files / nothing
        }

        virtual void onWidgetMouseContentDragged(ofMouseEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle mouse dragged...
        }

        virtual void onCoreWidgetKeyPressed(ofKeyEventArgs &e)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            if (getIsActiveWidget() && getIsEnabled())
            {
                mHeldDownKeys[e.key] = true;
                onWidgetKeyPressed(e);
            }
        }

        virtual void onWidgetKeyPressed(ofKeyEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle key pressed
            return;
        }

        virtual void onWidgetHovered(WidgetEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle mouse entered...
        }

        virtual void onWidgetUnhovered(WidgetEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle mouse exited...
        }

        virtual void onWidgetMouseOver(WidgetEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle mouse exited...
        }

        virtual void onCoreWidgetWidgetClicked(ofMouseEventArgs &e)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            if (getIsActiveWidget() && !getIsDragging() && !isDraggingContent())
            {
                if (getIsEnabled())
                    onWidgetClicked(e);
            }
        }

        virtual void onWidgetClicked(ofMouseEventArgs &e)
        {
            // Don't implement any base widget logic here, this event is for specific widget click events / functionality
        }

        virtual void onCoreWidgetKeyReleased(ofKeyEventArgs &e)
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            if (getIsActiveWidget() && getIsEnabled())
            {
                mHeldDownKeys[e.key] = false;
                onWidgetKeyReleased(e);
            }
        }

        //! Check is a certain key is currently held down
        bool isKeyPressed(int key)
        {
            if (key <= 255)
                return mHeldDownKeys[key];
            return false;
        }

        virtual void onWidgetKeyReleased(ofKeyEventArgs &e)
        {
            // Implement any behaviour in derrived class which should handle key pressed
        }

        shared_ptr<uiViz> getViz()
        {
            //ofLogNotice("ofxAquamarine") << "[" << getPersistentId() << "] getViz()";
            return viz;
        }

        int getZindex()
        {
            return mZIndex;
        }

        virtual void setZindex(int zIndex)
        {
            mZIndex = zIndex;
        }

        bool isTopLevel()
        {
            return getZindex() == 0;
        }

        void setDragData(string dragData)
        {
            // cout << "[" << getPersistentId() << "] SETTING DRAG DATA:\n" << mDragData;
            mDragData = dragData;
            eventTransmit(getPersistentId(), WIDGET_EVENT::DRAG_AND_DROP_DATA_SET, "<event>" + mDragData + "</event>", *this);
        }

        string getDragData()
        {
            return mDragData;
        }

        void clearDragData()
        {
            mDragData = "";
        }

        int getPotentiallyStartedDraggingStartX()
        {
            return mPotentiallyStartedDraggingStartX;
        }

        void setPotentiallyStartedDraggingStartX(int val)
        {
            mPotentiallyStartedDraggingStartX = val;
        }

        int getPotentiallyStartedDraggingStartY()
        {
            return mPotentiallyStartedDraggingStartY;
        }

        void setPotentiallyStartedDraggingStartY(int val)
        {
            mPotentiallyStartedDraggingStartY = val;
        }

        bool getDidMoveFocus()
        {
            return mDidMoveFocus;
        }

        void setDidMoveFocus(bool val)
        {
            mDidMoveFocus = val;
        }

        bool getIgnoreThemeChanges()
        {
            return mIgnoreThemeChanges;
        }

        void setIgnoreThemeChanges(bool val)
        {
            mIgnoreThemeChanges = val;
        }

        bool getIsSystemWidget()
        {
            return mIsSystemWidget;
        }

        void setIsSystemWidget(bool val)
        {
            mIsSystemWidget = val;
        }

        bool getIsVisualDebugMode()
        {
            if (Shared::getVizDebug() > -1)
            {
                return (Shared::getVizDebug() == 1);
            }
            return mIsVisualDebugMode;
        }

        void setIsVisualDebugMode(bool value)
        {
            mIsVisualDebugMode = value;
        }

        bool getUpdatedSinceUnhovered()
        {
            return mUpdatedSinceUnhovered;
        }

        void setUpdatedSinceUnhovered(bool val)
        {
            mUpdatedSinceUnhovered = val;
        }

        void setShowingAsPopout(bool val)
        {
            mShowingAsPopout = val;
        }

        bool getShowingAsPopout()
        {
            return mShowingAsPopout;
        }

        virtual bool hasActiveElement()
        {
            return false;
        }

        void setMetadata(string key, string val)
        {
            mMetadata[key] = val;
        }

        void setMetadata(string key, float val)
        {
            setMetadata(key, ofToString(val));
        }

        void setMetadata(string key, double val)
        {
            setMetadata(key, ofToString(val));
        }

        void setMetadata(string key, int val)
        {
            setMetadata(key, ofToString(val));
        }

        void setMetadata(string key, int64_t val)
        {
            setMetadata(key, ofToString(val));
        }

        void setMetadata(string key, bool val)
        {
            setMetadata(key, ofToString(val));
        }

        string getMetadata(string key)
        {
            std::map<string, string>::iterator it = mMetadata.find(key);
            if (it != mMetadata.end())
            {
                return it->second;
            }
            else
            {
                return "";
            }
        }

        float getMetadataFloat(string key)
        {
            return ofToFloat(getMetadata(key));
        }

        double getMetadataDouble(string key)
        {
            return ofToDouble(getMetadata(key));
        }

        int getMetadataInt(string key)
        {
            return ofToInt(getMetadata(key));
        }

        int64_t getMetadataInt64(string key)
        {
            return ofToInt64(getMetadata(key));
        }

        bool getMetadataBool(string key)
        {
            return ofToBool(getMetadata(key));
        }

        void deleteMetadata(string key)
        {
            std::map<string, string>::iterator it = mMetadata.find(key);
            if (it != mMetadata.end())
            {
                mMetadata.erase(it);
            }
        }

        void clearMetadata()
        {
            mMetadata.clear();
        }

        void setPopoutWidget(WidgetBase *w)
        {
            if (w)
                w->setPopoutOpenerWidget(this);
            mPopoutWidget = w;
        }

        WidgetBase *getPopoutWidget()
        {
            return mPopoutWidget;
        }

        WidgetBase *getPopoutOpenerWidget()
        {
            return mPopoutOpenerWidget;
        }

        void setPopoutOpenerWidget(WidgetBase *w)
        {
            mPopoutOpenerWidget = w;
        }

    protected:
        bool getX_ExprWasSetProgramatically()
        {
            return mX_ExprWasSetProgramatically;
        }

        void setX_ExprWasSetProgramatically(bool val)
        {
            mX_ExprWasSetProgramatically = val;
        }

        bool getY_ExprWasSetProgramatically()
        {
            return mY_ExprWasSetProgramatically;
        }

        void setY_ExprWasSetProgramatically(bool val)
        {
            mY_ExprWasSetProgramatically = val;
        }

        bool getWidth_ExprWasSetProgramatically()
        {
            return mWidth_ExprWasSetProgramatically;
        }

        void setWidth_ExprWasSetProgramatically(bool val)
        {
            mWidth_ExprWasSetProgramatically = val;
        }

        bool getHeight_ExprWasSetProgramatically()
        {
            return mHeight_ExprWasSetProgramatically;
        }

        void setHeight_ExprWasSetProgramatically(bool val)
        {
            mHeight_ExprWasSetProgramatically = val;
        }

        ofxExternalDrop &getExternalDrop()
        {
            if (!externalDropInitialized)
            {
                externalDrop = ofxExternalDrop(true);
                externalDropInitialized = true;
            }
            return externalDrop;
        }

        void clearExternalDrop()
        {
            externalDrop = ofxExternalDrop(false);
            externalDropInitialized = false;
        }

        bool getTransmitOscMessages()
        {
            return mTransmitOscMessages;
        }

        void setTransmitOscMessages(bool val)
        {
            mTransmitOscMessages = val;
        }

        PopoutDirection mPopoutDirection = PopoutDirection::NONE;
        Elm vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE;

    private:
        WidgetContext context;
        bool mHeldDownKeys[256];

        bool getIsChildOfModal(WidgetBase *widget)
        {
            WidgetBase *w = parent();
            while (w)
            {
                if (w->getWidgetId() == getViz()->getModalWidgetId())
                    return true;
                w = w->parent();
            }
            return getWidgetId() == getViz()->getModalWidgetId();
        }

        ofxExternalDrop externalDrop;
        bool externalDropInitialized = false;

        void init()
        {
            viz = Shared::getViz();
            ofAddListener(getViz()->scaleChanged, this, &WidgetBase::onCoreWidgetScaleChanged);
            ofAddListener(ofEvents().windowResized, this, &WidgetBase::onCoreWidgetWindowResized);
            ofAddListener(ofEvents().mouseMoved, this, &WidgetBase::onCoreWidgetMouseMoved);
            ofAddListener(ofEvents().mousePressed, this, &WidgetBase::onCoreWidgetMousePressed);
            ofAddListener(ofEvents().mouseReleased, this, &WidgetBase::onCoreWidgetMouseReleased);
            ofAddListener(ofEvents().mouseScrolled, this, &WidgetBase::onCoreWidgetMouseScrolled);
            ofAddListener(ofEvents().fileDragEvent, this, &WidgetBase::onCoreWidgetFileDragEvent);
            ofAddListener(ofEvents().mouseDragged, this, &WidgetBase::onCoreWidgetMouseDragged);
            ofAddListener(ofEvents().mouseEntered, this, &WidgetBase::onCoreWindowMouseEntered);
            ofAddListener(ofEvents().mouseExited, this, &WidgetBase::onCoreWindowMouseExited);
            ofAddListener(ofEvents().keyPressed, this, &WidgetBase::onCoreWidgetKeyPressed);
            ofAddListener(ofEvents().keyReleased, this, &WidgetBase::onCoreWidgetKeyReleased);

            // Touch events
            ofAddListener(ofEvents().touchDown, this, &WidgetBase::onCoreWidgetTouchDown);
            ofAddListener(ofEvents().touchUp, this, &WidgetBase::onCoreWidgetTouchUp);

            ofAddListener(widgetEventReceived, this, &WidgetBase::onCoreWidgetEventReceived);
            ofAddListener(widgetResized, this, &WidgetBase::onCoreWidgetResized);

            // Aspect ratio
            if (getHeight() > 0)
            {
                setAspectRatio(getWidth() / (float)getHeight());
            }
            else
            {
                setAspectRatio(1.0f);
            }

            for (int i = 0; i < 255; i++)
            {
                mHeldDownKeys[i] = false;
            }

            // ofxExternalDrop externalDrop = ofxExternalDrop();
        }

        // Lambda type event handlers
        std::function<void(ofMouseEventArgs &)> m_onCoreWidgetMousePressedFunction = [this](ofMouseEventArgs &) {};
        std::function<void(ofMouseEventArgs &)> m_onCoreWidgetMouseReleasedFunction = [this](ofMouseEventArgs &) {};

        widgetTelemetry mWidgetTelemetry;
        WidgetTheme mTheme = WidgetTheme();

        bool mIsPermanentWidget = false;
        bool mIsHighlighted = false;
        bool mShouldDeleteThisWidget = false;
        bool mShouldCloseThisWidget = false;
        bool mIsEnabled = true;
        bool mWasJustInitialized = true;
        WidgetBase *mTargetDropWidget = nullptr;
        bool mShouldShowClipboardForThisWidget = false;
        string mPersistentId = "";
        string mOwnerWidgetId = "";
        string mWidgetClassType = "";
        bool mShouldPersist = false;
        bool mShouldPersistTheme = false;
        bool mNeedsUpdate = true;
        bool mWidgetUINeedsUpdate = true;
        int mX = 0;
        int mY = 0;
        int mWidth = 100;
        int mHeight = 100;
        bool mIsAutoResizeToContentsWidth = false;
        bool mIsAutoResizeToContentsHeight = false;
        int mScreenPadding = 0;

        string mX_Expr = "";
        string mY_Expr = "";
        string mWidth_Expr = "";
        string mHeight_Expr = "";
        bool mIgnoreExpr_Layout = false;
        bool mExpressionRecalculationRequired = true;
        bool mX_ExprWasSetProgramatically = false;
        bool mY_ExprWasSetProgramatically = false;
        bool mWidth_ExprWasSetProgramatically = false;
        bool mHeight_ExprWasSetProgramatically = false;
        bool mUpdatedSinceUnhovered = true;
        bool mShowingAsPopout = false;

        WidgetBase *mPopoutFromWidget = nullptr;
        int mPopoutX = 0;
        int mPopoutY = 0;
        int mRawPopoutX = 0;
        int mRawPopoutY = 0;
        int mPopoutOffset = 15;

        int mMinWidth = 75;
        int mMinHeight = 75;
        int mMaxWidth = ofGetWindowWidth() * 10;
        int mMaxHeight = ofGetWindowHeight() * 10;
        bool mIsRoundedRectangle = true;
        bool mIsShadow = true;
        string mTitle = "";
        int mTitleWidth = 150;
        float mScrollOffsetX = 0.0f;
        float mScrollOffsetY = 0.0f;
        float mScrollScaleX = 10.0f;
        float mScrollScaleY = 10.0f;
        bool mIsVisible = true;
        Coord::vizBounds mContentBoundsScaled = Coord::vizBounds(0, 0, 0, 0);

        ScrollAction mScrollAction = ScrollAction::ZOOM;
        CloseAction mCloseAction = CloseAction::REMOVE;

        bool mEnableScrollXPositive = false;
        bool mEnableScrollXNegative = false;
        bool mEnableScrollYPositive = false;
        bool mEnableScrollYNegative = false;

        uint64_t mElapsedTimerMillis = 0;

        int mScrollingStartedMS_Y = 0;
        int mScrollingStartedMS_X = 0;
        bool mIsScrollingXMouseDown = false;
        bool mIsScrollingYMouseDown = false;

        bool mLockAspectRatio = false;
        float mAspectRatio = 1.0f;

        bool mIsDraggable = true;
        bool mIsResizable = true;

        bool mIsDragging = false;
        bool mIsResizing = false;
        bool mIsAutoUpdateWhenActive = true;
        bool mIsUnhoverLoseFocus = true;
        bool mShouldCloseIfOtherWidgetClicked = false;
        bool mUseFbo = true;

        bool mRespondedToFirstMousePressedEvent = false;
        bool mRespondedToFirstMouseReleasedEvent = false;
        bool mMousePressWasRegisteredWhenCheckingForReleased = false;

        shared_ptr<uiViz> viz;
        int mZIndex = 0;
        bool mIsNextInLineForFocus = true;
        bool mIsDraggingContent = false;
        int mPotentiallyStartedDraggingStartX = 0;
        int mPotentiallyStartedDraggingStartY = 0;
        int mMinPixelsMovedToRegisterDrag = 20;
        Coord::vizPoint mLastMouseClickCoords = Coord::vizPoint(0, 0);

        bool mTempIsDraggable = true;
        bool mTempIsResizable = true;

        bool mDraggingFileIn = false;
        bool mIsWindowHovered = true;

        string mDragData = "";
        bool mTransmitOscMessages = true;
        ofFbo mContentDragIconFbo;

        // vector<TargetWidgetAction> mTargetWidgetActions;
        // std::vector<std::reference_wrapper<WidgetBase>> mTargetWidgets;

        bool mDidMoveFocus = false;
        bool mIgnoreThemeChanges = false;
        bool mIsSystemWidget = false;
        bool mIsVisualDebugMode = false;

        Elm vizElm_WIDGET_SCROLLBAR_X;
        Elm vizElm_WIDGET_SCROLLBAR_Y;

        map<string, string> mMetadata;
        std::function<void(WidgetBase *sender, WidgetBase *closedWidget, const string &)> mWidgetClosedCallback = [](WidgetBase *sender, WidgetBase *closedWidget, string result) {};
        WidgetBase *mPopoutWidget = nullptr;
        WidgetBase *mPopoutOpenerWidget = nullptr;
    };

    //------------------------------ WidgetElm : derive all widget elements from this --------------------------------
    class WidgetElm : public WidgetBase
    {

    public:
        WidgetElm(string persistentId, string widgetXML, WidgetBase *parent) : WidgetBase(persistentId, widgetXML)
        {
            init();
            mParent = parent; // The hosting widget (not necessarily Widget, could be anything derrived from WidgetBase
            loadState(widgetXML);
        }

        int getRelativeX()
        {
            return WidgetBase::getX();
        }

        int getRelativeY()
        {
            return WidgetBase::getY();
        }

        virtual int getX() override
        {
            return (parent() != NULL && parent() != nullptr) ? parent()->getX() + WidgetBase::getX() : WidgetBase::getX();
        }

        virtual int getY() override
        {
            return (parent() != NULL && parent() != nullptr) ? parent()->getY() + WidgetBase::getY() : WidgetBase::getY();
        }

        virtual int getUsableX() override
        {
            return (parent() != NULL && parent() != nullptr) ? getNonScaledPadding() + WidgetBase::getX() + parent()->getX() + getScrollOffsetX() : getNonScaledPadding() + WidgetBase::getX() + getScrollOffsetX();
        }

        virtual int getUsableY() override
        {
            return (parent() != NULL && parent() != nullptr) ? getNonScaledPadding() + WidgetBase::getY() + parent()->getY() + getScrollOffsetY() : getNonScaledPadding() + WidgetBase::getY() + getScrollOffsetY();
        }

        virtual void setX(int x) override
        {
            if (WidgetBase::getX() != x)
            {
                setWidgetNeedsUpdate(true);
                setNeedsUpdate(true);
                setShouldRedetermineElementVisibility(true);
            }
            WidgetBase::setX(x);
        }

        virtual void setY(int y) override
        {
            if (WidgetBase::getY() != y)
            {
                setWidgetNeedsUpdate(true);
                setNeedsUpdate(true);
                setShouldRedetermineElementVisibility(true);
            }
            WidgetBase::setY(y);
        }

        bool isInsideWidgetBounds()
        {
            bool insideX = ((getX() + getWidth()) >= parent()->getX() && (getX()) < (parent()->getX() + parent()->getWidth()));
            bool insideY = ((getY() + getHeight()) >= parent()->getY() && (getY()) < (parent()->getY() + parent()->getHeight()));

            if (!insideX || !insideY)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        void determineElementVisibility()
        {
            if (!mShouldRedetermineElementVisibility || (parent() && !parent()->getIsVisible()))
            {
                return;
            }
            else
            {
                mShouldRedetermineElementVisibility = false;
            }

            if (!mOutOfBounds_ElementWasHidden)
                mOutOfBounds_ElementWasOriginallyVisible = getIsVisible();
            if (!isInsideWidgetBounds())
            {
                // mOutOfBounds_ElementWasOriginallyVisible = getIsVisible();
                setIsVisible(false);
                mOutOfBounds_ElementWasHidden = true;
            }
            else
            {
                if (mOutOfBounds_ElementWasOriginallyVisible && mOutOfBounds_ElementWasHidden)
                {
                    setIsVisible(true);
                    mOutOfBounds_ElementWasHidden = true;
                }
            }
        }

        bool getShouldRedetermineElementVisibility()
        {
            return mShouldRedetermineElementVisibility;
        }

        void setShouldRedetermineElementVisibility(bool val)
        {
            mShouldRedetermineElementVisibility = val;
        }

        virtual void setWidth(int width) override
        {
            if (WidgetBase::getWidth() != width)
            {
                mAllocatedSinceLastResize = false;
                setWidgetNeedsUpdate(true);
                setNeedsUpdate(true);
            }
            WidgetBase::setWidth(width);
        }

        virtual void setHeight(int height) override
        {
            if (WidgetBase::getHeight() != height)
            {
                mAllocatedSinceLastResize = false;
                setWidgetNeedsUpdate(true);
                setNeedsUpdate(true);
            }
            WidgetBase::setHeight(height);
        }

        float getCurrentForegroundAlpha()
        {
            return mCurrentForegroundAlpha;
        }

        float getCurrentBackgroundAlpha()
        {
            return mCurrentBackgroundAlpha;
        }

        virtual void setTheme(WidgetTheme val) override
        {
            if (getIgnoreThemeChanges())
                return;
            WidgetBase::setTheme(val);
            WidgetTheme theme = getTheme();
            setIsRoundedRectangle(theme.ElementRoundedRectangle);
        }

        virtual void onWidgetLoaded() override
        {
            // Implement any behaviour for when a widget has loaded
        }

        virtual bool loadState(string widgetXML) override
        {
            WidgetBase::loadState(widgetXML, "element");
            ofxXmlSettings mWidgetXML = ofxXmlSettings();

#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
            cout << "[" << getPersistentId() << "]WidgetElm->loadState()\n";
#endif

            if (!mWidgetXML.loadFromBuffer(widgetXML.c_str()))
            {
#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
                cout << "[" << getPersistentId() << "]WidgetElm->Unable to load XML\n"
                     << widgetXML.c_str() << "\n";
#endif
                return false;
            }

            mWidgetXML.pushTag("element");

            // Get the x,y values. If not found set to zero (top left of host widget)
            if (parent() != NULL && parent() != nullptr)
            {
                if (getX_Expr() == "")
                {
                    setX(mWidgetXML.getAttribute("bounds", "x", 0));
                }
                if (getY_Expr() == "")
                {
                    setY(mWidgetXML.getAttribute("bounds", "y", 0));
                }
            }

            WidgetTheme theme = (parent() ? parent()->getTheme() : getTheme());

            if (mWidgetXML.tagExists("appearance"))
            {
                theme.loadForElement(theme, widgetXML);
            }
            mWidgetXML.popTag(); // element

            onWidgetLoaded();

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings mWidgetXML;
            if (!getShouldPersist())
                return mWidgetXML;

            mWidgetXML = WidgetBase::saveState("element");
            while (mWidgetXML.popTag() > 0)
                ; // pop to top!
            mWidgetXML.setAttribute("element", "class", "WidgetElm", 0);
            mWidgetXML.pushTag("element");

            // Store the relative x,y values NOT absolute!
            if (getX_Expr() == "")
            {
                mWidgetXML.setAttribute("bounds", "x", getRelativeX(), 0);
            }
            if (getY_Expr() == "")
            {
                mWidgetXML.setAttribute("bounds", "y", getRelativeY(), 0);
            }

            if (getShouldPersistTheme())
            {
                mWidgetXML.pushTag("appearance");
                mWidgetXML.addTag("foregroundColor");
                mWidgetXML.setAttribute("foregroundColor", "color", getTheme().getElementForegroundColorHex(), 0);

                mWidgetXML.addTag("foregroundAlpha");
                mWidgetXML.setAttribute("foregroundAlpha", "hovered", getTheme().ElementHoveredForegroundAlpha, 1);
                mWidgetXML.setAttribute("foregroundAlpha", "unhovered", getTheme().ElementUnhoveredForegroundAlpha, 1);

                mWidgetXML.addTag("backgroundColor");
                mWidgetXML.setAttribute("backgroundColor", "color", getTheme().getElementBackgroundColorHex(), 0);

                mWidgetXML.addTag("backgroundAlpha");
                mWidgetXML.setAttribute("backgroundAlpha", "hovered", getTheme().ElementHoveredBackgroundAlpha, 1);
                mWidgetXML.setAttribute("backgroundAlpha", "unhovered", getTheme().ElementUnhoveredBackgroundAlpha, 1);

                mWidgetXML.popTag(); // appearance
            }

            mWidgetXML.popTag(); // element

            return mWidgetXML;
        }

        virtual void update(WidgetContext context) override
        {
            // Implemented by descendant class...
        }

        bool isActiveOrParentActive()
        {
            if (parent())
            {
                return (parent()->getWidgetId() == getActiveWidgetId() || getWidgetId() == getActiveWidgetId());
            }
            else
            {
                return (getWidgetId() == getActiveWidgetId());
            };
        }

        bool isParentActive()
        {
            if (parent())
            {
                return (parent()->getWidgetId() == getActiveWidgetId());
            }
            return false;
        }

        bool isRootActive()
        {
            if (root())
            {
                return (root()->getWidgetId() == getActiveWidgetId());
            }
            return false;
        }

        virtual void onCoreWidgetKeyPressed(ofKeyEventArgs &e) override
        {
            // ONLY override this method if you want to override core widget behaviour. Use "onWidget$$$$" methods in descendant classes
            //  to respond to this event and prevent modifying how core widget functionality works...
            if (parent())
            {
                if (!parent()->getIsActiveWidget() && !parent()->hasActiveElement())
                    return;
                if (getIsFocused(false) && getIsEnabled())
                    onWidgetKeyPressed(e);

                if (!parent()->getDidMoveFocus() && e.key == OF_KEY_TAB)
                {
                    parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::FOCUS_NEXT_ELEMENT, "<event></event>", *this);
                    return;
                }
            }
        }

        virtual void onCoreWidgetMouseReleased(ofMouseEventArgs &e) override
        {
            mIsElmDragging = false;
            WidgetBase::onCoreWidgetMouseReleased(e);
        }

        virtual void onCoreWidgetMousePressed(ofMouseEventArgs &e) override
        {
            if (e.button == OF_MOUSE_BUTTON_LEFT && isWidgetHovered() && getIsActiveWidget())
            {
                mIsElmDragging = true;
            }

            WidgetBase::onCoreWidgetMousePressed(e);
        }

        virtual void onCoreWidgetMouseDragged(ofMouseEventArgs &e) override
        {
            // The below condition caters for the situation where a user began dragging on an elm quickly
            // and the mouse is no longer over the elm, but the mouse button is still down!
            if (getIsElmDragging())
            {
                if (getIsEnabled())
                    onWidgetMouseDragged(e);
            }
            else
            {
                WidgetBase::onCoreWidgetMouseDragged(e);
            }
        }

        virtual void onCoreWidgetEventReceived(WidgetEventArgs &args) override
        {
            if (!getIsEnabled())
                return;
            if (args.eventName == WIDGET_EVENT::CLIPBOARD_PASTE)
            {
                setWidgetNeedsUpdate(true);
                setNeedsUpdate(true);
            }
        }

        virtual void onAllSiblingElementsLoaded()
        {
        }

        void handleDrawEvents()
        {
            WidgetEventArgs args = WidgetEventArgs(getWidgetId(), *this);
            bool hovered = isWidgetHovered();

            if (hovered)
            {
                setTargetDropWidgetId();
            }

            if ((hovered) && !getIsDragging() && !getIsResizing() && !getViz()->getIsAnyWidgetDraggingOrResizing())
            {
                if (hovered)
                {
                    ofNotifyEvent(widgetHovered, args);
                }

                if (((isTopLevel() || getActiveWidgetId() == "")))
                {
                    // if (ofGetMousePressed()) ofNotifyEvent(widgetClicked, args);
                    // if (!getIsActiveWidget()) setActiveWidget();

                    if (!getIsActiveWidget())
                    {
                        setActiveWidget();
                        onWidgetHovered(args);
                    }
                    onWidgetMouseOver(args);
                    if (ofGetMousePressed())
                    {
                        ofNotifyEvent(widgetClicked, args);
                    }

                } // getValue() setValue(), settings and send feedback (widget with a menu on the left)
            }
            else if (!getViz()->getIsAnyWidgetDraggingOrResizing() && getIsActiveWidget())
            {
                clearTargetDropWidgetId();

                ofNotifyEvent(widgetUnhovered, args);
                onWidgetUnhovered(args);
                if (ofGetMousePressed())
                {
                    ofNotifyEvent(widgetLostFocus, args);
                    clearActiveWidgetId();
                }
                else if (getIsUnhoverLoseFocus())
                {
                    clearActiveWidgetId();
                }
            }
            else
            {
                // This is an inactive widget
                if (!getIsActiveWidget() && getShouldCloseIfOtherWidgetClicked() && ofGetMousePressed() && !getIsDragging() && !getIsResizing())
                {
                    closeThisWidget("CLOSED_DEFOCUSED");
                }
            }
        }

        void coreDraw()
        {

            if (!getIsVisible())
                return;

            getWidgetTelemetry()->startDraw(); //////////////////////////////////////////////////////////

            handleDrawEvents();

            if ((getIsActiveWidget() || getIsFocused(true)) && getIsEnabled())
            {
                if (mCurrentForegroundAlpha != getTheme().ElementHoveredForegroundAlpha)
                {
                    mCurrentForegroundAlpha = scaleAnimation(getWidgetId() + "_foreground_hovered", mCurrentForegroundAlpha, getTheme().ElementHoveredForegroundAlpha, 0.6f);
                }
                if (mCurrentBackgroundAlpha != getTheme().ElementHoveredBackgroundAlpha)
                {
                    mCurrentBackgroundAlpha = scaleAnimation(getWidgetId() + "_background_hovered", mCurrentBackgroundAlpha, getTheme().ElementHoveredBackgroundAlpha, 0.6f);
                }
            }
            else
            {
                if (mCurrentForegroundAlpha != getTheme().ElementUnhoveredForegroundAlpha)
                {
                    mCurrentForegroundAlpha = scaleAnimation(getWidgetId() + "_foreground_unhovered", mCurrentForegroundAlpha, getTheme().ElementUnhoveredForegroundAlpha, 0.6f);
                }
                if (mCurrentBackgroundAlpha != getTheme().ElementUnhoveredBackgroundAlpha)
                {
                    mCurrentBackgroundAlpha = scaleAnimation(getWidgetId() + "_background_unhovered", mCurrentBackgroundAlpha, getTheme().ElementUnhoveredBackgroundAlpha, 0.6f);
                }
            }

            allocateFbo();
            fboBegin();
            draw(this->context);
            fboEnd();
            fboDraw();

            if (parent() && parent()->getIsVisualDebugMode())
                WidgetElm::draw(this->context);

            if (getIsActiveWidget() && getScrollAction() == ScrollAction::SCROLL)
            {
                coreDrawWidgetScrollbars(getTheme().WidgetAccentHighContrast1Color_withAlpha(0.5f));
            }

            this->context.isInitialDraw = false;
            getWidgetTelemetry()->endDraw(); //////////////////////////////////////////////////////////
        }

        bool isTransitioningAlpha() override
        {
            if (!getIsVisible())
                return false;
            if (getIsActiveWidget())
            {
                return mCurrentForegroundAlpha != getTheme().ElementHoveredForegroundAlpha || mCurrentBackgroundAlpha != getTheme().ElementHoveredBackgroundAlpha;
            }
            else
            {
                return mCurrentForegroundAlpha != getTheme().ElementUnhoveredForegroundAlpha || mCurrentBackgroundAlpha != getTheme().ElementUnhoveredBackgroundAlpha;
            }
        }

        virtual void setNeedsUpdate(bool needsUpdate) override
        {
            WidgetBase::setNeedsUpdate(needsUpdate);

            WidgetBase *p = parent();

            if (p)
            {
                p->setNeedsUpdate(needsUpdate);
            }
        }

        virtual void draw(WidgetContext context) override
        {
            // Implemented by descendant class...

            int elmIndex = parent() ? parent()->getWidgetElementIndex(getPersistentId()) : 0;
            int hueAngle = (parent() ? 255.0f * ((float)elmIndex) / max(1, (int)parent()->getWidgetElementSize()) : 0);
            ofColor color = ofColor::fromHsb(hueAngle, 255, getTheme().IsDark ? 200 : 100);
            ofColor color2 = ofColor(color.r, color.g, color.b, 200);

            Elm vizElm_BLANK;
            vizElm_BLANK.setRectangle(scale(getX()), scale(getY()), scale(getWidth()), scale(getHeight()));
            vizElm_BLANK.setStroke(scale(2), color);
            vizElm_BLANK.draw();

            Elm vizElm_BLANK2;
            vizElm_BLANK2.setRectangle(scale(getX()), scale(getY()), scale(getWidth()), scale(getHeight()), color2);
            vizElm_BLANK2.draw();

            Elm errRect;
            ofRectangle errRectOf = titleFont->rect(ofToString(elmIndex) + ":" + getPersistentId());
            errRect.setRectangle(errRectOf.getX(), errRectOf.getY(), errRectOf.getWidth(), errRectOf.getHeight());

            ofPushStyle();
            ofSetColor(getTheme().IsDark ? ofColor::black : ofColor::white);
            titleFont->draw(ofToString(elmIndex) + ":" + getPersistentId(),
                            getViz()->scale(getX() + (getWidth() / 2.0f)) - errRect.getRectangle().width / 2.0f,
                            getViz()->scale(getY() + (getHeight() / 2.0f)) + errRect.getRectangle().height / 2.0f);
            ofPopStyle();
        }

        virtual ~WidgetElm()
        {

#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
            cout << "[" << getPersistentId() << "]~WidgetElm()\n";
#endif
        }

        virtual WidgetBase *parent() override
        {
            return mParent;
        }

        bool getIsFocused(bool parentMustBeHovered)
        {
            if (!getIsVisible())
                return false;
            return (mIsFocused && !parentMustBeHovered) || (mIsFocused && parentMustBeHovered && parent() && parent()->isWidgetHovered());
        }

        void setIsFocused(bool val)
        {
            mIsFocused = val;
        }

        bool getIsSiblingFocused()
        {
            return mIsSiblingFocused;
        }

        void setIsSiblingFocused(bool val)
        {
            mIsSiblingFocused = val;
        }

        bool getIsElmDragging()
        {
            return mIsElmDragging;
        }

        bool getAllSiblingElementsLoaded()
        {
            return mAllSiblingElementsLoaded;
        }

        void setAllSiblingElementsLoaded(bool val)
        {
            mAllSiblingElementsLoaded = val;
        }

        void allocateFbo()
        {
            if (!mAllocatedSinceLastResize)
            {

#ifdef TARGET_OPENGLES
                fbo.allocate(scale(getWidth()), scale(getHeight()), GL_RGBA);
#else
                fbo.allocate(scale(getWidth()), scale(getHeight()), GL_RGBA, 16); // GL_RGBA32F, 32);
#endif

                fbo.begin();
                ofClear(255, 255, 255);
                fbo.end();
                mAllocatedSinceLastResize = true;
            }
        }
        void fboBegin()
        {
            fbo.begin();
            ofClear(0, 0, 0, 0);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            ofPushMatrix();
            ofTranslate(-1 * scale(getX()), -1 * scale(getY()));
            ofPushStyle();
        }

        void fboEnd()
        {
            ofPopStyle();
            ofPopMatrix();
            fbo.end();
        }

        void fboDraw()
        {
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

#ifdef TARGET_OPENGLES
            // fbo.draw(scale(getX()), scale(getY()), scale(-1*getWidth()), scale(-1*getHeight()));
            fbo.draw(1.5f * scale(getX()), -1.5f * scale(getY()), scale(getWidth()), -1.5f * scale(getHeight()));
#else
            fbo.draw(scale(getX()), scale(getY()), scale(getWidth()), scale(getHeight()));
#endif

            ofEnableAlphaBlending();
        }

        void setPopoutWidget(WidgetBase *w, PopoutDirection popoutDirection, WidgetTheme theme)
        {
            setPopoutWidget(w, popoutDirection, theme, true);
        }

        void setPopoutWidget(WidgetBase *w, PopoutDirection popoutDirection)
        {
            setPopoutWidget(w, popoutDirection, WidgetTheme(), false);
        }

        void setPopoutWidgetIgnoreTheme(WidgetBase *w, PopoutDirection popoutDirection)
        {
            setPopoutWidget(w, popoutDirection, WidgetTheme(), true);
        }

        void setPopoutWidgetShownCallback(std::function<void()> popoutShownCallback)
        {
            mPopoutShownCallback = popoutShownCallback;
        }

        void setPopoutWidgetClosedCallback(std::function<void(WidgetBase *sender, WidgetBase *closedWidget, const string &)> popoutClosedCallback)
        {
            if (getPopoutWidget())
            {
                getPopoutWidget()->setWidgetClosedCallback(popoutClosedCallback);
            }
        }

        void showPopoutWidgetIfSet()
        {
            if (getPopoutWidget())
            {
                if (mThemeWasSetForPopout)
                {
                    getPopoutWidget()->popoutFrom(this, mPopoutDirection, true);
                }
                else
                {
                    getPopoutWidget()->popoutFrom(this, mPopoutDirection);
                }
                setActiveWidget();
                mPopoutShownCallback(); // call it...
            }
        }

    private:
        WidgetContext context;

        WidgetBase *mParent;
        shared_ptr<ofxSmartFont> titleFont;
        bool mIsElmDragging = false;
        bool mShouldRedetermineElementVisibility = true;
        bool mOutOfBounds_ElementWasHidden = false;
        bool mOutOfBounds_ElementWasOriginallyVisible = true;

        float mCurrentForegroundAlpha = 0.80f;
        float mCurrentBackgroundAlpha = 0.70f;

        bool mIsFocused = false;
        bool mIsSiblingFocused = false;
        bool mAllSiblingElementsLoaded = false;
        ofFbo fbo;
        bool mAllocatedSinceLastResize = false;

        // Popout stuff
        PopoutDirection mPopoutDirection = PopoutDirection::DOWN;
        bool mThemeWasSetForPopout = false;

        std::function<void()> mPopoutShownCallback = []() {};

        void init()
        {
            titleFont = (getViz()->getScale() < 1) ? getViz()->getSmallFont() : getViz()->getMediumFont();
            WidgetBase::setWidth(35);
            WidgetBase::setHeight(30);
            setIsResizing(false);
            setIsDragging(false);
            setIsResizable(false);
            setIsDraggable(false);
            setScrollAction(ScrollAction::NONE);
        }

        void setPopoutWidget(WidgetBase *w, PopoutDirection popoutDirection, WidgetTheme theme, bool themeWasSet)
        {
            if (w && w != nullptr)
            {
                mThemeWasSetForPopout = themeWasSet;
                mPopoutDirection = popoutDirection;
                WidgetBase::setPopoutWidget(w);
                w->setIsVisible(false);
            }
        }
    };

#include "element/WidgetElmTextbox.h"
#include "element/WidgetElmTextarea.h"
#include "element/WidgetElmLabel.h"
#include "element/WidgetElmButton.h"
#include "element/WidgetElmSlider.h"
#include "element/WidgetElmCheckbox.h"
#include "element/WidgetElmDropdown.h"
#include "element/WidgetElmBreadcrumb.h"
#include "element/WidgetElmImage.h"
#include "element/WidgetElmProgressBar.h"

    using Aquamarine::WidgetElmTextbox;
    using Aquamarine::WidgetElmTextarea;
    using Aquamarine::WidgetElmLabel;
    using Aquamarine::WidgetElmButton;
    using Aquamarine::WidgetElmSlider;
    using Aquamarine::WidgetElmCheckbox;
    using Aquamarine::WidgetElmDropdown;
    using Aquamarine::WidgetElmBreadcrumb;
    using Aquamarine::WidgetElmImage;
    using Aquamarine::WidgetElmProgressBar;

    template <typename T>
    WidgetElm *createElement(string persistentId, string elementXML, WidgetBase *parent) { return new T(persistentId, elementXML, parent); }
    typedef std::map<std::string, WidgetElm *(*)(string persistentId, string elementXML, WidgetBase *parent)> element_map_type;

    //------------------------------ Widget : derive all widgets which require rich interactivity --------------------------------
    class Widget : public WidgetBase
    {

    public:
        element_map_type elmClassMap;

        void mapElements()
        {
            elmClassMap[WIDGET_ELM_CLASS::TEXTBOX] = &createElement<WidgetElmTextbox>;
            elmClassMap[WIDGET_ELM_CLASS::TEXTAREA] = &createElement<WidgetElmTextarea>;
            elmClassMap[WIDGET_ELM_CLASS::LABEL] = &createElement<WidgetElmLabel>;
            elmClassMap[WIDGET_ELM_CLASS::BUTTON] = &createElement<WidgetElmButton>;
            elmClassMap[WIDGET_ELM_CLASS::SLIDER] = &createElement<WidgetElmSlider>;
            elmClassMap[WIDGET_ELM_CLASS::CHECKBOX] = &createElement<WidgetElmCheckbox>;
            elmClassMap[WIDGET_ELM_CLASS::DROPDOWN] = &createElement<WidgetElmDropdown>;
            elmClassMap[WIDGET_ELM_CLASS::BREADCRUMB] = &createElement<WidgetElmBreadcrumb>;
            elmClassMap[WIDGET_ELM_CLASS::IMAGE] = &createElement<WidgetElmImage>;
            elmClassMap[WIDGET_ELM_CLASS::PROGRESS_BAR] = &createElement<WidgetElmProgressBar>;
        }

        enum class TitleStyle
        { /* C++ 11 */
          NONE,
          TOP_STATIC,
          TOP_OVERLAY,
          TOP_OVERLAY_FULL,
          TOP_WITH_WIDGET_RESIZE,
          BOTTOM_STATIC,
          BOTTOM_OVERLAY,
          BOTTOM_OVERLAY_FULL,
          BOTTOM_WITH_WIDGET_RESIZE,
          LEFT_STATIC,
          LEFT_OVERLAY,
          LEFT_OVERLAY_FULL,
          LEFT_WITH_WIDGET_RESIZE,
          RIGHT_STATIC,
          RIGHT_OVERLAY,
          RIGHT_OVERLAY_FULL,
          RIGHT_WITH_WIDGET_RESIZE
        };

        string getTitleStyleString(TitleStyle val)
        {
            switch (val)
            {
            case TitleStyle::NONE:
                return "NONE";
            case TitleStyle::TOP_STATIC:
                return "TOP_STATIC";
            case TitleStyle::TOP_OVERLAY:
                return "TOP_OVERLAY";
            case TitleStyle::TOP_OVERLAY_FULL:
                return "TOP_OVERLAY_FULL";
            case TitleStyle::TOP_WITH_WIDGET_RESIZE:
                return "TOP_WITH_WIDGET_RESIZE";
            case TitleStyle::BOTTOM_STATIC:
                return "BOTTOM_STATIC";
            case TitleStyle::BOTTOM_OVERLAY:
                return "BOTTOM_OVERLAY";
            case TitleStyle::BOTTOM_OVERLAY_FULL:
                return "BOTTOM_OVERLAY_FULL";
            case TitleStyle::BOTTOM_WITH_WIDGET_RESIZE:
                return "BOTTOM_WITH_WIDGET_RESIZE";
            case TitleStyle::LEFT_STATIC:
                return "LEFT_STATIC";
            case TitleStyle::LEFT_OVERLAY:
                return "LEFT_OVERLAY";
            case TitleStyle::LEFT_OVERLAY_FULL:
                return "LEFT_OVERLAY_FULL";
            case TitleStyle::LEFT_WITH_WIDGET_RESIZE:
                return "LEFT_WITH_WIDGET_RESIZE";
            case TitleStyle::RIGHT_STATIC:
                return "RIGHT_STATIC";
            case TitleStyle::RIGHT_OVERLAY:
                return "RIGHT_OVERLAY";
            case TitleStyle::RIGHT_OVERLAY_FULL:
                return "RIGHT_OVERLAY_FULL";
            case TitleStyle::RIGHT_WITH_WIDGET_RESIZE:
                return "RIGHT_WITH_WIDGET_RESIZE";
            default:
                return "NONE";
            }
        };

        TitleStyle getTitleStyleEnum(string val)
        {
            if (val == "NONE")
                return TitleStyle::NONE;
            if (val == "TOP_STATIC")
                return TitleStyle::TOP_STATIC;
            if (val == "TOP_OVERLAY")
                return TitleStyle::TOP_OVERLAY;
            if (val == "TOP_OVERLAY_FULL")
                return TitleStyle::TOP_OVERLAY_FULL;
            if (val == "TOP_WITH_WIDGET_RESIZE")
                return TitleStyle::TOP_WITH_WIDGET_RESIZE;
            if (val == "BOTTOM_STATIC")
                return TitleStyle::BOTTOM_STATIC;
            if (val == "BOTTOM_OVERLAY")
                return TitleStyle::BOTTOM_OVERLAY;
            if (val == "BOTTOM_OVERLAY_FULL")
                return TitleStyle::BOTTOM_OVERLAY_FULL;
            if (val == "BOTTOM_WITH_WIDGET_RESIZE")
                return TitleStyle::BOTTOM_WITH_WIDGET_RESIZE;
            if (val == "LEFT_STATIC")
                return TitleStyle::LEFT_STATIC;
            if (val == "LEFT_OVERLAY")
                return TitleStyle::LEFT_OVERLAY;
            if (val == "LEFT_OVERLAY_FULL")
                return TitleStyle::LEFT_OVERLAY_FULL;
            if (val == "LEFT_WITH_WIDGET_RESIZE")
                return TitleStyle::LEFT_WITH_WIDGET_RESIZE;
            if (val == "RIGHT_STATIC")
                return TitleStyle::RIGHT_STATIC;
            if (val == "RIGHT_OVERLAY")
                return TitleStyle::RIGHT_OVERLAY;
            if (val == "RIGHT_OVERLAY_FULL")
                return TitleStyle::RIGHT_OVERLAY_FULL;
            if (val == "RIGHT_WITH_WIDGET_RESIZE")
                return TitleStyle::RIGHT_WITH_WIDGET_RESIZE;
            return TitleStyle::NONE;
        };

        enum class AdditionalState
        {
            NONE,
            SELECTED
        };

        enum class FboMode
        {
            LOW,
            MEDIUM,
            HIGH,
            ULTRA
        };

        Widget(string persistentId, string widgetXML) : WidgetBase(persistentId, widgetXML)
        {
            init();
            loadState(widgetXML);
        }

        Widget(string persistentId, string widgetXML, string ownerWidgetId) : WidgetBase(persistentId, widgetXML, ownerWidgetId)
        {
            init();
            loadState(widgetXML);
        }

        virtual ~Widget()
        {
            // NOTE ON VIRTUAL DESTRUCTOR : For this exampe: Base *myObj = new Derived(); delete myObj; If your derived class destructor is NOT virtual then
            // only base class object will get deleted (because pointer is of base class "Base *myObj"). So there will be memory leak for derived object.

#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
            cout << "[" << getPersistentId() << "]~Widget()->clearChildWidgets(); /************************/ \n";
#endif
            clearChildWidgets();

#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
            cout << "[" << getPersistentId() << "]~Widget()->clearWidgetElements();  /************************/ \n";
#endif
            clearWidgetElements();
        }

        virtual void setTheme(WidgetTheme val) override
        {
            if (getIgnoreThemeChanges())
                return;
            WidgetBase::setTheme(val);
            WidgetTheme theme = getTheme();

            setIsRoundedRectangle(theme.RoundedRectangle);

            for (WidgetElm &elm : mWidgetElements)
            {
                elm.setTheme(val);
            }

            for (Widget &childWidget : getChildWidgets())
            {
                childWidget.setTheme(val); // this will set child elements
            }
        }

        virtual void setIsHighlighted(bool val) override
        {
            WidgetBase::setIsHighlighted(val);

            if (val)
            {
                vizElm_WIDGET_UI_SHADOW.setRectGradient(getTheme().WidgetAccent1Color_withAlpha(getTheme().ElementHoveredForegroundAlpha), getTheme().WidgetAccent1Color_withAlpha(getTheme().ElementHoveredForegroundAlpha), getTheme().WidgetAccent1Color_withAlpha(getTheme().ElementHoveredForegroundAlpha), getTheme().WidgetAccent2Color_withAlpha(getTheme().ElementHoveredForegroundAlpha));
            }
            else
            {
                ofColor shadowDarkest = ofColor(0, 0, 0, getIsActiveWidget() ? 40 : 20);
                ofColor shadowMedium = ofColor(0, 0, 0, getIsActiveWidget() ? 15 : 10);
                ofColor shadowLightest = ofColor(0, 0, 0, getIsActiveWidget() ? 10 : 5);
                vizElm_WIDGET_UI_SHADOW.setRectGradient(shadowMedium, shadowDarkest, shadowMedium, shadowLightest);
            }
        }

        virtual void setWasJustInitialized(bool val) override
        {
            WidgetBase::setWasJustInitialized(val);
            if (val)
            {
                mUICurrentWidgetAlpha = 0.2f;
            }
        }

        virtual void onWidgetLoaded() override
        {
            // Implement any behaviour for when a widget has loaded
        }

        virtual bool loadState(string widgetXML) override
        {

            WidgetBase::loadState(widgetXML);

            ofxXmlSettings mWidgetXML = ofxXmlSettings();

            long start = ofGetSystemTimeMillis();

#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
            cout << "[" << getPersistentId() << "]Widget->loadState()\n";
#endif
            if (!mWidgetXML.loadFromBuffer(widgetXML.c_str()))
            {
#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
                cout << "[" << getPersistentId() << "]Widget->Unable to load XML\n"
                     << widgetXML.c_str() << "\n";
#endif
                return false;
            }
#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
            cout << "[" << getPersistentId() << "]Widget->Loaded XML in \n" + ofToString(ofGetSystemTimeMillis() - start) + "ms\n";
#endif

            mWidgetXML.pushTag("widget");

            /**** Title ****/
            string titleStyle = ofToUpper(mWidgetXML.getAttribute("title", "style", getTitleStyleString(getTitleStyle()), 0));
            setTitleStyle(getTitleStyleEnum(titleStyle));
            setIsTitleAutohide(mWidgetXML.getAttribute("title", "autohide", getIsTitleAutohide()));

            if (isTitleStatic())
            {
                if (getTitleStyle() != TitleStyle::TOP_STATIC || getTitleStyle() != TitleStyle::BOTTOM_STATIC ||
                    getTitleStyle() != TitleStyle::LEFT_STATIC || getTitleStyle() != TitleStyle::RIGHT_STATIC)
                {
                    setTitleStyle(TitleStyle::TOP_STATIC);
                }
                int size = mWidgetXML.getAttribute("title", "staticSize", getStaticTitleBarSize(false));
                switch (getTitleStyle())
                {
                case TitleStyle::TOP_STATIC:
                    setStaticTopTitleBarSize(size);
                    break;
                case TitleStyle::BOTTOM_STATIC:
                    setStaticBottomTitleBarSize(size);
                    break;
                case TitleStyle::LEFT_STATIC:
                    setStaticLeftTitleBarSize(size);
                    break;
                case TitleStyle::RIGHT_STATIC:
                    setStaticRightTitleBarSize(size);
                    break;
                }
            }

            /**** Bounds ****/
            setWidgetSize(
                mWidgetXML.getAttribute("bounds", "width", getWidth()),
                mWidgetXML.getAttribute("bounds", "height", getHeight()),
                true);

            setWidgetPosition(
                mWidgetXML.getAttribute("bounds", "x", getX()),
                mWidgetXML.getAttribute("bounds", "y", getY()),
                true);

            /**** Behaviour ****/
            setDrawWidgetChrome(mWidgetXML.getAttribute("behaviour", "drawWidgetChrome", getDrawWidgetChrome()));
            setDrawWidgetTitle(mWidgetXML.getAttribute("behaviour", "drawWidgetTitle", getDrawWidgetTitle()));

            /**** Appearance ****/
            setIsMinimized(mWidgetXML.getAttribute("appearance", "minimized", getIsMinimized(), 0));

            WidgetTheme theme = getTheme();

            if (mWidgetXML.pushTag("appearance"))
            {
                theme.loadForWidget(theme, widgetXML);
                setTheme(theme);
                mWidgetXML.popTag(); // appearance
            }

            mWidgetXML.popTag(); // widget

            /**** Elements collection ****/
            TiXmlElement *rootElm = mWidgetXML.doc.RootElement();
            if (rootElm == NULL)
            {
                cout << "[" << getPersistentId() << "]Widget XML is invalid\n";
                return false;
            }
            TiXmlElement *elementsElm = rootElm->FirstChildElement("elements");

            if (elementsElm != NULL)
            {
                for (TiXmlNode *child = elementsElm->FirstChild(); child; child = child->NextSibling())
                {
                    TiXmlPrinter printer;
                    child->Accept(&printer);
                    string elmXML = printer.CStr();

                    ofxXmlSettings elementSettings = ofxXmlSettings();
                    if (elementSettings.loadFromBuffer(elmXML))
                    {
                        string elmId = elementSettings.getAttribute("element", "id", "");
                        string elmClass = elementSettings.getAttribute("element", "class", "");

                        if (elmClass != "" && elmId != "")
                        {
                            WidgetElm *existingElm = getWidgetElement(elmId);

                            if (existingElm != nullptr)
                            {
                                existingElm->loadState(elmXML);
                            }
                            else
                            {
                                WidgetElm *e = addWidgetElement(elmClass, elmId, elmXML);
                            }
                            // cout << "element [" << this->getPersistentId() << "." << e->getPersistentId() << "] (" << elmClass << ") loaded\n";
                        }
                    }
                }

                for (WidgetElm &elm : mWidgetElements)
                {
                    elm.setAllSiblingElementsLoaded(true);
                    elm.onAllSiblingElementsLoaded();
                }
            }

            onWidgetLoaded();

            return true;
        }

        static vector<WidgetSignature> getChildWidgetsList(string widgetXML)
        {

            vector<WidgetSignature> widgets;

            ofxXmlSettings mWidgetXML = ofxXmlSettings();

#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
            cout << "Widget->getChildWidgetsList()\n";
#endif
            if (!mWidgetXML.loadFromBuffer(widgetXML.c_str()))
            {
#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
                cout << "Widget->Unable to load XML\n"
                     << widgetXML.c_str() << "\n";
#endif
                return widgets;
            }

            /**** Child Widgets collection ****/
            TiXmlElement *rootElm = mWidgetXML.doc.RootElement();
            if (rootElm != NULL)
            {
                TiXmlElement *childWidgetsElm = rootElm->FirstChildElement("childWidgets");

                if (childWidgetsElm != NULL)
                {
                    for (TiXmlNode *child = childWidgetsElm->FirstChild(); child; child = child->NextSibling())
                    {
                        TiXmlPrinter printer;
                        child->Accept(&printer);
                        string widgetXML = printer.CStr();

                        ofxXmlSettings widgetSettings = ofxXmlSettings();
                        if (widgetSettings.loadFromBuffer(widgetXML))
                        {
                            string persistentId = widgetSettings.getAttribute("widget", "id", "");
                            string widgetClass = widgetSettings.getAttribute("widget", "class", "");
                            widgets.push_back(WidgetSignature(persistentId, widgetClass, widgetXML));
                        }
                    }
                }
            }

            return widgets;
        }

        virtual ofxXmlSettings saveState() override
        {

            ofxXmlSettings mWidgetXML = WidgetBase::saveState();
            mWidgetXML.setAttribute("widget", "class", "Widget", 0);
            mWidgetXML.pushTag("widget");

            /**** Title ****/
            mWidgetXML.setAttribute("title", "style", getTitleStyleString(getTitleStyle()), 0);

            /*
             NONE,
             TOP_STATIC, TOP_OVERLAY, TOP_OVERLAY_FULL, TOP_WITH_WIDGET_RESIZE,
             BOTTOM_STATIC, BOTTOM_OVERLAY, BOTTOM_OVERLAY_FULL, BOTTOM_WITH_WIDGET_RESIZE,
             LEFT_STATIC, LEFT_OVERLAY, LEFT_OVERLAY_FULL, LEFT_WITH_WIDGET_RESIZE,
             RIGHT_STATIC, RIGHT_OVERLAY, RIGHT_OVERLAY_FULL, RIGHT_WITH_WIDGET_RESIZE
             */
            mWidgetXML.setAttribute("title", "autohide", getIsTitleAutohide(), 0);
            if (isTitleStatic())
            {
                mWidgetXML.setAttribute("title", "staticSize", getStaticTitleBarSize(false), 0);
            }

            /**** Behaviour ****/
            mWidgetXML.setAttribute("behaviour", "drawWidgetChrome", getDrawWidgetChrome(), 0);
            mWidgetXML.setAttribute("behaviour", "drawWidgetTitle", getDrawWidgetTitle(), 0);

            /**** Appearance ****/
            mWidgetXML.setAttribute("appearance", "minimized", getIsMinimized(), 0);

            if (getShouldPersistTheme())
            {
                mWidgetXML.pushTag("appearance");
                mWidgetXML.addTag("widgetColor");
                mWidgetXML.setAttribute("widgetColor", "color", getTheme().getWidgetColorHex(), 0);

                mWidgetXML.addTag("widgetContentColor");
                mWidgetXML.setAttribute("widgetContentColor", "color", getTheme().getWidgetContentColorHex(), 0);

                mWidgetXML.addTag("widgetAccentColor");
                mWidgetXML.setAttribute("widgetAccentColor", "color1", getTheme().getWidgetAccent1ColorHex(), 0);
                mWidgetXML.setAttribute("widgetAccentColor", "color2", getTheme().getWidgetAccent2ColorHex(), 0);
                mWidgetXML.setAttribute("widgetAccentColor", "color3", getTheme().getWidgetAccent3ColorHex(), 0);
                mWidgetXML.setAttribute("widgetAccentColor", "color4", getTheme().getWidgetAccent4ColorHex(), 0);

                mWidgetXML.addTag("widgetAlternatingColor");
                mWidgetXML.setAttribute("widgetAlternatingColor", "color1", getTheme().getWidgetAlternating1ColorHex(), 0);
                mWidgetXML.setAttribute("widgetAlternatingColor", "color1", getTheme().getWidgetAlternating2ColorHex(), 0);

                mWidgetXML.addTag("widgetModalBackgroundColor");
                mWidgetXML.setAttribute("widgetModalBackgroundColor", "color", getTheme().getWidgetModalBackgroundColorHex(), 0);

                mWidgetXML.addTag("widgetAlpha");
                mWidgetXML.setAttribute("widgetAlpha", "hovered", getTheme().HoveredWidgetAlpha, 0);
                mWidgetXML.setAttribute("widgetAlpha", "unhovered", getTheme().UnhoveredWidgetAlpha, 0);
                mWidgetXML.setAttribute("widgetAlpha", "contentChrome", getTheme().ContentChromeAlpha, 0);

                mWidgetXML.addTag("titleFontColor");
                mWidgetXML.setAttribute("titleFontColor", "color", getTheme().getTitleFontColorHex(), 0);

                mWidgetXML.addTag("titleFontAlpha");
                mWidgetXML.setAttribute("titleFontAlpha", "hovered", getTheme().HoveredTitleFontAlpha, 0);
                mWidgetXML.setAttribute("titleFontAlpha", "unhovered", getTheme().UnhoveredTitleFontAlpha, 0);

                mWidgetXML.addTag("titleColor");
                mWidgetXML.setAttribute("titleColor", "color", getTheme().getTitleColorHex(), 0);

                mWidgetXML.addTag("titleAlpha");
                mWidgetXML.setAttribute("titleAlpha", "hovered", getTheme().HoveredTitleAlpha, 0);
                mWidgetXML.setAttribute("titleAlpha", "unhovered", getTheme().UnhoveredTitleAlpha, 0);

                mWidgetXML.addTag("titleGradientColor");
                mWidgetXML.setAttribute("titleGradientColor", "color1", getTheme().getTitleGradientColor1Hex(), 0);
                mWidgetXML.setAttribute("titleGradientColor", "color2", getTheme().getTitleGradientColor2Hex(), 0);
                mWidgetXML.setAttribute("titleGradientColor", "color3", getTheme().getTitleGradientColor3Hex(), 0);
                mWidgetXML.setAttribute("titleGradientColor", "color4", getTheme().getTitleGradientColor4Hex(), 0);

                mWidgetXML.addTag("typographyColor");
                mWidgetXML.setAttribute("typographyColor", "primary", getTheme().getTypographyPrimaryColorHex(), 0);
                mWidgetXML.setAttribute("typographyColor", "secondary", getTheme().getTypographySecondaryColorHex(), 0);
                mWidgetXML.setAttribute("typographyColor", "tertiary", getTheme().getTypographyTertiaryColorHex(), 0);
                mWidgetXML.setAttribute("typographyColor", "quaternary", getTheme().getTypographyQuaternaryColorHex(), 0);

                mWidgetXML.addTag("selection");
                mWidgetXML.setAttribute("selection", "foreground", getTheme().getSelectionForegroundColorHex(), 0);
                mWidgetXML.setAttribute("selection", "background", getTheme().getSelectionBackgroundColorHex(), 0);

                mWidgetXML.addTag("hover");
                mWidgetXML.setAttribute("hover", "foreground", getTheme().getHoverForegroundColorHex(), 0);
                mWidgetXML.setAttribute("hover", "background", getTheme().getHoverBackgroundColorHex(), 0);

                mWidgetXML.popTag(); // appearance
            }

            mWidgetXML.addTag("elements");

            TiXmlElement *rootElm = mWidgetXML.doc.RootElement();
            TiXmlElement *elementsElm = rootElm->FirstChildElement("elements");

            if (elementsElm != NULL)
            {
                for (WidgetElm &elm : mWidgetElements)
                {
                    ofxXmlSettings elmSettings = elm.saveState();
                    string elmXML;
                    elmSettings.copyXmlToString(elmXML);

                    TiXmlElement *elmNode = new TiXmlElement("element");
                    elmNode->Parse(elmXML.c_str(), 0, TIXML_DEFAULT_ENCODING);
                    elementsElm->InsertEndChild(*elmNode);
                }
            }
            mWidgetXML.popTag(); // widget

            return mWidgetXML;
        }

        virtual void replaceExpressions(Widget *w, string &exprStr)
        {
            ofStringReplace(exprStr, " ", "");

            string thisPersistentId = w->getPersistentId();
            string parentPersistentId = "";

            if (w->getPersistentId() == thisPersistentId)
            {
                ofStringReplace(exprStr, "${SELF.", "${" + w->getPersistentId() + ".");

                if (w->parent())
                {
                    parentPersistentId = w->parent()->getPersistentId();
                    ofStringReplace(exprStr, "${PARENT.", "${" + parentPersistentId + ".");
                    // possible optimization : only if ${PARENT. found .... maybe not though, what if sub widgets to it rely on parent?
                    replaceExpressions(w->parent(), exprStr);
                }

                if (w->root())
                {
                    string rootPersistentId = w->root()->getPersistentId();
                    ofStringReplace(exprStr, "${ROOT.", "${" + rootPersistentId + ".");
                }
            }

            if (w->getPersistentId() != "")
            {
                ofStringReplace(exprStr, "${" + thisPersistentId + ".X}", ofToString(w->getWidgetStateX()));
                ofStringReplace(exprStr, "${" + thisPersistentId + ".Y}", ofToString(w->getWidgetStateY()));
                ofStringReplace(exprStr, "${" + thisPersistentId + ".WIDTH}", ofToString(w->getWidgetStateWidth()));
                ofStringReplace(exprStr, "${" + thisPersistentId + ".HEIGHT}", ofToString(w->getWidgetStateHeight()));
                ofStringReplace(exprStr, "${" + thisPersistentId + ".RIGHT}", ofToString(w->getWidgetStateX() + w->getWidgetStateWidth()));
                ofStringReplace(exprStr, "${" + thisPersistentId + ".BOTTOM}", ofToString(w->getWidgetStateY() + w->getWidgetStateHeight()));

                ofStringReplace(exprStr, "${" + thisPersistentId + ".ABSOLUTE_X}", ofToString(w->getX()));
                ofStringReplace(exprStr, "${" + thisPersistentId + ".ABSOLUTE_Y}", ofToString(w->getY()));

                ofStringReplace(exprStr, "${" + thisPersistentId + ".ABSOLUTE_USABLE_X_FIXED}", ofToString(w->getUsableX(false)));
                ofStringReplace(exprStr, "${" + thisPersistentId + ".ABSOLUTE_USABLE_Y_FIXED}", ofToString(w->getUsableY(false)));

                ofStringReplace(exprStr, "${" + thisPersistentId + ".ABSOLUTE_USABLE_X}", ofToString(w->getUsableX()));
                ofStringReplace(exprStr, "${" + thisPersistentId + ".ABSOLUTE_USABLE_Y}", ofToString(w->getUsableY()));

                ofStringReplace(exprStr, "${" + thisPersistentId + ".USABLE_WIDTH}", ofToString(w->getUsableWidth()));
                ofStringReplace(exprStr, "${" + thisPersistentId + ".USABLE_HEIGHT}", ofToString(w->getUsableHeight()));

                ofStringReplace(exprStr, "${WINDOW.WIDTH}", ofToString(w->getViz()->deScale(ofGetWindowWidth()) - w->getScreenPadding()));
                ofStringReplace(exprStr, "${WINDOW.HEIGHT}", ofToString(w->getViz()->deScale(ofGetWindowHeight()) - w->getScreenPadding()));
                ofStringReplace(exprStr, "${PADDING}", ofToString(w->getNonScaledPadding()));

                // This is for sibling widgets referencing each other
                for (Widget &exprWidget : w->getChildWidgets())
                {
                    if (exprWidget.getPersistentId() != "")
                    {
                        ofStringReplace(exprStr, "${" + exprWidget.getPersistentId() + ".X}", ofToString(exprWidget.getX()));
                        ofStringReplace(exprStr, "${" + exprWidget.getPersistentId() + ".Y}", ofToString(exprWidget.getY()));
                        ofStringReplace(exprStr, "${" + exprWidget.getPersistentId() + ".WIDTH}", ofToString(exprWidget.getWidgetStateWidth()));
                        ofStringReplace(exprStr, "${" + exprWidget.getPersistentId() + ".HEIGHT}", ofToString(exprWidget.getWidgetStateHeight()));
                        ofStringReplace(exprStr, "${" + exprWidget.getPersistentId() + ".RIGHT}", ofToString(exprWidget.getX() + exprWidget.getWidgetStateWidth()));
                        ofStringReplace(exprStr, "${" + exprWidget.getPersistentId() + ".RIGHT_USABLE_WIDTH}", ofToString(w->getUsableWidth() - exprWidget.getX() - exprWidget.getWidgetStateWidth() + w->getUsableX()));
                        ofStringReplace(exprStr, "${" + exprWidget.getPersistentId() + ".BOTTOM}", ofToString(exprWidget.getY() + exprWidget.getWidgetStateHeight()));
                        ofStringReplace(exprStr, "${" + exprWidget.getPersistentId() + ".BOTTOM_USABLE_HEIGHT}", ofToString(w->getUsableHeight() - exprWidget.getY() - exprWidget.getWidgetStateHeight() + w->getUsableY()));
                    }
                }

                // This is for sibling elements referencing each other
                for (WidgetElm &exprElm : w->getWidgetElements())
                {
                    if (exprElm.getPersistentId() != "")
                    {
                        ofStringReplace(exprStr, "${" + exprElm.getPersistentId() + ".X}", ofToString(exprElm.getX()));
                        ofStringReplace(exprStr, "${" + exprElm.getPersistentId() + ".Y}", ofToString(exprElm.getY()));
                        ofStringReplace(exprStr, "${" + exprElm.getPersistentId() + ".WIDTH}", ofToString(exprElm.getWidth()));
                        ofStringReplace(exprStr, "${" + exprElm.getPersistentId() + ".HEIGHT}", ofToString(exprElm.getHeight()));
                        ofStringReplace(exprStr, "${" + exprElm.getPersistentId() + ".RIGHT}", ofToString(exprElm.getX() + exprElm.getWidth()));
                        ofStringReplace(exprStr, "${" + exprElm.getPersistentId() + ".RIGHT_USABLE_WIDTH}", ofToString(w->getUsableWidth() - exprElm.getX() - exprElm.getWidth() + w->getUsableX()));
                        ofStringReplace(exprStr, "${" + exprElm.getPersistentId() + ".BOTTOM}", ofToString(exprElm.getY() + exprElm.getHeight()));
                        ofStringReplace(exprStr, "${" + exprElm.getPersistentId() + ".BOTTOM_USABLE_HEIGHT}", ofToString(w->getUsableHeight() - exprElm.getY() - exprElm.getHeight() + w->getUsableY()));
                    }
                }
            }
        }

        virtual bool getIsMousePressedAndReleasedOverWidget(bool includeWidgetChromeElements) override
        {
            bool isMousePressedAndReleasedOverWidget = WidgetBase::getIsMousePressedAndReleasedOverWidget(includeWidgetChromeElements);
            if (isMousePressedAndReleasedOverWidget && !includeWidgetChromeElements && isTitleHovered())
            {
                return false;
            }
            else
            {
                return isMousePressedAndReleasedOverWidget;
            }
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
// Override this in derrived widget to handle incoming events from uiVizElements, etc
#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
            string xml;
            args.eventXML.copyXmlToString(xml);
            ofLogNotice("Widget") << "[" + getPersistentId() + "] received unhandled event " << args.widgetId << "." << args.eventName << " with data \n"
                                       << xml;
#endif
        }

        void recalculateElementExpressions(WidgetElm *elm)
        {

            if (getIgnoreExpr_Layout())
                return;

            if (elm->getX_Expr() != "" || elm->getY_Expr() != "" || elm->getWidth_Expr() != "" || elm->getHeight_Expr() != "")
            {

                // replace string tokens ${SCREEN.WIDTH}, ${PERSISTENTID.RIGHT}, etc
                string exprStringX = elm->getX_Expr();
                string exprStringY = elm->getY_Expr();
                string exprStringWidth = elm->getWidth_Expr();
                string exprStringHeight = elm->getHeight_Expr();

                ofStringReplace(exprStringX, " ", "");
                ofStringReplace(exprStringY, " ", "");
                ofStringReplace(exprStringWidth, " ", "");
                ofStringReplace(exprStringHeight, " ", "");

                if (exprStringX != "")
                    replaceElementExpressions(elm, exprStringX);
                if (exprStringY != "")
                    replaceElementExpressions(elm, exprStringY);
                if (exprStringWidth != "")
                    replaceElementExpressions(elm, exprStringWidth);
                if (exprStringHeight != "")
                    replaceElementExpressions(elm, exprStringHeight);

                if (exprStringX != "")
                {
                    if (calculator.compile(exprStringX.c_str(), expression))
                    {
                        elm->setX(expression.value());
                        // ofLogNotice("WidgetElm") << elm->getPersistentId() << ".setX(): \"" << elm->getX_Expr() << "\" [ELM:---->" + exprStringX + "<----] = " << ofToString(expression.value());
                    }
                    else
                    {
                        ofLogWarning("WidgetElm") << elm->getPersistentId() << ".setX(): failed to evaluate expression \"" << elm->getX_Expr() << "\" [ELM:---->" + exprStringX + "<----] : " << calculator.error().c_str();
                    }
                }

                if (exprStringY != "")
                {
                    if (calculator.compile(exprStringY.c_str(), expression))
                    {
                        elm->setY(expression.value());
                        // ofLogNotice("WidgetElm") << elm->getPersistentId() << ".setY(): \"" << elm->getY_Expr() << "\" [ELM:---->" + exprStringY + "<----] = " << ofToString(expression.value());
                    }
                    else
                    {
                        ofLogWarning("WidgetElm") << elm->getPersistentId() << ".setY: failed to evaluate expression \"" << elm->getY_Expr() << "\" [ELM:---->" + exprStringY + "<----] : " << calculator.error().c_str();
                    }
                }

                if (exprStringWidth != "")
                {
                    if (calculator.compile(exprStringWidth.c_str(), expression))
                    {
                        elm->setWidth(expression.value());
                        // ofLogNotice("WidgetElm") << elm->getPersistentId() << ".setWidth(): \"" << elm->getWidth_Expr() << "\" [ELM:---->" + exprStringWidth + "<----] = " << ofToString(expression.value());
                    }
                    else
                    {
                        ofLogWarning("WidgetElm") << elm->getPersistentId() << ".setWidth: failed to evaluate expression \"" << elm->getWidth_Expr() << "\" [ELM:---->" + exprStringWidth + "<----] : " << calculator.error().c_str();
                    }
                }

                if (exprStringHeight != "")
                {
                    if (calculator.compile(exprStringHeight.c_str(), expression))
                    {
                        elm->setHeight(expression.value());
                        // ofLogNotice("WidgetElm") << elm->getPersistentId() << ".setHeight(): \"" << elm->getHeight_Expr() << "\" [ELM:---->" + exprStringHeight + "<----] = " << ofToString(expression.value());
                    }
                    else
                    {
                        ofLogWarning("WidgetElm") << elm->getPersistentId() << ".setHeight(): failed to evaluate expression \"" << elm->getHeight_Expr() << "\" [ELM:---->" + exprStringHeight + "<----] : " << calculator.error().c_str();
                    }
                }
            }
        }

        void replaceElementExpressions(WidgetElm *elm, string &exprStr)
        {

            int absoluteX = elm->getX();
            int absoluteY = elm->getY();

            if (elm->parent())
            {
                absoluteX = 0;
                absoluteY = 0;
            }

            string thisPersistentId = elm->getPersistentId();
            if (elm->getPersistentId() == thisPersistentId)
            {
                ofStringReplace(exprStr, "${SELF.", "${" + thisPersistentId + ".");
            }

            ofStringReplace(exprStr, "${PARENT.ABSOLUTE_X}", ofToString(absoluteX));
            ofStringReplace(exprStr, "${PARENT.ABSOLUTE_Y}", ofToString(absoluteY));
            /*
            ofStringReplace(exprStr, "${PARENT.ABSOLUTE_USABLE_X}", ofToString(this->getUsableX()));
            ofStringReplace(exprStr, "${PARENT.ABSOLUTE_USABLE_Y}", ofToString(this->getUsableY()));
            */

            ofStringReplace(exprStr, "${PARENT.USABLE_WIDTH}", ofToString(this->getUsableWidth()));
            ofStringReplace(exprStr, "${PARENT.USABLE_HEIGHT}", ofToString(this->getUsableHeight()));

            ofStringReplace(exprStr, "${PARENT.WIDTH}", ofToString(this->getWidgetStateWidth()));
            ofStringReplace(exprStr, "${PARENT.HEIGHT}", ofToString(this->getWidgetStateHeight()));
            ofStringReplace(exprStr, "${PADDING}", ofToString(getNonScaledPadding()));

            int relativeX = this->getUsableX() - this->getX();
            int relativeY = this->getUsableY() - this->getY();
            int relativeX_Fixed = this->getUsableX(false) - this->getX();
            int relativeY_Fixed = this->getUsableY(false) - this->getY();

            ofStringReplace(exprStr, "${PARENT.CENTER_X}", ofToString((getCoreUsableWidth() - (float)elm->getWidth()) / 2 + relativeX));
            ofStringReplace(exprStr, "${PARENT.CENTER_Y}", ofToString((getCoreUsableHeight() - (float)elm->getHeight()) / 2 + relativeY));
            ofStringReplace(exprStr, "${PARENT.LEFT}", ofToString(relativeX));
            ofStringReplace(exprStr, "${PARENT.RIGHT}", ofToString(getWidgetStateWidth() - elm->getWidth() - getNonScaledPadding() - (isTitleRight() ? (isTitleResize() ? mUICurrentTitleBarSize : getUnhoveredTitleBarSize(false)) : 0)));
            ofStringReplace(exprStr, "${PARENT.TOP}", ofToString(relativeY));
            ofStringReplace(exprStr, "${PARENT.BOTTOM}", ofToString(getWidgetStateHeight() - (float)elm->getHeight() - (float)getNonScaledPadding() - (isTitleBottom() ? (isTitleResize() ? mUICurrentTitleBarSize : getUnhoveredTitleBarSize(false)) : 0)));

            ofStringReplace(exprStr, "${PARENT.LEFT_FIXED}", ofToString(relativeX_Fixed));
            ofStringReplace(exprStr, "${PARENT.TOP_FIXED}", ofToString(relativeY_Fixed));

            for (Widget &exprWidget : getChildWidgets())
            {
                if (exprWidget.getPersistentId() != "")
                {
                    string exprWidgetId = exprWidget.getPersistentId();
                    ofStringReplace(exprStr, "${" + exprWidgetId + ".X}", ofToString(exprWidget.getX()));
                    ofStringReplace(exprStr, "${" + exprWidgetId + ".Y}", ofToString(exprWidget.getY()));
                    ofStringReplace(exprStr, "${" + exprWidgetId + ".WIDTH}", ofToString(exprWidget.getWidgetStateWidth()));
                    ofStringReplace(exprStr, "${" + exprWidgetId + ".HEIGHT}", ofToString(exprWidget.getWidgetStateHeight()));
                    ofStringReplace(exprStr, "${" + exprWidgetId + ".RIGHT}", ofToString(exprWidget.getX() + exprWidget.getWidgetStateWidth()));
                    ofStringReplace(exprStr, "${" + exprWidgetId + ".RIGHT_USABLE_WIDTH}", ofToString(getUsableWidth() - (exprWidget.getX() + exprWidget.getWidgetStateWidth()) + getUsableX()));
                    ofStringReplace(exprStr, "${" + exprWidgetId + ".BOTTOM}", ofToString(exprWidget.getY() + exprWidget.getWidgetStateHeight()));
                    ofStringReplace(exprStr, "${" + exprWidgetId + ".BOTTOM_USABLE_HEIGHT}", ofToString(getUsableHeight() - (exprWidget.getY() + exprWidget.getWidgetStateHeight()) + getUsableY()));
                }
            }

            for (WidgetElm &exprElm : mWidgetElements)
            {
                if (exprElm.getPersistentId() != "")
                {
                    string exprElmId = exprElm.getPersistentId();
                    ofStringReplace(exprStr, "${" + exprElmId + ".X}", ofToString(exprElm.getRelativeX()));
                    ofStringReplace(exprStr, "${" + exprElmId + ".Y}", ofToString(exprElm.getRelativeY()));
                    ofStringReplace(exprStr, "${" + exprElmId + ".WIDTH}", ofToString(exprElm.getWidgetStateWidth()));
                    ofStringReplace(exprStr, "${" + exprElmId + ".HEIGHT}", ofToString(exprElm.getWidgetStateHeight()));
                    ofStringReplace(exprStr, "${" + exprElmId + ".RIGHT}", ofToString(exprElm.getRelativeX() + exprElm.getWidgetStateWidth()));
                    ofStringReplace(exprStr, "${" + exprElmId + ".RIGHT_USABLE_WIDTH}", ofToString(getUsableWidth() - (exprElm.getX() + exprElm.getWidgetStateWidth()) + getUsableX()));
                    ofStringReplace(exprStr, "${" + exprElmId + ".BOTTOM}", ofToString(exprElm.getRelativeY() + exprElm.getWidgetStateHeight()));
                    ofStringReplace(exprStr, "${" + exprElmId + ".BOTTOM_USABLE_HEIGHT}", ofToString(getUsableHeight() - (exprElm.getY() + exprElm.getWidgetStateHeight()) + getUsableY()));
                }
            }
        }

        WidgetElm *addOrLoadWidgetElement(WidgetElm *elm, string ELM_CLASS, string persistentId, string widgetXML)
        {
            mapElements();
            // Does the class exist in the map?
            if (elmClassMap.count(ELM_CLASS) > 0)
            {

                WidgetElm *existingElm = getWidgetElement(persistentId);

                if (existingElm != nullptr)
                {
                    // existingElm->loadState(widgetXML); don't do this - causes dynamically loaded elms to lose their value!
                    elm = existingElm;
                }
                else
                {
                    elm = elmClassMap[ELM_CLASS](persistentId, widgetXML, this);
                    addWidgetElement(*elm);
                }

                return elm;
            }
            else
            {
                ofLogWarning("Widget") << "[Element:" << persistentId << "] of class \"" << ELM_CLASS << "\" failed to load. [" + widgetXML + "]";
            }

            return elm;
        }

        WidgetElm *getWidgetElement(string persistentId)
        {
            for (WidgetElm &elm : mWidgetElements)
            {
                if (ofToLower(elm.getPersistentId()) == ofToLower(persistentId))
                {
                    return &elm;
                }
            }
#if VIZ_DEBUG_LEVEL >= 1 && VIZ_DEBUG_LEVEL < 2
            ofLogError("Widget") << "Expected element [" << getPersistentId() << "->" << "" << persistentId << "] was NOT found! (Segfault likely!)";
#endif
            return nullptr;
        }

        virtual int getWidgetElementIndex(string persistentId) override
        {
            int idx = 0;
            for (WidgetElm &elm : mWidgetElements)
            {
                if (ofToLower(elm.getPersistentId()) == ofToLower(persistentId))
                {
                    return idx;
                }
                idx++;
            }
            return 0;
        }

        virtual int getWidgetElementSize() override
        {
            return mWidgetElements.size();
        }

        bool focusWidgetElement(WidgetElm &e)
        {
            return focusWidgetElement(e.getPersistentId());
        }

        bool focusNextWidgetElement()
        {
            if (getDidMoveFocus())
                return false;
            bool nextElmShouldFocus = false;
            if (mWidgetElements.size() > 0)
            {

                for (WidgetElm &elm : mWidgetElements)
                {

                    if (nextElmShouldFocus)
                    {

                        if (elm.getIsVisible())
                        {
                            focusWidgetElement(elm.getPersistentId());
                            setDidMoveFocus(true);
                            return true;
                        }
                    }

                    if (elm.getIsFocused(false))
                    {
                        nextElmShouldFocus = true;
                    }
                }

                // We didn't focus the next, back to 1st...
                for (WidgetElm &elm : mWidgetElements)
                {
                    focusWidgetElement(elm.getPersistentId());
                    setDidMoveFocus(true);
                    return true;
                }
            }
            return false;
        }

        bool focusWidgetElement(string persistentId)
        {
            for (WidgetElm &elm : mWidgetElements)
            {
                elm.setIsFocused(false);
                elm.setIsSiblingFocused(false);
            }
            for (WidgetElm &elm : mWidgetElements)
            {
                if (elm.getPersistentId() == persistentId)
                {
                    elm.setActiveWidget();
                    elm.setWidgetNeedsUpdate(true);
                    elm.setNeedsUpdate(true);
                    setWidgetNeedsUpdate(true);
                    setNeedsUpdate(true);

                    // Let all siblings know that one of them is focused...
                    for (WidgetElm &elmSib : mWidgetElements)
                    {
                        elmSib.setIsSiblingFocused(true);
                    }

                    elm.setIsFocused(true);
                    elm.setIsSiblingFocused(false);

                    return true;
                }
            }
            return false;
        }

        WidgetElmLabel *getLabel(string persistentId)
        {
            return dynamic_cast<WidgetElmLabel *>(getWidgetElement(persistentId));
        }

        WidgetElmButton *getButton(string persistentId)
        {
            return dynamic_cast<WidgetElmButton *>(getWidgetElement(persistentId));
        }

        WidgetElmTextarea *getTextarea(string persistentId)
        {
            return dynamic_cast<WidgetElmTextarea *>(getWidgetElement(persistentId));
        }

        WidgetElmTextbox *getTextbox(string persistentId)
        {
            return dynamic_cast<WidgetElmTextbox *>(getWidgetElement(persistentId));
        }

        WidgetElmSlider *getSlider(string persistentId)
        {
            return dynamic_cast<WidgetElmSlider *>(getWidgetElement(persistentId));
        }

        WidgetElmCheckbox *getCheckbox(string persistentId)
        {
            return dynamic_cast<WidgetElmCheckbox *>(getWidgetElement(persistentId));
        }

        WidgetElmDropdown *getDropdown(string persistentId)
        {
            return dynamic_cast<WidgetElmDropdown *>(getWidgetElement(persistentId));
        }

        WidgetElmBreadcrumb *getBreadcrumb(string persistentId)
        {
            return dynamic_cast<WidgetElmBreadcrumb *>(getWidgetElement(persistentId));
        }

        WidgetElmImage *getImage(string persistentId)
        {
            return dynamic_cast<WidgetElmImage *>(getWidgetElement(persistentId));
        }

        WidgetElmProgressBar *getProgressBar(string persistentId)
        {
            return dynamic_cast<WidgetElmProgressBar *>(getWidgetElement(persistentId));
        }

        void removeWidgetElement(WidgetElm *e)
        {

            if (e == nullptr || mWidgetElements.size() == 0)
                return;

            for (WidgetElm &elm : mWidgetElements)
            {
                if (ofToLower(elm.getPersistentId()) == ofToLower(e->getPersistentId()))
                {

#if VIZ_DEBUG_LEVEL >= 1 && VIZ_DEBUG_LEVEL < 2
                    cout << "REMOVING ELEMENT [" << e->getPersistentId() << "] FROM WIDGET: [" << getPersistentId() << "]\n";
#endif

                    mWidgetElements.erase(std::remove_if(
                                              mWidgetElements.begin(), mWidgetElements.end(),
                                              [&](WidgetElm &element)
                                              {
                                                  return ofToLower(element.getPersistentId()) == ofToLower(e->getPersistentId());
                                              }),
                                          mWidgetElements.end());

                    delete &elm;
                    return;
                }
            }
        }

        Widget *parentWidget()
        {
            return mParent;
        }

        Widget *rootWidget()
        {
            Widget *r = this;
            Widget *w = parentWidget();
            while (w)
            {
                w = w->parentWidget();
                r = w;
            }
            return r;
        }

        virtual void update(WidgetContext context) override
        {
            // Implemented by descendant class...
        }

        virtual bool needsUpdateBeforeDraw(bool shouldResetTimer)
        {
            // Implemented by descendant class if needed...
            return false;
        }

        virtual void draw(WidgetContext context) override
        {
            // Implemented by descendant class...
        }

        virtual void updateMinimized()
        {
            // Implemented by descendant class...
        }

        virtual void drawMinimized()
        {
            // Implemented by descendant class...
        }

        bool isWidgetHovered() override
        {
            // if (!getIsVisible()) return false;
            return widgetFrame.isHovered_Rect();
        }

        bool isTitleHovered()
        {
            return widgetTitle.isHovered_Rect();
        }

        bool isInsideWidgetBounds(int x, int y, int width, int height)
        {
            return y >= getUsableY(false) && y + height < getUsableY(false) + getUsableHeight();
            //& x >= getUsableX(false) && x + width < getUsableX(false) + getUsableWidth();
        }

        bool isInsideWidgetBoundsY(int y, int height)
        {
            return y >= getUsableY(false) && y + height < getUsableY(false) + getUsableHeight();
        }

        virtual void setExpressionRecalculationRequired(bool val) override
        {
            WidgetBase::setExpressionRecalculationRequired(val);
            for (Widget &widget : mChildWidgets)
            {
                widget.WidgetBase::setExpressionRecalculationRequired(val);
            }
        }

        void clearChildWidgets()
        {
            while (mChildWidgets.size() > 0)
            {
                for (Widget &widget : mChildWidgets)
                {
                    removeChildWidget(widget);
                }
            }
        }

        void setChildWidgetsVisibility(bool val)
        {
            for (Widget &widget : mChildWidgets)
            {
                widget.setIsVisible(val);
            }
        }

        Widget *parent()
        {
            return mParent;
        }

        Widget *getChildWidget(string widgetId)
        {
            for (Widget &widget : mChildWidgets)
            {
                if (widget.getWidgetId() == widgetId)
                    return &widget;
            }
            return nullptr;
        }

        Widget *getChildWidgetByPersistentId(string persistentId)
        {
            for (Widget &widget : mChildWidgets)
            {
                if (widget.getPersistentId() == persistentId)
                    return &widget;
            }
            return nullptr;
        }

        //! Is called when a child is added to this widget. Use this method to manipulate child widgets after they are added
        virtual void onChildWidgetAdded(Widget &widget)
        {
        }

        //! Is called when all child widgets have been loaded to this widget by WidgetManager. Use this method to manipulate child widgets after they are added
        virtual void onChildWidgetsLoaded()
        {
        }

        void addChildWidget(Widget &widget)
        {
            addChildWidget(widget, false);
        }

        void addChildWidget(Widget &widget, bool hideTitleBar)
        {
            addChildWidgetDocked(widget, DockLocation::NONE, false, hideTitleBar);
        }

        void addChildWidgetDocked(Widget &widget, DockLocation dockLocation, bool usePadding, bool hideTitleBar)
        {
            removeChildWidget(widget);

            switch (dockLocation)
            {
            case DockLocation::TOP:
                widget.setX_Expr("${PARENT.ABSOLUTE_USABLE_X}" + std::string(!usePadding ? " - ${PADDING}" : ""));
                widget.setY_Expr("${PARENT.ABSOLUTE_USABLE_Y}" + std::string(!usePadding ? " - ${PADDING}" : ""));
                widget.setWidth_Expr("${PARENT.USABLE_WIDTH}" + std::string(!usePadding ? " + ${PADDING}*2" : ""));
                break;
            case DockLocation::BOTTOM:
                widget.setX_Expr("${PARENT.ABSOLUTE_USABLE_X}" + std::string(!usePadding ? " - ${PADDING}" : ""));
                widget.setY_Expr("${PARENT.BOTTOM} - ${SELF.HEIGHT}" + std::string(usePadding ? " - ${PADDING}" : ""));
                widget.setWidth_Expr("${PARENT.USABLE_WIDTH}" + std::string(!usePadding ? " + ${PADDING}*2" : ""));
                break;
            case DockLocation::LEFT:
                widget.setX_Expr("${PARENT.ABSOLUTE_USABLE_X}" + std::string(!usePadding ? " - ${PADDING}" : ""));
                widget.setY_Expr("${PARENT.ABSOLUTE_USABLE_Y}" + std::string(!usePadding ? " - ${PADDING}" : ""));
                widget.setHeight_Expr("${PARENT.USABLE_HEIGHT}" + std::string(!usePadding ? " + ${PADDING}*2" : ""));
                break;
            case DockLocation::RIGHT:
                widget.setX_Expr("${PARENT.RIGHT} - ${SELF.WIDTH}" + std::string(usePadding ? " - ${PADDING}" : ""));
                widget.setY_Expr("${PARENT.ABSOLUTE_USABLE_Y}" + std::string(!usePadding ? " - ${PADDING}" : ""));
                widget.setHeight_Expr("${PARENT.USABLE_HEIGHT}" + std::string(!usePadding ? " + ${PADDING}*2" : ""));
                break;
            case DockLocation::NONE:
                break;
            }

            if (dockLocation != DockLocation::NONE || hideTitleBar)
            {
                if (!usePadding)
                    widget.setIsRoundedRectangle(getIsRoundedRectangle());
                widget.setIsDraggable(false);
                widget.setIsResizable(false);
                widget.setTitleStyle(TitleStyle::NONE);
            }

            widget.mParent = this;
            widget.setIsShadow(false);
            mChildWidgets.push_back(widget);
            widget.setWasJustInitialized(true);
            ofAddListener(widget.widgetEventReceived, this, &Widget::onWidgetEventReceived);
            WidgetEventArgs args = WidgetEventArgs(getWidgetId(), *this);
            widget.onWidgetAddedAsChild(args);
            onChildWidgetAdded(widget);
        }

        void removeChildWidget(string widgetId)
        {

            for (Widget &widget : mChildWidgets)
            {
                if (widget.getWidgetId() == widgetId)
                {

                    ofRemoveListener(widget.widgetEventReceived, this, &Widget::onWidgetEventReceived);

                    std::vector<std::reference_wrapper<Widget>> childrenWidgets = widget.getChildWidgets();

                    widget.beforeDelete();
                    mChildWidgets.erase(std::remove_if(
                                            mChildWidgets.begin(), mChildWidgets.end(),
                                            [&](Widget &widget)
                                            {
                                                return widget.getWidgetId() == widgetId;
                                            }),
                                        mChildWidgets.end());

                    delete &widget;

                    for (Widget &childOfWidget : childrenWidgets)
                    {
#if VIZ_DEBUG_LEVEL >= 2 && VIZ_DEBUG_LEVEL < 3
                        cout << "<----[" << widget.getPersistentId() << "." << childOfWidget.getPersistentId() << "]~ChildWidget()\n";
#endif

                        childOfWidget.deleteThisWidget();
                        childOfWidget.clearChildWidgets();
                    }

                    return;
                }
            }
        }

        void removeChildWidgetByPersistentId(string persistentId)
        {
            for (Widget &widget : mChildWidgets)
            {
                if (widget.getPersistentId() == persistentId)
                {
                    removeChildWidget(widget.getWidgetId());
                    return;
                }
            }
        }

        void removeChildWidget(Widget &w)
        {
            for (Widget &widget : mChildWidgets)
            {
                if (widget.getWidgetId() == w.getWidgetId())
                {
                    removeChildWidget(w.getWidgetId());
                }
            }
        }

        Widget *findChildWidget(string widgetId)
        {
            for (Widget &childWidget : getChildWidgets())
            {
                Widget *w = findChildWidget(&childWidget, widgetId);
                if (w)
                    return w;
            }
            return nullptr;
        }

        Widget *findChildWidget(Widget *parentWidget, string widgetId)
        {
            if (parentWidget && parentWidget->matchesWidgetId(widgetId))
            {
                return parentWidget;
            }

            for (Widget &childWidget : parentWidget->getChildWidgets())
            {
                Widget *w = findChildWidget(&childWidget, widgetId);
                if (w)
                {
                    return w;
                }
            }
            return nullptr;
        }

        virtual std::vector<std::reference_wrapper<Widget>> getChildWidgets()
        {
            // Implemented by descendant class...
            return mChildWidgets;
        }

        bool hasActiveChildWidgets()
        {
            for (Widget &widget : getChildWidgets())
            {
                if (widget.getIsActiveWidget())
                {
                    return true;
                }
            }
            return false;
        }

        void updateWidget()
        {
            if (!getIsVisible())
            {
                setWidgetNeedsUpdate(false);
                return;
            }

            // if (getIsMinimized()) {
            //     setWidgetNeedsUpdate(false);
            //     updateMinimized();

            // }

            if (!widgetNeedsUpdate())
            {
                return;
            }
            // updateWidgetPopoutArrow();
            setWidgetNeedsUpdate(false);
            updateWidgetUI();
            if (!getIsMinimized())
            {
                update(this->context);
            }
            else
            {
                updateMinimized();
            }
            updateWidgetElements();
            this->context.isInitialUpdate = false;
        }

        void drawWidgetContentBlock(int x, int y, int width, int height)
        {
            getIsRoundedRectangle() ? vizElm_WIDGET_CONTENT_BLOCK.setRectRounded(scale(x), scale(y), scale(width), scale(height), getScaledPadding(), getTheme().WidgetContentColor) : vizElm_WIDGET_CONTENT_BLOCK.setRectangle(scale(x), scale(y), scale(width), scale(height), getTheme().WidgetContentColor);

            vizElm_WIDGET_CONTENT_BLOCK.draw();
        }

        void drawWidgetContentBlock()
        {
            getIsRoundedRectangle() ? vizElm_WIDGET_CONTENT_BLOCK.setRectRounded(scale(getUsableX()), scale(getUsableY()), scale(getUsableWidth()), scale(getUsableHeight()), getScaledPadding(), getTheme().WidgetContentColor) : vizElm_WIDGET_CONTENT_BLOCK.setRectangle(scale(getUsableX()), scale(getUsableY()), scale(getUsableWidth()), scale(getUsableHeight()), getTheme().WidgetContentColor);

            vizElm_WIDGET_CONTENT_BLOCK.draw();
        }

        void coreBeforeDraw()
        {
            if (!needsUpdate())
                setNeedsUpdate(needsUpdateBeforeDraw(true));
        }

        void coreDraw()
        {

            bool widgetContentUpdated = false;

            if (needsUpdate())
            {
                getWidgetTelemetry()->startUpdate(); //////////////////////////////////////////////////////////
                // This is needed for automatic resize widgets
                setNeedsUpdate(false);
                if (!getIsMinimized())
                {
                    update(this->context);
                }
                else
                {
                    updateMinimized();
                }
                updateWidgetElements();
                widgetContentUpdated = true;
                this->context.isInitialUpdate = false;
                getWidgetTelemetry()->endUpdate(); //////////////////////////////////////////////////////////
            }

            if (widgetNeedsUpdate())
            {
                updateWidget();
                updateWidgetUI();

                if (widgetContentUpdated || getWidgetStateWidth() != mFboAllocatedWidth || getWidgetStateHeight() != mFboAllocatedHeight)
                {
                    // Only update content if the bounds of the widget have changed
                    widgetContentUpdated = true;
                }
            }

            if (!getIsDragging() && !getIsResizing())
            {
                ofPushStyle();
                drawWidgetUI(!isTitleStatic());
                ofPopStyle();

                coreDrawWidgetContent(widgetContentUpdated);

                //--------------------------- TITLE HOVER (OVERLAY) -----------------------------------
                if (!isTitleStatic())
                {
                    ofPushStyle();
                    drawWidgetUITitle(false);
                    ofPopStyle();
                }
            }
            else
            {

                coreDrawWidgetContent(widgetContentUpdated);

                ofPushStyle();
                drawWidgetUI(false);
                ofPopStyle();
            }

            if (getIsActiveWidget() && getScrollAction() == ScrollAction::SCROLL)
                coreDrawWidgetScrollbars(getTheme().WidgetAccentHighContrast1Color_withAlpha(0.5f)); // drawWidgetScrollbars(getTitleColor(getContentChromeAlpha()));

            /*
                    if (getWidgetId() == getTargetDropWidgetId()) {
                        drawPotentiallyDropTargetIndication();
                    }
            */
        }

        void clearWidgetElements()
        {
            while (mWidgetElements.size() > 0)
            {
                for (WidgetElm &elm : mWidgetElements)
                {
                    removeWidgetElement(&elm);
                }
            }
        }

        void drawWidgetElements()
        {
            for (WidgetElm &elm : mWidgetElements)
            {

                if (elm.getIsVisible())
                {
                    elm.coreDraw();

                    if (getDidMoveFocus())
                    {
                        // Reset the flag, and jump out
                        setDidMoveFocus(false);
                        return;
                    }
                }
            }
        }

        virtual vector<reference_wrapper<WidgetElm>> getWidgetElements()
        {
            return mWidgetElements;
        }

        bool isWidgetElementActive()
        {
            // if (!getIsActiveWidget()) return false; // if mouse loses focus from one widget directly onto an ELM, it doesn't activate if this line is uncommented
            mTransitioningAlpha = false;
            for (WidgetElm &elm : mWidgetElements)
            {
                if (elm.needsUpdate())
                {
                    if (elm.parent())
                        elm.parent()->setNeedsUpdate(true);
                    elm.setNeedsUpdate(false);
                }
                if (elm.getIsFocused(true) && elm.parent()->isTopLevel())
                    return true; // focused and PARENT widget is active
                if (elm.isTransitioningAlpha())
                    mTransitioningAlpha = true;
                if (elm.getIsActiveWidget() || (elm.parent()->isTopLevel() && elm.isWidgetHovered()))
                {
                    // elm.setZindex(0);
                    // elm.getIsActiveWidget();
                    return true;
                }
            }
            return false;
        }

        void updateWidgetElements()
        {
            for (WidgetElm &elm : mWidgetElements)
            {
                recalculateElementExpressions(&elm);
                elm.setShouldRedetermineElementVisibility(true);
                if (elm.getIsVisible())
                {
                    elm.update(this->context);
                }
            }
        }

        void coreDrawWidgetContent(bool widgetContentUpdated)
        {
            if (!getUseFbo())
            {
                /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
                 Draw directly to screen
                 -------------------------
                 Slower, more beautiful
                 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
                getWidgetTelemetry()->startDraw(); //////////////////////////////////////////////////////////

                ofPushStyle();
                if (getIsMinimized())
                {
                    drawMinimized();
                }
                else
                {
                    draw(this->context);

                    ofPushStyle();
                    drawWidgetUITitle(false);
                    ofPopStyle();

                    drawWidgetElements();
                    this->context.isInitialDraw = false;
                }
                ofPopStyle();
                getWidgetTelemetry()->endDraw(); //////////////////////////////////////////////////////////
            }
            else
            {
                /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
                 Draw using a Frame Buffer
                 -------------------------
                 Slightly less beautiful (color accuracy), faster performance
                 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

                // NOTE: because we refresh the fbo if getIsActiveWidget(), it means that if the widget is
                // hovered, updates will take place... consider optimization in the future

                if ((getIsActiveWidget() && getIsAutoUpdateWhenActive()) || widgetContentUpdated || isWidgetElementActive() || isTransitioningAlpha())
                {

                    getWidgetTelemetry()->startDraw(); //////////////////////////////////////////////////////////

                    allocateFbo(false);

                    fbo.begin();
                    ofClear(0, 0, 0, 0);
                    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

                    ofPushMatrix();
                    ofTranslate(-1 * scale(getWidgetStateX()), -1 * scale(getWidgetStateY()));
                    ofPushStyle();
                    if (getIsMinimized())
                    {
                        drawMinimized();
                    }
                    else
                    {
                        draw(this->context);

                        ofPushStyle();
                        drawWidgetUITitle(false);
                        ofPopStyle();

                        drawWidgetElements();
                        this->context.isInitialDraw = false;
                    }
                    ofPopStyle();
                    ofPopMatrix();
                    fbo.end();

                    getWidgetTelemetry()->endDraw(); //////////////////////////////////////////////////////////
                }
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                fbo.draw(scale(getWidgetStateX()), scale(getWidgetStateY()), mFboAllocatedWidth, mFboAllocatedHeight);
                ofEnableAlphaBlending();
            }
        }

        virtual void drawWidget()
        {
            if (!getIsVisible())
            {
                return;
            }

            coreBeforeDraw();
            handleDrawEvents();
            coreDraw();
        }

        void handleDrawEvents()
        {
            WidgetEventArgs args = WidgetEventArgs(getWidgetId(), *this);

            bool hovered = isWidgetHovered();

            if (hovered)
            {
                setTargetDropWidgetId();
            }

            if (hovered && !getIsDragging() && !getIsResizing() && !getViz()->getIsAnyWidgetDraggingOrResizing())
            {
                setUpdatedSinceUnhovered(false);
                ofNotifyEvent(widgetHovered, args);

                if (getIsTitleAutohide())
                {
                    mDidUpdateForAutohide = false;
                }

                /*
                            todo
                            file picker save
                            color (transparrency of menus)
                            ne wproject wizzard
                            package DMG - missing files
                            RELEASE

                */

                WidgetBase *r = root();

                if ((isTopLevel() || getActiveWidgetId() == "") || (r && getViz()->getTopmostActiveRootWidgetId() == r->getWidgetId()))
                {
                    if (!getIsActiveWidget() && getIsNextInLineForFocus())
                    {
                        setActiveWidget();
                        onWidgetHovered(args);
                        /*

                        // At one point the above line and below commented were at odds with each other:
                        // With above line active and below commented, sometimes there are cases where
                        // context menu, when clicked would allow a widget (behind the top active widget)
                        // to "jump forward" in Zindex. Below fixed that. Howevever, with below uncommented
                        // context menus for clipboard dont respond, because it doesnt become active!!

                        if (parentWidget()) {
                            // Dealing with a child widget
                            if (parentWidget()->getIsActiveWidget()) setActiveWidget();
                        } else {
                            setActiveWidget();
                        }//*/

                        for (Widget &childWidget : getChildWidgets())
                        {
                            if (childWidget.isWidgetHovered())
                            {
                                getViz()->setActiveChildWidgetId(childWidget.getWidgetId());
                                childWidget.setUpdatedSinceUnhovered(false);
                            }
                            else
                            {
                                if (!childWidget.getUpdatedSinceUnhovered())
                                {
                                    childWidget.setNeedsUpdate(true);
                                    childWidget.setUpdatedSinceUnhovered(true);
                                    // cout << "updating child..." + childWidget.getPersistentId() + "_" + ofToString(ofGetSystemTimeMillis());
                                }
                            }
                        }
                    }

                    onWidgetMouseOver(args);
                    if (ofGetMousePressed())
                        ofNotifyEvent(widgetClicked, args);
                }
            }
            else if (!getViz()->getIsAnyWidgetDraggingOrResizing() && getIsActiveWidget())
            {
                clearTargetDropWidgetId();

                if (!getUpdatedSinceUnhovered())
                {
                    setNeedsUpdate(true);
                    setUpdatedSinceUnhovered(true);
                    // cout << "updating PARENT..." + getPersistentId() + "_" + ofToString(ofGetSystemTimeMillis());
                }

                ofNotifyEvent(widgetUnhovered, args);
                onWidgetUnhovered(args);
                if (ofGetMousePressed())
                {
                    ofNotifyEvent(widgetLostFocus, args);
                    clearActiveWidgetId();
                }
                else if (getIsUnhoverLoseFocus())
                {
                    clearActiveWidgetId();
                }
            }
            else
            {
                // This is an inactive widget
                if (!getIsActiveWidget() && getShouldCloseIfOtherWidgetClicked() && ofGetMousePressed() && !getIsDragging() && !getIsResizing())
                {
                    closeThisWidget("CLOSED_DEFOCUSED");
                }
            }
        }

        virtual vector<Icon> getWidgetTitleIcons()
        {
            // Override in descendant to modify...
            if (!getShowTitleIcons())
                return vector<Icon>();

            if (getShowWidgetPin())
            {
                return {
                    IconCache::getIcon("REG_WIDGET_CLOSE"),
                    getIsWidgetPinned() ? IconCache::getIcon("REG_REG_RADIO_BUTTON_CHECKED") : IconCache::getIcon("REG_REG_RADIO_BUTTON_UNCHECKED")};
            }
            else
            {
                return {
                    IconCache::getIcon("REG_WIDGET_CLOSE")};
            }
        }

        vector<Icon> getPositionedTitleIcons()
        {
            return mPositionedTitleIcons;
        }

        virtual vector<Icon> getWidgetTitleHoveredIcons()
        {
            // Override in descendant to modify...
            if (!getShowTitleIcons())
                return vector<Icon>();

            return getWidgetTitleIcons();
        }

        virtual vector<Icon> getWidgetTitleMinimizedIcons()
        {
            // Override in descendant to modify...
            if (!getShowTitleIcons())
                return vector<Icon>();

            return {
                IconCache::getIcon("REG_WIDGET_EXPAND_UP")};
        }

        virtual void titleIconClicked(int iconTag, Coord::vizBounds deScaledClickBounds)
        {
            switch (iconTag)
            {
            case IconCache::IconTag::WIDGET_CLOSE:
                closeThisWidget();
                break;
            case IconCache::IconTag::WIDGET_EXPAND_DOWN:
                setIsMinimized(true);
                break;
            case IconCache::IconTag::WIDGET_EXPAND_UP:
                setIsMinimized(false);
                break;
            case IconCache::IconTag::WIDGET_PIN:
                setIsWidgetPinned(false);
                break;
            case IconCache::IconTag::WIDGET_UNPIN:
                setIsWidgetPinned(true);
                break;
            }
        }

        virtual void titleIconHovered(int iconTag, Coord::vizBounds deScaledClickBounds)
        {

            if (!getShowPopoutWidgetWhenTitleMenuIconHovered())
                return;

            Widget *w = getPopoutWidgetForMenuTag(iconTag);
            if (w != nullptr && w != NULL)
            {

                w->setMinWidth(8);
                w->setMinHeight(8);

                w->setWidgetMinimizedSize(deScale(deScaledClickBounds.width),
                                          deScale(deScaledClickBounds.height));

                w->setWidgetBounds(deScaledClickBounds.x,
                                   deScaledClickBounds.y,
                                   deScaledClickBounds.width,
                                   deScaledClickBounds.height);

                w->setIsVisible(true);
                w->setNeedsUpdate(true);
                w->setActiveWidget();
            }
        }

        bool getShowPopoutWidgetWhenTitleMenuIconHovered()
        {
            return mShowPopoutWidgetWhenTitleMenuIconHovered;
        }

        void setShowPopoutWidgetWhenTitleMenuIconHovered(bool val)
        {
            mShowPopoutWidgetWhenTitleMenuIconHovered = val;
        }

        virtual Widget *getPopoutWidgetForMenuTag(int menuTag)
        {
            // Override in descendant...
            return nullptr;
        }

        PopoutDirection getSuggestedPopoutDirectionFromWidgetTitleBar()
        {
            if (isTitleTop())
            {
                return PopoutDirection::DOWN;
            }
            else if (isTitleBottom())
            {
                return PopoutDirection::UP;
            }
            else if (isTitleLeft())
            {
                return PopoutDirection::RIGHT;
            }
            else if (isTitleRight())
            {
                return PopoutDirection::LEFT;
            }
            return PopoutDirection::DOWN;
        }

        Coord::vizBounds getPopoutScaledBoundsForTitleIcon(int iconTag)
        {

            Icon icon = getWidgetTitleIconByTag(iconTag);

            Coord::vizBounds iconBounds(0, 0, 0, 0);

            if (isTitleTop())
            {
                iconBounds = Coord::vizBounds(
                    icon.getScaledBounds().x + icon.getScaledBounds().width / 2,
                    icon.getScaledBounds().y + icon.getScaledBounds().height + getScaledPadding() * 4,
                    icon.getScaledBounds().width,
                    icon.getScaledBounds().height);
            }
            else if (isTitleBottom())
            {
                iconBounds = Coord::vizBounds(
                    icon.getScaledBounds().x + icon.getScaledBounds().width / 2,
                    icon.getScaledBounds().y - getScaledPadding() * 4,
                    icon.getScaledBounds().width,
                    icon.getScaledBounds().height);
            }
            else if (isTitleLeft())
            {
                iconBounds = Coord::vizBounds(
                    icon.getScaledBounds().x + icon.getScaledBounds().width + getScaledPadding() * 4,
                    icon.getScaledBounds().y + icon.getScaledBounds().height / 2,
                    icon.getScaledBounds().width,
                    icon.getScaledBounds().height);
            }
            else if (isTitleRight())
            {
                iconBounds = Coord::vizBounds(
                    icon.getScaledBounds().x - getScaledPadding() * 4,
                    icon.getScaledBounds().y + icon.getScaledBounds().height / 2,
                    icon.getScaledBounds().width,
                    icon.getScaledBounds().height);
            }
            return iconBounds;
        }

        virtual void showContextMenu(int menuTag, int x, int y)
        {

            Widget *contextMenuWidget = getPopoutWidgetForMenuTag(menuTag);

            // Show it!
            if (contextMenuWidget != nullptr)
            {
                contextMenuWidget->setPopoutWidgetDirection(PopoutDirection::NONE);
                // contextMenuWidget->setTheme(getViz()->getThemeManager()->getContrastingSystemTheme(getTheme(), true));
                contextMenuWidget->setTheme(getViz()->getThemeManager()->getThemeForPopout());

                contextMenuWidget->setWidgetPosition(x, y, false);
                contextMenuWidget->setShowingAsPopout(true);
                contextMenuWidget->setIsVisible(true);
                contextMenuWidget->setActiveWidget();
                contextMenuWidget->setNeedsUpdate(true);
                contextMenuWidget->setWidgetNeedsUpdate(true);
                // onContentWidgetUnhovered have a pin icon for menu
                //  if pinned, don't hide when lost focus, otherwise do hide!
                //  add select key custom content to menu
                //  write a function to determine if context menu is full obscured, if so, hide it!

                // tweak getPopoutWidgetForHoveredTitleIcon to return correct widget (eg the switch statement above), then can use it
            }
        }

        void allocateFbo(bool initialize)
        {

            if (!getUseFbo())
                return;

            // Do we really need to allocate ?
            if (!initialize && mFboAllocatedWidth == scale(getWidgetStateWidth()) && mFboAllocatedHeight == scale(getWidgetStateHeight()))
                return;

            // $$$ EXPENSIVE allocate fbo $$$
            mFboAllocatedWidth = scale(getWidgetStateWidth());
            mFboAllocatedHeight = scale(getWidgetStateHeight());

            if (mFboAllocatedWidth == 0 || mFboAllocatedHeight == 0)
                return;

#ifdef TARGET_OPENGLES
            fbo.allocate(mFboAllocatedWidth, mFboAllocatedHeight, GL_RGBA);
#else
            switch (getFboMode())
            {
            case FboMode::LOW:
                // Use only one fbo with GL_RGBA - the introduced alpha channel reduces the colour somewhat and
                // does not look pristine. In addition, use only 8 samples
                fbo.allocate(mFboAllocatedWidth, mFboAllocatedHeight);
                break;
            case FboMode::MEDIUM:
                // Use only one fbo with GL_RGBA - the introduced alpha channel reduces the colour somewhat and
                // does not look pristine. Use 16 samples
                fbo.allocate(mFboAllocatedWidth, mFboAllocatedHeight, GL_RGBA, 16);
                break;
            case FboMode::HIGH:
                // Use 2 x fbos, one dedicated to content and one dedicated to widget chrome. Colors look much better
                // because the alpha channel is not used as much in the content, thereform making them more accurate
                fbo.allocate(mFboAllocatedWidth, mFboAllocatedHeight, GL_RGBA, 16); // GL_RGBA32F, 32);
                break;
            case FboMode::ULTRA:
                // Use 2 x fbos, one dedicated to content and one dedicated to widget chrome. Colors look much better
                // because the alpha channel is not used as much in the content, thereform making them more accurate
                // in addition,
                // TODO: OVERSAMPLING
                fbo.allocate(mFboAllocatedWidth, mFboAllocatedHeight, GL_RGBA, 16);
                break;
            }
#endif

            if (initialize)
            {
                fbo.begin();
                ofClear(255, 255, 255, 0);
                fbo.end();
            }
        }

        AdditionalState getAdditionalState()
        {
            return mAdditionalState;
        }

        void setAdditionalState(AdditionalState additionalState)
        {
            mAdditionalState = additionalState;
        }

        bool getIsVisible() override
        {

            return WidgetBase::getIsVisible();
        }

        virtual void setIsVisible(bool value) override
        {
            WidgetBase::setIsVisible(value);
            if (!value)
            {
                if (getIsActiveWidget())
                    clearActiveWidgetId();
            }
            else
            {
                setWasJustInitialized(true);
            }
        }

        bool getIsMinimized()
        {
            return mIsMinimized;
        }

        void setIsMinimized(bool value)
        {
            mIsMinimized = value;
            scaleUITitleSize();
            setWidgetNeedsUpdate(true);
            setNeedsUpdate(true);
        }

        bool getDrawWidgetChrome()
        {
            return mDrawWidgetChrome;
        }

        void setDrawWidgetChrome(bool value)
        {
            mDrawWidgetChrome = value;
        }

        bool getDrawWidgetTitle()
        {
            return mDrawWidgetTitle;
        }

        void setDrawWidgetTitle(bool value)
        {
            mDrawWidgetTitle = value;
        }

        float getCurrentWidgetAlpha()
        {
            return mUICurrentWidgetAlpha;
        }

        // Read from config in due course
        ofColor getDividerColor()
        {
            return ofColor::lightGray;
        }

        bool getDrawnByWidgetManager()
        {
            return mDrawnByWidgetManager;
        }

        void setDrawnByWidgetManager(bool value)
        {
            mDrawnByWidgetManager = value;
        }

        bool isWidgetUICompletelyTransparent()
        {
            return mUICurrentWidgetAlpha <= 0;
        }

        bool getIsTitleAutohide()
        {
            return mIsTitleAutohide;
        }

        void setIsTitleAutohide(bool val)
        {
            mIsTitleAutohide = val;
        }

        bool getShowTitleIcons()
        {
            return mShowTitleIcons;
        }

        void setShowTitleIcons(bool val)
        {
            mShowTitleIcons = val;
        }

        int getFboNeedsUpdate()
        {
            return mFboNeedsUpdate;
        }

        void setFboNeedsUpdate(bool val)
        {
            mFboNeedsUpdate = val;
        }

        int getWidgetStateX()
        {
            if (getIsMinimized())
            {
                return getX() + mMinimizedOffsetX;
            }
            else
            {
                return getX() + mNonMinimizedOffsetX;
            }
        }

        int getWidgetStateY()
        {
            if (getIsMinimized())
            {
                return getY() + mMinimizedOffsetY;
            }
            else
            {
                return getY() + mNonMinimizedOffsetY;
            }
        }

        virtual int getWidgetStateWidth() override
        {
            if (getIsMinimized())
            {
                return mMinimizedWidth + getNonScaledPadding() * 2.0f;
            }
            else
            {
                return getWidth();
            }
        }

        virtual int getWidgetStateHeight() override
        {
            if (getIsMinimized())
            {
                return mMinimizedHeight + getNonScaledPadding() * 2.0f;
            }
            else
            {
                return getHeight();
            }
        }

        virtual int getUsableX() override
        {
            return getUsableX(true);
        }

        virtual int getUsableY() override
        {
            return getUsableY(true);
        }

        virtual int getUsableX(bool inclScrollOffset) override
        {
            int scrollOffsetX = inclScrollOffset ? getScrollOffsetX() : 0;
            if (getIsMinimized() || !getDrawWidgetChrome())
            {
                return getWidgetStateX() + getNonScaledPadding() + scrollOffsetX; // Todo make configurable
            }
            else
            {
                if (mTitleStyle == TitleStyle::LEFT_WITH_WIDGET_RESIZE)
                {
                    return getWidgetStateX() + deScale(mUICurrentTitleBarSize) + getNonScaledPadding() + scrollOffsetX;
                }
                else if (mTitleStyle == TitleStyle::LEFT_OVERLAY_FULL || mTitleStyle == TitleStyle::LEFT_OVERLAY || mTitleStyle == TitleStyle::LEFT_STATIC)
                {
                    return getWidgetStateX() + getUnhoveredTitleBarSize(false) + getNonScaledPadding() + scrollOffsetX;
                }
                else if (mTitleStyle == TitleStyle::RIGHT_WITH_WIDGET_RESIZE)
                {
                    return getWidgetStateX() + getNonScaledPadding() + scrollOffsetX;
                }
                else
                { // (mTitleStyle == RIGHT_OVERLAY_FULL || mTitleStyle == RIGHT_OVERLAY || mTitleStyle == RIGHT_STATIC ||  mTitleStyle == NONE) {
                    return getWidgetStateX() + getNonScaledPadding() + scrollOffsetX;
                }
            }
        }

        virtual int getUsableY(bool inclScrollOffset) override
        {
            int scrollOffsetY = inclScrollOffset ? getScrollOffsetY() : 0;
            if (getIsMinimized() || !getDrawWidgetChrome())
            {
                return getWidgetStateY() + getNonScaledPadding() + scrollOffsetY; // Todo make configurable
            }
            else
            {
                if (mTitleStyle == TitleStyle::TOP_WITH_WIDGET_RESIZE)
                {
                    return getWidgetStateY() + deScale(mUICurrentTitleBarSize) + getNonScaledPadding() + scrollOffsetY;
                }
                else if (mTitleStyle == TitleStyle::TOP_OVERLAY_FULL || mTitleStyle == TitleStyle::TOP_OVERLAY || mTitleStyle == TitleStyle::TOP_STATIC)
                {
                    return getWidgetStateY() + getUnhoveredTitleBarSize(false) + getNonScaledPadding() + scrollOffsetY;
                }
                else if (mTitleStyle == TitleStyle::BOTTOM_WITH_WIDGET_RESIZE)
                {
                    return getWidgetStateY() + getNonScaledPadding() + scrollOffsetY;
                }
                else
                { // (mTitleStyle == BOTTOM_OVERLAY_FULL || mTitleStyle == BOTTOM_OVERLAY || mTitleStyle == BOTTOM_STATIC ||  mTitleStyle == NONE) {
                    return getWidgetStateY() + getNonScaledPadding() + scrollOffsetY;
                }
            }
        }

        int getCoreUsableWidth()
        {
            if (getIsMinimized())
            {
                return mMinimizedWidth;
            }
            else
            {
                if (mTitleStyle == TitleStyle::LEFT_WITH_WIDGET_RESIZE || mTitleStyle == TitleStyle::RIGHT_WITH_WIDGET_RESIZE)
                {
                    return getWidth() - deScale(mUICurrentTitleBarSize) - getNonScaledPadding() * 2.0f;
                }
                else
                {
                    return getWidth() - ((isTitleTop() || isTitleBottom() || isTitleNone() || !getDrawWidgetTitle()) ? 0 : getUnhoveredTitleBarSize(false)) - getNonScaledPadding() * 2.0f;
                }
            }
        }

        int getCoreUsableHeight()
        {
            if (getIsMinimized())
            {
                return mMinimizedHeight;
            }
            else
            {
                if (mTitleStyle == TitleStyle::TOP_WITH_WIDGET_RESIZE || mTitleStyle == TitleStyle::BOTTOM_WITH_WIDGET_RESIZE)
                {
                    return getHeight() - deScale(mUICurrentTitleBarSize) - getNonScaledPadding() * 2.0f;
                }
                else
                {
                    return getHeight() - ((isTitleLeft() || isTitleRight() || isTitleNone() || !getDrawWidgetTitle()) ? 0 : getUnhoveredTitleBarSize(false)) - getNonScaledPadding() * 2.0f;
                }
            }
        }

        virtual int getUsableWidth() override
        {
            return getCoreUsableWidth();
        }

        virtual int getUsableHeight() override
        {
            return getCoreUsableHeight();
        }

        int getUsableWidthIncludingTitle()
        {
            return getCoreUsableWidth() + getUnhoveredTitleBarSize(false);
        }

        int getUsableHeightIncludingTitle()
        {
            return getCoreUsableHeight() + getUnhoveredTitleBarSize(false);
        }

        void setOffsetsX(float minimizedOffsetX, float nonMinimizedOffsetX)
        {
            mMinimizedOffsetX = minimizedOffsetX;
            mNonMinimizedOffsetX = nonMinimizedOffsetX;
        }

        void setOffsetsY(float minimizedOffsetY, float nonMinimizedOffsetY)
        {
            mMinimizedOffsetY = minimizedOffsetY;
            mNonMinimizedOffsetY = nonMinimizedOffsetY;
        }

        void setPosition(float x, float y)
        {
            float offsetX = getIsMinimized() ? mMinimizedOffsetX : mNonMinimizedOffsetX;
            float offsetY = getIsMinimized() ? mMinimizedOffsetY : mNonMinimizedOffsetY;

            int clampX = x;
            clampX = std::max((float)clampX - offsetX, (float)getScreenPadding());
            clampX = std::min((float)clampX, (float)(deScale(ofGetWindowWidth()) - getScreenPadding() - getWidgetStateWidth()));
            setX(clampX);

            int clampY = y;
            clampY = std::max((float)clampY - offsetY, (float)getScreenPadding());
            clampY = std::min((float)clampY, (float)(deScale(ofGetWindowHeight()) - getScreenPadding() - getWidgetStateHeight()));
            setY(clampY);
        }

        void applyDockingLogic()
        {

            if (getX_Expr() == "" && WidgetBase::getX() >= (deScale(ofGetWindowWidth()) - getScreenPadding() - getWidgetStateWidth()))
            {
                setX_Expr("${WINDOW.WIDTH} - ${SELF.WIDTH}");
                setX_ExprWasSetProgramatically(true);
            }
            if (getY_Expr() == "" && WidgetBase::getY() >= (deScale(ofGetWindowHeight()) - getScreenPadding() - getWidgetStateHeight()))
            {
                setY_Expr("${WINDOW.HEIGHT} - ${SELF.HEIGHT}");
                setY_ExprWasSetProgramatically(true);
            }

            if (getWidth_Expr() == "" && getWidgetStateWidth() >= (deScale(ofGetWindowWidth()) - getScreenPadding() * 2))
            {
                setWidth_Expr("${WINDOW.WIDTH}");
                setWidth_ExprWasSetProgramatically(true);
            }
            if (getHeight_Expr() == "" && getWidgetStateHeight() >= (deScale(ofGetWindowHeight()) - getScreenPadding()))
            {
                setHeight_Expr("${WINDOW.HEIGHT}");
                setHeight_ExprWasSetProgramatically(true);
            }
        }

        virtual void setWidgetPosition(float x, float y, bool enableDockingLogic) override
        {
            if (getX() != x || getY() != y)
            {
                setPosition(x, y);
                if (enableDockingLogic)
                    applyDockingLogic();
                setWidgetNeedsUpdate(true);
            }
        }

        virtual Elm::vizRect setSize(int width, int height) override
        {
            return WidgetBase::setSize(width, height);
        }

        void setPopoutWidgetDirection(PopoutDirection val)
        {
            mPopoutDirection = val;
        }

        void setWidgetSize(int width, int height, bool enableDockingLogic, bool widthSetByExpression, bool heightSetByExpression)
        {

            bool widthChanged = getWidth() != width;
            bool heightChanged = getHeight() != height;

            if (widthChanged || heightChanged ||
                (getLockAspectRatio() && height != width * getAspectRatio()))
            {

                mWidthChangedByExpression = widthSetByExpression && widthChanged;
                mHeightChangedByExpression = heightSetByExpression && heightChanged;

                if (mPopoutDirection != PopoutDirection::NONE && getShowingAsPopout())
                {
                    int min_ArrowX = deScale(vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE.getTriangle().x3 + getScaledPadding()); // For resizing from the left
                    int min_ArrowY = deScale(vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE.getTriangle().y3 + getScaledPadding());
                    int max_ArrowX = deScale(vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE.getTriangle().x1 + getScaledPadding());
                    int max_ArrowY = deScale(vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE.getTriangle().y1 + getScaledPadding());

                    int clampedWidth = min_ArrowX - getX() + getNonScaledPadding();
                    int clampedHeight = min_ArrowY - getY() + getNonScaledPadding();

                    switch (mPopoutDirection)
                    {
                    case PopoutDirection::DOWN:
                        clampedWidth = max((int)width, (int)clampedWidth);
                        clampedHeight = height;
                        break;
                    case PopoutDirection::UP:
                        clampedWidth = max((int)width, (int)clampedWidth);
                        // WIP : setY(getY() + getHeight() - height - 15);
                        // Cant resize for now when popped from bottom (TODO: FIX)
                        clampedHeight = getHeight();
                        break;
                    case PopoutDirection::LEFT:
                        // Cant resize for now when popped from left (TODO: FIX)
                        clampedWidth = getWidth();
                        clampedHeight = max((int)height, (int)clampedHeight);
                        break;
                    case PopoutDirection::RIGHT:
                        clampedWidth = width;
                        clampedHeight = max((int)height, (int)clampedHeight);
                        break;
                    }
                    WidgetBase::setWidgetSize(clampedWidth, clampedHeight, false);
                }
                else
                {
                    WidgetBase::setWidgetSize(width, height, enableDockingLogic);
                    if (enableDockingLogic)
                        applyDockingLogic();
                }
                setWidgetNeedsUpdate(true);

                for (WidgetElm &elm : mWidgetElements)
                {
                    // ofResizeEventArgs e(width, height);
                    // elm.onCoreWidgetResized(e);
                    ofResizeEventArgs args(width, height);
                    ofNotifyEvent(elm.widgetResized, args);
                }
            }
        }

        void setWidgetSize(int width, int height, bool enableDockingLogic) override
        {
            mWidthChangedByExpression = false;
            mHeightChangedByExpression = false;
            setWidgetSize(width, height, enableDockingLogic, false, false);
        }

        bool getWidthChangedByExpression()
        {
            return mWidthChangedByExpression;
        }

        bool getHeightChangedByExpression()
        {
            return mHeightChangedByExpression;
        }

        void setWidgetBounds(float x, float y, float width, float height)
        {
            if (getX() != x || getY() != y || getWidth() != width || getHeight() != height)
            {
                setPosition(x, y);
                setSize(width, height);
                setWidgetNeedsUpdate(true);
            }
        }

        void setMinimizedSize(int width, int height)
        {
            mMinimizedWidth = width;
            mMinimizedHeight = height;
        }

        void setWidgetMinimizedSize(float width, float height)
        {
            setMinimizedSize(width, height);
            setWidgetNeedsUpdate(true);
        }

        virtual void setTitle(string title) override
        {
            WidgetBase::setTitle(title);
            mDrawableWidgetTitle = title;
        }

        void setTitleStyle(TitleStyle titleStyle)
        {
            mTitleStyle = titleStyle;
        }

        TitleStyle getTitleStyle()
        {
            return mTitleStyle;
        }

        int getStaticTitleBarSize(bool scaled)
        {
            return scaled ? scale(mStaticTitleBarSize) : mStaticTitleBarSize;
        }

        void setStaticTopTitleBarSize(int val)
        {
            setTitleStyle(TitleStyle::TOP_STATIC);
            mStaticTitleBarSize = val;
            scaleUITitleSize();
        }

        void setStaticBottomTitleBarSize(int val)
        {
            setTitleStyle(TitleStyle::BOTTOM_STATIC);
            mStaticTitleBarSize = val;
            scaleUITitleSize();
        }

        void setStaticLeftTitleBarSize(int val)
        {
            setTitleStyle(TitleStyle::LEFT_STATIC);
            mStaticTitleBarSize = val;
            scaleUITitleSize();
        }

        void setStaticRightTitleBarSize(int val)
        {
            setTitleStyle(TitleStyle::RIGHT_STATIC);
            mStaticTitleBarSize = val;
            scaleUITitleSize();
        }

        int getUnhoveredTitleBarSize(bool scaled)
        {
            if (mTitleStyle == TitleStyle::NONE)
            {
                return (scaled ? getScaledPadding() : getNonScaledPadding());
            }
            else
            {
                if (getIsTitleAutohide() && !isWidgetHovered() && !getIsResizing())
                {
                    return 0;
                }
                else
                {
                    // return (scaled ? getScaledPadding() : getNonScaledPadding()) * 4;
                    return getStaticTitleBarSize(scaled);
                }
            }
        }

        int getHoveredTitleBarSize(bool scaled)
        {
            if (getIsMinimized())
            {
                if (isTitleLeft() || isTitleRight())
                {
                    return scaled ? scale(getWidgetStateWidth()) : getWidgetStateWidth();
                }
                else
                {
                    return scaled ? scale(getWidgetStateHeight()) : getWidgetStateHeight();
                }
            }
            else
            {
                if (mTitleStyle == TitleStyle::TOP_OVERLAY_FULL || mTitleStyle == TitleStyle::BOTTOM_OVERLAY_FULL)
                {
                    return widgetFrame.getRectangle().height;
                }
                else if (mTitleStyle == TitleStyle::LEFT_OVERLAY_FULL || mTitleStyle == TitleStyle::RIGHT_OVERLAY_FULL)
                {
                    return widgetFrame.getRectangle().width;
                }
                else
                {
                    return (scaled ? getScaledPadding() : getNonScaledPadding()) * 8; // Or some user defined value!
                }
            }
        }

        void setZindex(int zIndex) override
        {
            WidgetBase::setZindex(zIndex);
            if (getWidgetElements().size() > 0)
            {
                for (WidgetElm &widgetElm : getWidgetElements())
                {
                    widgetElm.setZindex(zIndex);
                }
            }
        }

        void setFboMode(FboMode fboMode)
        {
            mFboMode = fboMode;
        }

        FboMode getFboMode()
        {
            return mFboMode;
        }

        bool getIsHighOrUltraFboMode()
        {
            return mFboMode == FboMode::HIGH || mFboMode == FboMode::ULTRA;
        }

        // vector of widgets

        void connectWidget(Widget widget)
        {
            /* Use this to connect another widget to this widget instance eg

            ~~~~~~      ~~~~~~~~~~
            | ME | <--- | WIDGET |
            ~~~~~~      ~~~~~~~~~~

             */
        }

        void connectToWidget(Widget widget)
        {
            /* Use this to connect this instance to another widget ie

            ~~~~~~~~~~      ~~~~~~
            | WIDGET | <--- | ME |
            ~~~~~~~~~~      ~~~~~~


             */
        }

        virtual void saveWidgetXMLToFile(string fileName)
        {
            ofxXmlSettings xml = saveState();
            xml.saveFile(fileName);
        }

        virtual ofPixels saveWidgetContentsToPixels(WidgetContext::DrawContext drawContext)
        {
            TitleStyle origTitleStyle = getTitleStyle();

            // populate the fbo
            setTitleStyle(TitleStyle::NONE);
            this->context.isInitialDraw = true;
            this->context.isInitialUpdate = true;
            this->context.drawContext = drawContext;
            if (!getIsMinimized())
            {
                update(this->context);
            }
            else
            {
                updateMinimized();
            }
            coreDrawWidgetContent(true);
            this->context.drawContext = drawContext;
            setTitleStyle(origTitleStyle);

            ofPixels pixels;
            pixels.allocate(scale(getWidth()), scale(getHeight()), OF_IMAGE_COLOR_ALPHA);
            fbo.readToPixels(pixels);
            this->context.isInitialDraw = false;
            this->context.isInitialUpdate = false;
            return pixels;
        }

        virtual void saveWidgetContentsToImageFile(string fileName, WidgetContext::DrawContext drawContext)
        {
            ofPixels pixels = saveWidgetContentsToPixels(drawContext);
            ofSaveImage(pixels, fileName, OF_IMAGE_QUALITY_BEST);
        }

        virtual void saveWidgetContentsToBuffer(ofBuffer &buffer, WidgetContext::DrawContext drawContext)
        {
            ofPixels pixels = saveWidgetContentsToPixels(drawContext);
            ofImage image;
            image.setFromPixels(pixels);
            image.save(buffer, OF_IMAGE_FORMAT_PNG, OF_IMAGE_QUALITY_BEST);
        }

        virtual void saveWidgetContentsToHTMLFile(string fileName)
        {
        }

        virtual ofFbo saveWidgetContentsToFbo() override
        {
            TitleStyle origTitleStyle = getTitleStyle();
            setTitleStyle(TitleStyle::NONE);

            bool origFboVal = getUseFbo();
            setUseFbo(false);

            this->context.isInitialDraw = true;
            this->context.isInitialUpdate = true;
            this->context.drawContext = WidgetContext::IMAGE;
            if (widgetNeedsUpdate())
            {
                updateWidget();
            }
            if (!getIsMinimized())
            {
                update(this->context);
            }
            else
            {
                updateMinimized();
            }

            ofFbo fboCopy;
#ifdef TARGET_OPENGLES
            fboCopy.allocate(scale(getWidth()), scale(getHeight()), GL_RGBA);
#else
            fboCopy.allocate(scale(getWidth()), scale(getHeight()), GL_RGBA, 16); // GL_RGBA32F, 32);
#endif
            fboCopy.begin();
            ofClear(0, 0, 0, 0);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            ofPushMatrix();
            ofTranslate(-1 * scale(getWidgetStateX()), -1 * scale(getWidgetStateY()));

            coreDrawWidgetContent(true);
            this->context.drawContext = WidgetContext::SCREEN;
            this->context.isInitialDraw = false;
            this->context.isInitialUpdate = false;
            fboCopy.end();

            setTitleStyle(origTitleStyle);
            setUseFbo(origFboVal);

            return fboCopy;
        }

        void onCoreWidgetScaleChanged(float &scale) override
        {
            scaleUITitleSize();
            setWidgetNeedsUpdate(true);
            setNeedsUpdate(true);

            mFboAllocatedWidth = 0;
            mFboAllocatedHeight = 0;

            allocateFbo(true);
            onWidgetScaleChanged(scale);
            initWidgetScale(scale);
            // initWidget(); // this causes much gammy-ness with widgdet states when resizing!

            setExpressionRecalculationRequired(true);

            for (WidgetElm &elm : mWidgetElements)
            {
                elm.initWidgetScale(scale);
                // elm.initWidget(); // this causes much gammy-ness with widgdet states when resizing!
            }

            updateWidgetPopoutArrow();
        }

        virtual void onCoreWidgetVisibilityChanged(bool visible)
        {
            for (Widget &childWidget : getChildWidgets())
            {
                childWidget.onWidgetVisibilityChanged(visible);
            }
        }

        void showChildWithLayoutExpressions(string xExpr, string yExpr, string widthExpr, string heightExpr, bool popout)
        {
            setIsDraggable(false);
            setShowingAsPopout(popout);

            setX_Expr(xExpr);
            setY_Expr(yExpr);
            setWidth_Expr(widthExpr);
            setHeight_Expr(heightExpr);

            setIsAutoResizeToContentsWidth(false);
            setIsAutoResizeToContentsHeight(false);

            if (parentWidget())
            {
                setExpressionRecalculationRequired(true);
                parentWidget()->setExpressionRecalculationRequired(true);
            }

            setIsVisible(true);
        }

        bool isPopoutFromTitle()
        {
            switch (mPopoutDirection)
            {
            case PopoutDirection::DOWN:
                return isTitleTop();
            case PopoutDirection::UP:
                return isTitleBottom();
            case PopoutDirection::LEFT:
                return isTitleRight();
            case PopoutDirection::RIGHT:
                return isTitleLeft();
            }
            return false;
        }

        ofColor getWidgetStateColor(float alpha)
        {

            switch (mAdditionalState)
            {
            case AdditionalState::SELECTED:
                return getTheme().WidgetColor_withAlpha(getTheme().HoveredWidgetAlpha);

            default:
            {
                return getTheme().WidgetColor_withAlpha(alpha);
            }
            }
        }

        ofColor getTitleStateColor()
        {
            switch (mAdditionalState)
            {
            default:
                return getTheme().TitleColor_withAlpha(isWidgetHovered() && getIsActiveWidget() ? getTheme().HoveredTitleAlpha : getTheme().UnhoveredTitleAlpha);
            }
        }

        ofColor getTitleStateGradientColor1()
        {
            switch (mAdditionalState)
            {
            default:
                return getTheme().TitleGradientColor1_withAlpha(isWidgetHovered() && getIsActiveWidget() ? getTheme().HoveredTitleAlpha : getTheme().UnhoveredTitleAlpha);
            }
        }

        ofColor getTitleStateGradientColor2()
        {
            switch (mAdditionalState)
            {
            default:
                return getTheme().TitleGradientColor2_withAlpha(isWidgetHovered() && getIsActiveWidget() ? getTheme().HoveredTitleAlpha : getTheme().UnhoveredTitleAlpha);
            }
        }

        ofColor getTitleStateGradientColor3()
        {
            switch (mAdditionalState)
            {
            default:
                return getTheme().TitleGradientColor3_withAlpha(isWidgetHovered() && getIsActiveWidget() ? getTheme().HoveredTitleAlpha : getTheme().UnhoveredTitleAlpha);
            }
        }

        ofColor getTitleStateGradientColor4()
        {
            switch (mAdditionalState)
            {
            default:
                return getTheme().TitleGradientColor4_withAlpha(isWidgetHovered() && getIsActiveWidget() ? getTheme().HoveredTitleAlpha : getTheme().UnhoveredTitleAlpha);
            }
        }

        ofColor getTitleFontStateColor()
        {

            switch (mAdditionalState)
            {
            default:
                return getTheme().TitleFontColor_withAlpha(isWidgetHovered() && getIsActiveWidget() ? getTheme().HoveredTitleFontAlpha : getTheme().UnhoveredTitleFontAlpha);
            }
        }

        bool isTitleOverlay()
        {
            return (mTitleStyle == TitleStyle::TOP_OVERLAY || mTitleStyle == TitleStyle::TOP_OVERLAY_FULL || mTitleStyle == TitleStyle::BOTTOM_OVERLAY || mTitleStyle == TitleStyle::BOTTOM_OVERLAY_FULL ||
                    mTitleStyle == TitleStyle::LEFT_OVERLAY || mTitleStyle == TitleStyle::LEFT_OVERLAY_FULL || mTitleStyle == TitleStyle::RIGHT_OVERLAY || mTitleStyle == TitleStyle::RIGHT_OVERLAY_FULL);
        }

        bool isTitleResize()
        {
            return (mTitleStyle == TitleStyle::TOP_WITH_WIDGET_RESIZE || mTitleStyle == TitleStyle::BOTTOM_WITH_WIDGET_RESIZE ||
                    mTitleStyle == TitleStyle::LEFT_WITH_WIDGET_RESIZE || mTitleStyle == TitleStyle::RIGHT_WITH_WIDGET_RESIZE);
        }

        bool isTitleTop()
        {
            return (mTitleStyle == TitleStyle::TOP_STATIC || mTitleStyle == TitleStyle::TOP_OVERLAY ||
                    mTitleStyle == TitleStyle::TOP_OVERLAY_FULL || mTitleStyle == TitleStyle::TOP_WITH_WIDGET_RESIZE);
        }

        bool isTitleBottom()
        {
            return (mTitleStyle == TitleStyle::BOTTOM_STATIC || mTitleStyle == TitleStyle::BOTTOM_OVERLAY ||
                    mTitleStyle == TitleStyle::BOTTOM_OVERLAY_FULL || mTitleStyle == TitleStyle::BOTTOM_WITH_WIDGET_RESIZE);
        }

        bool isTitleLeft()
        {
            return (mTitleStyle == TitleStyle::LEFT_STATIC || mTitleStyle == TitleStyle::LEFT_OVERLAY ||
                    mTitleStyle == TitleStyle::LEFT_OVERLAY_FULL || mTitleStyle == TitleStyle::LEFT_WITH_WIDGET_RESIZE);
        }

        bool isTitleRight()
        {
            return (mTitleStyle == TitleStyle::RIGHT_STATIC || mTitleStyle == TitleStyle::RIGHT_OVERLAY ||
                    mTitleStyle == TitleStyle::RIGHT_OVERLAY_FULL || mTitleStyle == TitleStyle::RIGHT_WITH_WIDGET_RESIZE);
        }

        bool isTitleStatic()
        {
            return (mTitleStyle == TitleStyle::TOP_STATIC || mTitleStyle == TitleStyle::BOTTOM_STATIC ||
                    mTitleStyle == TitleStyle::LEFT_STATIC || mTitleStyle == TitleStyle::RIGHT_STATIC);
        }

        bool isTitleNone()
        {
            return (mTitleStyle == TitleStyle::NONE);
        }

        virtual bool hasActiveChildWidgetOrElement(bool includeClipboard)
        {
            if (getIsActiveWidget())
                return true;
            if (includeClipboard && shouldShowClipboardForThisWidget())
                return true;

            for (WidgetElm &elm : getWidgetElements())
            {
                if (elm.getIsActiveWidget())
                    return true;
                if (includeClipboard && elm.shouldShowClipboardForThisWidget())
                    return true;
            }

            for (Widget &childWidget : getChildWidgets())
            {
                if (childWidget.getIsActiveWidget())
                    return true;
                if (includeClipboard && childWidget.shouldShowClipboardForThisWidget())
                    return true;
                for (WidgetElm &elm : childWidget.getWidgetElements())
                {
                    if (elm.getIsActiveWidget())
                        return true;
                    if (includeClipboard && elm.shouldShowClipboardForThisWidget())
                        return true;
                }
            }
            return false;
        }

        virtual void onCoreWidgetWindowResized() override
        {

            setExpressionRecalculationRequired(true);
            setMaxWidth(ofGetWindowWidth() - getScreenPadding() * 2);
            setMaxHeight(ofGetWindowHeight() - getScreenPadding() * 2);

            setWidgetPosition(getWidgetStateX(), getWidgetStateY(), true);

            // ^^ If window has shrunk, the widget will be brought back into view - this isn't necessarily always the desired behaviour, can make a behaviour enum :
            // Dock left, stay in viewX, etc
            setWidgetNeedsUpdate(true);
            if (mPopoutDirection != PopoutDirection::NONE && getShowingAsPopout())
                setIsVisible(false);

            mFboAllocatedWidth = 0;
            mFboAllocatedHeight = 0;

            allocateFbo(true);
        }

        virtual void onCoreWidgetMouseScrolled(ofMouseEventArgs &e) override
        {
            if (getIsActiveWidget())
            {
                switch (getScrollAction())
                {
                case ScrollAction::ZOOM:
                    if (!getIsResizable() || isTitleHovered())
                        return;
                    setWidgetSize(getWidth() + e.scrollY * getScrollScaleX(), getHeight() + e.scrollY * getScrollScaleY(), true);
                    mMustNotifyAfterUserResizingEndedTimeout = ofGetSystemTimeMillis() + 500;
                    onWidgetMouseScrolled(e);
                    break;

                case ScrollAction::SCROLL:
                    WidgetBase::onCoreWidgetMouseScrolled(e);
                    break;
                }
            }
        }

        virtual void onCoreWidgetWindowResized(ofResizeEventArgs &e) override
        {
            onCoreWidgetWindowResized();
        }

        virtual void onCoreWidgetKeyPressed(ofKeyEventArgs &e) override
        {
            if (getIsActiveWidget())
            {
                WidgetBase::onCoreWidgetKeyPressed(e);
                for (WidgetElm &elm : mWidgetElements)
                {
                    if (elm.getIsFocused(true))
                    {
                        // elm.onWidgetKeyPressed(e); // this is handled by the elm's onCoreWidgetKeyPressed event

                        if (e.key == OF_KEY_TAB)
                        {
                            focusNextWidgetElement();
                        }
                    }
                }
            }
        }

        virtual void onCoreWidgetMouseReleased(ofMouseEventArgs &e) override
        {

            if (getIsActiveWidget() && !getIsDragging() && !getIsResizing())
            {
                onCoreWidgetWidgetClicked(e);
            }
            WidgetBase::onCoreWidgetMouseReleased(e);

            setIsDragging(false);
            setIsResizing(false);
            if (getViz()->getIsAnyWidgetDraggingOrResizing())
            {
                getViz()->revertBoostUserExperience();
            }
            getViz()->setIsAnyWidgetDraggingOrResizing(false);
        }

        virtual void onCoreWidgetMouseDragged(ofMouseEventArgs &e) override
        {
            if (getIsActiveWidget())
            {

                if (getExternalDrop().getIsDragging())
                {
                    getExternalDrop().handleExternalDragAndDropMotionNotify();
                    if (!ofGetMousePressed())
                        getExternalDrop().handleExternalDragAndDropButtonReleaseEvent();
                }

                if (getIsEnabled())
                    onWidgetMouseDragged(e);
                if (getIsActiveWidget() && !getIsScrollingXMouseDown() && !getIsScrollingYMouseDown() && !getIsDragging() && !getIsResizing())
                {
                    onCoreWidgetMouseContentDragged(e);
                }
            }
        }

        virtual void onWidgetMouseScrolled(ofMouseEventArgs &e) override
        {
            // Implement any behaviour in derrived class which should handle mouse released...
        }

        bool getIsWidgetPinned()
        {
            return mIsWidgetPinned;
        }

        void setIsWidgetPinned(bool val)
        {
            mIsWidgetPinned = val;
        }

        bool getShowWidgetPin()
        {
            return mShowWidgetPin;
        }

        void setShowWidgetPin(bool val)
        {
            mShowWidgetPin = val;
        }

    private:
        WidgetContext context;
        bool mIsWidgetPinned = false;
        bool mShowWidgetPin = false;
        bool mWidthChangedByExpression = false;
        bool mHeightChangedByExpression = false;

        vector<Icon> mPositionedTitleIcons;
        bool mShowPopoutWidgetWhenTitleMenuIconHovered = false;

        void addWidgetElement(WidgetElm &e)
        {
            removeWidgetElement(&e);
            mWidgetElements.push_back(e);
        }

        WidgetElm *addWidgetElement(string ELM_CLASS, string persistentId, string widgetXML)
        {
            mapElements();
            // Does the class exist in the map?
            if (elmClassMap.count(ELM_CLASS) > 0)
            {
                WidgetElm *elm = elmClassMap[ELM_CLASS](persistentId, widgetXML, this);
                addWidgetElement(*elm);
                return elm;
            }
            else
            {
                ofLogWarning("Widget") << "[Element:" << persistentId << "] of class \"" << ELM_CLASS << "\" failed to load. [" + widgetXML + "]";
            }

            return nullptr;
        }

        virtual void onCoreWidgetEventReceived(WidgetEventArgs &args) override
        {

            /*****************************************************************
             Internal messages sent by children to be handled by this widget
             *****************************************************************/

            if (args.eventName == WIDGET_EVENT::FOCUS_NEXT_ELEMENT)
            {
                focusNextWidgetElement();
                return;
            }

            if (args.eventName == WIDGET_EVENT::FOCUS_ELEMENT)
            {
                focusWidgetElement(args.widgetId);
                return;
            }

            if (args.eventName == WIDGET_EVENT::CLIPBOARD_PASTE)
            {
                focusWidgetElement(args.widgetId);
                return;
            }
        }

        void init()
        {
            mMinimizedWidth = getWidth() * 0.25f;
            mMinimizedHeight = getHeight() * 0.25f;

            getViz()->addWidgetId(getWidgetId());
            scaleUITitleSize();
            setWasJustInitialized(true);
            mUICurrentWidgetAlpha = 0.0f; // Let it fade in on inital draw

            setUseFbo(getViz()->getUseFbo());

            if (getViz()->getUseFbo())
            {
                //  allocateFbo(true); // disabled since it may not even be visible yet!
            }
        }

        void updateWidgetUI()
        {
            widgetFrame.setRectangle(scale(getWidgetStateX()), scale(getWidgetStateY()), scale(getWidgetStateWidth()), scale(getWidgetStateHeight()));
            updateWidgetUITitle();

            widgetResizeBottom.setRectangle(scale(getWidgetStateX()), scale(getWidgetStateY() + getWidgetStateHeight() - getNonScaledPadding()), scale(getWidgetStateWidth()), getScaledPadding());
            widgetResizeLeft.setRectangle(scale(getWidgetStateX()), scale(getWidgetStateY()), getScaledPadding(), scale(getWidgetStateHeight()));
            widgetResizeRight.setRectangle(scale(getWidgetStateX() + getWidgetStateWidth() - getNonScaledPadding()), scale(getWidgetStateY()), getScaledPadding(), scale(getWidgetStateHeight()));
            titleFont = (getViz()->getScale() < 1) ? getViz()->getSmallFont() : getViz()->getMediumFont();
            ofRectangle rectTitleFontAreaOf = titleFont->rect(getTitle() + "Xy"); // (concat an upper case and a descender letter)
            rectTitleFontArea.setRectangle(rectTitleFontAreaOf.getX(), rectTitleFontAreaOf.getY(), rectTitleFontAreaOf.getWidth(), rectTitleFontAreaOf.getHeight());
            updateWidgetUIPathElm();
            // updateWidgetPopoutArrow();
        }

        void scaleUITitleSize()
        {
            // The reason we can't have this called inside updateWidgetUI is because
            // it would cause any animating title resize to "jump" to a position
            if (getIsMinimized())
            {
                mUICurrentTitleBarSize = getHoveredTitleBarSize(true);
            }
            else
            {
                mUICurrentTitleBarSize = getUnhoveredTitleBarSize(true);
            }
        }

        void updateWidgetUITitle()
        {
            Coord::vizBounds titleBounds = getWidgetUITitleBounds(false);
            widgetTitle.setRectangle(titleBounds.x, titleBounds.y, titleBounds.width, titleBounds.height);
            updateWidgetTitlePathElm();
        }

        Coord::vizBounds getWidgetUITitleBounds(bool isAllocating)
        {
            if (isTitleTop())
            {
                return Coord::vizBounds(scale(getWidgetStateX()), scale(getWidgetStateY()), scale(getWidgetStateWidth()), (isAllocating ? getHoveredTitleBarSize(true) : mUICurrentTitleBarSize));
            }
            else if (isTitleBottom())
            {
                return Coord::vizBounds(scale(getWidgetStateX()), isAllocating ? getHoveredTitleBarSize(true) : scale(getWidgetStateY()) + scale(getWidgetStateHeight()) + mUICurrentTitleBarSize * -1.0f, scale(getWidgetStateWidth()), mUICurrentTitleBarSize);
            }
            else if (isTitleLeft())
            {
                return Coord::vizBounds(scale(getWidgetStateX()), scale(getWidgetStateY()), mUICurrentTitleBarSize, scale(getWidgetStateHeight()));
            }
            else if (isTitleRight())
            {
                return Coord::vizBounds(isAllocating ? getHoveredTitleBarSize(true) : scale(getWidgetStateX()) + scale(getWidgetStateWidth()) + mUICurrentTitleBarSize * -1.0f, scale(getWidgetStateY()), mUICurrentTitleBarSize, scale(getWidgetStateHeight()));
            }
            return Coord::vizBounds(0, 0, 10, 10);
        }

        void drawWidgetUITitle(bool skipDraw)
        {

            if (!getDrawWidgetTitle() || isWidgetUICompletelyTransparent() || mTitleStyle == TitleStyle::NONE)
                return;

            if (getIsMinimized() && !isWidgetHovered())
            {
                return;
            }

            // Title Autohide check
            if (!getIsDragging() && !getIsResizing() && getIsTitleAutohide() && !mDidUpdateForAutohide)
            {
                if ((!isWidgetHovered() && mUICurrentTitleBarSize > 0) || (isWidgetHovered() && getIsActiveWidget() && mUICurrentTitleBarSize <= 0))
                {
                    scaleUITitleSize();
                    setWidgetNeedsUpdate(true);
                    setNeedsUpdate(true);
                    mDidUpdateForAutohide = true;
                    return;
                }
            }

            if (getIsMinimized())
            {
                // mUICurrentTitleBarSize = getHoveredTitleBarSize(true);
            }

            //--------------------------- TITLE HOVER (CONTENT RESIZE) -----------------------------------
            if (!isTitleStatic())
            {
                handleTitle();
            }

            //--------------------------- WIDGET TITLE BAR -----------------------------------
            if (!skipDraw)
            {
                ofPushStyle();
                ofSetColor(getTitleStateColor());
                vizElm_WIDGET_TITLE.draw();
                ofPopStyle();
            }

            //--------------------------- WIDGET ICONS -----------------------------------
            int iconWidthUsage = 0;
            if (!skipDraw && !getIsDragging())
            {
                iconWidthUsage = drawWidgetTitleIcons();
            }

            //--------------------------- WIDGET TITLE TEXT -----------------------------------
            // mMustNotifyAfterUserResizingEndedTimeout is > 0 when user is resizing, skip title text draw
            if (!skipDraw && mMustNotifyAfterUserResizingEndedTimeout == 0)
            {
                if ((getTitle() != "" || getIsVisualDebugMode()) && !isTitleLeft() && !isTitleRight() && !getIsMinimized() &&
                    ((!getIsTitleAutohide() || isWidgetHovered()) || (getIsTitleAutohide() && getIsResizing()) || (getIsTitleAutohide() && getIsDragging())))
                {

                    if (widgetTitle.getRectangle().height > rectTitleFontArea.getRectangle().height)
                    {

                        int avalWidthForText = scale(getUsableWidth()) - iconWidthUsage;
                        // Shorten the title text if too long ...
                        while (titleFont->width(mDrawableWidgetTitle) > avalWidthForText && mDrawableWidgetTitle.length() > 0)
                        {
                            mDrawableWidgetTitle.pop_back();
                        }

                        ofPushStyle();
                        ofSetColor(getTitleFontStateColor());
                        titleFont->draw(getIsVisualDebugMode() ? ("[" + getWidgetId() + "|" + ofToString(getX()) + "," + ofToString(getY()) + "," + ofToString(getWidth()) + "," + ofToString(getHeight()) + getTitle() + "]") : mDrawableWidgetTitle,
                                        widgetTitle.getRectangle().x + getScaledPadding(),
                                        widgetTitle.getRectangle().y + (widgetTitle.getRectangle().height / 2.0f + rectTitleFontArea.getRectangle().height / 2.5f));
                        ofPopStyle();
                    }
                }
            }
        }

        Icon getWidgetTitleIconByTag(int iconTag)
        {
            for (Icon icon : mPositionedTitleIcons)
            {
                if (icon.getTag() == iconTag)
                {
                    return icon;
                }
            }
            return Icon();
        }

        int drawWidgetTitleIcons()
        {
            vector<Icon> icons = getIsMinimized() ? getWidgetTitleMinimizedIcons() : ((isTitleHovered() || getIsDragging()) && getIsActiveWidget() ? getWidgetTitleHoveredIcons() : getWidgetTitleIcons());
            int iconWidthUsage = 0;

            if (icons.size() > 0)
            {
                for (int i = 0; i <= icons.size() - 1; i++)
                {

                    int iconX = 0;
                    int iconY = 0;

                    if (isTitleTop() || isTitleBottom())
                    {
                        iconX = widgetTitle.getRectangle().x + scale(getCoreUsableWidth()) + ((icons[i].getScaledBounds().width * 1.5f * i) * -1) - getScaledPadding() * 2;
                        iconY = (widgetTitle.getRectangle().y) + (widgetTitle.getRectangle().height - icons[i].getScaledBounds().height) / 2;
                    }
                    else
                    {
                        iconX = widgetTitle.getRectangle().x + (widgetTitle.getRectangle().width - icons[i].getScaledBounds().width) / 2;
                        iconY = (widgetTitle.getRectangle().y) + scale(getCoreUsableHeight()) + ((icons[i].getScaledBounds().height * 1.5f * i) * -1) - getScaledPadding() * 2;
                    }

                    if (getIsActiveWidget() && !getViz()->getIsAnyWidgetDraggingOrResizing())
                    {
                        icons[i].setScaledPos(iconX, iconY);
                        icons[i].setColor(getTheme().TitleFontColor_withAlpha(0.5f));

                        if (icons[i].isHovered())
                        {
                            icons[i].setColor(getTheme().TitleFontColor);

                            Coord::vizBounds iconBounds = Coord::vizBounds(
                                deScale(icons[i].getScaledBounds().x),
                                deScale(icons[i].getScaledBounds().y),
                                deScale(icons[i].getScaledBounds().width),
                                deScale(icons[i].getScaledBounds().height));

                            Elm vizElm_ICON_HOVER;
                            vizElm_ICON_HOVER.setRectRounded(
                                scale(iconBounds.x),
                                scale(iconBounds.y),
                                scale(iconBounds.width),
                                scale(iconBounds.height),
                                scale(5),
                                getTheme().TitleColor_withAlpha(getTheme().ContentChromeAlpha));
                            vizElm_ICON_HOVER.draw();

                            titleIconHovered(icons[i].getTag(), iconBounds);

                            if (!mIsTitleResizing && getIsMousePressedAndReleasedOverWidget(true))
                            {
                                titleIconClicked(icons[i].getTag(), iconBounds);
                            }
                        }

                        if (widgetTitle.getRectangle().height >= deScale(icons[i].getScaledBounds().height) && (!getIsTitleAutohide() || isWidgetHovered()))
                        {
                            icons[i].drawSvg();
                        }

                        iconWidthUsage += icons[i].getScaledBounds().width;
                    }
                }
            }
            mPositionedTitleIcons = icons;
            return iconWidthUsage + getScaledPadding();
        }

        void drawWidgetUI(bool skipDrawTitle)
        {
            // Widget ui fade in / out
            if (getWasJustInitialized() && !getIsModalWidget())
            {
                mUICurrentWidgetAlpha = scaleAnimation(getWidgetId() + "_initialized", mUICurrentWidgetAlpha, getTheme().UnhoveredWidgetAlpha, 0.3f);
                if (mUICurrentWidgetAlpha >= getTheme().UnhoveredWidgetAlpha)
                {
                    setWasJustInitialized(false);
                }
            }
            else
            {
                if ((isWidgetHovered() && getIsActiveWidget()) || getIsModalWidget())
                {
                    removeAnimation(getWidgetId() + "_unhovered");
                    mUICurrentWidgetAlpha = scaleAnimation(getWidgetId() + "_hovered", mUICurrentWidgetAlpha, getTheme().HoveredWidgetAlpha, 0.3f);
                }
                else
                {
                    removeAnimation(getWidgetId() + "_hovered");
                    mUICurrentWidgetAlpha = scaleAnimation(getWidgetId() + "_unhovered", mUICurrentWidgetAlpha, getTheme().UnhoveredWidgetAlpha, 0.3f);
                }
            }

            // Jump out if it's still completely transparrent!
            if (isWidgetUICompletelyTransparent())
                return;

            if (getDrawWidgetChrome())
            {
                // ofSetColor();
                // vizElm_WIDGET_UI.setRectGradient(ofColor::red, getTitleStateGradientColor2(), getTitleStateGradientColor3(), getTitleStateGradientColor4());
                if (getIsHighlighted())
                {
                    vizElm_WIDGET_UI_SHADOW.draw();
                }
                else
                {
                    if (getIsShadow())
                        vizElm_WIDGET_UI_SHADOW.draw();
                }

                //  }

                vizElm_WIDGET_UI.setColor(getWidgetStateColor(mUICurrentWidgetAlpha));
                vizElm_WIDGET_UI.draw();

                // Uncomment to see resize bars
                /*
                 if (getIsResizable()) {
                 // move to it's own function, fade in only within 3/4 of position
                 ofPushStyle();
                 ofSetColor(255, 255, 255, 255 * mUICurrentWidgetAlpha*0.5f);
                 ofDrawRectRounded(widgetResizeBottom.getX(), widgetResizeBottom.getY(), widgetResizeBottom.getWidth(), widgetResizeBottom.getHeight(), 5);
                 ofDrawRectRounded(widgetResizeLeft.getX(), widgetResizeLeft.getY(), widgetResizeLeft.getWidth(), widgetResizeLeft.getHeight(), 5);
                 ofDrawRectRounded(widgetResizeRight.getX(), widgetResizeRight.getY(), widgetResizeRight.getWidth(), widgetResizeRight.getHeight(), 5);
                ofPopStyle();
                 }
                 */

                // Popout arrow
                if (mPopoutDirection != PopoutDirection::NONE)
                {
                    vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE.setColor(getWidgetStateColor(mUICurrentWidgetAlpha));
                    vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE.draw();
                    if (isPopoutFromTitle())
                    {
                        vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE.setColor(getTitleStateColor());
                        vizElm_WIDGET_TITLE_BUBBLE_TRIANGLE.draw();
                    }
                }
            }

            // Explanation for the following line:
            // We DONT want to skip drawing the title if user is dragging, very odd looking line, just think double negative...
            if (!skipDrawTitle)
            {
                bool skipDraw = !isTitleStatic() && !getIsDragging() && !getIsResizing() && getIsActiveWidget();
                drawWidgetUITitle(skipDraw);
            }

            if (getIsDragging() || getIsResizing() || mIsTitleResizing || isWidgetHovered())
            {

                //--------------------------- MOVING -----------------------------------
                handleMoveDrag();

                //--------------------------- RESIZING -----------------------------------
                handleResize();
            }
            else
            {
            }

            //--------------------------- RESIZING BY USER ENDED-----------------------------------
            if (mMustNotifyAfterUserResizingEndedTimeout > 0 && ofGetSystemTimeMillis() >= mMustNotifyAfterUserResizingEndedTimeout && !ofGetMousePressed())
            {
                mMustNotifyAfterUserResizingEndedTimeout = 0;
                onWidgetResizingByUserEnded(getWidth(), getHeight());
            }
        }

        void handleTitle()
        {
            /************************
             Do NOT call updateWidget or updateWidgetUI
             inside this function
             ************************/

            if (isTitleHovered() && getIsActiveWidget())
            {

                if (mUICurrentTitleBarSize < getHoveredTitleBarSize(true))
                {
                    if (!mIsTitleResizing)
                    {
                        getViz()->boostUserExperience(false);
                    }
                    mIsTitleResizing = true;

                    if (getIsTitleAutohide())
                    {
                        mDidUpdateForAutohide = false;
                    }

                    mUICurrentTitleBarSize = scaleAnimationForUI(getWidgetId() + "_hovered_titlebar_size", mUICurrentTitleBarSize, getHoveredTitleBarSize(true), 0.75f);
                    updateWidgetUITitle();

                    if (isTitleResize())
                    {
                        setNeedsUpdate(true);
                    }
                }
                else
                {
                    if (mIsTitleResizing && (!getViz()->getIsAnyWidgetDraggingOrResizing()))
                    {
                        getViz()->revertBoostUserExperience();
                    }
                    setNeedsUpdate(true);
                    mIsTitleResizing = false;
                }
            }
            else
            {
                if (mUICurrentTitleBarSize > getUnhoveredTitleBarSize(true))
                {
                    if (!mIsTitleResizing)
                    {
                        getViz()->boostUserExperience(false);
                    }
                    mIsTitleResizing = true;

                    if (getIsTitleAutohide())
                    {
                        mDidUpdateForAutohide = false;
                    }

                    if (getIsDragging() || getIsResizing())
                    {
                        return;
                    }
                    mUICurrentTitleBarSize = scaleAnimationForUI(getWidgetId() + "_unhovered_titlebar_size", mUICurrentTitleBarSize, getUnhoveredTitleBarSize(true), 0.75f);
                    updateWidgetUITitle();

                    if (isTitleResize())
                    {
                        setNeedsUpdate(true);
                    }
                }
                else
                {
                    if (mIsTitleResizing && (!getViz()->getIsAnyWidgetDraggingOrResizing()))
                    {
                        getViz()->revertBoostUserExperience();
                    }
                    mIsTitleResizing = false;
                }
            }
        }

        void handleMoveDrag()
        {
            /************************
             Do NOT call updateWidget or updateWidgetUI
             inside this function
             ************************/

            if (!getIsActiveWidget())
            {
                return;
            }

            if (!getIsResizing() && (getIsDragging() || (ofGetMousePressed() && isTitleHovered())))
            {
                if (!getIsDragging())
                {

                    if (mDraggingStartedXOriginal == -1 || mDraggingStartedYOriginal == -1)
                    {
                        mDraggingStartedXOriginal = ofGetMouseX();
                        mDraggingStartedYOriginal = ofGetMouseY();
                    }

                    if (isTitleHovered() && mDraggingStartedXOriginal == ofGetMouseX() && mDraggingStartedYOriginal == ofGetMouseY())
                    {
                        return;
                    }

                    setIsDragging(true);
                    getViz()->setIsAnyWidgetDraggingOrResizing(true);
                    getViz()->boostUserExperience(true);
                    mDraggingStartedX = deScale(ofGetMouseX()) - getWidgetStateX();
                    mDraggingStartedY = deScale(ofGetMouseY()) - getWidgetStateY();
                    update(WidgetContext());
                }

                if (!getIsDraggable())
                {
                    return;
                }

                float newX = deScale(ofGetMouseX()) - mDraggingStartedX;
                float newY = deScale(ofGetMouseY()) - mDraggingStartedY;

                setWidgetPosition(newX, newY, true);

                //--------------------------- "UN"DOCKING -----------------------------------
                if (getX_Expr() != "" && getX_ExprWasSetProgramatically() &&
                    WidgetBase::getX() < (deScale(ofGetWindowWidth()) - getScreenPadding() * 2 - getWidgetStateWidth()))
                {
                    setX_Expr("");
                }

                if (getY_Expr() != "" && getY_ExprWasSetProgramatically() &&
                    WidgetBase::getY() < (deScale(ofGetWindowHeight()) - getScreenPadding() * 2 - getWidgetStateHeight()))
                {
                    setY_Expr("");
                }
                //---------------------------------------------------------------------------
            }
            else
            {
                setIsDragging(false);
                mDraggingStartedXOriginal = -1;
                mDraggingStartedYOriginal = -1;
                if (!getViz()->getIsAnyWidgetDraggingOrResizing())
                {
                    getViz()->revertBoostUserExperience();
                }
            }

            getViz()->setIsAnyWidgetDraggingOrResizing(getIsDragging());
        }

        void handleResize()
        {
            /************************
             Do NOT call updateWidget or updateWidgetUI
             inside this function
             ************************/

            if (!getIsActiveWidget())
            {
                return;
            }

            if (!getIsResizable() || getIsDragging() || getIsMinimized())
            {
                return;
            }

            if (getIsResizing() || (ofGetMousePressed() && ((widgetResizeBottom.isHovered_Rect()) ||
                                                            (widgetResizeLeft.isHovered_Rect()) ||
                                                            (widgetResizeRight.isHovered_Rect()))))
            {

                if (!getIsResizing())
                {
                    setIsResizing(true);
                    mMustNotifyAfterUserResizingEndedTimeout = ofGetSystemTimeMillis() + 500;

                    getViz()->boostUserExperience(true);
                    mDraggingStartedX = deScale(ofGetMouseX()) - getWidgetStateX();
                    mDraggingStartedY = deScale(ofGetMouseY()) - getWidgetStateY();
                    mResizingStartedX = deScale(ofGetMouseX()) - getWidgetStateWidth();
                    mResizingStartedY = deScale(ofGetMouseY()) - getWidgetStateHeight();

                    if (widgetResizeLeft.isHovered_Rect())
                    {
                        mIsResizingLeft = true;
                    }
                }

                if (mIsResizingLeft)
                {
                    //  setWidgetBounds(deScale(ofGetMouseX()) + mResizingStartedX, getWidgetStateY(), getWidgetStateWidth() , getWidgetStateHeight());

                    setWidgetBounds(getWidgetStateX(), getWidgetStateY(), getWidgetStateWidth(), getWidgetStateHeight());

                    /*
                    setWidgetBounds(
                                    deScale(ofGetMouseX()) - mDraggingStartedX,
                                    deScale(ofGetMouseY()) - mDraggingStartedY,
                                    mDraggingStartedX + mResizingStartedX + (-1.0f * deScale(ofGetMouseX())),
                                    mDraggingStartedY + mResizingStartedY + (-1.0f * deScale(ofGetMouseY())));

                    */
                    // BUG :: Left resize doesnt work!!!
                }
                else
                {
                    setWidgetSize(deScale(ofGetMouseX()) - mResizingStartedX, deScale(ofGetMouseY()) - mResizingStartedY, true);
                }

                //--------------------------- "UN"DOCKING -----------------------------------
                if (getWidth_Expr() != "" && getWidth_ExprWasSetProgramatically() &&
                    getWidgetStateWidth() < (deScale(ofGetWindowWidth()) - getScreenPadding() * 2))
                {
                    setWidth_Expr("");
                }
                if (getHeight_Expr() != "" && getHeight_ExprWasSetProgramatically() &&
                    getWidgetStateHeight() < (deScale(ofGetWindowHeight()) - getScreenPadding() * 2))
                {
                    setHeight_Expr("");
                }
                //---------------------------------------------------------------------------
            }
            else
            {

                setIsResizing(false);
                mIsResizingLeft = false;

                if (!getViz()->getIsAnyWidgetDraggingOrResizing())
                {
                    getViz()->revertBoostUserExperience();
                }
            }

            getViz()->setIsAnyWidgetDraggingOrResizing(getIsResizing());
        }

        void updateWidgetTitlePathElm()
        {
            // $$$ expensive $$$
            switch (getTitleStyle())
            {
            case TitleStyle::TOP_STATIC:
            {
                getIsRoundedRectangle() ? vizElm_WIDGET_TITLE.setRectRounded(widgetTitle.getRectangle().x, widgetTitle.getRectangle().y, widgetTitle.getRectangle().width, widgetTitle.getRectangle().height, scale(5), scale(5), 0, 0) : vizElm_WIDGET_TITLE.setRectangle(widgetTitle.getRectangle().x, widgetTitle.getRectangle().y, widgetTitle.getRectangle().width, widgetTitle.getRectangle().height);
                break;
            }
            case TitleStyle::BOTTOM_STATIC:
            {
                getIsRoundedRectangle() ? vizElm_WIDGET_TITLE.setRectRounded(widgetTitle.getRectangle().x, widgetTitle.getRectangle().y, widgetTitle.getRectangle().width, widgetTitle.getRectangle().height, 0, 0, scale(5), scale(5)) : vizElm_WIDGET_TITLE.setRectangle(widgetTitle.getRectangle().x, widgetTitle.getRectangle().y, widgetTitle.getRectangle().width, widgetTitle.getRectangle().height);
                break;
            }
            case TitleStyle::LEFT_STATIC:
            {
                getIsRoundedRectangle() ? vizElm_WIDGET_TITLE.setRectRounded(widgetTitle.getRectangle().x, widgetTitle.getRectangle().y, widgetTitle.getRectangle().width, widgetTitle.getRectangle().height, scale(5), 0, 0, scale(5)) : vizElm_WIDGET_TITLE.setRectangle(widgetTitle.getRectangle().x, widgetTitle.getRectangle().y, widgetTitle.getRectangle().width, widgetTitle.getRectangle().height);
                break;
            }
            case TitleStyle::RIGHT_STATIC:
            {
                getIsRoundedRectangle() ? vizElm_WIDGET_TITLE.setRectRounded(widgetTitle.getRectangle().x, widgetTitle.getRectangle().y, widgetTitle.getRectangle().width, widgetTitle.getRectangle().height, 0, scale(5), scale(5), 0) : vizElm_WIDGET_TITLE.setRectangle(widgetTitle.getRectangle().x, widgetTitle.getRectangle().y, widgetTitle.getRectangle().width, widgetTitle.getRectangle().height);
                break;
            }
            default:
            {
                getIsRoundedRectangle() ? vizElm_WIDGET_TITLE.setRectRounded(widgetTitle.getRectangle().x, widgetTitle.getRectangle().y, widgetTitle.getRectangle().width, widgetTitle.getRectangle().height, scale(5)) : vizElm_WIDGET_TITLE.setRectangle(widgetTitle.getRectangle().x, widgetTitle.getRectangle().y, widgetTitle.getRectangle().width, widgetTitle.getRectangle().height);
                break;
            }
            }

            if (getTheme().getTitleGradientSet())
            {
                vizElm_WIDGET_TITLE.setRectGradient(getTitleStateGradientColor1(), getTitleStateGradientColor2(), getTitleStateGradientColor3(), getTitleStateGradientColor4());
            }
        }

        void updateWidgetUIPathElm()
        {
            // $$$ expensive $$$
            getIsRoundedRectangle() ? vizElm_WIDGET_UI.setRectRounded(widgetFrame.getRectangle().x, widgetFrame.getRectangle().y, widgetFrame.getRectangle().width, widgetFrame.getRectangle().height, scale(5)) : vizElm_WIDGET_UI.setRectangle(widgetFrame.getRectangle().x, widgetFrame.getRectangle().y, widgetFrame.getRectangle().width, widgetFrame.getRectangle().height);

            // Widget Shadow
            if (getIsShadow())
            {
                int shadowSize = scale(8);

                vizElm_WIDGET_UI_SHADOW.setRectRounded(
                    vizElm_WIDGET_UI.getRectangle().x - shadowSize,
                    vizElm_WIDGET_UI.getRectangle().y - shadowSize,
                    vizElm_WIDGET_UI.getRectangle().width + shadowSize * 2,
                    vizElm_WIDGET_UI.getRectangle().height + shadowSize * 2,
                    shadowSize, shadowSize, shadowSize, shadowSize);

                setIsHighlighted(getIsHighlighted());
            }
        }

        bool isTransitioningAlpha() override
        {
            if (!getIsVisible())
                return false;
            return mTransitioningAlpha;
        }

        virtual bool hasActiveElement() override
        {
            for (WidgetElm &elm : mWidgetElements)
            {
                if (elm.getIsActiveWidget())
                    return true;
            }
            return false;
        }

        Widget *mParent = NULL;

        int mMinimizedWidth, mMinimizedHeight;

        int mNonMinimizedOffsetX = 0;
        int mNonMinimizedOffsetY = 0;

        int mMinimizedOffsetX = 0;
        int mMinimizedOffsetY = 0;

        bool mIsMinimized = false;
        bool mDrawWidgetChrome = true;
        bool mDrawWidgetTitle = true;

        TitleStyle mTitleStyle = TitleStyle::TOP_STATIC;
        int mStaticTitleBarSize = 20;
        string mDrawableWidgetTitle = "";

        AdditionalState mAdditionalState = AdditionalState::NONE;
        bool mIsTitleAutohide = false;
        bool mShowTitleIcons = true;
        bool mDidUpdateForAutohide = false; // TODO : set it to false when unhovered!
        shared_ptr<ofxSmartFont> titleFont;
        Elm rectTitleFontArea;
        ofRectangle rectTitleFontAreaOf;

        bool mDrawnByWidgetManager = false;

        float mUICurrentTitleBarSize = 0;

        bool mIsResizingLeft = false;
        bool mIsTitleResizing = false;
        float mDraggingStartedX = 0;
        float mDraggingStartedY = 0;
        float mResizingStartedX = 0;
        float mResizingStartedY = 0;
        int mDraggingStartedXOriginal = -1;
        int mDraggingStartedYOriginal = -1;
        uint64_t mMustNotifyAfterUserResizingEndedTimeout = 0;

        Elm widgetFrame;
        Elm widgetTitle;

        Elm widgetResizeBottom;
        Elm widgetResizeLeft;
        Elm widgetResizeRight;

        float mUICurrentWidgetAlpha = 0.2f;

        std::vector<std::reference_wrapper<Widget>> mChildWidgets;

        Elm vizElm_WIDGET_CONTENT_BLOCK;
        Elm vizElm_WIDGET_TITLE;
        Elm vizElm_WIDGET_UI;
        Elm vizElm_WIDGET_UI_SHADOW;

        ofFbo fbo;
        bool mFboNeedsUpdate = true;
        int mFboAllocatedWidth = 0;
        int mFboAllocatedHeight = 0;

        FboMode mFboMode = FboMode::HIGH;
        bool mTransitioningAlpha = false;

        vector<reference_wrapper<WidgetElm>> mWidgetElements;
    };

}