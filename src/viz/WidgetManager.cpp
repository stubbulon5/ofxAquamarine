
#include "WidgetManager.h"
#include "App.h"
#include "BG.h"
#include "Widgets.h"
#include "ofxOsc.h"


namespace Aquamarine
{
    std::vector<std::reference_wrapper<Widget>> WidgetManager::mWidgets;
    Widget *currentPopout = nullptr;

    typedef exprtk::expression<float> expression_t;
    expression_t expression;

    typedef exprtk::parser<float> calculator_t;
    calculator_t calculator;

    bool popoutMovedToFront = false;
    int popoutCount = 0;

    BG *vizBG = nullptr;

    string prev_eventSenderType, prev_eventSenderId, prev_eventTargetId, prev_eventName, prev_eventXML;

    WidgetManager::ApplicationProperties applicationProperties(
        "ApplicationName", "0.0.0", "xml");

    WidgetManager::ProjectProperties currentProjectProperties(
        "Untitled Project", "untitled", "", "");

    bool SHOW_DEBUG_INFO = false;

    WidgetClipboardMenu *clipboardMenu = nullptr;
    WidgetBase *targetDropWidget = nullptr;
    WidgetDialog *dialog = nullptr;
    App *app_ptr = nullptr;

    static ofEvent<WidgetEventArgs> widgetManagerEventReceived;

    const string WidgetManager::WIDGET_CLASS = "Widget";
    const string WidgetManager::WIDGET_CLASS_MENU = "WidgetMenu";                      // these are not yet general purpose, and rely on parent widgets initializing them
    const string WidgetManager::WIDGET_CLASS_MENU_COLLECTION = "WidgetMenuCollection"; // these are not yet general purpose, and rely on parent widgets initializing them
    const string WidgetManager::WIDGET_CLASS_TABLE = "WidgetTable";
    const string WidgetManager::WIDGET_CLASS_MATRIX = "WidgetMatrix";
    const string WidgetManager::WIDGET_CLASS_SEQUENCER = "WidgetSequencer";
    const string WidgetManager::WIDGET_CLASS_PIANO_ROLL = "WidgetPianoRoll";
    const string WidgetManager::WIDGET_CLASS_VIDEO_PLAYER = "WidgetVideoPlayer";
    const string WidgetManager::WIDGET_CLASS_VIDEO_GRABBER = "WidgetVideoGrabber";
    const string WidgetManager::WIDGET_CLASS_SOUND_PLAYER = "WidgetSoundPlayer";
    const string WidgetManager::WIDGET_CLASS_IMAGE_VIEW = "WidgetImageView";
    const string WidgetManager::WIDGET_CLASS_DEBUG = "WidgetDebug";
    const string WidgetManager::WIDGET_CLASS_EVENT_LISTENER = "WidgetEventListener";
    const string WidgetManager::WIDGET_CLASS_CLIPBOARD_MENU = "WidgetClipboardMenu";
    const string WidgetManager::WIDGET_CLASS_SETTINGS = "WidgetSettings";
    const string WidgetManager::WIDGET_CLASS_TEXT_EDITOR = "WidgetTextEditor";
    const string WidgetManager::WIDGET_CLASS_COLOR_PICKER = "WidgetColorPicker";

    // System
    const string WidgetManager::WIDGET_CLASS_FILE_LIST = "WidgetFileList";
    const string WidgetManager::WIDGET_CLASS_FILE_LOCATIONS_LIST = "WidgetFileLocationsList";
    const string WidgetManager::WIDGET_CLASS_FILE_EXPLORER = "WidgetFileExplorer";
    const string WidgetManager::WIDGET_CLASS_FILE_LOAD = "WidgetFileLoad";
    const string WidgetManager::WIDGET_CLASS_FILE_SAVE = "WidgetFileSave";
    const string WidgetManager::WIDGET_CLASS_THEME_EDITOR = "WidgetThemeEditor";
    const string WidgetManager::WIDGET_CLASS_THEME_PREVIEW = "WidgetThemePreview";
    const string WidgetManager::WIDGET_CLASS_WIDGET_LIST = "WidgetWidgetList";
    const string WidgetManager::WIDGET_CLASS_DIALOG = "WidgetDialog";
    const string WidgetManager::WIDGET_CLASS_AQUAMARINE_ABOUT = "WidgetAquamarineAbout";

    void WidgetManager::BOOTSTRAP(App* app)
    {

        app_ptr = app;

        if (applicationProperties.applicationVersion == "0.0.0")
        {
            string exceptionStr = "A valid version number has not been set for this aplication. Terminating...";
            throw std::runtime_error(exceptionStr.c_str());
            OF_EXIT_APP(1);
        }
    
        // app->registerDefaultWidgets();
        // app->registerCustomWidgets();

        // Clipboard menu
        if (clipboardMenu == nullptr)
        {
            clipboardMenu = new WidgetClipboardMenu("CLIPBOARD", "<widget><appearance visible='0'/></widget>");
            WidgetManager::addWidget(*clipboardMenu, false, "", false);
        }
    }

    widget_map_type& WidgetManager::getRegisteredWidgets()
    {
        if(app_ptr == nullptr) {
            throw std::runtime_error(R"(
            ***************************** ERROR *****************************
            WidgetManager::BOOTSTRAP(App* app) was not called, MUST call it before using WidgetManager::getRegisteredWidgets()
            *****************************************************************
            )");
        }
        return app_ptr->getRegisteredWidgets();
    }    

    void WidgetManager::addWidget(Widget &widget, bool shouldPersist, string ownerWidgetId, bool transmitOsc)
    {
        if (widget.getPersistentId() == "")
        {
            widget.setPersistentId(WidgetSignature::generateRandomId(widget.getWidgetClassType()));
        }
        WidgetManager::removeWidget(widget.getPersistentId());
        // widget.initWidget();
        widget.setShouldPersist(shouldPersist);
        widget.setWasJustInitialized(true);
        widget.setTheme(widget.getTheme());
        widget.setSize(widget.getWidth(), widget.getHeight()); // ensure min, max bounds
        widget.updateWidget();
        widget.setOwnerWidgetId(ownerWidgetId);
        mWidgets.push_back(widget);
        setCurrentProjectIsDirty(true);

        if (transmitOsc)
        {
            Shared::getViz()->addToEventTransmitQueue(
                "/widget_manager/widget_added", "widget_manager", "WIDGET_MANAGER", "widget_added", widget.getXML(), true);
        }
    }

    void WidgetManager::addWidget(Widget &widget, bool shouldPersist, string ownerWidgetId)
    {
        WidgetManager::addWidget(widget, shouldPersist, ownerWidgetId, true);
    }

    void WidgetManager::addWidget(Widget &widget, bool shouldPersist)
    {
        WidgetManager::addWidget(widget, shouldPersist, "");
    }

