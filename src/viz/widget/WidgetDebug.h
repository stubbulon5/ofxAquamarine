
#pragma once
#include "Widget.h"
#include "WidgetTable.h"
#include "ofxOsc.h"
#include "WidgetMenuCollection.h"

namespace Aquamarine
{

    class WidgetDebug : public Widget
    {

    public:
        WidgetDebug(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
        {
            initWidget();
        }

        virtual ~WidgetDebug()
        {
            if (debugTabs)
                ofRemoveListener(debugTabs->menuItemSelected, this, &WidgetDebug::onMenuItemSelected);
        }

        bool needsUpdateBeforeDraw(bool shouldResetTimer) override
        {
            if (getTimerMillis() >= 500)
            { // refresh debug info every 1/2 second ...
                if (shouldResetTimer)
                    resetTimer();
                return true;
            }
            return false;
        }

        void update(WidgetContext context) override
        {
            if (!mWidgetMapRef || mWidgetMapRef->size() == 0)
                return;
            debugInfo = debugTemplate;
            setTitle("DEBUG INFO: [" + ofToString(ofGetFrameRate(), 0) + "fps]");

            int LEFT_PADDING = 50;

            ofGetFrameRate() < getViz()->getUserExperience() ? getTheme().setTitleFontColor(ofColor(255, 0, 0, 200)) : getTheme().setTitleFontColor(ofColor(0, 0, 0, 200));

            // Widget* active = WidgetManager::getWidget(getViz()->getActiveWidgetId());
            Widget *active = WidgetManager::getActiveParentWidget();
            Widget *prior = WidgetManager::getWidget(getViz()->getPriorActiveWidgetId());
            Widget *activeChild = WidgetManager::getWidget(getViz()->getActiveChildWidgetId());
            Widget *priorChild = WidgetManager::getWidget(getViz()->getPriorActiveChildWidgetId());
            WidgetBase *target = WidgetManager::getTargetDropWidget();
            Widget *nextInlineForFocus = WidgetManager::getWidget(getViz()->getNextInLineForFocusWidgetId());

            try
            {
                ofStringReplace(debugInfo, "${POPOUT_IDS}", Shared::implode(Shared::getViz()->getPopoutWidgetIds(), ','));

                if (active)
                {
                    ofStringReplace(debugInfo, "${ACTIVE}", active ? active->getPersistentId() + " " + active->getWidgetId() : "---");
                    if (active->getWidgetClassType() != WidgetManager::WIDGET_CLASS_DEBUG)
                    {
                        dragData = active->getDragData();
                        ofStringReplace(dragData, "<", "&lt;");
                        ofStringReplace(dragData, ">", "&gt;");
                    }
                    ofStringReplace(debugInfo, "${ACTIVE_DRAG_DATA}", dragData);
                }
                if (prior)
                    ofStringReplace(debugInfo, "${PRIOR}", prior ? prior->getPersistentId() + " " + prior->getWidgetId() : "---");
                if (activeChild)
                    ofStringReplace(debugInfo, "${ACTIVE_CHILD}", activeChild ? activeChild->getPersistentId() + " " + activeChild->getWidgetId() : "---");
                if (priorChild)
                    ofStringReplace(debugInfo, "${PRIOR_CHILD}", priorChild ? priorChild->getPersistentId() + " " + priorChild->getWidgetId() : "---");
                if (target)
                    ofStringReplace(debugInfo, "${TARGET}", target ? target->getPersistentId() + " " + target->getWidgetId() : "---");
                if (nextInlineForFocus)
                    ofStringReplace(debugInfo, "${NEXT_FOR_FOCUS}", nextInlineForFocus->getPersistentId());
            }
            catch (const std::exception &e)
            {
                cout << "WidgetDebug error: " << e.what();
            }

            debugInfo += "<small-mono>WIDGET                                             UPT   DRW   UPT#    DRW#</small-mono><br/>";

            // std::vector<std::reference_wrapper<Widget>> mWidgetMapRef = WidgetManager::getWidgets();

            for (int i = mWidgetMapRef->size() - 1; i >= 0; i--)
            {
                Widget &widget = mWidgetMapRef->at(i);
                string s = widget.getPersistentId();
                if (s.length() < LEFT_PADDING)
                {
                    s.insert(s.end(), LEFT_PADDING - s.size(), ' ');
                }

                if (widget.getPersistentId() != "APP_DEBUG_INFO")
                { // fix this ... (debug widget is showing some weird numbers)
                    string s1 = ofToString(widget.getWidgetTelemetry()->getUpdateMS());

                    int padSize1 = std::max(0, 5 - (int)s1.size());
                    s1.insert(s1.end(), padSize1, ' ');

                    string s2 = ofToString(widget.getWidgetTelemetry()->getDrawMS());
                    int padSize2 = std::max(0, 5 - (int)s2.size());
                    s2.insert(s2.end(), padSize2, ' ');

                    string s3 = ofToString(widget.getWidgetTelemetry()->getUpdateSampleCount());
                    int padSize3 = std::max(0, 5 - (int)s3.size());
                    s3.insert(s3.end(), padSize3, ' ');

                    string s4 = ofToString(widget.getWidgetTelemetry()->getDrawSampleCount());
                    int padSize4 = std::max(0, 5 - (int)s4.size());
                    s4.insert(s4.end(), padSize4, ' ');

                    // mem and cpu
                    debugInfo += "<small-mono>" + s +
                                 " " + s1 +
                                 " " + s2 +
                                 " " + s3 +
                                 " " + s4 +
                                 "<br/></small-mono>";

                    for (WidgetElm &childElm : widget.getWidgetElements())
                    {
                        string s = childElm.getPersistentId();
                        if (s.length() < LEFT_PADDING)
                        {
                            s.insert(s.end(), LEFT_PADDING - s.size(), ' ');
                        }

                        string s1 = ofToString(childElm.getWidgetTelemetry()->getUpdateMS());

                        int padSize1 = std::max(0, 5 - (int)s1.size());
                        s1.insert(s1.end(), padSize1, ' ');

                        string s2 = ofToString(childElm.getWidgetTelemetry()->getDrawMS());
                        int padSize2 = std::max(0, 5 - (int)s2.size());
                        s2.insert(s2.end(), padSize2, ' ');

                        string s3 = ofToString(childElm.getWidgetTelemetry()->getUpdateSampleCount());
                        int padSize3 = std::max(0, 5 - (int)s3.size());
                        s3.insert(s3.end(), padSize3, ' ');

                        string s4 = ofToString(childElm.getWidgetTelemetry()->getDrawSampleCount());
                        int padSize4 = std::max(0, 5 - (int)s4.size());
                        s4.insert(s4.end(), padSize4, ' ');

                        // mem and cpu
                        debugInfo += "<small-mono>..." + s +
                                     " " + s1 +
                                     " " + s2 +
                                     " " + s3 +
                                     " " + s4 +
                                     "<br/></small-mono>";
                    }

                    for (Widget &childWidget : widget.getChildWidgets())
                    {
                        string s = childWidget.getPersistentId();
                        if (s.length() < LEFT_PADDING)
                        {
                            s.insert(s.end(), LEFT_PADDING - s.size(), ' ');
                        }

                        string s1 = ofToString(widget.getWidgetTelemetry()->getUpdateMS());

                        int padSize1 = std::max(0, 5 - (int)s1.size());
                        s1.insert(s1.end(), padSize1, ' ');

                        string s2 = ofToString(widget.getWidgetTelemetry()->getDrawMS());
                        int padSize2 = std::max(0, 5 - (int)s2.size());
                        s2.insert(s2.end(), padSize2, ' ');

                        string s3 = ofToString(widget.getWidgetTelemetry()->getUpdateSampleCount());
                        int padSize3 = std::max(0, 5 - (int)s3.size());
                        s3.insert(s3.end(), padSize3, ' ');

                        string s4 = ofToString(widget.getWidgetTelemetry()->getDrawSampleCount());
                        int padSize4 = std::max(0, 5 - (int)s4.size());
                        s4.insert(s4.end(), padSize4, ' ');

                        // mem and cpu
                        debugInfo += "<small-mono>---" + s +
                                     " " + s1 +
                                     " " + s2 +
                                     " " + s3 +
                                     " " + s4 +
                                     "<br/></small-mono>";

                        for (WidgetElm &childElm : childWidget.getWidgetElements())
                        {
                            string s = childElm.getPersistentId();
                            if (s.length() < LEFT_PADDING)
                            {
                                s.insert(s.end(), LEFT_PADDING - s.size(), ' ');
                            }

                            string s1 = ofToString(childElm.getWidgetTelemetry()->getUpdateMS());

                            int padSize1 = std::max(0, 5 - (int)s1.size());
                            s1.insert(s1.end(), padSize1, ' ');

                            string s2 = ofToString(childElm.getWidgetTelemetry()->getDrawMS());
                            int padSize2 = std::max(0, 5 - (int)s2.size());
                            s2.insert(s2.end(), padSize2, ' ');
                            // mem and cpu
                            debugInfo += "<small-mono>..." + s +
                                         " " + s1 +
                                         " " + s2 +
                                         " " + s3 +
                                         " " + s4 +
                                         "<br/></small-mono>";
                        }
                    }
                }

                debugInfo += "<small-mono><br/></small-mono>";
            }

            debugInfo += "<small-mono>ANIMATIONS:<br/></small-mono>";

            for (Viz::AnimationData animation : getViz()->getAnimations())
            {
                string s = animation.animationName;
                if (s.length() < LEFT_PADDING)
                {
                    s.insert(s.end(), LEFT_PADDING - s.size(), ' ');
                }

                debugInfo += "<small-mono>..." + s + "<br/></small-mono>";
            }

            debugInfo += "<small-mono>POPOUTS:<br/></small-mono>";

            for (string widgetId : getViz()->getPopoutWidgetIds())
            {
                string s = widgetId;
                if (s.length() < LEFT_PADDING)
                {
                    s.insert(s.end(), LEFT_PADDING - s.size(), ' ');
                }

                debugInfo += "<small-mono>..." + s + "<br/></small-mono>";
            }
        }

        void draw(WidgetContext context) override
        {

            if (debugTabs->getSelectedTag() != "DEBUG")
                return;
            ofPushStyle();
            ofSetColor(getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha));
            ofRectangle r = getViz()->getFonts(getTheme())->drawFormattedColumn(debugInfo, getViz()->scale(getUsableX()), getViz()->scale(getUsableY()), getViz()->scale(getUsableWidth() - getViz()->getNonScaledPadding() * 2.0f));
            ofPopStyle();

            setContentBoundsScaled(Coord::vizBounds(r.x, r.y, r.width, r.height));
        }

