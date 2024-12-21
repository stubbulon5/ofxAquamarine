//
//  uiVizWidgetManager.cpp
//  bloom-macos
//
//  Created by Stuart Barnes on 06/09/2018.
//

#include "uiVizWidgetManager.h"
#include "uiVizBG.h"
#include "widget/uiVizWidgetDebug.h"
#include "widget/uiVizWidgetEventListener.h"
#include "widget/uiVizWidgetClipboardMenu.h"
#include "widget/uiVizWidgetTable.h"
#include "widget/uiVizWidgetMatrix.h"
#include "widget/uiVizWidgetSequencer.h"
#include "widget/uiVizWidgetPianoRoll.h"
#include "widget/uiVizWidgetVideoPlayer.h"
#include "widget/uiVizWidgetVideoGrabber.h"
#include "widget/uiVizWidgetSoundPlayer.h"
#include "widget/uiVizWidgetImageView.h"
#include "widget/uiVizWidgetSettings.h"
#include "widget/uiVizWidgetTextEditor.h"
#include "widget/system/uiVizWidgetFileList.h"
#include "widget/system/uiVizWidgetFileLocationsList.h"
#include "widget/system/uiVizWidgetFileExplorer.h"
#include "widget/system/uiVizWidgetFileLoad.h"
#include "widget/system/uiVizWidgetFileSave.h"
#include "widget/system/uiVizWidgetThemeEditor.h"
#include "widget/system/uiVizWidgetThemePreview.h"
#include "widget/system/uiVizWidgetWidgetList.h"
#include "widget/system/uiVizWidgetDialog.h"
#include "widget/uiVizWidgetMenuCollection.h"

#include "ofxOsc.h"

namespace Aquamarine
{
    std::vector<std::reference_wrapper<uiVizWidget>> uiVizWidgetManager::mWidgets;

    template <typename T>
    uiVizWidget *createWidget(string persistentId, string widgetXML) { return new T(persistentId, widgetXML); }
    uiVizWidgetManager::widget_map_type widgetClassMap;
    uiVizWidget *currentPopout = nullptr;

    typedef exprtk::expression<float> expression_t;
    expression_t expression;

    typedef exprtk::parser<float> calculator_t;
    calculator_t calculator;

    bool popoutMovedToFront = false;
    int popoutCount = 0;

    uiVizBG *vizBG = nullptr;

    string prev_eventSenderType, prev_eventSenderId, prev_eventTargetId, prev_eventName, prev_eventXML;

    uiVizWidgetManager::ApplicationProperties applicationProperties(
        "ApplicationName", "0.0.0", "xml");

    uiVizWidgetManager::ProjectProperties currentProjectProperties(
        "Untitled Project", "untitled", "", "");

    bool SHOW_DEBUG_INFO = false;

    uiVizWidgetClipboardMenu *clipboardMenu = nullptr;
    uiVizWidgetBase *targetDropWidget = nullptr;

    static ofEvent<uiVizWidgetEventArgs> widgetManagerEventReceived;