    void WidgetManager::addWidgetForPopout(Widget &widget, string ownerWidgetId, bool shouldCloseIfOtherWidgetClicked)
    {
        widget.setCloseAction(Widget::CloseAction::HIDE);
        if (widget.getIsModalWidget())
        {
        }
        widget.setShouldCloseIfOtherWidgetClicked(shouldCloseIfOtherWidgetClicked);
        widget.setIsVisible(false);
        WidgetManager::addWidget(widget, false, ownerWidgetId);
    }

    Widget *WidgetManager::loadWidget(string WIDGET_CLASS, string persistentId, string widgetXML)
    {
        WidgetManager::BOOTSTRAP(app_ptr);
        if (persistentId == "")
        {
            persistentId = WidgetSignature::generateRandomId(WIDGET_CLASS);
        }

        WidgetManager::removeWidget(*(WidgetManager::getWidgetByPersistentId(persistentId)));

        Widget *widget;
        bool wasFoundInMap = false;

        // Does the class exist in the map?
        if (app_ptr->getRegisteredWidgets().count(WIDGET_CLASS) > 0)
        {
            widget = app_ptr->getRegisteredWidgets()[WIDGET_CLASS](persistentId, widgetXML);
            widget->setWidgetClassType(WIDGET_CLASS);
            wasFoundInMap = true;
        }
        else
        {
            ofLogWarning("WidgetManager") << "[Widget:" << persistentId << "] of class \"" << WIDGET_CLASS << "\" failed to load. [" + widgetXML + "]";
        }
        if (!wasFoundInMap)
            widget = new Widget(persistentId, widgetXML);

        // Child widgets...
        if (widget)
        {
            WidgetManager::loadChildWidgets(*widget, widgetXML);
            widget->setSize(widget->getWidth(), widget->getHeight()); // ensure min, max bounds
            widget->setWidgetNeedsUpdate(true);
            recalculateWidgetExpressions(*widget);
        }

        return widget;
    }

    Widget *WidgetManager::loadWidget(string widgetXML)
    {
        WidgetSignature widget = Widget::getWidgetSignatureFromXML(widgetXML);
        return WidgetManager::loadWidget(widget.widgetClass, widget.persistentId, widget.widgetXML);
    }

    Widget *WidgetManager::loadWidgetFromFile(string WIDGET_CLASS, string persistentId, string widgetFilePath)
    {
        string widgetXML = Widget::getWidgetXMLFromFile(widgetFilePath, true);
        return WidgetManager::loadWidget(WIDGET_CLASS, persistentId, widgetXML);
    }

    Widget *WidgetManager::loadWidgetFromFile(string widgetFilePath)
    {
        WidgetSignature widget = Widget::getWidgetSignatureFromFile(widgetFilePath);
        return WidgetManager::loadWidget(widget.widgetClass, widget.persistentId, widget.widgetXML);
    }

    Widget *WidgetManager::loadAndAddWidget(string WIDGET_CLASS, string persistentId, string widgetXML, bool shouldPersist)
    {
        Widget *widget = WidgetManager::loadWidget(WIDGET_CLASS, persistentId, widgetXML);

        if (widget->didWidgetLoad() == false)
        {
            WidgetManager::showModal(
                "Error loading Widget!",
                "<xlarge><br/>There was a problem loading state for Widget '" + persistentId + "'. Please check that the XML is valid.</xlarge>",
                true,
                "Ok",
                "BTN_OK",
                [&](WidgetEventArgs args) {});
        }
        WidgetManager::addWidget(*widget, shouldPersist);
        return WidgetManager::getWidgetByPersistentId(widget->getPersistentId());
    }

    void WidgetManager::loadWidgetFromFileToExisting(string widgetFilePath, Widget &widget)
    {
        string widgetXML = Widget::getWidgetXMLFromFile(widgetFilePath, true);
        widget.loadState(widgetXML);
        WidgetManager::loadChildWidgets(widget, widgetXML);
    }

    void WidgetManager::loadChildWidgets(Widget &targetWidget, string widgetXML)
    {
        WidgetManager::BOOTSTRAP(app_ptr);
        vector<WidgetSignature> childWidgetList = Widget::getChildWidgetsList(widgetXML);
        for (WidgetSignature childWidget : childWidgetList)
        {
            Widget *w = WidgetManager::loadWidget(childWidget.widgetClass, childWidget.persistentId, childWidget.widgetXML);
            w->setIsAutoResizeToContentsWidth(false);
            targetWidget.addChildWidget(*w, true);
        }
        targetWidget.onChildWidgetsLoaded();
    }

    Widget *WidgetManager::loadChildWidget(Widget &targetWidget, string widgetXML)
    {
        WidgetManager::BOOTSTRAP(app_ptr);
        Widget *w = WidgetManager::loadWidget(widgetXML);
        targetWidget.addChildWidget(*w, true);
        return w;
    }

    void WidgetManager::recalculateAllWidgetExpressions()
    {
        for (Widget &widget : mWidgets)
        {
            widget.setExpressionRecalculationRequired(true);
        }
    }

