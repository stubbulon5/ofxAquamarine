#pragma once
#include "ofMain.h"
#include "Shared.h"

namespace Aquamarine
{
    //------------------------------ ELM CLASS --------------------------------
    // Fundamental building block for all widgets, no draw calls should be made outside of this class

    class Elm
    {

        /*


         void setStringBounds(int x, int y, int width, int height) {
         bounds = Viz::vizBounds(x, y, width, height);
         p.rectangle(x, y, width, height);
         path.update(p);
         }

         void setStringBounds(int x, int y, int width, int height, ofColor color = ofColor(0, 0, 0, 180)) {
         bounds = Viz::vizBounds(x, y, width, height);
         p.rectangle(x, y, width, height);
         path.update(p, color);
         }

         Viz::vizBounds getStringBounds() {
         return bounds;
         }

         */

    public:
        Elm() {}

        string id = "";

        // eventually get rid of this method once all shapes neede are implemented
        void update(ofPath path)
        {
            mPath = path;
            mIsDirty = true;
        }

        // eventually get rid of this method once all shapes neede are implemented
        void update(ofPath path, ofColor color)
        {
            mPath = path;
            mIsDirty = true;
            setColor(color);
        }

        /*************************************************************
         * vizRect
         ************************************************************/
        struct vizRect
        {
            int x;
            int y;
            int width;
            int height;
            float topLeftRadius = 0;
            float topRightRadius = 0;
            float bottomRightRadius = 0;
            float bottomLeftRadius = 0;

            vizRect(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {};

            vizRect(int x, int y, int width, int height, float topLeftRadius, float topRightRadius, float bottomRightRadius, float bottomLeftRadius) : x(x), y(y), width(width), height(height),
                                                                                                                                                       topLeftRadius(topLeftRadius), topRightRadius(topRightRadius), bottomRightRadius(bottomRightRadius), bottomLeftRadius(bottomLeftRadius) {};
        };

        void setRectangle(int x, int y, int width, int height)
        {
            // if (mRect.x == x && mRect.y == y && mRect.width == width && mRect.height == height) return; // explore this early exit optimization later
            mRect = vizRect(x, y, width, height);
            mPath.clear();
            mPath.rectangle(x, y, width, height); // openFrameworks
            mIsDirty = true;
        }

        void setRectangle(int x, int y, int width, int height, ofColor color)
        {
            setRectangle(x, y, width, height);
            setColor(color);
        }

        void setRectRounded(int x, int y, int width, int height, float topLeftRadius, float topRightRadius, float bottomRightRadius, float bottomLeftRadius)
        {

            int maxRadius = std::min((int)width / 4, (int)height / 4);
            mTopLeftRadius = std::min((int)maxRadius, (int)topLeftRadius);
            mTopRightRadius = std::min((int)maxRadius, (int)topRightRadius);
            mBottomRightRadius = std::min((int)maxRadius, (int)bottomRightRadius);
            mBottomLeftRadius = std::min((int)maxRadius, (int)bottomLeftRadius);

            mRect = vizRect(x, y, width, height, mTopLeftRadius, mTopRightRadius, mBottomRightRadius, mBottomLeftRadius);

            mPath.clear();
            mPath.rectRounded(x, y, 0, width, height, mTopLeftRadius, mTopRightRadius, mBottomRightRadius, mBottomLeftRadius); // openFrameworks

            mIsDirty = true;
        }

        void setRectRounded(int x, int y, int width, int height, float radius)
        {
            setRectRounded(x, y, width, height, radius, radius, radius, radius);
        }

        void setRectRounded(int x, int y, int width, int height, float radius, ofColor color)
        {
            setRectRounded(x, y, width, height, radius);
            setColor(color);
        }

        void setRectGradient(ofColor topRight, ofColor bottomRight, ofColor bottomLeft, ofColor topLeft)
        {
            // Colors
            mMesh = mPath.getTessellation();
            int numVert = std::max(0, (int)mMesh.getNumVertices());

            int x_half = (mRect.x + mRect.width / 2.0f);
            int y_half = (mRect.y + mRect.height / 2.0f);

            for (int i = 0; i < numVert; i++)
            {

                auto v = mMesh.getVertex(i);
                mMesh.addTexCoord(v);

                if (v.x < x_half && v.y < y_half)
                { // top left
                    mMesh.addColor(topLeft);
                }
                else if (v.x > x_half && v.y < y_half)
                { // top right
                    mMesh.addColor(topRight);
                }
                else if (v.x > x_half && v.y > y_half)
                { // bottom right
                    mMesh.addColor(bottomRight);
                }
                else if (v.x < x_half && v.y > y_half)
                { // bottom left
                    mMesh.addColor(bottomLeft);
                }
            }

            mIsDirty = false;
            mWasGradientSet = true;
        }

        vizRect getRectangle()
        {
            return mRect;
        }

