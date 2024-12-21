#include "uiVizIcon.h"

namespace Aquamarine
{
    void uiVizIconCache::cleanse()
    {
        for (auto const &icon : mIconCacheMap)
        {
            delete icon.second;
        }
    }

    void uiVizIconCache::scale(float scale)
    {

        int iconSizeFromFile = 48; // The files we are sourcing are sized at 48x48px

        int iconSizeReg = 18;
        int iconSizeMed = 27;
        int iconSizeLrg = 40;

        float iconSizeRegScale = ((float)iconSizeReg / (float)iconSizeFromFile) * (float)scale; // When scale is 1, this is the regular size
        float iconSizeMedScale = (iconSizeMed / (float)iconSizeFromFile) * (float)scale;
        float iconSizeLrgScale = (iconSizeLrg / (float)iconSizeFromFile) * (float)scale;

        int iconSizeRegScaledSize = iconSizeFromFile * (float)iconSizeRegScale;
        int iconSizeMedScaledSize = iconSizeFromFile * (float)iconSizeMedScale;
        int iconSizeLrgScaledSize = iconSizeFromFile * (float)iconSizeLrgScale;

        for (auto const &icon : mIconCacheMap)
        {

            switch (icon.second->getIconSize())
            {
            case uiVizIcon::IconSize::REGULAR:
                icon.second->reload(iconSizeRegScale, uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize));
                break;
            case uiVizIcon::IconSize::MEDIUM:
                icon.second->reload(iconSizeMedScale, uiVizCoord::vizBounds(0, 0, iconSizeMedScaledSize, iconSizeMedScaledSize));
                break;
            case uiVizIcon::IconSize::LARGE:
                icon.second->reload(iconSizeLrgScale, uiVizCoord::vizBounds(0, 0, iconSizeLrgScaledSize, iconSizeLrgScaledSize));
                break;
            }
        }
    }
    float uiVizIconCache::getIconSizeRegScale(float scale)
    {
        int iconSizeFromFile = 48; // The files we are sourcing are sized at 48x48px
        int iconSizeReg = 18;
        float iconSizeRegScale = ((float)iconSizeReg / (float)iconSizeFromFile) * (float)scale; // When scale is 1, this is the regular size
        return iconSizeRegScale;
    }

    float uiVizIconCache::getIconSizeReg(float scale)
    {
        int iconSizeFromFile = 48; // The files we are sourcing are sized at 48x48px
        float iconSizeRegScale = uiVizIconCache::getIconSizeRegScale(scale);
        int iconSizeRegScaledSize = iconSizeFromFile * (float)iconSizeRegScale;
        return iconSizeRegScaledSize;
    }

    void uiVizIconCache::bootstrapIconMapDefault(float scale)
    {
        int iconSizeFromFile = 48; // The files we are sourcing are sized at 48x48px

        int iconSizeReg = 18;
        int iconSizeMed = 27;
        int iconSizeLrg = 40;

        float iconSizeRegScale = ((float)iconSizeReg / (float)iconSizeFromFile) * (float)scale; // When scale is 1, this is the regular size
        float iconSizeMedScale = (iconSizeMed / (float)iconSizeFromFile) * (float)scale;
        float iconSizeLrgScale = (iconSizeLrg / (float)iconSizeFromFile) * (float)scale;

        int iconSizeRegScaledSize = iconSizeFromFile * (float)iconSizeRegScale;
        int iconSizeMedScaledSize = iconSizeFromFile * (float)iconSizeMedScale;
        int iconSizeLrgScaledSize = iconSizeFromFile * (float)iconSizeLrgScale;

        /* Regular */
        mIconCacheMap["NOT_FOUND"] = new uiVizIcon("ui/svg/ic_close_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::NOT_FOUND);
        mIconCacheMap["REG_WIDGET_CLOSE"] = new uiVizIcon("ui/svg/ic_close_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::WIDGET_CLOSE);
        mIconCacheMap["REG_WIDGET_HELP"] = new uiVizIcon("ui/svg/ic_help_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::WIDGET_HELP);
        mIconCacheMap["REG_WIDGET_EXPAND_UP"] = new uiVizIcon("ui/svg/ic_expand_less_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::WIDGET_EXPAND_UP);
        mIconCacheMap["REG_WIDGET_EXPAND_DOWN"] = new uiVizIcon("ui/svg/ic_expand_more_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::WIDGET_EXPAND_DOWN);
        mIconCacheMap["REG_WIDGET_SETTINGS"] = new uiVizIcon("ui/svg/ic_settings_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::WIDGET_SETTINGS);
        mIconCacheMap["REG_REG_RADIO_BUTTON_CHECKED"] = new uiVizIcon("ui/svg/ic_radio_button_checked_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::WIDGET_PIN);
        mIconCacheMap["REG_REG_RADIO_BUTTON_UNCHECKED"] = new uiVizIcon("ui/svg/ic_radio_button_unchecked_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::WIDGET_UNPIN);
        mIconCacheMap["REG_REG_PLAY_BUTTON"] = new uiVizIcon("ui/svg/ic_play_arrow_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::PLAY);
        mIconCacheMap["REG_REG_STOP_BUTTON"] = new uiVizIcon("ui/svg/ic_stop_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::STOP);
        mIconCacheMap["REG_REG_PAUSE_BUTTON"] = new uiVizIcon("ui/svg/ic_pause_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::PAUSE);
        mIconCacheMap["REG_FILE"] = new uiVizIcon("ui/svg/ic_insert_drive_file_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_FILE);
        mIconCacheMap["REG_FOLDER"] = new uiVizIcon("ui/svg/ic_folder_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_FOLDER);
        mIconCacheMap["REG_COMPUTER"] = new uiVizIcon("ui/svg/ic_desktop_mac_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_COMPUTER);
        mIconCacheMap["REG_HOME"] = new uiVizIcon("ui/svg/ic_home_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_HOME);
        mIconCacheMap["REG_DESKTOP"] = new uiVizIcon("ui/svg/ic_web_asset_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_DESKTOP);
        mIconCacheMap["REG_DOCUMENTS"] = new uiVizIcon("ui/svg/ic_insert_drive_file_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_DOCUMENTS);
        mIconCacheMap["REG_DOWNLOADS"] = new uiVizIcon("ui/svg/ic_file_download_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_DOWNLOADS);
        mIconCacheMap["REG_CLOUD"] = new uiVizIcon("ui/svg/ic_cloud_queue_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_DOWNLOADS);
        mIconCacheMap["REG_DRIVE"] = new uiVizIcon("ui/svg/ic_sd_storage_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_DOWNLOADS);
        mIconCacheMap["REG_USER"] = new uiVizIcon("ui/svg/ic_account_circle_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_USER);
        mIconCacheMap["REG_BUG"] = new uiVizIcon("ui/svg/ic_bug_report_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_BUG);
        mIconCacheMap["REG_DOT"] = new uiVizIcon("ui/svg/ic_lens_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_DOT);
        mIconCacheMap["REG_DOWNLOAD"] = new uiVizIcon("ui/svg/ic_file_download_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_DOWNLOAD);
        mIconCacheMap["REG_INFO"] = new uiVizIcon("ui/svg/ic_info_outline_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_INFO);
        mIconCacheMap["REG_NEW_RELEASE"] = new uiVizIcon("ui/svg/ic_new_releases_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_NEW_RELEASE);
        mIconCacheMap["REG_REFRESH"] = new uiVizIcon("ui/svg/ic_new_releases_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_REFRESH);
        mIconCacheMap["REG_QUEUE_MUSIC"] = new uiVizIcon("ui/svg/ic_queue_music_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeRegScaledSize, iconSizeRegScaledSize), (float)iconSizeRegScale, uiVizIcon::IconSize::REGULAR, ofColor::white, IconTag::ICON_QUEUE_MUSIC);

        mIconCacheMap["MED_CONTENT_ARROW_UP"] = new uiVizIcon("ui/svg/ic_arrow_upward_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeMedScaledSize, iconSizeMedScaledSize), (float)iconSizeMedScale, uiVizIcon::IconSize::MEDIUM, ofColor::white, IconTag::ICON_UP);
        mIconCacheMap["MED_CONTENT_ARROW_DOWN"] = new uiVizIcon("ui/svg/ic_arrow_downward_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeMedScaledSize, iconSizeMedScaledSize), (float)iconSizeMedScale, uiVizIcon::IconSize::MEDIUM, ofColor::white, IconTag::ICON_DOWN);
        mIconCacheMap["MED_CONTENT_ARROW_LEFT"] = new uiVizIcon("ui/svg/ic_arrow_back_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeMedScaledSize, iconSizeMedScaledSize), (float)iconSizeMedScale, uiVizIcon::IconSize::MEDIUM, ofColor::white, IconTag::ICON_LEFT);
        mIconCacheMap["MED_CONTENT_ARROW_RIGHT"] = new uiVizIcon("ui/svg/ic_arrow_forward_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeMedScaledSize, iconSizeMedScaledSize), (float)iconSizeMedScale, uiVizIcon::IconSize::MEDIUM, ofColor::white, IconTag::ICON_RIGHT);
        mIconCacheMap["MED_PLAY_BUTTON"] = new uiVizIcon("ui/svg/ic_play_arrow_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeMedScaledSize, iconSizeMedScaledSize), (float)iconSizeMedScale, uiVizIcon::IconSize::MEDIUM, ofColor::white, IconTag::PLAY);

        /* Medium */
        mIconCacheMap["MED_WIDGET_CLIPBOARD"] = new uiVizIcon("ui/svg/ic_content_paste_" + ofToString(iconSizeFromFile) + "px.svg", uiVizCoord::vizBounds(0, 0, iconSizeMedScaledSize, iconSizeMedScaledSize), (float)iconSizeMedScale, uiVizIcon::IconSize::MEDIUM, ofColor::white, uiVizIconCache::IconTag::WIDGET_CLIPBOARD);
    }

    void uiVizIconCache::bootstrapIconMap(float scale, std::map<std::string, uiVizIcon *> map)
    {
        mIconCacheMap = map;
        uiVizIconCache::bootstrapIconMapDefault(scale);
    }

    uiVizIcon uiVizIconCache::getIcon(string iconKey)
    {

        for (auto const &icon : mIconCacheMap)
        {
            if (icon.first == iconKey)
            {
                return *icon.second;
            }
        }

        // default to 'x'
        return *mIconCacheMap["NOT_FOUND"];
    }

    uiVizIcon &uiVizIconCache::getIconRef(string iconKey)
    {

        for (auto const &icon : mIconCacheMap)
        {
            if (icon.first == iconKey)
            {
                return *icon.second;
            }
        }

        // default to 'x'
        return *mIconCacheMap["NOT_FOUND"];
    }

    std::map<std::string, uiVizIcon *> uiVizIconCache::mIconCacheMap;
}