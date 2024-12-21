
#pragma once
#include "uiVizWidget.h"

namespace Aquamarine
{
    class uiVizWidgetWindowTitle : public uiVizWidget
    {

    public:
        uiVizWidgetWindowTitle(string persistentId, string widgetXML) : uiVizWidget(persistentId, widgetXML)
        {
            initWidget();
        }

        virtual ~uiVizWidgetWindowTitle() {
        };

        void draw(uiVizWidgetContext context) override
        {

            if (mIsDragging || (getIsActiveWidget() && getIsMousePressedOverWidget(true)))
            {
                if (!mIsDragging)
                {
                    mIsDragging = true;
                    getViz()->setIsAnyWidgetDraggingOrResizing(true);
                    mDraggingStartedX = deScale(ofGetMouseX()) - ofGetMainLoop()->getCurrentWindow()->getWindowPosition().x;
                    mDraggingStartedY = deScale(ofGetMouseY()) - ofGetMainLoop()->getCurrentWindow()->getWindowPosition().y;
                    getViz()->boostUserExperience(true);
                }

                float newX = deScale(ofGetMouseX()) - mDraggingStartedX;
                float newY = deScale(ofGetMouseY()) - mDraggingStartedY;

                ofSetWindowPosition(newX, newY);
            }
        }

        /*
        void onScaleChanged(float & newScale) override {
            uiVizWidget::onScaleChanged(newScale);
            setWidgetBounds(0, 0, scale(ofGetWindowWidth()), 30);
        }
        */

        void update(uiVizWidgetContext context) override
        {
            setMinWidth(ofGetWindowWidth());
            setWidgetBounds(0, 0, ofGetWindowWidth(), 30);
        }

        virtual void onMouseReleased(ofMouseEventArgs &e)
        {
            mIsDragging = false;
            getViz()->setIsAnyWidgetDraggingOrResizing(false);
            getViz()->revertBoostUserExperience();
        }

    private:
        void initWidget() override
        {
            setMinHeight(30);
            setMinWidth(ofGetWindowWidth());
            setIsResizable(false);
        }

        bool mIsDragging = false;
        int mDraggingStartedX = 0;
        int mDraggingStartedY = 0;
    };
}