#pragma once
#include "../Widget.h"

namespace Aquamarine
{

    class WidgetElmBreadcrumbItemSlice
    {
    public:
        Coord::vizBounds fontRect = Coord::vizBounds(0, 0, 0, 0);
        Coord::vizBounds bounds = Coord::vizBounds(0, 0, 0, 0);
        Elm button;
        ofColor regularColor;
        ofColor selectedColor;
        ofColor hoveredColor;
        ofColor regularFontColor;
        ofColor selectedFontColor;
        ofColor hoveredFontColor;
        ofColor disabledFontColor;

        float currentAlpha = 0.0f;

        bool isHovered()
        {
            int mX = ofGetMouseX();
            int mY = ofGetMouseY();
            return mX > button.getRectangle().x && mY > button.getRectangle().y && mX < button.getRectangle().x + button.getRectangle().width && mY < button.getRectangle().y + button.getRectangle().height;
        }

        WidgetElmBreadcrumbItemSlice() {};

        ~WidgetElmBreadcrumbItemSlice()
        {
        }
    };

    class WidgetElmBreadcrumbItem
    {
    public:
        string label = "";
        string key = "";
        vector<string> metadata;
        bool isSelected = false;
        WidgetElmBreadcrumbItemSlice breadcrumbSlice = WidgetElmBreadcrumbItemSlice();

        WidgetElmBreadcrumbItem(string key, string label) : key(key), label(label) {};
        WidgetElmBreadcrumbItem(string key, string label, vector<string> metadata) : key(key), label(label), metadata(metadata) {};

        ~WidgetElmBreadcrumbItem()
        {
        }
    };

    class WidgetElmBreadcrumb : public WidgetElm
    {

