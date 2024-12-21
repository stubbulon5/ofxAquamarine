
#pragma once

namespace Aquamarine
{
    class uiVizCoord
    {
    public:
        struct vizBounds
        {
            int x;
            int y;
            int width;
            int height;
            vizBounds(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {};
        };

        struct vizPoint
        {
            int x;
            int y;
            vizPoint(int x, int y) : x(x), y(y) {};
        };

        struct vizIndexOffset
        {
            int index;
            int offset;
            vizIndexOffset(int index, int offset) : index(index), offset(offset) {};
        };
    };

}
