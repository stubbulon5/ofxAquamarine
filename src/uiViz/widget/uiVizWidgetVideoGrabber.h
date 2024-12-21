
#pragma once
#include "uiVizWidgetMediaPlayerBase.h"

namespace Aquamarine
{
    class uiVizWidgetVideoGrabber : public uiVizWidget
    {
    public:
        uiVizWidgetVideoGrabber(string persistentId, string widgetXML) : uiVizWidget(persistentId, widgetXML)
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

            vector<ofVideoDevice> devices = videoGrabber.listDevices();

            for (vector<ofVideoDevice>::iterator it = devices.begin(); it != devices.end(); ++it)
            {
                videoGrabber.setDeviceID(it->id);
                break;
            }

            videoGrabber.setDeviceID(0);
            videoGrabber.setDesiredFrameRate(30);
            videoGrabber.setup(640, 480, true);
            videoGrabber.initGrabber(640, 480, true);

            settings.pushTag("properties");
            setDesiredFrameRate(settings.getAttribute("properties", "desiredFrameRate", 30));
            settings.popTag(); // properties
            settings.popTag(); // widget

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings settings = uiVizWidget::saveState();
            settings.setAttribute("widget", "class", uiVizWidgetManager::WIDGET_CLASS_VIDEO_GRABBER, 0);

            settings.pushTag("widget");
            settings.pushTag("properties");

            // TODO
            settings.setAttribute("properties", "desiredFrameRate", getDesiredFrameRate(), 30);

            settings.popTag(); // properties
            settings.popTag(); // widget

            return settings;
        }

        virtual ~uiVizWidgetVideoGrabber()
        {
            videoGrabber.unbind();
            stopThread();
        };

        virtual float calculatePercComplete()
        {
            return 0;
        }

        virtual void update(uiVizWidgetContext context) override
        {
            lock();
            videoImage = threadImage;
            unlock();
        }

        virtual void draw(uiVizWidgetContext context) override
        {
            videoImage.draw(
                scale(getUsableX()),
                scale(getUsableY()),
                scale(getUsableWidth()),
                scale(getUsableHeight()));
            if (isPlaying)
                setNeedsUpdate(true);
        }

        void threadedFunction()
        {
            while (isThreadRunning())
            {
                videoGrabber.update();
                if (videoGrabber.isFrameNew())
                {
                    lock();
                    threadImage.setFromPixels(videoGrabber.getPixels());
                    unlock();
                }
            }
        }

        virtual void play()
        {
            startThread(true);
            isPlaying = true;
        }

        virtual void stop()
        {
            stopThread();
            videoGrabber.close();
            isPlaying = false;
        }

        virtual void pause()
        {
            stopThread();
            isPlaying = false;
        }

        virtual bool getIsPaused()
        {
            return isPlaying;
        }

        void onWidgetClicked(ofMouseEventArgs &e) override
        {
            isPlaying ? pause() : play();

            // settingsWidget->popoutFrom(descale(ofGetMouseX()), descale(ofGetMouseY()), PopoutDirection::UP);
        }

        bool getDesiredFrameRate()
        {
            return mDesiredFrameRate;
        }

        void setDesiredFrameRate(int val)
        {
            mDesiredFrameRate = val;
            videoGrabber.setDesiredFrameRate(val);
        }

    private:
        ofVideoGrabber videoGrabber;
        ofImage videoImage, threadImage;
        bool isPlaying = false;
        int mDesiredFrameRate = 30;

        uiVizWidget *settingsWidget = nullptr;

        void initWidget() override
        {

            /*
                    add record, save to file
                    settings dialog

                    if (!settingsWidget) {

                        settingsWidget = uiVizWidgetManager::loadWidget(uiVizWidgetManager::WIDGET_CLASS, getPersistentId() + "_SETTINGS", R"(
                        <widget>
                        <bounds width="200" height="300" minWidth="75" minHeight="75"  />
                        </widget>
                        )");
                        addChildWidget(*settingsWidget);

                    }
            */
        }
    };
}