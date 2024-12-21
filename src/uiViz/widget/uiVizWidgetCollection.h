//
//  uiVizWidgetCollection.h
//  bloom-macos
//
//  Created by Stuart Barnes on 20/08/2018.
//
#pragma once
#include "uiVizWidget.h"

namespace Aquamarine
{
    class uiVizWidgetCollection
    {

    public:
        uiVizWidgetCollection(int width, int height) : mWidth(width), mHeight(height)
        {
        }

        virtual ~uiVizWidgetCollection()
        {
        }

        /*
        void add(uiVizWidget widget) {
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
        std::vector<uiVizWidget> mWidgets;
    };

}