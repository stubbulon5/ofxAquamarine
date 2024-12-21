//
//  WidgetCollection.h
//  bloom-macos
//
//  Created by Stuart Barnes on 20/08/2018.
//
#pragma once
#include "Widget.h"

namespace Aquamarine
{
    class WidgetCollection
    {

    public:
        WidgetCollection(int width, int height) : mWidth(width), mHeight(height)
        {
        }

        virtual ~WidgetCollection()
        {
        }

        /*
        void add(Widget widget) {
            mWidgets.push_back(widget);
        }

        void draw() {
            for(auto w:mWidgets){

                w.draw();
            }
        }

        // Todo, load from setings file
        void load() {

        }
    */
    private:
        int mWidth, mHeight;
        std::vector<Widget> mWidgets;
    };

}