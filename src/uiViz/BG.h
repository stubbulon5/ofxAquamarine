
#include "Shared.h"
#pragma once
namespace Aquamarine
{

    class BG
    {

    public:
        BG(bool shouldDraw)
        {
            WidgetTheme theme = Shared::getViz()->getThemeManager()->getDefaultTheme();
            setTheme(theme);
            mShouldDraw = shouldDraw;
        }

        ~BG()
        {
        }

        void setTheme(WidgetTheme theme)
        {
            bgCol1 = theme.BackgroundColor1_withAlpha(1.0f);
            bgCol2 = theme.BackgroundColor2_withAlpha(1.0f);
            bgCol3 = theme.BackgroundColor3_withAlpha(1.0f);
            bgCol4 = theme.BackgroundColor4_withAlpha(1.0f);
        }

        void draw()
        {
            if(!mShouldDraw) return;
            ofMesh temp;
            temp.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            temp.addVertex(ofPoint(0, 0));
            temp.addColor(bgCol1);
            temp.addVertex(ofPoint(ofGetWindowWidth(), 0));
            temp.addColor(bgCol2);
            temp.addVertex(ofPoint(0, ofGetWindowHeight()));
            temp.addColor(bgCol3);
            temp.addVertex(ofPoint(ofGetWindowWidth(), ofGetWindowHeight()));
            temp.addColor(bgCol4);
            temp.draw();
        }

    private:
        ofColor bgCol1, bgCol2, bgCol3, bgCol4;
        bool mShouldDraw = true;
    };
}