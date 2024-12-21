
#pragma once
#include "Widget.h"

namespace Aquamarine
{
    class WidgetMediaPlayerBase : public Widget
    {
    public:
        WidgetMediaPlayerBase(string persistentId, string widgetXML) : Widget(persistentId, widgetXML)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {

            ofxXmlSettings settings = ofxXmlSettings();
            if (!settings.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            settings.pushTag("widget");

            // loadFile(settings.getAttribute("properties", "file", "")); don't do this! call from derriving class
            setDoesRespondToFileDrop(settings.getAttribute("properties", "doesRespondToFileDrop", false));
            setIsAutoPlay(settings.getAttribute("properties", "isAutoPlay", false));
            setShowScrubber(settings.getAttribute("properties", "showScrubber", true));
            setShowPlaybackButtons(settings.getAttribute("properties", "showPlaybackButtons", true));

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = Widget::saveState();
            settings.setAttribute("widget", "class", "WidgetMediaPlayerBase", 0);

            settings.pushTag("widget");
            settings.setAttribute("properties", "file", filePath, 0);
            settings.setAttribute("properties", "doesRespondToFileDrop", getDoesRespondToFileDrop(), 0);
            settings.setAttribute("properties", "isAutoPlay", getIsAutoPlay(), 0);
            settings.pushTag("properties");
            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~WidgetMediaPlayerBase() {
        };

        virtual void updateMediaPlayer()
        {
            // Override
        }

        virtual float calculatePercComplete()
        {
            return 0.0f;
        }

        virtual void update(WidgetContext context) override
        {
            if (!getIsLoaded())
                return;

            if (getIsPlaying())
                updateMediaPlayer();

            if (isWidgetHovered())
            {
                // Scrubber
                if (getShowScrubber() && scrubber)
                {
                    if (!scrubber->getIsElmDragging())
                    {
                        float percComplete = calculatePercComplete();
                        scrubber->setPerc(percComplete, false);
                        scrubber->setIsVisible(true);
                    }
                }
                if (getShowPlaybackButtons())
                {
                    play_pause->setIsVisible(true);
                    if (getIsPlaying())
                    {
                        if (play_pause->getIconTag() != "REG_REG_PAUSE_BUTTON")
                            play_pause->setIconTag("REG_REG_PAUSE_BUTTON");
                    }
                    else
                    {
                        if (play_pause->getIconTag() != "REG_REG_PLAY_BUTTON")
                            play_pause->setIconTag("REG_REG_PLAY_BUTTON");
                    }
                }
            }
            else
            {
                // Scrubber
                if (getShowScrubber() && scrubber->getIsVisible())
                {
                    scrubber->setIsVisible(false);
                }
                if (getShowPlaybackButtons())
                {
                    play_pause->setIsVisible(false);
                }

                setNeedsUpdate(true);
                setWidgetNeedsUpdate(true);
            }

            if (isPlaying)
                setNeedsUpdate(true);
        }

        virtual bool loadFile(string path)
        {
            // Override this
            filePath = path;
            return false;
        }

        virtual void play()
        {
            isPlaying = true;
        }

        virtual void stop()
        {
            isPlaying = false;
        }

        virtual void pause()
        {
            isPlaying = false;
        }

        bool getIsLoaded()
        {
            return isLoaded;
        }

        bool getIsPlaying()
        {
            return isPlaying;
        }

        virtual bool getIsPaused()
        {
            return isPlaying;
        }

        virtual void setPlaybackPosition(float perc)
        {
            // Override this...
        }

        virtual int getDuration()
        {
            return 0;
        }

        virtual void onWidgetEventReceived(WidgetEventArgs &args) override
        {

            string event = args.getFullEventName();

            if (args.eventName == WIDGET_EVENT::VALUE_CHANGED && scrubber && args.sender.getPersistentId() == scrubber->getPersistentId())
            {
                setPlaybackPosition(scrubber->getPerc());
                setNeedsUpdate(true);
            }
            else if (args.eventName == WIDGET_EVENT::CLICK && play_pause && args.sender.getPersistentId() == play_pause->getPersistentId())
            {
                getIsPlaying() ? pause() : play();
            }
            else if (args.eventName == WIDGET_EVENT::RECEIVE_DRAG_AND_DROP_DATA)
            {
                if (!getDoesRespondToFileDrop())
                    return;
                string filePath = args.eventXML.getValue("event", "");
                if (ofFile::doesFileExist(filePath, false))
                {
                    loadFile(filePath);
                    play();
                }
            }
        }

        bool getShowScrubber()
        {
            return mShowScrubber;
        }

        void setShowScrubber(bool val)
        {
            mShowScrubber = val;
        }

        bool getShowPlaybackButtons()
        {
            return mShowPlaybackButtons;
        }

        void setShowPlaybackButtons(bool val)
        {
            mShowPlaybackButtons = val;
        }

        bool getDoesRespondToFileDrop()
        {
            return mDoesRespondToFileDrop;
        }

        void setDoesRespondToFileDrop(bool val)
        {
            mDoesRespondToFileDrop = val;
        }

        bool getIsAutoPlay()
        {
            return mIsAutoPlay;
        }

        void setIsAutoPlay(bool val)
        {
            mIsAutoPlay = val;
        }

    protected:
        string filePath;
        bool mDoesRespondToFileDrop = false;
        bool mIsAutoPlay = false;
        bool isPlaying = false;
        bool isLoaded = false;

        WidgetElmSlider *scrubber = nullptr;
        WidgetElmButton *play_pause = nullptr;

    private:
        bool mShowScrubber = true;
        bool mShowPlaybackButtons = true;

        void initWidget() override
        {

            play_pause = dynamic_cast<WidgetElmButton *>(addOrLoadWidgetElement(play_pause, WIDGET_ELM_CLASS::BUTTON, "PLAY_PAUSE", R"(
            <element>
            <bounds xExpr="${PARENT.LEFT}+${PADDING}/2" yExpr="${PARENT.BOTTOM}-${PADDING}/2" widthExpr="18" heightExpr="18" />
            <properties icon="REG_REG_PLAY_BUTTON"/>
            </element>
            )"));
            play_pause->setIsVisible(false);

            scrubber = dynamic_cast<WidgetElmSlider *>(addOrLoadWidgetElement(scrubber, WIDGET_ELM_CLASS::SLIDER, "SCRUBBER", R"(
            <element>
            <bounds xExpr="${PARENT.LEFT}+${PADDING}+${PLAY_PAUSE.WIDTH}" yExpr="${PARENT.BOTTOM}-${PADDING}/2" widthExpr="${PARENT.USABLE_WIDTH}-${PADDING}*2-${PLAY_PAUSE.WIDTH}" heightExpr="18"   />
            </element>
            )"));
            scrubber->setIsVisible(false);
        }
    };
}