    public:
        WidgetElmBreadcrumb(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
        {
            initWidget();
            loadState(widgetXML);
        }

        /*! Inside \<element>:\n
            \<items>\n
            \<item key="Item_1">Item 1\</item>\n
            \<item key="Item_2">Item 2\</item>\n
            \<item key="Item_3">Item 3\</item>\n
            \</items>\n
        */
        virtual bool loadState(string widgetXML) override
        {
            WidgetElm::loadState(widgetXML);
            ofxXmlSettings settings = ofxXmlSettings();

            if (!settings.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            settings.pushTag("element");

            setIconTag(settings.getAttribute("properties", "icon", "REG_REG_PLAY_BUTTON"));
            mSelectedIndex = 0;
            if (settings.pushTag("items"))
            {
                vector<WidgetElmBreadcrumbItem> items = vector<WidgetElmBreadcrumbItem>();
                for (int j = 0; j < settings.getNumTags("item"); j++)
                {
                    items.push_back(
                        WidgetElmBreadcrumbItem(
                            settings.getAttribute("item", "key", "HC" + ofToString(j), j),
                            settings.getValue("item", "", j)));
                    mSelectedIndex++;
                }
                setBreadcrumbItemItems(items);
                settings.popTag(); // items
            }

            // Get some values here
            settings.popTag();

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings mWidgetXML = WidgetElm::saveState();

            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::BREADCRUMB, 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual ~WidgetElmBreadcrumb()
        {
        }

        virtual void update(WidgetContext context) override
        {
            int accumWidth = 0;
            int index = 0;

            getIsRoundedRectangle() ? breadcrumbBG.setRectRounded(Shared::getViz()->scale(getX()), Shared::getViz()->scale(getY()), Shared::getViz()->scale(getWidth()), Shared::getViz()->scale(getHeight()), scale(5), getTheme().ElementBackgroundColor_withAlpha(getTheme().ElementUnhoveredBackgroundAlpha)) : breadcrumbBG.setRectangle(Shared::getViz()->scale(getX()), Shared::getViz()->scale(getY()), Shared::getViz()->scale(getWidth()), Shared::getViz()->scale(getHeight()), getTheme().ElementBackgroundColor_withAlpha(getTheme().ElementUnhoveredBackgroundAlpha));

            for (WidgetElmBreadcrumbItem &item : mBreadcrumbItemItems)
            {

                int x = Shared::getViz()->scale(getUsableX()) - getScaledPadding() + accumWidth;
                int y = Shared::getViz()->scale(getY());

                fontStyle = Shared::getViz()->getLargeFontStyle();
                ofRectangle fontRect = Shared::getViz()->getTextBounds(item.label, fontStyle, x, y);

                item.breadcrumbSlice.fontRect = Coord::vizBounds(x, y, fontRect.width, fontRect.height);

                int cellWidth = item.breadcrumbSlice.fontRect.width + ((getIconTag() != "") ? icon.getScaledBounds().width + getScaledPadding() * 1.0 : getScaledPadding() * 1.0);

                getIsRoundedRectangle() ? item.breadcrumbSlice.button.setRectRounded(x, y, cellWidth, Shared::getViz()->scale(getHeight()), scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : item.breadcrumbSlice.button.setRectangle(x, y, cellWidth, Shared::getViz()->scale(getHeight()), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));

                accumWidth += cellWidth;

                if (index >= mSelectedIndex)
                    break;
                index++;
            }

            setContentBoundsScaled(Coord::vizBounds(
                getContentBoundsScaled().x,
                getContentBoundsScaled().y,
                accumWidth + getScaledPadding(),
                getUsableHeight()));

            if (mBreadcrumbItemItemsChanged)
            {
                scrollX_End();
                mBreadcrumbItemItemsChanged = false;
            }
        }

        void onWidgetResized(int width, int height) override
        {
            mBreadcrumbItemItemsChanged = true;
        }

        virtual void draw(WidgetContext context) override
        {

            breadcrumbBG.draw();

            vector<WidgetElmBreadcrumbItem> items = getBreadcrumbItems();
            int index = 0;
            for (WidgetElmBreadcrumbItem &item : items)
            {

                bool isHovered = item.breadcrumbSlice.button.isHovered_Rect() && getIsEnabled();

                item.breadcrumbSlice.button.setColor(getTheme().ElementBackgroundColor_withAlpha((isHovered ? getCurrentBackgroundAlpha() : getTheme().ElementUnhoveredBackgroundAlpha)));
                if (isHovered)
                {
                    item.breadcrumbSlice.button.draw();
                }

                ofPushStyle();
                ofSetColor(getTheme().ElementForegroundColor_withAlpha((isHovered ? getCurrentForegroundAlpha() : getTheme().ElementUnhoveredForegroundAlpha)));

                int iconHeight = icon.getScaledBounds().height / 2;
                int buttonHeight = item.breadcrumbSlice.button.getRectangle().height / 2;
                int offset = (iconHeight * 0.5f + buttonHeight * 0.5f) * 0.5f;

                if (getIconTag() != "" && index > 0)
                {
                    icon.setScaledPos(
                        item.breadcrumbSlice.fontRect.x,
                        item.breadcrumbSlice.button.getRectangle().y + offset);
                    icon.drawSvg();
                }

                fontStyle.color = getTheme().TypographyPrimaryColor_withAlpha(getTheme().ElementHoveredForegroundAlpha);

                Shared::getViz()->getFonts(getTheme())->draw(item.label, fontStyle, item.breadcrumbSlice.fontRect.x + ((getIconTag() != "") ? icon.getScaledBounds().width : getScaledPadding() * 1.0f), item.breadcrumbSlice.fontRect.y + item.breadcrumbSlice.fontRect.height * 0.25f + scale(getUsableHeight() * 0.5f));
                ofPopStyle();

                if (isHovered && getIsMousePressedAndReleasedOverWidget(false))
                {
                    press(index);
                }

                if (index >= mSelectedIndex)
                    break;
                index++;
            }
        }

        void press(int index)
        {
            index = min(index, (int)getBreadcrumbItems().size() - 1);
            mSelectedIndex = index;
            string key = getBreadcrumbItemKeyString(index);
            parent()->eventTransmit(getPersistentId(), WIDGET_EVENT::CLICK, "<event index=\"" + ofToString(index) + "\">" + Shared::encodeForXML(key) + "</event>", *this);
            mBreadcrumbItemItemsChanged = true;
            setNeedsUpdate(true);
        }

        void setIconTag(string val)
        {
            mIconTag = val;
            icon = IconCache::getIcon(val);
        }

        string getIconTag()
        {
            return mIconTag;
        }

        vector<WidgetElmBreadcrumbItem> getBreadcrumbItems()
        {
            return mBreadcrumbItemItems;
        }

        string getBreadcrumbItemKeyString(int index)
        {
            vector<WidgetElmBreadcrumbItem> breadcrumbItems = getBreadcrumbItems();
            index = min(index, (int)breadcrumbItems.size() - 1);
            string retVal = "";
            for (int i = 0; i <= index; i++)
            {
                retVal += mDelimiter + breadcrumbItems[i].key;
            }
            return retVal;
        }

        void setBreadcrumbItemItems(string delimitedItems, string delimiter)
        {
            mDelimiter = delimiter;
            vector<string> items = ofSplitString(delimitedItems, delimiter);
            vector<WidgetElmBreadcrumbItem> breadcrumbItems = vector<WidgetElmBreadcrumbItem>();
            int index = -1;
            for (string item : items)
            {
                if (item == "")
                    continue;
                breadcrumbItems.push_back(
                    WidgetElmBreadcrumbItem(
                        item,
                        item));
                index++;
                mSelectedIndex = index;
            }
            setBreadcrumbItemItems(breadcrumbItems);
        }

        void setBreadcrumbItemItems(vector<WidgetElmBreadcrumbItem> items)
        {
            mBreadcrumbItemItems = items;
            mBreadcrumbItemItemsChanged = true;
            setNeedsUpdate(true);
            setWidgetNeedsUpdate(true);
        }

    private:
        Elm breadcrumbBG;

        string mIconTag = "REG_REG_PLAY_BUTTON";
        string mDelimiter = "|";
        int mSelectedIndex = 0;

        ofRectangle titleRect;
        ofxFontStash2::Style fontStyle;
        Icon icon;
        bool mBreadcrumbItemItemsChanged = true;

        vector<WidgetElmBreadcrumbItem> mBreadcrumbItemItems;

        void initWidget() override
        {
            setWidth(100);
            setEnableScrollYPositive(false);
            setEnableScrollYNegative(false);
            setEnableScrollXPositive(true);
            setEnableScrollXNegative(true);
            setScrollAction(ScrollAction::SCROLL);
        }
    };

}