    void WidgetManager::recalculateWidgetExpressions(Widget &w)
    {

        // if (!w.getIsVisible() && !w.getWasJustInitialized()) return; // uncommenting causes "jump" when ben invisible becomes visible

        w.setExpressionRecalculationRequired(false);

        if (w.getIgnoreExpr_Layout())
            return;

        if (w.getX_Expr() != "" || w.getY_Expr() != "" || w.getWidth_Expr() != "" || w.getHeight_Expr() != "")
        {

            // replace string tokens ${SCREEN.WIDTH}, ${PERSISTENTID.RIGHT}, etc
            string exprStringX = w.getX_Expr();
            string exprStringY = w.getY_Expr();
            string exprStringWidth = w.getWidth_Expr();
            string exprStringHeight = w.getHeight_Expr();

            // Self
            if (exprStringX != "")
                w.replaceExpressions(&w, exprStringX);
            if (exprStringY != "")
                w.replaceExpressions(&w, exprStringY);
            if (exprStringWidth != "")
                w.replaceExpressions(&w, exprStringWidth);
            if (exprStringHeight != "")
                w.replaceExpressions(&w, exprStringHeight);

            for (Widget &exprWidget : mWidgets)
            {
                if (exprWidget.getIsVisible() && w.getPersistentId() != "")
                {
                    if (exprStringX != "")
                        w.replaceExpressions(&exprWidget, exprStringX);
                    if (exprStringY != "")
                        w.replaceExpressions(&exprWidget, exprStringY);
                    if (exprStringWidth != "")
                        w.replaceExpressions(&exprWidget, exprStringWidth);
                    if (exprStringHeight != "")
                        w.replaceExpressions(&exprWidget, exprStringHeight);
                }
            }
            /* USE TO BE ::: (but then ROOT widgets did not layout realive to each other ...)

            for(Widget &exprWidget:mWidgets) {
                if (exprWidget.getIsVisible() && exprWidget.getPersistentId() != "") {
                    if (exprStringX != "") exprWidget.replaceExpressions(&w, exprStringX);
                    if (exprStringY != "") exprWidget.replaceExpressions(&w, exprStringY);
                    if (exprStringWidth != "") exprWidget.replaceExpressions(&w, exprStringWidth);
                    if (exprStringHeight != "") exprWidget.replaceExpressions(&w, exprStringHeight);
                }
            }
            */

            if (exprStringX != "")
            {
                if (calculator.compile(exprStringX.c_str(), expression))
                {
                    w.setWidgetPosition(expression.value(), w.getY(), false);
                }
                else
                {
                    ofLogWarning("WidgetManager") << w.getPersistentId() << ".setWidgetPosition([x]): failed to evaluate expression \"" << w.getX_Expr() << "\" [---->" + exprStringX + "<----] : " << calculator.error().c_str();
                }
            }

            if (exprStringY != "")
            {
                if (calculator.compile(exprStringY.c_str(), expression))
                {
                    w.setWidgetPosition(w.getX(), expression.value(), false);
                }
                else
                {
                    ofLogWarning("WidgetManager") << w.getPersistentId() << ".setWidgetPosition([y]): failed to evaluate expression \"" << w.getY_Expr() << "\" [---->" + exprStringY + "<----] : " << calculator.error().c_str();
                }
            }

            if (exprStringWidth != "")
            {
                if (calculator.compile(exprStringWidth.c_str(), expression))
                {
                    w.setWidgetSize(expression.value(), w.getWidgetStateHeight(), false, true, false);
                }
                else
                {
                    ofLogWarning("WidgetManager") << w.getPersistentId() << ".setWidgetSize([width]): failed to evaluate expression \"" << w.getWidth_Expr() << "\" [---->" + exprStringWidth + "<----] : " << calculator.error().c_str();
                }
            }

            if (exprStringHeight != "")
            {
                if (calculator.compile(exprStringHeight.c_str(), expression))
                {
                    w.setWidgetSize(w.getWidgetStateWidth(), expression.value(), false, false, true);
                }
                else
                {
                    ofLogWarning("WidgetManager") << w.getPersistentId() << ".setWidgetSize([height]): failed to evaluate expression \"" << w.getHeight_Expr() << "\" [---->" + exprStringHeight + "<----] : " << calculator.error().c_str();
                }
            }
        }

        if (w.widgetNeedsUpdate())
        {
            // Important: Update immediately to reflect layout,
            // then set flag to 1 for next next draw call ...
            w.updateWidget();
            w.setWidgetNeedsUpdate(true);
        }

        for (Widget &childWidget : w.getChildWidgets())
        {
            WidgetManager::recalculateWidgetExpressions(childWidget);
            childWidget.updateWidgetPopoutArrow();
        }

        // Update popout arrow / locations if it's owned by 'w'
        if (Shared::getViz()->getIsPopoutVisible())
        {
            for (string popoutId : Shared::getViz()->getPopoutWidgetIds())
            {
                Widget *popout = WidgetManager::getWidget(popoutId);
                if (popout && popout->getOwnerWidgetId() == w.getWidgetId())
                {
                    popout->updateWidgetPopoutArrow();
                    break;
                }
            }
        }
    }

    void WidgetManager::drawWidget(Widget &widget, int widgetZIndex)
    {

        // Expression based layout
        if (widget.getIsRootWidget() && widget.getExpressionRecalculationRequired())
        {
            recalculateWidgetExpressions(widget); //(recursive)
            // Do any other root widgets reference this one in their layout expressions?
            for (Widget &w : mWidgets)
            {
                if (!w.getExpressionRecalculationRequired() && w.getExpressionReferencesPersistentId(widget.getPersistentId()))
                {
                    recalculateWidgetExpressions(w);
                }
            }
        }

        widget.drawWidget();

        if (widget.shouldShowClipboardForThisWidget() && !clipboardMenu->getIsVisible())
        {
            WidgetManager::showClipboardPopupMenu(Shared::getViz()->deScale(ofGetMouseX()) - 20, Shared::getViz()->deScale(ofGetMouseY()) - 20, widget.getWidgetId());
        }

        // Widget Elms
        for (WidgetElm &widgetElm : widget.getWidgetElements())
        {
            if (widgetElm.shouldShowClipboardForThisWidget() && !clipboardMenu->getIsVisible())
            {
                WidgetManager::showClipboardPopupMenu(Shared::getViz()->deScale(ofGetMouseX()) - 20, Shared::getViz()->deScale(ofGetMouseY()) - 20, widgetElm.getWidgetId());
            }
            widgetElm.determineElementVisibility();
        }

        for (Widget &childWidget : widget.getChildWidgets())
        {
            childWidget.setZindex(widgetZIndex);
            // childWidget.drawWidget();
            WidgetManager::drawWidget(childWidget, widgetZIndex);
        }

        // Content drag icon to draw?
        if (widget.isDraggingContent())
        {

            if (Shared::getViz()->getTargetDropWidgetId() != "")
            {
                targetDropWidget = getWidget(Shared::getViz()->getTargetDropWidgetId());
                widget.setTargetDropWidget(targetDropWidget);
            }

            ofPushStyle();
            widget.drawContentDragIcon();
            ofPopStyle();
        }
    }