        void setWidgetMapRef(std::vector<std::reference_wrapper<Widget>> *widgetMapRef)
        {
            mWidgetMapRef = widgetMapRef;
        }

        void addToOscReceiveLog(ofxOscMessage &m)
        {
            string eventSenderType = m.getArgAsString(0);
            string eventSenderId = m.getArgAsString(1);
            string eventTargetId = m.getArgAsString(2);
            string eventName = m.getArgAsString(3);
            string eventXML = m.getArgAsString(4);

            if (eventSenderId == getPersistentId() || eventSenderId == "OSCSENDLOG" || eventSenderId == "OSCRECEIVELOG")
                return; // dont log osc events for debug widget!

            oscReceiveLog->addRow(
                WidgetTableRow("HEADER", {WidgetTableCell(
                                                   "type",
                                                   eventSenderType),
                                               WidgetTableCell(
                                                   "sender",
                                                   eventSenderId),
                                               WidgetTableCell(
                                                   "target",
                                                   eventTargetId),
                                               WidgetTableCell(
                                                   "event",
                                                   eventName),
                                               WidgetTableCell(
                                                   "data",
                                                   eventXML)}));
        }

        void addToOscSendLog(ofxOscMessage &m)
        {
            string eventSenderType = m.getArgAsString(0);
            string eventSenderId = m.getArgAsString(1);
            string eventTargetId = m.getArgAsString(2);
            string eventName = m.getArgAsString(3);
            string eventXML = m.getArgAsString(4);

            if (eventSenderId == getPersistentId() || eventSenderId == "OSCSENDLOG" || eventSenderId == "OSCRECEIVELOG")
                return; // dont log osc events for debug widget!

            oscSendLog->addRow(
                WidgetTableRow("HEADER", {WidgetTableCell(
                                                   "type",
                                                   eventSenderType),
                                               WidgetTableCell(
                                                   "sender",
                                                   eventSenderId),
                                               WidgetTableCell(
                                                   "target",
                                                   eventTargetId),
                                               WidgetTableCell(
                                                   "event",
                                                   eventName),
                                               WidgetTableCell(
                                                   "data",
                                                   eventXML)}));
        }

