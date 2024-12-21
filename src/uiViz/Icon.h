
#pragma once
#include "ofMain.h"
#include "ofxSvg.h"
#include "Coord.h"

namespace Aquamarine
{
    class Icon
    {

    public:
        enum IconSize
        {
            REGULAR,
            MEDIUM,
            LARGE
        };

        Icon() : mPath(""), mScaledBounds(Coord::vizBounds(0, 0, 0, 0)), mOriginalScaledBounds(Coord::vizBounds(0, 0, 0, 0)), mScale(1.0f), mIconSize(Icon::IconSize::REGULAR), mColor(ofColor::white), mTag(0) {}

        Icon(string path, Coord::vizBounds scaledBounds, float scale, Icon::IconSize iconSize, ofColor color, int tag, string title)
            : mPath(path), mScaledBounds(scaledBounds), mOriginalScaledBounds(scaledBounds), mScale(scale), mIconSize(iconSize), mColor(color), mTag(tag), mTitle(title)
        {
            Icon(path, scaledBounds, scale, iconSize, color, tag);
        }

        Icon(string path, Coord::vizBounds scaledBounds, float scale, Icon::IconSize iconSize, ofColor color, int tag)
            : mPath(path), mScaledBounds(scaledBounds), mOriginalScaledBounds(scaledBounds), mScale(scale), mIconSize(iconSize), mColor(color), mTag(tag)
        {

            if (mPath == "")
                return;

            ofFile *f = new ofFile(mPath);
            try
            {
                if (f->exists())
                {

                    mExtension = ofToLower(f->getExtension());

                    if (mExtension == "svg")
                    {
                        mSvg.load(mPath);
                        mImageLoaded = true;
                        mSvgLoaded = true;
                        scaleSvg(scale, scale);
                    }
                    else
                    {
                        mImage.load(mPath);
                        mImageLoaded = true;
                    }
                    init();
                }
                else
                {
                    ofLogError("Icon") << "load(): path does not exist: \"" << mPath << "\"\n";
                }
            }
            catch (...)
            {
            }
            delete f;
        }

        void reload(float scale, Coord::vizBounds scaledBounds)
        {
            mScaledBounds = scaledBounds;
            if (mSvgLoaded)
            {
                mSvg.load(mPath);
                mImageLoaded = true;
                scaleSvg(scale, scale);
            }
            init();
        }

        ofImage &getImage()
        {
            return mImage;
        }

        ofxSVG &getSvg()
        {
            return mSvg;
        }

        int getTag()
        {
            return mTag;
        }

        ofColor getColor()
        {
            return mColor;
        }

        Coord::vizBounds getScaledBounds()
        {
            return mScaledBounds;
        }

        void setScaledBounds(int x, int y, int width, int height)
        {
            mScaledBounds = Coord::vizBounds(x, y, width, height);

            if (mExtension == "svg")
            {
                setScaledSize(width, height);
            }
        }

        void setScaledPos(int x, int y)
        {
            mScaledBounds = Coord::vizBounds(x, y, mScaledBounds.width, mScaledBounds.height);
        }

        void setScaledSize(int width, int height)
        {

            if (mExtension == "svg")
            {
                float scaleWidth = (float)width / (float)mOriginalScaledBounds.width;
                float scaleHeight = (float)height / (float)mOriginalScaledBounds.height;
                // this crashes things -> scaleSvg(scaleWidth, scaleHeight);
            }

            mScaledBounds = Coord::vizBounds(mScaledBounds.x, mScaledBounds.y, width, height);
        }

        void setColor(ofColor color)
        {
            mColor = color;
        }

        bool getIsVisible()
        {
            return mVisible;
        }

        void setIsVisible(bool val)
        {
            mVisible = val;
        }

        void drawSvg()
        {

            if (!imageLoaded() || !getIsVisible())
                return;

            ofPushStyle();
            ofPushMatrix();
            ofTranslate(mScaledBounds.x, mScaledBounds.y);

            for (int j = 0; j < getSvg().getNumPath(); j++)
            {
                ofPath p = getSvg().getPathAt(j);
                p.setStrokeColor(mColor);
                p.setColor(mColor);
                // p.scale(mSvgScale, mSvgScale);
                p.draw();
            }
            ofPopMatrix();
            ofPopStyle();
        }

