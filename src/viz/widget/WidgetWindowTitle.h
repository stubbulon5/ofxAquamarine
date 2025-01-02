
#pragma once
#include "Widget.h"

namespace Aquamarine
{
    class WidgetWindowTitle : public Widget
    {

    public:
        WidgetWindowTitle(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
        {
            initWidget();
        }

        virtual ~WidgetWindowTitle() {
        };

        void draw(WidgetContext context) override
        {

            if (mIsDragging || (getIsActiveWidget() && getIsMousePressedOverWidget(true)))
            {
                if (!mIsDragging)
                {
                    mIsDragging = true;
                    Shared::getViz()->setIsAnyWidgetDraggingOrResizing(true);
                    mDraggingStartedX = deScale(ofGetMouseX()) - ofGetMainLoop()->getCurrentWindow()->getWindowPosition().x;
                    mDraggingStartedY = deScale(ofGetMouseY()) - ofGetMainLoop()->getCurrentWindow()->getWindowPosition().y;
                    Shared::getViz()->boostUserExperience(true);
                }

                float newX = deScale(ofGetMouseX()) - mDraggingStartedX;
                float newY = deScale(ofGetMouseY()) - mDraggingStartedY;

                ofSetWindowPosition(newX, newY);
            }
        }

        /*
        void onScaleChanged(float & newScale) override {
            Widget::onScaleChanged(newScale);
            setWidgetBounds(0, 0, scale(ofGetWindowWidth()), 30);
        }
        */

        void update(WidgetContext context) override
        {
            setMinWidth(ofGetWindowWidth());
            setWidgetBounds(0, 0, ofGetWindowWidth(), 30);
        }

        virtual void onMouseReleased(ofMouseEventArgs &e)
        {
            mIsDragging = false;
            Shared::getViz()->setIsAnyWidgetDraggingOrResizing(false);
            Shared::getViz()->revertBoostUserExperience();
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