    void WidgetManager::drawWidgets()
    {
        vizBG->draw();
        /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
         MOVE THE ACTIVE WIDGET TO THE FRONT IF NEEDED
         THIS ACTUALLY MEANS MOVING IT TO THE BACK OF THE VECTOR...
         *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
        if (Shared::getViz()->getIsPopoutVisible())
        {
            vector<string> popoutWidgetIds = Shared::getViz()->getPopoutWidgetIds();
            if (!popoutMovedToFront || popoutCount != popoutWidgetIds.size())
            {
                popoutCount = popoutWidgetIds.size();
                for (string popoutId : popoutWidgetIds)
                {
                    Widget *popout = WidgetManager::getWidget(popoutId);

                    if (!popout || popout == nullptr)
                    {
                        throw std::logic_error("Nullptr popout!!");
                    }

                    if (popout->getPersistentId() == "CLIPBOARD_MENU" || popout->getPersistentId() == "CLIPBOARD")
                    {
                        moveWidgetToFront(*clipboardMenu);
                    }
                    else
                    {
                        moveWidgetToFront(*popout);
                    }
                    popout->setActiveWidget();
                }
                popoutMovedToFront = true;
            }
        }
        else
        {
            popoutMovedToFront = false;
            Widget *activeWidget = getActiveParentWidget();
            if (activeWidget != nullptr && activeWidget->getZindex() > 0)
            {
                if (childWidgetCount(*activeWidget) > 0)
                {
                    for (Widget &childWidget : activeWidget->getChildWidgets())
                    {
                        moveWidgetToFront(childWidget);
                    }
                }
                moveWidgetToFront(*activeWidget);
            }
        }

        /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
         Set the correct next eligible widget for focusing. All widgets should have
         their IsNextInLineForFocus flag set. Only one should be true, rest false
         *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
        bool hasSetNextInLine = false;
        for (int i = mWidgets.size() - 1; i >= 0; i--)
        {
            Widget &currWidget = mWidgets[i];
            currWidget.setIsNextInLineForFocus(false);
            if (!hasSetNextInLine && currWidget.getIsVisible() && currWidget.isWidgetHovered() && !currWidget.getIsActiveWidget())
            {
                hasSetNextInLine = true;
                currWidget.setIsNextInLineForFocus(true);
            }
        }

        /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
         Prevent invisible widgets from becoming active!
         *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
        Widget *activeWidget = WidgetManager::getWidget(Shared::getViz()->getActiveWidgetId());
        if (activeWidget && !activeWidget->getIsVisible())
        {
            Shared::getViz()->clearActiveWidgetId();
        }

        if (WidgetManager::checkShouldCloseOrDelete(activeWidget))
        {
            return;
        }

        /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
         Draw widgets. Each widget is responsible for drawing it's child widgets.
         Draw drag icons if content is being dragged.
         *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
        int widgetZIndex = mWidgets.size() - 1;

        for (int i = 0; i < mWidgets.size(); i++)
        {
            Widget &widget = mWidgets[i];
            widget.setZindex(widgetZIndex);

            // Modal background ?
            if (widget.getIsModalWidget())
            {
                Elm vizElm_modal_background;
                vizElm_modal_background.setRectangle(
                    0, 0, ofGetWindowWidth(), ofGetWindowHeight(),
                    widget.getTheme().getWidgetModalBackgroundColor());
                vizElm_modal_background.draw();
            }

            // Draw the widget and child widgets (recursively)
            WidgetManager::drawWidget(widget, widgetZIndex);
            widgetZIndex--;

            if (WidgetManager::checkShouldCloseOrDelete(&widget))
            {
                return;
            }
        }

        // Local event listener event dispatch
        while (Shared::getViz()->hasLocalListenEventsInTransmitQueue())
        {
            Viz::WidgetEvent e = Shared::getViz()->getNextLocalListenMessageToTransmit();
            WidgetManager::handleEventReceive(e.eventSenderType, e.eventSenderId, e.eventTargetId, e.eventName, e.eventXML);
        }

        // Osc Send
        while (Shared::getViz()->hasOscEventsInTransmitQueue())
        {
            ofxOscMessage m;
            Shared::getViz()->getNextOscMessageToTransmit(m);
            if (m.getAddress() != "")
            {
                cout << "Transmitting OSC Event:\n";
                Shared::getViz()->sendOscMessage(m);

                if (SHOW_DEBUG_INFO)
                {
                    dynamic_cast<WidgetDebug *>(WidgetManager::getWidgetByPersistentId("APP_DEBUG_INFO"))->addToOscSendLog(m);
                }
            }
        }

        // Osc Receive
        while (Shared::getViz()->hasOscEventsInReceiveQueue())
        {
            cout << "Receiving OSC Event:\n";
            ofxOscMessage m;
            Shared::getViz()->getNextOscMessage(m);
            WidgetManager::handleEventReceive(m.getArgAsString(0), m.getArgAsString(1), m.getArgAsString(2), m.getArgAsString(3), m.getArgAsString(4));

            if (SHOW_DEBUG_INFO)
            {
                dynamic_cast<WidgetDebug *>(WidgetManager::getWidgetByPersistentId("APP_DEBUG_INFO"))->addToOscReceiveLog(m);
            }
        }

        // Fps counter
        if (Shared::getViz()->getShowFps())
        {
            Shared::getViz()->getFonts()->draw(
                ofToString(ofGetFrameRate(), 0) + " fps",
                Shared::getViz()->getMediumFontStyle(),
                ofGetWindowWidth() - Shared::getViz()->scale(35),
                ofGetWindowHeight() - Shared::getViz()->scale(10));
        }
    }

    void WidgetManager::updateWidget(Widget &widget)
    {
        widget.setNeedsUpdate(true);
        widget.setWidgetNeedsUpdate(true);
        for (Widget &childWidget : widget.getChildWidgets())
        {
            childWidget.setNeedsUpdate(true);
            childWidget.setWidgetNeedsUpdate(true);
        }
    }

    void WidgetManager::updateWidgets()
    {
        for (Widget &widget : mWidgets)
        {
            WidgetManager::updateWidget(widget);
        }
    }

    bool WidgetManager::checkShouldCloseOrDelete(Widget *widget)
    {
        if (!widget)
            return false;
        // Closing widget - notify owner
        if (widget->shouldCloseThisWidget())
        {
            widget->setIsVisible(false);
            if (widget->getOwnerWidgetId() != "")
            {
                Widget *widgetOwner = WidgetManager::getWidget(widget->getOwnerWidgetId());
                if (widgetOwner)
                    widgetOwner->onWidgetOwnedWidgetClosed(widget);
            }
        }

        // Removing widget - this should always be last!!
        if (widget->shouldDeleteThisWidget())
        {
            for (int i = 0; i < mWidgets.size(); i++)
            {
                Widget &checkWidget = mWidgets[i];
                if (widget->findChildWidget(checkWidget.getOwnerWidgetId()))
                {
                    checkWidget.deleteThisWidget();
                }
            }
            removeWidget(*widget);
            return true;
        }
        return false;
    }

    void WidgetManager::handleEventReceive(string eventSenderType, string eventSenderId, string eventTargetId, string eventName, string eventXML)
    {

        if (prev_eventSenderType != eventSenderType || prev_eventSenderId != eventSenderId ||
            prev_eventTargetId != eventTargetId || prev_eventName != eventName || prev_eventXML != eventXML)
        {

            if (eventSenderType == "widget_manager")
            {
                if (eventName == "widget_added")
                {
                    Widget *w = WidgetManager::loadWidget(eventXML);
                    WidgetManager::addWidget(*w, true);
                }
                else if (eventName == "widget_show_modal")
                {
                    Widget *w = WidgetManager::loadWidget(eventXML);
                    WidgetManager::showModal(w, true);
                }
            }
            else if (eventSenderType == "widget")
            {

                Widget *sender = WidgetManager::getWidgetByPersistentId(eventSenderId);
                Widget *target = (eventTargetId != "" ? WidgetManager::getWidgetByPersistentId(eventTargetId) : nullptr);

                if (sender)
                {
                    if (target)
                    {
                        target->eventTransmit(eventSenderId, eventName, eventXML, *sender, nullptr, false);
                    }
                    else
                    {
                        sender->eventTransmit(eventSenderId, eventName, eventXML, *sender, nullptr, false);
                    }
                }

                WidgetEventArgs args = WidgetEventArgs(eventSenderId, eventName, eventXML, *sender, target);
                ofNotifyEvent(widgetManagerEventReceived, args);
            }
        }

        prev_eventSenderType = eventSenderType;
        prev_eventSenderId = eventSenderId;
        prev_eventTargetId = eventTargetId;
        prev_eventName = eventName;
        prev_eventXML = eventXML;
    }