        void setVisualDebugMode(bool val)
        {
            if (val)
            {
                Shared::setVizDebug(1);
            }
            else
            {
                Shared::setVizDebug(-1);
            }
            WidgetManager::updateWidgets();
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {
            if (args.sender.matchesWidgetOrParent(CHK_VisualDebug))
            {
                setVisualDebugMode(CHK_VisualDebug->getValue());
            }
        }

    private:
        ofxFontStash2::Style defaultStyle;
        string debugTemplate =
            "<br/><br/><br/>"
            "<medium>Popouts : ${POPOUT_IDS}</medium><br/>"
            "<medium>Active Widget : ${ACTIVE}</medium><br/>"
            "<small>${ACTIVE_DRAG_DATA}</small><br/>"
            "<medium>Prior Widget  : ${PRIOR}</medium><br/>"
            "<medium>Active Child : ${ACTIVE_CHILD}</medium><br/>"
            "<medium>Prior Child  : ${PRIOR_CHILD}</medium><br/>"
            "<medium>Drop Target   : ${TARGET}</medium><br/>"
            "<medium>Next Focus   : ${NEXT_FOR_FOCUS}</medium><br/>"
            "<medium>------[Widget Performance]------</medium><br/>";

        std::vector<std::reference_wrapper<Widget>> *mWidgetMapRef = nullptr;

        string debugInfo = "";
        string dragData = "";

        WidgetMenuCollection *debugTabs;

        WidgetTable *oscReceiveLog = nullptr;
        WidgetTable *oscSendLog = nullptr;
        WidgetElmCheckbox *CHK_VisualDebug = nullptr;

        bool loaded = false;

        virtual void onMenuItemSelected(WidgetMenuCollectionArgs &args)
        {
        }

        void initWidget() override
        {
            setWidth(600);
            setHeight(deScale(ofGetWindowHeight()) - scale(30));
            setPosition(
                ofGetWindowWidth() - scale(getWidth()),
                ofGetWindowHeight() - scale(getHeight() + 30));
            defaultStyle = getViz()->getSmallFontStyleMono();
            setScrollAction(ScrollAction::SCROLL);
            setEnableScrollYPositive(true);
            setEnableScrollYNegative(true);
            setEnableScrollXPositive(false);
            setEnableScrollXNegative(false);

            if (!loaded)
            {

                oscReceiveLog = new WidgetTable("oscReceiveLog", R"(
                <widget><bounds xExpr="${PARENT.ABSOLUTE_USABLE_X}" yExpr="${PARENT.ABSOLUTE_USABLE_Y}+${DEBUG_TABS.HEIGHT}" widthExpr="${PARENT.USABLE_WIDTH}" heightExpr="${PARENT.USABLE_HEIGHT}-${DEBUG_TABS.HEIGHT}" />
                    <properties autoResizeHeight="1" autoResizeWidth="1">
                        <header>
                            <cell>Type</cell><cell>Sender</cell><cell>Target</cell><cell>Event</cell><cell>Data</cell>
                        </header>
                    </properties>
                </widget>
                )");
                oscReceiveLog->setIsRoundedRectangle(false);
                // addChildWidget(*oscReceiveLog, true);

                oscSendLog = new WidgetTable("oscSendLog", R"(
                <widget><bounds xExpr="${PARENT.ABSOLUTE_USABLE_X}" yExpr="${PARENT.ABSOLUTE_USABLE_Y}+${DEBUG_TABS.HEIGHT}" widthExpr="${PARENT.USABLE_WIDTH}" heightExpr="${PARENT.USABLE_HEIGHT}-${DEBUG_TABS.HEIGHT}" />
                    <properties autoResizeHeight="1" autoResizeWidth="1">
                        <header>
                            <cell>Type</cell><cell>Sender</cell><cell>Target</cell><cell>Event</cell><cell>Data</cell>
                        </header>
                    </properties>
                </widget>
                )");
                oscSendLog->setIsRoundedRectangle(false);
                //   addChildWidget(*oscSendLog, true);

                debugTabs = new WidgetMenuCollection("DEBUG_TABS", R"(
                <widget>
                    <bounds xExpr="${PARENT.ABSOLUTE_USABLE_X}" yExpr="${PARENT.ABSOLUTE_USABLE_Y}" widthExpr="${PARENT.USABLE_WIDTH}"  />
                    <properties menuType="TAB" />
                </widget>
                )");

                debugTabs->setIsRoundedRectangle(false);
                debugTabs->setMenuCollection({WidgetMenuCollectionItem("DEBUG", nullptr, "DEBUG", IconCache::getIcon("REG_BUG")),
                                              WidgetMenuCollectionItem("OSC_RECEIVE", oscReceiveLog, "OSC [IN]", IconCache::getIcon("REG_BUG")),
                                              WidgetMenuCollectionItem("OSC_SEND", oscSendLog, "OSC [OUT]", IconCache::getIcon("REG_BUG"))});

                debugTabs->setItemHeightExpression("${PARENT.USABLE_HEIGHT}-30");
                addChildWidget(*debugTabs, true);
                ofAddListener(debugTabs->menuItemSelected, this, &WidgetDebug::onMenuItemSelected);

                CHK_VisualDebug = dynamic_cast<WidgetElmCheckbox *>(addOrLoadWidgetElement(CHK_VisualDebug, WIDGET_ELM_CLASS::CHECKBOX, "CHK_VISUAL_DEBUG", R"(
                <element>
                <title>Visual Debug</title>
                <bounds widthExpr="${PARENT.USABLE_WIDTH} - ${PADDING}*2" />
                </element>
                )"));

                CHK_VisualDebug->setX_Expr("${PARENT.LEFT}");
                CHK_VisualDebug->setY_Expr("${PARENT.BOTTOM}");
                // CHK_VisualDebug->setIsVisible(true);

                loaded = true;
            }
        }
    };

}