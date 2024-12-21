
#pragma once
#include "uiVizWidgetMediaPlayerBase.h"

namespace Aquamarine
{
    class uiVizWidgetSoundPlayer : public uiVizWidgetMediaPlayerBase
    {
    public:
        uiVizWidgetSoundPlayer(string persistentId, string widgetXML) : uiVizWidgetMediaPlayerBase(persistentId, widgetXML)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            uiVizWidgetMediaPlayerBase::loadState(widgetXML);
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
            ofxXmlSettings settings = uiVizWidgetMediaPlayerBase::saveState();
            settings.setAttribute("widget", "class", uiVizWidgetManager::WIDGET_CLASS_SOUND_PLAYER, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~uiVizWidgetSoundPlayer()
        {
            soundPlayer.unload();
        };

        virtual void updateMediaPlayer()
        {
        }

        virtual float calculatePercComplete()
        {
            return (float)soundPlayer.getPositionMS() / (float)getDuration();
        }

        virtual bool loadFile(string path) override
        {
            filePath = path;
            if (!ofFile::doesFileExist(filePath, false))
                return false;
            soundPlayer.load(filePath);
            isLoaded = soundPlayer.isLoaded();
            if (isLoaded)
                mCalculatedSoundLengthMS = calculateSoundLengthMS();
            return isLoaded;
        }

        virtual void play() override
        {
            soundPlayer.isPlaying() ? soundPlayer.setPaused(false) : soundPlayer.play();
            isPlaying = true;
        }

        virtual void stop() override
        {
            soundPlayer.stop();
            isPlaying = false;
        }

        virtual void pause() override
        {
            soundPlayer.setPaused(true);
            isPlaying = false;
        }

        virtual bool getIsPaused() override
        {
            return !soundPlayer.isPlaying();
        }

        virtual void setPlaybackPosition(float perc)
        {
            soundPlayer.setPosition(perc);
        }

        virtual int getDuration() override
        {
            return mCalculatedSoundLengthMS;
        }

    private:
        int calculateSoundLengthMS()
        {
            soundPlayer.play();
            soundPlayer.setPosition(0.9999999f);
            int ms = soundPlayer.getPositionMS();
            soundPlayer.setPosition(0);
            soundPlayer.stop();
            return ms;
        }

        ofSoundPlayer soundPlayer;
        int mCalculatedSoundLengthMS = 0;

        void initWidget() override
        {
        }
    };
}