    void uiVizWidgetManager::addWidget(uiVizWidget &widget, bool shouldPersist, string ownerWidgetId, bool transmitOsc)
    {
        if (widget.getPersistentId() == "")
        {
            widget.setPersistentId(uiVizWidgetSignature::generateRandomId(widget.getWidgetClassType()));
        }
        uiVizWidgetManager::removeWidget(widget.getPersistentId());
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
            uiVizShared::getViz()->addToEventTransmitQueue(
                "/widget_manager/widget_added", "widget_manager", "WIDGET_MANAGER", "widget_added", widget.getXML(), true);
        }
    }

    void uiVizWidgetManager::addWidget(uiVizWidget &widget, bool shouldPersist, string ownerWidgetId)
    {
        uiVizWidgetManager::addWidget(widget, shouldPersist, ownerWidgetId, true);
    }

    void uiVizWidgetManager::addWidget(uiVizWidget &widget, bool shouldPersist)
    {
        uiVizWidgetManager::addWidget(widget, shouldPersist, "");
    }

    void uiVizWidgetManager::addWidgetForPopout(uiVizWidget &widget, string ownerWidgetId, bool shouldCloseIfOtherWidgetClicked)
    {
        widget.setCloseAction(uiVizWidget::CloseAction::HIDE);
        if (widget.getIsModalWidget())
        {
        }
        widget.setShouldCloseIfOtherWidgetClicked(shouldCloseIfOtherWidgetClicked);
        widget.setIsVisible(false);
        uiVizWidgetManager::addWidget(widget, false, ownerWidgetId);
    }

    uiVizWidget *uiVizWidgetManager::loadWidget(string WIDGET_CLASS, string persistentId, string widgetXML)
    {
        uiVizWidgetManager::bootstrapWidgetMapDefault();
        if (persistentId == "")
        {
            persistentId = uiVizWidgetSignature::generateRandomId(WIDGET_CLASS);
        }

        uiVizWidgetManager::removeWidget(*(uiVizWidgetManager::getWidgetByPersistentId(persistentId)));

        uiVizWidget *widget;
        bool wasFoundInMap = false;

        // Does the class exist in the map?
        if (widgetClassMap.count(WIDGET_CLASS) > 0)
        {
            widget = widgetClassMap[WIDGET_CLASS](persistentId, widgetXML);
            widget->setWidgetClassType(WIDGET_CLASS);
            wasFoundInMap = true;
        }
        else
        {
            ofLogWarning("uiVizWidgetManager") << "[Widget:" << persistentId << "] of class \"" << WIDGET_CLASS << "\" failed to load. [" + widgetXML + "]";
        }
        if (!wasFoundInMap)
            widget = new uiVizWidget(persistentId, widgetXML);

        // Child widgets...
        if (widget)
        {
            uiVizWidgetManager::loadChildWidgets(*widget, widgetXML);
            widget->setSize(widget->getWidth(), widget->getHeight()); // ensure min, max bounds
            widget->setWidgetNeedsUpdate(true);
            recalculateWidgetExpressions(*widget);
        }

        return widget;
    }

    uiVizWidget *uiVizWidgetManager::loadWidget(string widgetXML)
    {
        uiVizWidgetSignature widget = uiVizWidget::getWidgetSignatureFromXML(widgetXML);
        return uiVizWidgetManager::loadWidget(widget.widgetClass, widget.persistentId, widget.widgetXML);
    }

    uiVizWidget *uiVizWidgetManager::loadWidgetFromFile(string WIDGET_CLASS, string persistentId, string widgetFilePath)
    {
        string widgetXML = uiVizWidget::getWidgetXMLFromFile(widgetFilePath, true);
        return uiVizWidgetManager::loadWidget(WIDGET_CLASS, persistentId, widgetXML);
    }

    uiVizWidget *uiVizWidgetManager::loadWidgetFromFile(string widgetFilePath)
    {
        uiVizWidgetSignature widget = uiVizWidget::getWidgetSignatureFromFile(widgetFilePath);
        return uiVizWidgetManager::loadWidget(widget.widgetClass, widget.persistentId, widget.widgetXML);
    }

    void uiVizWidgetManager::loadWidgetFromFileToExisting(string widgetFilePath, uiVizWidget &widget)
    {
        string widgetXML = uiVizWidget::getWidgetXMLFromFile(widgetFilePath, true);
        widget.loadState(widgetXML);
        uiVizWidgetManager::loadChildWidgets(widget, widgetXML);
    }

    void uiVizWidgetManager::loadChildWidgets(uiVizWidget &targetWidget, string widgetXML)
    {
        uiVizWidgetManager::bootstrapWidgetMapDefault();
        vector<uiVizWidgetSignature> childWidgetList = uiVizWidget::getChildWidgetsList(widgetXML);
        for (uiVizWidgetSignature childWidget : childWidgetList)
        {
            uiVizWidget *w = uiVizWidgetManager::loadWidget(childWidget.widgetClass, childWidget.persistentId, childWidget.widgetXML);
            w->setIsAutoResizeToContentsWidth(false);
            targetWidget.addChildWidget(*w, true);
        }
        targetWidget.onChildWidgetsLoaded();
    }

    uiVizWidget *uiVizWidgetManager::loadChildWidget(uiVizWidget &targetWidget, string widgetXML)
    {
        uiVizWidgetManager::bootstrapWidgetMapDefault();
        uiVizWidget *w = uiVizWidgetManager::loadWidget(widgetXML);
        targetWidget.addChildWidget(*w, true);
        return w;
    }

    void uiVizWidgetManager::recalculateAllWidgetExpressions()
    {
        for (uiVizWidget &widget : mWidgets)
        {
            widget.setExpressionRecalculationRequired(true);
        }
    }

    void uiVizWidgetManager::recalculateWidgetExpressions(uiVizWidget &w)
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

            for (uiVizWidget &exprWidget : mWidgets)
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

            for(uiVizWidget &exprWidget:mWidgets) {
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
                    ofLogWarning("uiVizWidgetManager") << w.getPersistentId() << ".setWidgetPosition([x]): failed to evaluate expression \"" << w.getX_Expr() << "\" [---->" + exprStringX + "<----] : " << calculator.error().c_str();
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
                    ofLogWarning("uiVizWidgetManager") << w.getPersistentId() << ".setWidgetPosition([y]): failed to evaluate expression \"" << w.getY_Expr() << "\" [---->" + exprStringY + "<----] : " << calculator.error().c_str();
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
                    ofLogWarning("uiVizWidgetManager") << w.getPersistentId() << ".setWidgetSize([width]): failed to evaluate expression \"" << w.getWidth_Expr() << "\" [---->" + exprStringWidth + "<----] : " << calculator.error().c_str();
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
                    ofLogWarning("uiVizWidgetManager") << w.getPersistentId() << ".setWidgetSize([height]): failed to evaluate expression \"" << w.getHeight_Expr() << "\" [---->" + exprStringHeight + "<----] : " << calculator.error().c_str();
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

        for (uiVizWidget &childWidget : w.getChildWidgets())
        {
            uiVizWidgetManager::recalculateWidgetExpressions(childWidget);
            childWidget.updateWidgetPopoutArrow();
        }

        // Update popout arrow / locations if it's owned by 'w'
        if (uiVizShared::getViz()->getIsPopoutVisible())
        {
            for (string popoutId : uiVizShared::getViz()->getPopoutWidgetIds())
            {
                uiVizWidget *popout = uiVizWidgetManager::getWidget(popoutId);
                if (popout && popout->getOwnerWidgetId() == w.getWidgetId())
                {
                    popout->updateWidgetPopoutArrow();
                    break;
                }
            }
        }
    }

    void uiVizWidgetManager::drawWidget(uiVizWidget &widget, int widgetZIndex)
    {

        // Expression based layout
        if (widget.getIsRootWidget() && widget.getExpressionRecalculationRequired())
        {
            recalculateWidgetExpressions(widget); //(recursive)
            // Do any other root widgets reference this one in their layout expressions?
            for (uiVizWidget &w : mWidgets)
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
            uiVizWidgetManager::showClipboardPopupMenu(uiVizShared::getViz()->deScale(ofGetMouseX()) - 20, uiVizShared::getViz()->deScale(ofGetMouseY()) - 20, widget.getWidgetId());
        }

        // Widget Elms
        for (uiVizWidgetElm &widgetElm : widget.getWidgetElements())
        {
            if (widgetElm.shouldShowClipboardForThisWidget() && !clipboardMenu->getIsVisible())
            {
                uiVizWidgetManager::showClipboardPopupMenu(uiVizShared::getViz()->deScale(ofGetMouseX()) - 20, uiVizShared::getViz()->deScale(ofGetMouseY()) - 20, widgetElm.getWidgetId());
            }
            widgetElm.determineElementVisibility();
        }

        for (uiVizWidget &childWidget : widget.getChildWidgets())
        {
            childWidget.setZindex(widgetZIndex);
            // childWidget.drawWidget();
            uiVizWidgetManager::drawWidget(childWidget, widgetZIndex);
        }

        // Content drag icon to draw?
        if (widget.isDraggingContent())
        {

            if (uiVizShared::getViz()->getTargetDropWidgetId() != "")
            {
                targetDropWidget = getWidget(uiVizShared::getViz()->getTargetDropWidgetId());
                widget.setTargetDropWidget(targetDropWidget);
            }

            ofPushStyle();
            widget.drawContentDragIcon();
            ofPopStyle();
        }
    }

    void uiVizWidgetManager::drawWidgets()
    {
        vizBG->draw();
        /*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*
         MOVE THE ACTIVE WIDGET TO THE FRONT IF NEEDED
         THIS ACTUALLY MEANS MOVING IT TO THE BACK OF THE VECTOR...
         *-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*/
        if (uiVizShared::getViz()->getIsPopoutVisible())
        {
            vector<string> popoutWidgetIds = uiVizShared::getViz()->getPopoutWidgetIds();
            if (!popoutMovedToFront || popoutCount != popoutWidgetIds.size())
            {
                popoutCount = popoutWidgetIds.size();
                for (string popoutId : popoutWidgetIds)
                {
                    uiVizWidget *popout = uiVizWidgetManager::getWidget(popoutId);

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
            uiVizWidget *activeWidget = getActiveParentWidget();
            if (activeWidget != nullptr && activeWidget->getZindex() > 0)
            {
                if (childWidgetCount(*activeWidget) > 0)
                {
                    for (uiVizWidget &childWidget : activeWidget->getChildWidgets())
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
            uiVizWidget &currWidget = mWidgets[i];
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
        uiVizWidget *activeWidget = uiVizWidgetManager::getWidget(uiVizShared::getViz()->getActiveWidgetId());
        if (activeWidget && !activeWidget->getIsVisible())
        {
            uiVizShared::getViz()->clearActiveWidgetId();
        }

        if (uiVizWidgetManager::checkShouldCloseOrDelete(activeWidget))
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
            uiVizWidget &widget = mWidgets[i];
            widget.setZindex(widgetZIndex);

            // Modal background ?
            if (widget.getIsModalWidget())
            {
                uiVizElm vizElm_modal_background;
                vizElm_modal_background.setRectangle(
                    0, 0, ofGetWindowWidth(), ofGetWindowHeight(),
                    widget.getTheme().getWidgetModalBackgroundColor());
                vizElm_modal_background.draw();
            }

            // Draw the widget and child widgets (recursively)
            uiVizWidgetManager::drawWidget(widget, widgetZIndex);
            widgetZIndex--;

            if (uiVizWidgetManager::checkShouldCloseOrDelete(&widget))
            {
                return;
            }
        }

        // Local event listener event dispatch
        while (uiVizShared::getViz()->hasLocalListenEventsInTransmitQueue())
        {
            uiViz::WidgetEvent e = uiVizShared::getViz()->getNextLocalListenMessageToTransmit();
            uiVizWidgetManager::handleEventReceive(e.eventSenderType, e.eventSenderId, e.eventTargetId, e.eventName, e.eventXML);
        }

        // Osc Send
        while (uiVizShared::getViz()->hasOscEventsInTransmitQueue())
        {
            ofxOscMessage m;
            uiVizShared::getViz()->getNextOscMessageToTransmit(m);
            if (m.getAddress() != "")
            {
                cout << "Transmitting OSC Event:\n";
                uiVizShared::getViz()->sendOscMessage(m);

                if (SHOW_DEBUG_INFO)
                {
                    dynamic_cast<uiVizWidgetDebug *>(uiVizWidgetManager::getWidgetByPersistentId("APP_DEBUG_INFO"))->addToOscSendLog(m);
                }
            }
        }

        // Osc Receive
        while (uiVizShared::getViz()->hasOscEventsInReceiveQueue())
        {
            cout << "Receiving OSC Event:\n";
            ofxOscMessage m;
            uiVizShared::getViz()->getNextOscMessage(m);
            uiVizWidgetManager::handleEventReceive(m.getArgAsString(0), m.getArgAsString(1), m.getArgAsString(2), m.getArgAsString(3), m.getArgAsString(4));

            if (SHOW_DEBUG_INFO)
            {
                dynamic_cast<uiVizWidgetDebug *>(uiVizWidgetManager::getWidgetByPersistentId("APP_DEBUG_INFO"))->addToOscReceiveLog(m);
            }
        }

        // Fps counter
        if (uiVizShared::getViz()->getShowFps())
        {
            uiVizShared::getViz()->getFonts()->draw(
                ofToString(ofGetFrameRate(), 0) + " fps",
                uiVizShared::getViz()->getMediumFontStyle(),
                ofGetWindowWidth() - uiVizShared::getViz()->scale(35),
                ofGetWindowHeight() - uiVizShared::getViz()->scale(10));
        }
    }

    void uiVizWidgetManager::updateWidget(uiVizWidget &widget)
    {
        widget.setNeedsUpdate(true);
        widget.setWidgetNeedsUpdate(true);
        for (uiVizWidget &childWidget : widget.getChildWidgets())
        {
            childWidget.setNeedsUpdate(true);
            childWidget.setWidgetNeedsUpdate(true);
        }
    }

    void uiVizWidgetManager::updateWidgets()
    {
        for (uiVizWidget &widget : mWidgets)
        {
            uiVizWidgetManager::updateWidget(widget);
        }
    }

    bool uiVizWidgetManager::checkShouldCloseOrDelete(uiVizWidget *widget)
    {
        if (!widget)
            return false;
        // Closing widget - notify owner
        if (widget->shouldCloseThisWidget())
        {
            widget->setIsVisible(false);
            if (widget->getOwnerWidgetId() != "")
            {
                uiVizWidget *widgetOwner = uiVizWidgetManager::getWidget(widget->getOwnerWidgetId());
                if (widgetOwner)
                    widgetOwner->onWidgetOwnedWidgetClosed(widget);
            }
        }

        // Removing widget - this should always be last!!
        if (widget->shouldDeleteThisWidget())
        {
            for (int i = 0; i < mWidgets.size(); i++)
            {
                uiVizWidget &checkWidget = mWidgets[i];
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

    void uiVizWidgetManager::handleEventReceive(string eventSenderType, string eventSenderId, string eventTargetId, string eventName, string eventXML)
    {

        if (prev_eventSenderType != eventSenderType || prev_eventSenderId != eventSenderId ||
            prev_eventTargetId != eventTargetId || prev_eventName != eventName || prev_eventXML != eventXML)
        {

            if (eventSenderType == "widget_manager")
            {
                if (eventName == "widget_added")
                {
                    uiVizWidget *w = uiVizWidgetManager::loadWidget(eventXML);
                    uiVizWidgetManager::addWidget(*w, true);
                }
                else if (eventName == "widget_show_modal")
                {
                    uiVizWidget *w = uiVizWidgetManager::loadWidget(eventXML);
                    uiVizWidgetManager::showModal(w, true);
                }
            }
            else if (eventSenderType == "widget")
            {

                uiVizWidget *sender = uiVizWidgetManager::getWidgetByPersistentId(eventSenderId);
                uiVizWidget *target = (eventTargetId != "" ? uiVizWidgetManager::getWidgetByPersistentId(eventTargetId) : nullptr);

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

                uiVizWidgetEventArgs args = uiVizWidgetEventArgs(eventSenderId, eventName, eventXML, *sender, target);
                ofNotifyEvent(widgetManagerEventReceived, args);
            }
        }

        prev_eventSenderType = eventSenderType;
        prev_eventSenderId = eventSenderId;
        prev_eventTargetId = eventTargetId;
        prev_eventName = eventName;
        prev_eventXML = eventXML;
    }

    void uiVizWidgetManager::removeAllWidgetsExceptMultiple(vector<uiVizWidgetBase *> exceptWidgets)
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
            for (uiVizWidget &widget : mWidgets)
            {
                if (exceptWidgets.size() > 0 && exceptWidgets[0] != nullptr)
                {
                    if (!widget.matchesWidget(exceptWidgets))
                    {
                        if (widget.getWidgetClassType() == WIDGET_CLASS_DEBUG)
                            continue;
                        if (widget.getPersistentId() == "CLIPBOARD_MENU" || widget.getPersistentId() == "CLIPBOARD")
                            continue;
                        if (widget.getOwnerWidgetId() == "" && uiVizWidgetManager::removeWidget(widget))
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
                        uiVizWidgetManager::removeWidget(widget))
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

    void uiVizWidgetManager::removeAllWidgetsExcept(uiVizWidget *exceptWidget)
    {
        uiVizWidgetManager::removeAllWidgetsExceptMultiple({exceptWidget});
    }

    void uiVizWidgetManager::removeAllWidgets()
    {
        removeAllWidgetsExcept(nullptr);
    }

    void uiVizWidgetManager::moveWidgetToFront(uiVizWidget &w)
    {
        int idx = getWidgetIndex(w.getWidgetId());
        auto it = mWidgets.begin() + idx;
        std::rotate(it, it + 1, mWidgets.end());
        w.setZindex(0);
    }

    void uiVizWidgetManager::moveWidgetToBack(uiVizWidget &w)
    {
        int idx = getWidgetIndex(w.getWidgetId());
        auto it = mWidgets.begin() + idx;
        std::rotate(mWidgets.begin(), it, it + 1);
        w.setZindex(mWidgets.size() - 1);
    }

    bool uiVizWidgetManager::removeWidget(uiVizWidget &w)
    {
        for (uiVizWidget &widget : mWidgets)
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

    bool uiVizWidgetManager::removeWidget(string widgetId)
    {
        vector<string> widgetIds;

        bool deletedWidget = false;

        // Build correct order in which to delete widgets
        for (uiVizWidget &widget : mWidgets)
        {
            if (ofToLower(widget.getWidgetId()) == ofToLower(widgetId) && widget.getOwnerWidgetId() == "" && !widget.getIsPermanentWidget())
            {
                widgetIds.push_back(widget.getWidgetId());
            }
        }

        for (uiVizWidget &widget : mWidgets)
        {
            if (ofToLower(widget.getOwnerWidgetId()) == ofToLower(widgetId) && !widget.getIsPermanentWidget())
            {
                widgetIds.push_back(widget.getWidgetId());
            }
        }

        for (uiVizWidget &widget : mWidgets)
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
            uiVizWidget *widgetToDelete = uiVizWidgetManager::getWidget(deleteWidgetId);
            widgetToDelete->clearIfModal();
            widgetToDelete->beforeDelete();

            mWidgets.erase(std::remove_if(
                               mWidgets.begin(), mWidgets.end(),
                               [&](uiVizWidget &widget)
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

    bool uiVizWidgetManager::removeWidgetByPersistentId(string persistentId)
    {
        for (uiVizWidget &widget : mWidgets)
        {
            if (widget.getPersistentId() == persistentId)
            {
                return uiVizWidgetManager::removeWidget(widget.getWidgetId());
            }
        }
        return false;
    }

    void uiVizWidgetManager::highlightWidget(string persistentId, bool highlight, bool clearOtherHighlights)
    {
        if (clearOtherHighlights)
        {
            uiVizWidgetManager::clearAllWidgetHighlights();
        }

        uiVizWidget *w = uiVizWidgetManager::getWidgetByPersistentId(persistentId);
        if (w != nullptr)
            w->setIsHighlighted(highlight);
    }

    void uiVizWidgetManager::clearAllWidgetHighlights()
    {
        for (uiVizWidget &widget : mWidgets)
        {
            widget.setIsHighlighted(false);
        }
    }

    std::vector<std::reference_wrapper<uiVizWidget>> uiVizWidgetManager::getWidgets()
    {
        return mWidgets;
    }

    string uiVizWidgetManager::getSuggestedNewWidgetPersistentId(string widgetClassType)
    {
        widgetClassType = ofToUpper(widgetClassType);
        ofStringReplace(widgetClassType, "UIVIZWIDGET", "");
        return widgetClassType + "_" + ofToString((int)ofRandom(100000, 999999));
    }

    int uiVizWidgetManager::getCountOfWidgetClassType(string widgetClassType)
    {
        int count = 0;
        for (uiVizWidget &widget : mWidgets)
        {

            string wCT = widget.getWidgetClassType();

            if (widget.getWidgetClassType() == widgetClassType)
            {
                count++;
            }

            if (childWidgetCount(widget) > 0)
            {
                for (uiVizWidget &childWidget : widget.getChildWidgets())
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

    uiVizWidget *uiVizWidgetManager::getFrontWidget()
    {
        if (mWidgets.size() == 0)
            return nullptr;
        uiVizWidget &w = mWidgets.at(mWidgets.size() - 1);
        return &w;
    }

    string uiVizWidgetManager::getWidgetPrettyName(string widgetClassType)
    {
        return uiVizShared::lang("WIDGET_NAME_" + widgetClassType); // eg WIDGET_NAME_uiVizWidget in Lanuguage XML file
    }

    uiVizWidget *uiVizWidgetManager::getWidget(string widgetId)
    {
        for (uiVizWidget &widget : mWidgets)
        {
            uiVizWidget *w = uiVizWidgetManager::getWidget(&widget, widgetId);
            if (w)
                return w;
        }
        return nullptr;
    }

    uiVizWidget *uiVizWidgetManager::getWidget(uiVizWidget *parentWidget, string widgetId)
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
        for (uiVizWidget &childWidget : parentWidget->getChildWidgets())
        {
            if (childWidget.getWidgetId() == widgetId)
            {
                return &childWidget;
            }
            uiVizWidget *w = uiVizWidgetManager::getWidget(&childWidget, widgetId);
            if (w)
                return w;
        }
        return nullptr;
    }

    uiVizWidget *uiVizWidgetManager::getWidgetByPersistentId(string persistentId)
    {
        string soughtPersistentId = ofToUpper(persistentId);
        for (uiVizWidget &widget : mWidgets)
        {
            uiVizWidget *w = uiVizWidgetManager::getWidgetByPersistentId(&widget, soughtPersistentId);
            if (w)
                return w;
        }
        return nullptr;
    }

    uiVizWidget *uiVizWidgetManager::getWidgetByPersistentId(uiVizWidget *parentWidget, string persistentId)
    {
        if (ofToUpper(parentWidget->getPersistentId()) == persistentId)
            return parentWidget;
        for (uiVizWidget &childWidget : parentWidget->getChildWidgets())
        {
            if (childWidget.getPersistentId() == persistentId)
            {
                return &childWidget;
            }
            uiVizWidget *w = uiVizWidgetManager::getWidgetByPersistentId(&childWidget, persistentId);
            if (w)
                return w;
        }
        return nullptr;
    }

    uiVizWidgetBase *uiVizWidgetManager::getWidgetBase(string widgetId, uiVizWidget &searchInsideWidget)
    {

        if (searchInsideWidget.getWidgetId() == widgetId)
        {
            return &searchInsideWidget;
        }

        for (uiVizWidgetElm &widgetElm : searchInsideWidget.getWidgetElements())
        {
            if (widgetElm.getWidgetId() == widgetId)
            {
                return &widgetElm;
            }
        }

        if (childWidgetCount(searchInsideWidget) > 0)
        {
            for (uiVizWidget &childWidget : searchInsideWidget.getChildWidgets())
            {
                if (childWidget.getWidgetId() == widgetId)
                {
                    return &childWidget;
                }

                for (uiVizWidgetElm &widgetElm : childWidget.getWidgetElements())
                {
                    if (widgetElm.getWidgetId() == widgetId)
                    {
                        return &widgetElm;
                    }
                }

                uiVizWidgetBase *searchChildWidget = uiVizWidgetManager::getWidgetBase(widgetId, childWidget);
                if (searchChildWidget != nullptr)
                    return searchChildWidget;
            }
        }

        return nullptr;
    }

    uiVizWidgetBase *uiVizWidgetManager::getWidgetBase(string widgetId)
    {
        for (uiVizWidget &widget : mWidgets)
        {
            uiVizWidgetBase *searchWidget = uiVizWidgetManager::getWidgetBase(widgetId, widget);
            if (searchWidget != nullptr)
                return searchWidget;
        }
        return nullptr;
    }

    int uiVizWidgetManager::getWidgetIndex(string widgetId)
    {
        int idx = 0;
        for (uiVizWidget &widget : mWidgets)
        {
            if (widget.getWidgetId() == widgetId)
            {
                return idx;
            }
            idx++;
        }
        return 0;
    }

    int uiVizWidgetManager::getWidgetZIndex(string widgetId)
    {
        int idx = 0;
        for (int i = mWidgets.size() - 1; i >= 0; i--)
        {
            uiVizWidget &widget = mWidgets[i];
            if (widget.getWidgetId() == widgetId)
            {
                return idx;
            }
            idx++;
        }
        return 0;
    }

    bool uiVizWidgetManager::hasActiveChildWidgets(uiVizWidget &parentWidget)
    {
        for (uiVizWidget &widget : parentWidget.getChildWidgets())
        {
            if (widget.getIsActiveWidget() || uiVizWidgetManager::hasActiveChildWidgets(widget))
            {
                return true;
            }
        }
        return false;
    }

    int uiVizWidgetManager::childWidgetCount(uiVizWidget &parentWidget)
    {
        return parentWidget.getChildWidgets().size();
    }

    uiVizWidget *uiVizWidgetManager::getActiveParentWidget()
    {
        for (int i = mWidgets.size() - 1; i >= 0; i--)
        {
            uiVizWidget &widget = mWidgets[i];
            if (widget.getIsModalWidget())
                return &widget;
            if (widget.getIsActiveWidget() || hasActiveChildWidgets(widget))
                return &widget;
        }
        return nullptr;
    }

    uiVizWidgetBase *uiVizWidgetManager::getTargetDropWidget()
    {
        return targetDropWidget;
    }

    bool uiVizWidgetManager::save(string fileName)
    {
        string widgetCollection = "<widgets version=\"" + applicationProperties.applicationVersion + "\" title=\"" + currentProjectProperties.projectName + "\">";

        for (uiVizWidget &widget : mWidgets)
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
                uiVizWidgetFileLocationsList::addRecentProject(fileName);
                return true;
            }
        }
        return false;
    }

    bool uiVizWidgetManager::load(string fileName, bool clearExisting, uiVizWidget *caller)
    {

        if (clearExisting)
        {
            if (caller)
            {
                uiVizWidgetManager::removeAllWidgetsExcept(caller);
                caller->shouldDeleteThisWidget();
            }
            else
            {
                uiVizWidgetManager::removeAllWidgets();
            }
        }

        uiVizShared::getViz()->clearActiveWidgetId();

        ofxXmlSettings settings = ofxXmlSettings();
        if (settings.load(fileName))
        {

            uiVizWidgetManager::bootstrapWidgetMapDefault();

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

                    uiVizWidget *widget = uiVizWidgetManager::loadWidget(widgetClass, persistentId, widgetXML);
                    uiVizWidgetManager::addWidget(*widget, true);
                    if (widget)
                    {
                        uiVizWidgetManager::loadChildWidgets(*widget, widgetXML);
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
        uiVizWidgetFileLocationsList::addRecentProject(fileName);

        // Let widgets know that all widgets have loaded
        for (uiVizWidget &widget : mWidgets)
        {
            widget.onWidgetAfterAllWidgetsLoaded(mWidgets);
            for (uiVizWidget &childWidget : widget.getChildWidgets())
            {
                childWidget.onWidgetAfterAllWidgetsLoaded(mWidgets);
            }
        }

        return true;
    }

    bool uiVizWidgetManager::load(string fileName, bool clearExisting)
    {
        return load(fileName, clearExisting, nullptr);
    }

    void uiVizWidgetManager::showSaveDialog(string proposedFilePath, string proposedFileName, const std::function<void(const string &)> &saveButtonPressedCallback)
    {
        string classType = uiVizWidgetManager::WIDGET_CLASS_FILE_SAVE;
        string widgetPersistentId = uiVizWidgetManager::getSuggestedNewWidgetPersistentId(classType);

        uiVizWidgetFileSave *w = dynamic_cast<uiVizWidgetFileSave *>(uiVizWidgetManager::loadWidget(classType, widgetPersistentId, R"(
        <widget>
        <bounds widthExpr="${WINDOW.WIDTH}/2" heightExpr="${WINDOW.HEIGHT}/2" minWidth="75" minHeight="75"  />
        </widget>
        )"));

        w->setSavePathSelectedCallback(saveButtonPressedCallback);
        w->setPath(proposedFilePath == "" ? uiVizWidgetFileLocationsList::getMostRecentDirectory() : proposedFilePath);
        w->setProposedFileName(proposedFileName);
        uiVizWidgetManager::showModal(w, true);
    }

    void uiVizWidgetManager::setTheme(uiVizWidgetTheme theme)
    {
        uiVizShared::getViz()->getThemeManager()->setDefaultTheme(theme);
        vizBG->setTheme(theme);
        uiVizWidgetTheme themeForSystem = uiVizShared::getViz()->getThemeManager()->getThemeByName(theme.Name, true);
        uiVizWidgetTheme contrastingThemeForSystem = uiVizShared::getViz()->getThemeManager()->getContrastingSystemTheme(theme, true);

        uiVizShared::getViz()->cacheFontStyles(uiVizShared::getViz()->getThemeManager()->getDefaultTheme());

        bool useConsthemeForSystem = false;
        for (uiVizWidget &widget : mWidgets)
        {

            useConsthemeForSystem = (widget.getWidgetClassType() == WIDGET_CLASS_MENU);
            useConsthemeForSystem ? widget.setTheme(contrastingThemeForSystem) : widget.setTheme(widget.getIsSystemWidget() ? themeForSystem : theme);

            for (uiVizWidget &childWidget : widget.getChildWidgets())
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
        uiVizWidgetManager::updateWidgets();
    }

    void uiVizWidgetManager::initWidgetManager(string applicationName, string applicationVersion, string fileExtension)
    {
        vizBG = new uiVizBG();
        applicationProperties = uiVizWidgetManager::ApplicationProperties(applicationName, applicationVersion, fileExtension);
        currentProjectProperties = uiVizWidgetManager::ProjectProperties("Untitled Project", "untitled", "", "");
        currentProjectProperties.fileName = "untitled.jam";
        setProjectProperties(currentProjectProperties.projectName, "");
    }

    void uiVizWidgetManager::setProjectProperties(string projectName, string absolutePath)
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

    uiVizWidgetManager::ProjectProperties uiVizWidgetManager::getCurrentProjectProperties()
    {
        return currentProjectProperties;
    }

    void uiVizWidgetManager::setCurrentProjectIsDirty(bool val)
    {
        currentProjectProperties.isDirty = val;
        string fileName = currentProjectProperties.fileName + (currentProjectProperties.isDirty ? "*" : "");
        ofSetWindowTitle(applicationProperties.applicationName + " - " + fileName);
    }

    int uiVizWidgetManager::getAppMajorVersion(string appVersionString)
    {
        std::vector<std::string> ver = ofSplitString(appVersionString, ".", false, true);
        return (ver.size() > 0) ? std::atoi(ver[0].c_str()) : 0;
    }

    int uiVizWidgetManager::getAppMinorVersion(string appVersionString)
    {
        std::vector<std::string> ver = ofSplitString(appVersionString, ".", false, true);
        return (ver.size() > 1) ? std::atoi(ver[1].c_str()) : 0;
    }

    int uiVizWidgetManager::getAppPointVersion(string appVersionString)
    {
        std::vector<std::string> ver = ofSplitString(appVersionString, ".", false, true);
        return (ver.size() > 2) ? std::atoi(ver[2].c_str()) : 0;
    }

    const string uiVizWidgetManager::WIDGET_CLASS = "uiVizWidget";
    const string uiVizWidgetManager::WIDGET_CLASS_MENU = "uiVizWidgetMenu";                      // these are not yet general purpose, and rely on parent widgets initializing them
    const string uiVizWidgetManager::WIDGET_CLASS_MENU_COLLECTION = "uiVizWidgetMenuCollection"; // these are not yet general purpose, and rely on parent widgets initializing them
    const string uiVizWidgetManager::WIDGET_CLASS_TABLE = "uiVizWidgetTable";
    const string uiVizWidgetManager::WIDGET_CLASS_MATRIX = "uiVizWidgetMatrix";
    const string uiVizWidgetManager::WIDGET_CLASS_SEQUENCER = "uiVizWidgetSequencer";
    const string uiVizWidgetManager::WIDGET_CLASS_PIANO_ROLL = "uiVizWidgetPianoRoll";
    const string uiVizWidgetManager::WIDGET_CLASS_VIDEO_PLAYER = "uiVizWidgetVideoPlayer";
    const string uiVizWidgetManager::WIDGET_CLASS_VIDEO_GRABBER = "uiVizWidgetVideoGrabber";
    const string uiVizWidgetManager::WIDGET_CLASS_SOUND_PLAYER = "uiVizWidgetSoundPlayer";
    const string uiVizWidgetManager::WIDGET_CLASS_IMAGE_VIEW = "uiVizWidgetImageView";
    const string uiVizWidgetManager::WIDGET_CLASS_DEBUG = "uiVizWidgetDebug";
    const string uiVizWidgetManager::WIDGET_CLASS_EVENT_LISTENER = "uiVizWidgetEventListener";
    const string uiVizWidgetManager::WIDGET_CLASS_CLIPBOARD_MENU = "uiVizWidgetClipboardMenu";
    const string uiVizWidgetManager::WIDGET_CLASS_SETTINGS = "uiVizWidgetSettings";
    const string uiVizWidgetManager::WIDGET_CLASS_TEXT_EDITOR = "uiVizWidgetTextEditor";

    // System
    const string uiVizWidgetManager::WIDGET_CLASS_FILE_LIST = "uiVizWidgetFileList";
    const string uiVizWidgetManager::WIDGET_CLASS_FILE_LOCATIONS_LIST = "uiVizWidgetFileLocationsList";
    const string uiVizWidgetManager::WIDGET_CLASS_FILE_EXPLORER = "uiVizWidgetFileExplorer";
    const string uiVizWidgetManager::WIDGET_CLASS_FILE_LOAD = "uiVizWidgetFileLoad";
    const string uiVizWidgetManager::WIDGET_CLASS_FILE_SAVE = "uiVizWidgetFileSave";
    const string uiVizWidgetManager::WIDGET_CLASS_THEME_EDITOR = "uiVizWidgetThemeEditor";
    const string uiVizWidgetManager::WIDGET_CLASS_THEME_PREVIEW = "uiVizWidgetThemePreview";
    const string uiVizWidgetManager::WIDGET_CLASS_WIDGET_LIST = "uiVizWidgetWidgetList";
    const string uiVizWidgetManager::WIDGET_CLASS_DIALOG = "uiVizWidgetDialog";

    void uiVizWidgetManager::bootstrapWidgetMapDefault()
    {
        if (applicationProperties.applicationVersion == "0.0.0")
        {
            string exceptionStr = "A valid version number has not been set for this aplication. Set it using 'uiVizWidgetManager::initWidgetManager' Terminating...";
            throw std::runtime_error(exceptionStr.c_str());
            OF_EXIT_APP(1);
        }
        widgetClassMap[WIDGET_CLASS] = &createWidget<uiVizWidget>;
        widgetClassMap[WIDGET_CLASS_DEBUG] = &createWidget<uiVizWidgetDebug>;
        widgetClassMap[WIDGET_CLASS_EVENT_LISTENER] = &createWidget<uiVizWidgetEventListener>;
        widgetClassMap[WIDGET_CLASS_MENU] = &createWidget<uiVizWidgetMenu>;
        widgetClassMap[WIDGET_CLASS_MENU_COLLECTION] = &createWidget<uiVizWidgetMenuCollection>;
        widgetClassMap[WIDGET_CLASS_TABLE] = &createWidget<uiVizWidgetTable>;
        widgetClassMap[WIDGET_CLASS_MATRIX] = &createWidget<uiVizWidgetMatrix>;
        widgetClassMap[WIDGET_CLASS_SEQUENCER] = &createWidget<uiVizWidgetSequencer>;
        widgetClassMap[WIDGET_CLASS_PIANO_ROLL] = &createWidget<uiVizWidgetPianoRoll>;
        widgetClassMap[WIDGET_CLASS_VIDEO_PLAYER] = &createWidget<uiVizWidgetVideoPlayer>;
        widgetClassMap[WIDGET_CLASS_VIDEO_GRABBER] = &createWidget<uiVizWidgetVideoGrabber>;
        widgetClassMap[WIDGET_CLASS_SOUND_PLAYER] = &createWidget<uiVizWidgetSoundPlayer>;
        widgetClassMap[WIDGET_CLASS_IMAGE_VIEW] = &createWidget<uiVizWidgetImageView>;
        widgetClassMap[WIDGET_CLASS_CLIPBOARD_MENU] = &createWidget<uiVizWidgetClipboardMenu>;
        widgetClassMap[WIDGET_CLASS_SETTINGS] = &createWidget<uiVizWidgetSettings>;
        widgetClassMap[WIDGET_CLASS_TEXT_EDITOR] = &createWidget<uiVizWidgetTextEditor>;

        widgetClassMap[WIDGET_CLASS_FILE_LIST] = &createWidget<uiVizWidgetFileList>;
        widgetClassMap[WIDGET_CLASS_FILE_LOCATIONS_LIST] = &createWidget<uiVizWidgetFileLocationsList>;
        widgetClassMap[WIDGET_CLASS_FILE_EXPLORER] = &createWidget<uiVizWidgetFileExplorer>;
        widgetClassMap[WIDGET_CLASS_FILE_LOAD] = &createWidget<uiVizWidgetFileLoad>;
        widgetClassMap[WIDGET_CLASS_FILE_SAVE] = &createWidget<uiVizWidgetFileSave>;
        widgetClassMap[WIDGET_CLASS_THEME_EDITOR] = &createWidget<uiVizWidgetThemeEditor>;
        widgetClassMap[WIDGET_CLASS_THEME_PREVIEW] = &createWidget<uiVizWidgetThemePreview>;
        widgetClassMap[WIDGET_CLASS_WIDGET_LIST] = &createWidget<uiVizWidgetWidgetList>;
        widgetClassMap[WIDGET_CLASS_DIALOG] = &createWidget<uiVizWidgetDialog>;

        // Clipboard menu
        if (clipboardMenu == nullptr)
        {
            clipboardMenu = new uiVizWidgetClipboardMenu("CLIPBOARD", "<widget><appearance visible='0'/></widget>");
            uiVizWidgetManager::addWidget(*clipboardMenu, false, "", false);
        }
    }

    uiVizWidgetManager::widget_map_type uiVizWidgetManager::getWidgetMap()
    {
        return widgetClassMap;
    }

    void uiVizWidgetManager::drawDebugInfo(bool debug)
    {
        SHOW_DEBUG_INFO = debug;

        if (!debug)
        {
            removeWidgetByPersistentId("APP_DEBUG_INFO");
            return;
        }

        if (SHOW_DEBUG_INFO)
        {
            uiVizWidgetDebug *debugWidget = dynamic_cast<uiVizWidgetDebug *>(uiVizWidgetManager::loadWidget(WIDGET_CLASS_DEBUG, "APP_DEBUG_INFO",
                                                                                                            "<widget></widget>"));
            debugWidget->setWidgetMapRef(&mWidgets);
            debugWidget->getTheme().UnhoveredWidgetAlpha = 0.95f;
            debugWidget->getTheme().HoveredWidgetAlpha = 0.95f;
            uiVizWidgetManager::addWidget(*debugWidget, false, "", false);
        }
    }

    /*************************************************************

     IMPLEMENTING CUSTOM WIDGETS IN YOUR APPLICATION:
     ------------------------------------------------

     1.) Derrive your new widget from the uiVizWidget class
     2.) Implement it's functionality
     3.) Bootstrap the widget manager widget map with your new widget(s). The map associates a string name
         with an actual class. eg:

     uiVizWidgetManager::widget_map_type appWidgetMap;
     appWidgetMap["uiVizWidget"] = &createWidget<uiVizWidget>;
     appWidgetMap["uiVizWidgetMusical"] = &createWidget<uiVizWidgetMusical>;
     appWidgetMap["uiVizWidgetChordWheel"] = &createWidget<uiVizWidgetChordWheel>;
     ...

     // Bootstrap widget manager with your custom widgets ...
     uiVizWidgetManager::bootstrapWidgetMap(appWidgetMap);

    *************************************************************/
    void uiVizWidgetManager::bootstrapWidgetMap(widget_map_type map)
    {
        widgetClassMap = map;
    }

    void uiVizWidgetManager::showClipboardPopupMenu(int x, int y, string clipboardOperationTargetWidgetId)
    {

        if (clipboardMenu->getMenu()->getIsVisible())
            return;

        clipboardMenu->setAllowCut(true);
        clipboardMenu->setAllowCopy(true);
        clipboardMenu->setAllowPaste(true);

        uiVizWidgetElmTextbox *target = uiVizWidgetManager::getTextboxById(clipboardOperationTargetWidgetId);
        if (target && target->getTextType() == uiVizWidgetElmTextbox::TextType::PASSWORD)
        {
            clipboardMenu->setAllowCut(false);
            clipboardMenu->setAllowCopy(false);
        }
        clipboardMenu->showClipboardMenu(x, y, clipboardOperationTargetWidgetId);
    }

    void uiVizWidgetManager::hideClipboardPopupMenu()
    {
        clipboardMenu->hideClipboardMenu(true);
    }

    void uiVizWidgetManager::showModal(uiVizWidget *widget, bool center)
    {
        if (!widget)
            return;
        uiVizShared::getViz()->clearPopoutWidgetIds();
        if (uiVizWidgetManager::getWidget(widget->getWidgetId()) == nullptr)
        {
            uiVizWidgetManager::addWidget(*widget, false, "", false);
        }

        widget->setIsVisible(true);
        widget->setModalWidget();
        moveWidgetToFront(*widget);
        center ? centerWidget(widget) : recalculateWidgetExpressions(*widget);

        uiVizShared::getViz()->addToEventTransmitQueue(
            "/widget_manager/widget_show_modal", "widget_manager", "WIDGET_MANAGER", "widget_show_modal", widget->getXML(), true);
    }

    void uiVizWidgetManager::hideModal()
    {
        uiVizWidget *delWidget = uiVizWidgetManager::getWidget(uiVizShared::getViz()->getModalWidgetId());
        delWidget->clearIfModal();
        if (delWidget)
            removeWidget(*delWidget);
    }

    uiVizWidgetElmTextbox *uiVizWidgetManager::getTextboxById(string widgetId)
    {
        return dynamic_cast<uiVizWidgetElmTextbox *>(uiVizWidgetManager::getWidgetBase(widgetId));
    }

    void uiVizWidgetManager::centerWidget(uiVizWidget *w)
    {
        if (!w)
            return;
        w->setX_Expr("${WINDOW.WIDTH}/2 - ${SELF.WIDTH}/2");
        w->setY_Expr("${WINDOW.HEIGHT}/2 - ${SELF.HEIGHT}/2");
        recalculateWidgetExpressions(*w);
        w->clearWidgetLayoutExpressions();
    }

}