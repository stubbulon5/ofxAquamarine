#pragma once
#include "../Widget.h"
#include "waveformDraw.h"
#include "ofxSoundPlayerObject.h"

namespace Aquamarine
{
    class WidgetElmAudioClip : public WidgetElm
    {

    public:
        WidgetElmAudioClip(string persistentId, string widgetXML, WidgetBase *parent) : WidgetElm(persistentId, widgetXML, parent)
        {
            initWidget();
            loadState(widgetXML);
        }

        virtual bool loadState(string widgetXML) override
        {
            WidgetElm::loadState(widgetXML);
            ofxXmlSettings mWidgetXML = ofxXmlSettings();

            if (!mWidgetXML.loadFromBuffer(widgetXML.c_str()))
            {
                return false;
            }

            mWidgetXML.pushTag("element");

            setClipFile(mWidgetXML.getAttribute("properties", "clip", "/Users/zabba/projects/of_v0.11.0_osx_release/apps/myApps/Cognition/bin/data/New 2.wav"));

            // Get some values here
            mWidgetXML.popTag();

            return true;
        }

        virtual ofxXmlSettings saveState() override
        {
            ofxXmlSettings mWidgetXML = WidgetElm::saveState();

            mWidgetXML.setAttribute("element", "class", WIDGET_ELM_CLASS::AUDIO_CLIP, 0);

            mWidgetXML.pushTag("element");
            // Populate some values here
            mWidgetXML.popTag();

            return mWidgetXML;
        }

        virtual ~WidgetElmAudioClip()
        {
        }

        // virtual void onWidgetResized(int width, int height) {

        // }

        virtual void update(WidgetContext context) override
        {

            getIsRoundedRectangle() ? vizElm_AUDIO_CLIP.setRectRounded(getViz()->scale(getX()), getViz()->scale(getY()), getViz()->scale(getWidth()), getViz()->scale(getHeight()), scale(5), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha())) : vizElm_AUDIO_CLIP.setRectangle(getViz()->scale(getX()), getViz()->scale(getY()), getViz()->scale(getWidth()), getViz()->scale(getHeight()), getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));

            titleFont = getLargeFontSizedForDimensions(getUsableWidth() * 0.95f, getUsableHeight() * 0.95f);

            titleRect = titleFont->rect(getTitle());

            fullFileWaveform.setup(
                getViz()->scale(getX()),
                getViz()->scale(getY()),
                getViz()->scale(getWidth()),
                getViz()->scale(getHeight()));
            wave.setup(
                getViz()->scale(getX()),
                getViz()->scale(getY()),
                getViz()->scale(getWidth()),
                getViz()->scale(getHeight()));
        }

        virtual void draw(WidgetContext context) override
        {

            vizElm_AUDIO_CLIP.setColor(getTheme().ElementBackgroundColor_withAlpha(getCurrentBackgroundAlpha()));
            vizElm_AUDIO_CLIP.draw();

            // if (getIsFocused(true)) {
            //     vizElm_AUDIO_CLIP_FOCUS.setRectangle(scale(getX() + (getWidth() / 2.0f)) - getScaledPadding() - titleRect.getWidth() / 2.0f,
            //                                      scale(getY() + (getHeight() / 2.0f )) - getScaledPadding() * 2,
            //                                      titleRect.width + getScaledPadding()*2,
            //                                      titleRect.height + getScaledPadding()*2.5f,
            //                                      getTheme().ElementForegroundColor_withAlpha(0.1f));

            //     vizElm_AUDIO_CLIP_FOCUS.draw();
            // }

            ofPushStyle();
            ofSetColor(getTheme().ElementForegroundColor_withAlpha(getCurrentForegroundAlpha()));

            if (getClipFile() != "")
            {
            }

            titleFont->draw(getTitle(),
                            scale(getX() + (getWidth() / 2.0f)) - titleRect.getWidth() / 2.0f,
                            scale(getY() + (getHeight() / 2.0f)) + titleRect.getHeight() / 2.0f);

            ofPopStyle();
            ofPushStyle();

            ofSetColor(ofColor::blue);
            fullFileWaveform.draw();

            ofPopStyle();

            // ofSetColor(ofColor::red);
            // float playhead = ofMap(player.getPosition(), 0,1, fullFileWaveform.getMinX(),fullFileWaveform.getMaxX()); //getViz()->scale(
            // ofDrawLine(playhead, 0, playhead, fullFileWaveform.getMaxY());

            // // if(fullFileWaveform.inside(ofGetMouseX(), ofGetMouseY())){
            // //     ofSetColor(ofColor::cyan);
            // //     ofDrawLine(ofGetMouseX(), 0, ofGetMouseX(), fullFileWaveform.getMaxY());
            // // }
            // ofSetColor(ofColor::white);
            // wave.draw();

            // 	player.drawDebug(
            //                 scale(getX()),
            //                 scale(getY()));
        }

        virtual void onWidgetKeyPressed(ofKeyEventArgs &e) override
        {
        }

        void setClipFile(string path)
        {
            mClipFile = path;

            player.load(
                mClipFile,
                // set the following to true if you want to stream the audio data from the disk on demand instead of
                // reading the whole file into memory. Default is false
                false);

            // the fullFileWaveform object will have a static waveform for the whole audio file data. This is only created once as it will read the whole sound file data.
            // For such, we need to get the sound buffer from the sound file in order to get the whole file's data.
            // calling player.getBuffer(), which actually is a function, will return the players current buffer, the one that is being sent to the sound device, so it will not work for what we are trying to achieve.
            // the waveformDraw's makeMeshFromBuffer(ofBuffer&) function will create a waveform from the buffer passed
            fullFileWaveform.makeMeshFromBuffer(player.getSoundFile().getBuffer());

            setNeedsUpdate(true);
            setWidgetNeedsUpdate(true);
        }

        string getClipFile()
        {
            return mClipFile;
        }

    private:
        Elm vizElm_AUDIO_CLIP;
        Elm vizElm_AUDIO_CLIP_FOCUS;
        string mClipFile = "";

        ofRectangle titleRect;
        shared_ptr<ofxSmartFont> titleFont;

        waveformDraw wave, fullFileWaveform;
        ofxSoundPlayerObject player;

        Icon icon;

        void initWidget() override
        {
            setWidth(100);
            setHeight(30);
        }
    };
}