    void WidgetManager::removeAllWidgetsExceptMultiple(vector<WidgetBase *> exceptWidgets)
    {
#if VIZ_DEBUG_LEVEL >= 1 && VIZ_DEBUG_LEVEL <= 2
        cout << "\n*************************************************************";
        cout << "\nCLEARING WIDGETS....";
        cout << "\n*************************************************************";
#endif

        bool whileDeletedWidget = true;

        while (whileDeletedWidget)
        {
            whileDeletedWidget = false;
            for (Widget &widget : mWidgets)
            {
                if (exceptWidgets.size() > 0 && exceptWidgets[0] != nullptr)
                {
                    if (!widget.matchesWidget(exceptWidgets))
                    {
                        if (widget.getWidgetClassType() == WIDGET_CLASS_DEBUG)
                            continue;
                        if (widget.getPersistentId() == "CLIPBOARD_MENU" || widget.getPersistentId() == "CLIPBOARD")
                            continue;
                        if (widget.getOwnerWidgetId() == "" && WidgetManager::removeWidget(widget))
                        {
                            whileDeletedWidget = true;
                            break;
                        }
                    }
                }
                else
                {
                    if (widget.getOwnerWidgetId() == "" &&
                        widget.getPersistentId() != "CLIPBOARD_MENU" && widget.getPersistentId() != "CLIPBOARD" &&
                        WidgetManager::removeWidget(widget))
                    {
                        whileDeletedWidget = true;
                        break;
                    }
                }
            }
        }

        string fileName = +"untitled." + applicationProperties.fileExtension;
        setProjectProperties("Untitled Project", ofToDataPath(fileName, true));
        setCurrentProjectIsDirty(true);
    }

    void WidgetManager::removeAllWidgetsExcept(Widget *exceptWidget)
    {
        WidgetManager::removeAllWidgetsExceptMultiple({exceptWidget});
    }

    void WidgetManager::removeAllWidgets()
    {
        removeAllWidgetsExcept(nullptr);
    }

    void WidgetManager::moveWidgetToFront(Widget &w)
    {
        int idx = getWidgetIndex(w.getWidgetId());
        auto it = mWidgets.begin() + idx;
        std::rotate(it, it + 1, mWidgets.end());
        w.setZindex(0);
    }

    void WidgetManager::moveWidgetToBack(Widget &w)
    {
        int idx = getWidgetIndex(w.getWidgetId());
        auto it = mWidgets.begin() + idx;
        std::rotate(mWidgets.begin(), it, it + 1);
        w.setZindex(mWidgets.size() - 1);
    }

    bool WidgetManager::removeWidget(Widget &w)
    {
        for (Widget &widget : mWidgets)
        {
            if (widget.getWidgetId() == w.getWidgetId())
            {
                if (w.getIsPermanentWidget())
                    return false;
                widget.clearIfModal();
                return removeWidget(w.getWidgetId()); // child widgets are deleted in w's destructor
            }
        }
        return false;
    }

    bool WidgetManager::removeWidget(string widgetId)
    {
        vector<string> widgetIds;

        bool deletedWidget = false;

        // Build correct order in which to delete widgets
        for (Widget &widget : mWidgets)
        {
            if (ofToLower(widget.getWidgetId()) == ofToLower(widgetId) && widget.getOwnerWidgetId() == "" && !widget.getIsPermanentWidget())
            {
                widgetIds.push_back(widget.getWidgetId());
            }
        }

        for (Widget &widget : mWidgets)
        {
            if (ofToLower(widget.getOwnerWidgetId()) == ofToLower(widgetId) && !widget.getIsPermanentWidget())
            {
                widgetIds.push_back(widget.getWidgetId());
            }
        }

        for (Widget &widget : mWidgets)
        {
            if (ofToLower(widget.getWidgetId()) == ofToLower(widgetId) && !widget.getIsPermanentWidget())
            {
                widgetIds.push_back(widget.getWidgetId());
            }
        }

        // Now delete the widgets
        while (widgetIds.size() > 0)
        {

            string deleteWidgetId = widgetIds[0];
            Widget *widgetToDelete = WidgetManager::getWidget(deleteWidgetId);
            widgetToDelete->clearIfModal();
            widgetToDelete->beforeDelete();

            mWidgets.erase(std::remove_if(
                               mWidgets.begin(), mWidgets.end(),
                               [&](Widget &widget)
                               {
                                   return widget.matchesWidgetId(deleteWidgetId);
                               }),
                           mWidgets.end());

            widgetIds.erase(std::remove_if(
                                widgetIds.begin(), widgetIds.end(),
                                [&](string wId)
                                {
                                    return wId == deleteWidgetId;
                                }),
                            widgetIds.end());

            if (widgetToDelete)
            {
#if VIZ_DEBUG_LEVEL >= 1 && VIZ_DEBUG_LEVEL < 2
                cout << "\nREMOVING WIDGET: [" << widgetToDelete->getPersistentId() << "] [" << widgetToDelete->getWidgetId() << "]";
#endif

                delete widgetToDelete;
                deletedWidget = true;
                setCurrentProjectIsDirty(true);
            }
        }

        return deletedWidget;
    }

    bool WidgetManager::removeWidgetByPersistentId(string persistentId)
    {
        for (Widget &widget : mWidgets)
        {
            if (widget.getPersistentId() == persistentId)
            {
                return WidgetManager::removeWidget(widget.getWidgetId());
            }
        }
        return false;
    }

    void WidgetManager::highlightWidget(string persistentId, bool highlight, bool clearOtherHighlights)
    {
        if (clearOtherHighlights)
        {
            WidgetManager::clearAllWidgetHighlights();
        }

        Widget *w = WidgetManager::getWidgetByPersistentId(persistentId);
        if (w != nullptr)
            w->setIsHighlighted(highlight);
    }

    void WidgetManager::clearAllWidgetHighlights()
    {
        for (Widget &widget : mWidgets)
        {
            widget.setIsHighlighted(false);
        }
    }

    std::vector<std::reference_wrapper<Widget>> WidgetManager::getWidgets()
    {
        return mWidgets;
    }

    string WidgetManager::getSuggestedNewWidgetPersistentId(string widgetClassType)
    {
        widgetClassType = ofToUpper(widgetClassType);
        ofStringReplace(widgetClassType, "Widget", "");
        return widgetClassType + "_" + ofToString((int)ofRandom(100000, 999999));
    }

