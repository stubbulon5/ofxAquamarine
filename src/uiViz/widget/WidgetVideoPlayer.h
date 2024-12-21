
#pragma once
#include "WidgetMediaPlayerBase.h"

namespace Aquamarine
{
    class WidgetVideoPlayer : public WidgetMediaPlayerBase
    {
    public:
        WidgetVideoPlayer(string persistentId, string widgetXML) : WidgetMediaPlayerBase(persistentId, widgetXML)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            WidgetMediaPlayerBase::loadState(widgetXML);
            ofxXmlSettings settings = ofxXmlSettings();
            if (!settings.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            settings.pushTag("widget");
            loadFile(settings.getAttribute("properties", "file", ""));
            getIsAutoPlay() ? play() : stop();

            settings.pushTag("properties");
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = WidgetMediaPlayerBase::saveState();
            settings.setAttribute("widget", "class", WidgetManager::WIDGET_CLASS_VIDEO_PLAYER, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~WidgetVideoPlayer()
        {
            videoPlayer.unbind();
        };

        virtual void updateMediaPlayer()
        {
            videoPlayer.update();
        }

        virtual float calculatePercComplete()
        {
            return (float)videoPlayer.getCurrentFrame() / (float)videoPlayer.getTotalNumFrames();
        }

        virtual void draw(WidgetContext context) override
        {
            if (!getIsLoaded())
                return;
            videoPlayer.draw(
                scale(getUsableX()),
                scale(getUsableY()),
                scale(getUsableWidth()),
                scale(getUsableHeight()));
        }

        virtual bool loadFile(string path) override
        {
            filePath = path;
            isLoaded = videoPlayer.load(filePath);
            return isLoaded;
        }

        virtual void play() override
        {
            videoPlayer.play();
            videoPlayer.setPaused(false);
            isPlaying = true;
        }

        virtual void stop() override
        {
            videoPlayer.stop();
            isPlaying = false;
        }

        virtual void pause() override
        {
            videoPlayer.setPaused(true);
            isPlaying = false;
        }

        virtual bool getIsPaused() override
        {
            return videoPlayer.isPaused();
        }

        virtual void setPlaybackPosition(float perc)
        {
            videoPlayer.setPosition(perc);
        }

        virtual int getDuration() override
        {
            return videoPlayer.getDuration();
        }

    private:
        ofVideoPlayer videoPlayer;

        WidgetElmSlider *scrubber = nullptr;
        WidgetElmButton *play_pause = nullptr;

        void initWidget() override
        {
        }
    };
}