        void setColor(ofColor color)
        {
            // No need to mark dirty, no geometry has changed!!
            mColor = color;
            mWasColorSet = true;
        }

        ofColor getColor()
        {
            return mColor;
        }

        void clear()
        {
            mPath = ofPath();
        }

        void draw()
        {
            if (mPath.isFilled())
            {
                if (mIsDirty)
                {
                    mMesh = mPath.getTessellation();

                    for (int i = 0; i < mMesh.getNumVertices(); i++)
                    {
                        mMesh.addTexCoord(mMesh.getVertex(i));
                    }
                    mIsDirty = false;
                }

                ofPushStyle();
                if (mWasColorSet && !mWasGradientSet)
                    ofSetColor(mColor);
                mMesh.draw(); // Comment this line to see what naughty draw calls are still taking place!!!!
                ofPopStyle();
            }
            else
            {
                ofPushStyle();
                if (mWasColorSet && !mWasGradientSet)
                    ofSetColor(mColor);
                mPath.draw(); // Comment this line to see what naughty draw calls are still taking place!!!!
                ofPopStyle();
            }
        }

        bool isHovered_Rect(int marginOfErrorX, int marginOfErrorY)
        {
            int mX = ofGetMouseX();
            int mY = ofGetMouseY();
            return mX > getRectangle().x - marginOfErrorX && mY > getRectangle().y - marginOfErrorY && mX < getRectangle().x + getRectangle().width + marginOfErrorX && mY < getRectangle().y + getRectangle().height + marginOfErrorY;
        }

        bool isHovered_Rect()
        {
            return isHovered_Rect(0, 0);
        }

        /*************************************************************
         * vizCircle
         ************************************************************/
        struct vizCircle
        {
            int x;
            int y;
            int radiusX;
            int radiusY;
            bool filled = true;

            vizCircle(int x, int y, int radiusX, int radiusY, bool filled) : x(x), y(y), radiusX(radiusX), radiusY(radiusY), filled(filled) {};
        };

        void setCircle(int x, int y, int radiusX, int radiusY)
        {
            mCirc = vizCircle(x, y, radiusX, radiusY, true);
            mPath.clear();
            mPath.moveTo(x, y);
            mPath.arc(x, y, radiusX, radiusY, 0, 360); // openFrameworks

            mPath.setCircleResolution(360); // Can lower or remove to bump up performance
            mPath.setFilled(true);

            mIsDirty = true;
        }

        void setCircle(int x, int y, int radiusX, int radiusY, ofColor color)
        {
            setCircle(x, y, radiusX, radiusY);
            setColor(color);
        }

        /*************************************************************
         * vizTriangle
         ************************************************************/
        struct vizTriangle
        {
            int x1, x2, x3;
            int y1, y2, y3;

            vizTriangle(float x1, float y1, float x2, float y2, float x3, float y3) : x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3) {};
        };

        void setTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
        {
            mTriangle = vizTriangle(x1, y1, x2, y2, x3, y3);
            mPath.clear();
            mPath.triangle(x1, y1, x2, y2, x3, y3); // openFrameworks
            mPath.setFilled(true);
            mIsDirty = true;
        }

        /*************************************************************
         * Vertex stuff
         ************************************************************/
        void setVertexMode(ofPrimitiveMode mode)
        {
            mMesh.setMode(mode);
        }

        void clearVerticies()
        {
            mMesh.clearVertices();
        }

        void addVertex(float x, float y, ofColor color)
        {
            mMesh.addVertex(ofPoint(x, y));
            mMesh.addColor(color);
            mIsDirty = true;
        }

        void setFilled(bool val)
        {
            mPath.setFilled(val);
            if (!val)
            {
                mPath.setStrokeColor(getColor());
            }
        }

        void setStroke(int strokeWidth, ofColor color)
        {
            setFilled(false);
            mPath.setStrokeWidth(strokeWidth);
            mPath.setStrokeColor(color);
            mIsDirty = true;
        }

        vizTriangle getTriangle()
        {
            return mTriangle;
        }

    private:
        ofVboMesh mMesh;
        ofPath mPath = ofPath();
        ofColor mColor = ofColor::white;

        ofVboMesh mAppendMesh;
        vector<ofPath> mPaths = vector<ofPath>();
        vector<ofColor> mColors = vector<ofColor>();

        vizRect mRect = vizRect(0, 0, 0, 0);
        vizCircle mCirc = vizCircle(0, 0, 0, 0, true);
        vizTriangle mTriangle = vizTriangle(0, 0, 0, 0, 0, 0);

        bool mIsDirty = true;
        bool mWasColorSet = false;
        bool mWasGradientSet = false;

        int mTopLeftRadius = 0;
        int mTopRightRadius = 0;
        int mBottomRightRadius = 0;
        int mBottomLeftRadius = 0;
    };
}