    int WidgetManager::getCountOfWidgetClassType(string widgetClassType)
    {
        int count = 0;
        for (Widget &widget : mWidgets)
        {

            string wCT = widget.getWidgetClassType();

            if (widget.getWidgetClassType() == widgetClassType)
            {
                count++;
            }

            if (childWidgetCount(widget) > 0)
            {
                for (Widget &childWidget : widget.getChildWidgets())
                {
                    if (childWidget.getWidgetClassType() == widgetClassType)
                    {
                        count++;
                    }
                }
            }
        }
        return count;
    }

    Widget *WidgetManager::getFrontWidget()
    {
        if (mWidgets.size() == 0)
            return nullptr;
        Widget &w = mWidgets.at(mWidgets.size() - 1);
        return &w;
    }

    string WidgetManager::getWidgetPrettyName(string widgetClassType)
    {
        return Shared::lang("WIDGET_NAME_" + widgetClassType); // eg WIDGET_NAME_Widget in Lanuguage XML file
    }

    Widget *WidgetManager::getWidget(string widgetId)
    {
        for (Widget &widget : mWidgets)
        {
            Widget *w = WidgetManager::getWidget(&widget, widgetId);
            if (w)
                return w;
        }
        return nullptr;
    }

    Widget *WidgetManager::getWidget(Widget *parentWidget, string widgetId)
    {
        /*
        ---------------------------------------------------------------------------------------
        !!! important
        if get exceptions in this block
        check the ~ destructors for any hanging  ofAddListener which don't have matching ofRemoveListener in the destructor!!!!!
        ---------------------------------------------------------------------------------------
        */

        if (parentWidget->getWidgetId() == widgetId)
            return parentWidget;
        for (Widget &childWidget : parentWidget->getChildWidgets())
        {
            if (childWidget.getWidgetId() == widgetId)
            {
                return &childWidget;
            }
            Widget *w = WidgetManager::getWidget(&childWidget, widgetId);
            if (w)
                return w;
        }
        return nullptr;
    }

    Widget *WidgetManager::getWidgetByPersistentId(string persistentId)
    {
        string soughtPersistentId = ofToUpper(persistentId);
        for (Widget &widget : mWidgets)
        {
            Widget *w = WidgetManager::getWidgetByPersistentId(&widget, soughtPersistentId);
            if (w)
                return w;
        }
        return nullptr;
    }

    Widget *WidgetManager::getWidgetByPersistentId(Widget *parentWidget, string persistentId)
    {
        if (ofToUpper(parentWidget->getPersistentId()) == persistentId)
            return parentWidget;
        for (Widget &childWidget : parentWidget->getChildWidgets())
        {
            if (childWidget.getPersistentId() == persistentId)
            {
                return &childWidget;
            }
            Widget *w = WidgetManager::getWidgetByPersistentId(&childWidget, persistentId);
            if (w)
                return w;
        }
        return nullptr;
    }

    WidgetBase *WidgetManager::getWidgetBase(string widgetId, Widget &searchInsideWidget)
    {

        if (searchInsideWidget.getWidgetId() == widgetId)
        {
            return &searchInsideWidget;
        }

        for (WidgetElm &widgetElm : searchInsideWidget.getWidgetElements())
        {
            if (widgetElm.getWidgetId() == widgetId)
            {
                return &widgetElm;
            }
        }

        if (childWidgetCount(searchInsideWidget) > 0)
        {
            for (Widget &childWidget : searchInsideWidget.getChildWidgets())
            {
                if (childWidget.getWidgetId() == widgetId)
                {
                    return &childWidget;
                }

                for (WidgetElm &widgetElm : childWidget.getWidgetElements())
                {
                    if (widgetElm.getWidgetId() == widgetId)
                    {
                        return &widgetElm;
                    }
                }

                WidgetBase *searchChildWidget = WidgetManager::getWidgetBase(widgetId, childWidget);
                if (searchChildWidget != nullptr)
                    return searchChildWidget;
            }
        }

        return nullptr;
    }

    WidgetBase *WidgetManager::getWidgetBase(string widgetId)
    {
        for (Widget &widget : mWidgets)
        {
            WidgetBase *searchWidget = WidgetManager::getWidgetBase(widgetId, widget);
            if (searchWidget != nullptr)
                return searchWidget;
        }
        return nullptr;
    }

    int WidgetManager::getWidgetIndex(string widgetId)
    {
        int idx = 0;
        for (Widget &widget : mWidgets)
        {
            if (widget.getWidgetId() == widgetId)
            {
                return idx;
            }
            idx++;
        }
        return 0;
    }

    int WidgetManager::getWidgetZIndex(string widgetId)
    {
        int idx = 0;
        for (int i = mWidgets.size() - 1; i >= 0; i--)
        {
            Widget &widget = mWidgets[i];
            if (widget.getWidgetId() == widgetId)
            {
                return idx;
            }
            idx++;
        }
        return 0;
    }

    bool WidgetManager::hasActiveChildWidgets(Widget &parentWidget)
    {
        for (Widget &widget : parentWidget.getChildWidgets())
        {
            if (widget.getIsActiveWidget() || WidgetManager::hasActiveChildWidgets(widget))
            {
                return true;
            }
        }
        return false;
    }

    int WidgetManager::childWidgetCount(Widget &parentWidget)
    {
        return parentWidget.getChildWidgets().size();
    }

    Widget *WidgetManager::getActiveParentWidget()
    {
        for (int i = mWidgets.size() - 1; i >= 0; i--)
        {
            Widget &widget = mWidgets[i];
            if (widget.getIsModalWidget())
                return &widget;
            if (widget.getIsActiveWidget() || hasActiveChildWidgets(widget))
                return &widget;
        }
        return nullptr;
    }

    WidgetBase *WidgetManager::getTargetDropWidget()
    {
        return targetDropWidget;
    }

    bool WidgetManager::save(string fileName)
    {
        string widgetCollection = "<widgets version=\"" + applicationProperties.applicationVersion + "\" title=\"" + currentProjectProperties.projectName + "\">";

        for (Widget &widget : mWidgets)
        {
            if (widget.getShouldPersist() && widget.getPersistentId() != "")
            {
                ofxXmlSettings widgetXML = widget.saveState();
                string xmlStr;
                widgetXML.copyXmlToString(xmlStr);
                widgetCollection += xmlStr;
            }
        }
        widgetCollection += "</widgets>";

        ofxXmlSettings settings = ofxXmlSettings();
        if (settings.loadFromBuffer(widgetCollection.c_str()))
        {
            if (settings.save(fileName))
            {
                setProjectProperties("TODO (SAVED...)", ofToDataPath(fileName, true));
                setCurrentProjectIsDirty(false);
                WidgetFileLocationsList::addRecentProject(fileName);
                return true;
            }
        }
        return false;
    }