        void scaleSvg(float x, float y)
        {

            if (!imageLoaded())
                return;

            for (int j = 0; j < mSvg.getNumPath(); j++)
            {
                mSvg.getPathAt(j).scale(x, y);
            }
        }

        void drawImage()
        {
            mImage.draw(mScaledBounds.x, mScaledBounds.y, mScaledBounds.width, mScaledBounds.height);
        }

        void draw()
        {
            (mExtension == "svg") ? drawSvg() : drawImage();
        }

        bool isHovered()
        {
            return (ofGetMouseX() >= mScaledBounds.x && ofGetMouseX() < mScaledBounds.x + mScaledBounds.width &&
                    ofGetMouseY() >= mScaledBounds.y && ofGetMouseY() < mScaledBounds.y + mScaledBounds.height);
        }

        bool isPressed()
        {
            return ofGetMousePressed() && isHovered();
        }

        bool imageLoaded()
        {
            return mImageLoaded;
        }

        float getScale()
        {
            return mScale;
        }

        IconSize getIconSize()
        {
            return mIconSize;
        }

        string getPath()
        {
            return mPath;
        }

        string getTitle()
        {
            return mTitle;
        }

        string getExtension()
        {
            return mExtension;
        }

        void setTitle(string val)
        {
            mTitle = val;
        }

        void init()
        {
        }

    private:
        ofImage mImage;
        ofxSVG mSvg;

        bool mImageLoaded = false;
        bool mSvgLoaded = false;
        bool mVisible = true;

        string mIconPath = "";
        string mPath = "";
        string mExtension = "";
        string mTitle = "";

        int mTag;
        Coord::vizBounds mScaledBounds;
        Coord::vizBounds mOriginalScaledBounds;
        float mScale = 1.0f;
        Icon::IconSize mIconSize;
        ofColor mColor;
    };

    class IconCache
    {

    public:
        enum IconTag
        {
            MENU_1 = -1004,
            MENU_2 = -1005,
            MENU_3 = -1006,
            MENU_4 = -1007,
            MENU_5 = -1008,
            ACTION_1 = -1009,
            ACTION_2 = -1010,
            ACTION_3 = -1011,
            ACTION_4 = -1012,
            ACTION_5 = -1013,

            NOT_FOUND = -2000,
            WIDGET_CLOSE = -2001,
            WIDGET_EXPAND_UP = -2002,
            WIDGET_EXPAND_DOWN = -2003,
            WIDGET_SETTINGS = -2004,
            WIDGET_CLIPBOARD = -2005,
            WIDGET_PIN = -2006,
            WIDGET_UNPIN = -2007,
            ADD_CIRCLE = -2008,
            MENU = -2009,
            MED_KEY = -2010,
            BOOK_CONTENT = -2011,
            WIDGET_THEMES = -2012,
            PLAY = -2013,
            STOP = -2014,
            PAUSE = -2014,
            ICON_FILE = -2015,
            ICON_FOLDER = -2016,
            ICON_COMPUTER = -2016,
            ICON_HOME = -2017,
            ICON_DESKTOP = -2018,
            ICON_DOCUMENTS = -2019,
            ICON_DOWNLOADS = -2020,
            ICON_USER = -2021,
            ICON_BUG = -2022,
            ICON_DOT = -2023,
            ICON_DOWNLOAD = -2024,
            ICON_INFO = -2025,
            ICON_NEW_RELEASE = -2026,
            ICON_REFRESH = -2027,
            ICON_UP = -2028,
            ICON_DOWN = -2029,
            ICON_LEFT = -2030,
            ICON_RIGHT = -2031,
            ICON_QUEUE_MUSIC = -2032,
            WIDGET_HELP = -2033,
            WIDGET_SETTINGS_MINI = -2034
        };

        static void cleanse();
        static void scale(float scale);
        static float getIconSizeRegScale(float scale);
        static float getIconSizeReg(float scale);
        static void bootstrapIconMapDefault(float scale);
        static void bootstrapIconMap(float scale, std::map<std::string, Icon *> map);
        static Icon getIcon(string iconKey);
        static Icon &getIconRef(string iconKey);
        static std::map<std::string, Icon *> mIconCacheMap;
    };

}