    bool WidgetManager::load(string fileName, bool clearExisting, Widget *caller)
    {

        if (clearExisting)
        {
            if (caller)
            {
                WidgetManager::removeAllWidgetsExcept(caller);
                caller->shouldDeleteThisWidget();
            }
            else
            {
                WidgetManager::removeAllWidgets();
            }
        }

        Shared::getViz()->clearActiveWidgetId();

        ofxXmlSettings settings = ofxXmlSettings();
        if (settings.load(fileName))
        {

            WidgetManager::BOOTSTRAP(app_ptr);

            TiXmlElement *rootElm = settings.doc.RootElement();

            string projectTitle = settings.getAttribute("widgets", "title", applicationProperties.applicationName + " project");
            setProjectProperties(projectTitle, fileName);

            for (TiXmlNode *child = rootElm->FirstChild(); child; child = child->NextSibling())
            {

                TiXmlPrinter printer;
                child->Accept(&printer);
                string widgetXML = printer.CStr();

                ofxXmlSettings widgetSettings = ofxXmlSettings();
                if (widgetSettings.loadFromBuffer(widgetXML))
                {
                    string persistentId = widgetSettings.getAttribute("widget", "id", "");
                    string widgetClass = widgetSettings.getAttribute("widget", "class", "");

                    Widget *widget = WidgetManager::loadWidget(widgetClass, persistentId, widgetXML);
                    WidgetManager::addWidget(*widget, true);
                    if (widget)
                    {
                        WidgetManager::loadChildWidgets(*widget, widgetXML);
                    }

#if VIZ_DEBUG_LEVEL >= 1 && VIZ_DEBUG_LEVEL <= 2
                    cout << "\nwidget [" << widget->getPersistentId() << "] (" << widgetClass << ") loaded";
#endif
                }
            }
        }
        else
        {
            return false;
        }
        setCurrentProjectIsDirty(false);
        WidgetFileLocationsList::addRecentProject(fileName);

        // Let widgets know that all widgets have loaded
        for (Widget &widget : mWidgets)
        {
            widget.onWidgetAfterAllWidgetsLoaded(mWidgets);
            for (Widget &childWidget : widget.getChildWidgets())
            {
                childWidget.onWidgetAfterAllWidgetsLoaded(mWidgets);
            }
        }

        return true;
    }

    bool WidgetManager::load(string fileName, bool clearExisting)
    {
        return load(fileName, clearExisting, nullptr);
    }

    void WidgetManager::showSaveDialog(string proposedFilePath, string proposedFileName, const std::function<void(const string &)> &saveButtonPressedCallback)
    {
        string classType = WidgetManager::WIDGET_CLASS_FILE_SAVE;
        string widgetPersistentId = WidgetManager::getSuggestedNewWidgetPersistentId(classType);

        WidgetFileSave *w = dynamic_cast<WidgetFileSave *>(WidgetManager::loadWidget(classType, widgetPersistentId, R"(
        <widget>
        <bounds widthExpr="${WINDOW.WIDTH}/2" heightExpr="${WINDOW.HEIGHT}/2" minWidth="75" minHeight="75"  />
        </widget>
        )"));

        w->setSavePathSelectedCallback(saveButtonPressedCallback);
        w->setPath(proposedFilePath == "" ? WidgetFileLocationsList::getMostRecentDirectory() : proposedFilePath);
        w->setProposedFileName(proposedFileName);
        WidgetManager::showModal(w, true);
    }

    void WidgetManager::showLoadDialog(string proposedFilePath, const std::function<void(const string &)> &loadButtonPressedCallback)
    {
        string classType = WidgetManager::WIDGET_CLASS_FILE_LOAD;
        string widgetPersistentId = WidgetManager::getSuggestedNewWidgetPersistentId(classType);

        WidgetFileLoad *w = dynamic_cast<WidgetFileLoad *>(WidgetManager::loadWidget(classType, widgetPersistentId, R"(
        <widget>
        <bounds widthExpr="${WINDOW.WIDTH}/2" heightExpr="${WINDOW.HEIGHT}/2" minWidth="75" minHeight="75"  />
        </widget>
        )"));

        w->setPathSelectedCallback(loadButtonPressedCallback);
        w->setPath(proposedFilePath == "" ? WidgetFileLocationsList::getMostRecentDirectory() : proposedFilePath);
        WidgetManager::showModal(w, true);
    }

    void WidgetManager::setTheme(WidgetTheme theme)
    {
        Shared::getViz()->getThemeManager()->setDefaultTheme(theme);
        if (vizBG != nullptr)
            vizBG->setTheme(theme);
        WidgetTheme themeForSystem = Shared::getViz()->getThemeManager()->getThemeByName(theme.Name, true);
        WidgetTheme contrastingThemeForSystem = Shared::getViz()->getThemeManager()->getContrastingSystemTheme(theme, true);

        Shared::getViz()->cacheFontStyles(Shared::getViz()->getThemeManager()->getDefaultTheme());

        bool useConsthemeForSystem = false;
        for (Widget &widget : mWidgets)
        {

            useConsthemeForSystem = (widget.getWidgetClassType() == WIDGET_CLASS_MENU);
            useConsthemeForSystem ? widget.setTheme(contrastingThemeForSystem) : widget.setTheme(widget.getIsSystemWidget() ? themeForSystem : theme);

            for (Widget &childWidget : widget.getChildWidgets())
            {
                // useConsthemeForSystem = (childWidget.getWidgetClassType() == WIDGET_CLASS_MENU);
                // useConsthemeForSystem ? childWidget.setTheme(contrastingThemeForSystem) :
                //     childWidget.setTheme(childWidget.getIsSystemWidget() ? themeForSystem : theme);

                // Only set contrasting theme for menu child widget (leave other widget types alone!)
                if (childWidget.getWidgetClassType() == WIDGET_CLASS_MENU)
                {
                    childWidget.setTheme(contrastingThemeForSystem);
                }
            }
        }
        WidgetManager::updateWidgets();
    }

    void WidgetManager::initWidgetManager(string applicationName, string applicationVersion, string fileExtension, bool drawBG)
    {
        vizBG = new BG(drawBG);
        applicationProperties = WidgetManager::ApplicationProperties(applicationName, applicationVersion, fileExtension);
        currentProjectProperties = WidgetManager::ProjectProperties("Untitled Project", "untitled", "", "");
        currentProjectProperties.fileName = "untitled." + fileExtension;
        setProjectProperties(currentProjectProperties.projectName, "");
    }

    void WidgetManager::setProjectProperties(string projectName, string absolutePath)
    {
        currentProjectProperties.projectName = projectName;
        currentProjectProperties.absolutePath = absolutePath;

        if (absolutePath != "")
        {
            ofFile loadedFile(absolutePath, ofFile::Mode::ReadOnly, false);
            currentProjectProperties.path = loadedFile.getEnclosingDirectory();
            currentProjectProperties.fileName = loadedFile.getFileName();
        }
        ofSetWindowTitle(applicationProperties.applicationName + " - " + currentProjectProperties.fileName);
    }

    WidgetManager::ProjectProperties WidgetManager::getCurrentProjectProperties()
    {
        return currentProjectProperties;
    }

    void WidgetManager::setCurrentProjectIsDirty(bool val)
    {
        currentProjectProperties.isDirty = val;
        string fileName = currentProjectProperties.fileName + (currentProjectProperties.isDirty ? "*" : "");
        ofSetWindowTitle(applicationProperties.applicationName + " - " + fileName);
    }

    int WidgetManager::getAppMajorVersion(string appVersionString)
    {
        std::vector<std::string> ver = ofSplitString(appVersionString, ".", false, true);
        return (ver.size() > 0) ? std::atoi(ver[0].c_str()) : 0;
    }

    int WidgetManager::getAppMinorVersion(string appVersionString)
    {
        std::vector<std::string> ver = ofSplitString(appVersionString, ".", false, true);
        return (ver.size() > 1) ? std::atoi(ver[1].c_str()) : 0;
    }

    int WidgetManager::getAppPointVersion(string appVersionString)
    {
        std::vector<std::string> ver = ofSplitString(appVersionString, ".", false, true);
        return (ver.size() > 2) ? std::atoi(ver[2].c_str()) : 0;
    }
    
    string WidgetManager::getFrameworkVersion()
    {
        return applicationProperties.frameworkVersion;
    }

    string WidgetManager::getFrameworkName()
    {
        return applicationProperties.frameworkName;
    }   

    void WidgetManager::drawDebugInfo(bool debug)
    {
        SHOW_DEBUG_INFO = debug;

        if (!debug)
        {
            removeWidgetByPersistentId("APP_DEBUG_INFO");
            return;
        }

        if (SHOW_DEBUG_INFO)
        {
            WidgetDebug *debugWidget = dynamic_cast<WidgetDebug *>(WidgetManager::loadWidget(WIDGET_CLASS_DEBUG, "APP_DEBUG_INFO",
                                                                                             "<widget></widget>"));
            debugWidget->setWidgetMapRef(&mWidgets);
            debugWidget->getTheme().UnhoveredWidgetAlpha = 0.95f;
            debugWidget->getTheme().HoveredWidgetAlpha = 0.95f;
            WidgetManager::addWidget(*debugWidget, false, "", false);
        }
    }

    /*************************************************************

     IMPLEMENTING CUSTOM WIDGETS IN YOUR APPLICATION:
     ------------------------------------------------

     1.) Derrive your new widget from the Widget class
     2.) Implement it's functionality
     3.) Bootstrap the widget manager widget map with your new widget(s). The map associates a string name
         with an actual class. eg:

     WidgetManager::widget_map_type appWidgetMap;
     appWidgetMap["Widget"] = &createWidget<Widget>;
     appWidgetMap["WidgetMusical"] = &createWidget<WidgetMusical>;
     appWidgetMap["WidgetChordWheel"] = &createWidget<WidgetChordWheel>;
     ...

     // Bootstrap widget manager with your custom widgets ...
     WidgetManager::bootstrapWidgetMap(appWidgetMap);

    *************************************************************/
    // void WidgetManager::bootstrapWidgetMap(widget_map_type map)
    // {
    //     // widgetClassMap = map;
    // }

    void WidgetManager::showClipboardPopupMenu(int x, int y, string clipboardOperationTargetWidgetId)
    {

        if (clipboardMenu->getIsVisible())
            return;

        clipboardMenu->setAllowCut(true);
        clipboardMenu->setAllowCopy(true);
        clipboardMenu->setAllowPaste(true);

        WidgetElmTextbox *target = WidgetManager::getTextboxById(clipboardOperationTargetWidgetId);
        if (target && target->getTextType() == WidgetElmTextbox::TextType::PASSWORD)
        {
            clipboardMenu->setAllowCut(false);
            clipboardMenu->setAllowCopy(false);
        }
        clipboardMenu->showClipboardMenu(x, y, clipboardOperationTargetWidgetId);
    }

    void WidgetManager::hideClipboardPopupMenu()
    {
        clipboardMenu->hideClipboardMenu(true);
    }

    void WidgetManager::showModal(Widget *widget, bool center)
    {
        if (!widget)
            return;
        Shared::getViz()->clearPopoutWidgetIds();
        if (WidgetManager::getWidget(widget->getWidgetId()) == nullptr)
        {
            WidgetManager::addWidget(*widget, false, "", false);
        }

        widget->setIsVisible(true);
        widget->setModalWidget();
        moveWidgetToFront(*widget);
        center ? centerWidget(widget) : recalculateWidgetExpressions(*widget);

        Shared::getViz()->addToEventTransmitQueue(
            "/widget_manager/widget_show_modal", "widget_manager", "WIDGET_MANAGER", "widget_show_modal", widget->getXML(), true);
    }

    void WidgetManager::showModal(string title, string message, bool formattedMessage, string buttonTitles, string buttonIDs, const std::function<void(const WidgetEventArgs &)> &callback) // buttonTitles & buttonIDs are seperated by '|'`s
    {
        if (dialog == nullptr)
        {
            dialog = new WidgetDialog("GENERIC_DIALOG", "<widget></widget>");
            WidgetManager::addWidget(*dialog, false, "", false);
        }

        Shared::getViz()->clearPopoutWidgetIds();
        dialog->setIsVisible(true);
        dialog->setModalWidget();
        dialog->setWidgetSize(500, 300, false);
        moveWidgetToFront(*dialog);
        centerWidget(dialog);

        dialog->showDialog(
            title,
            message,
            formattedMessage,
            buttonTitles,
            buttonIDs,
            callback);
    }

    void WidgetManager::hideModal()
    {
        Widget *delWidget = WidgetManager::getWidget(Shared::getViz()->getModalWidgetId());
        delWidget->clearIfModal();
        if (delWidget)
            removeWidget(*delWidget);
    }

    WidgetElmTextbox *WidgetManager::getTextboxById(string widgetId)
    {
        return dynamic_cast<WidgetElmTextbox *>(WidgetManager::getWidgetBase(widgetId));
    }

    void WidgetManager::centerWidget(Widget *w)
    {
        if (!w)
            return;
        w->setX_Expr("${WINDOW.WIDTH}/2 - ${SELF.WIDTH}/2");
        w->setY_Expr("${WINDOW.HEIGHT}/2 - ${SELF.HEIGHT}/2");
        recalculateWidgetExpressions(*w);
        w->clearWidgetLayoutExpressions();
    }

}