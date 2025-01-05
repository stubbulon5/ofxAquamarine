
#include "Shared.h"
#include "core/machineid/machineid.h"
#include "Poco/Base64Encoder.h"
#include "uriparser/Uri.h"


namespace Aquamarine
{
    string Shared::APP_SETTINGS_PATH;
    string Shared::APP_SETTINGS_FILE_NAME = "settings.json";
    string Shared::APP_SETTINGS_FILE_FULL_PATH;
    std::shared_ptr<Viz> Shared::viz;
    int Shared::mVizDebug = -1;

    /*
     static methods
     */
    json Shared::initSettingsFile(string settingsPath, string settingsFileName)
    {
        APP_SETTINGS_PATH = settingsPath;
        APP_SETTINGS_FILE_NAME = settingsFileName;

        ofDirectory::createDirectory(APP_SETTINGS_PATH, true, false);
        APP_SETTINGS_FILE_FULL_PATH = ofFilePath::join(APP_SETTINGS_PATH, APP_SETTINGS_FILE_NAME);

        json doc;
        try
        {
            std::ifstream ifs(APP_SETTINGS_FILE_FULL_PATH);
            std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            doc = json::parse(data);
            return doc;

        }
        catch (exception &error)
        {
            std::cerr << "\nWriting new settings file... : `" << APP_SETTINGS_FILE_FULL_PATH << std::endl;
            doc["settings"]["userExperience"] = 60;
            doc["settings"]["useFbo"] = true;
            doc["settings"]["showFps"] = false;
            doc["settings"]["language"] = "english";
            doc["settings"]["fontPath"] = "fonts/Verdana.ttf";
            doc["settings"]["userInterfaceScaling"] = 1.0;
            doc["settings"]["themeName"] = "System";
            doc["settings"]["recentFiles"] = {};
            doc["settings"]["recentDirectories"] = {};
            doc["settings"]["recentProjects"] = {};
            doc["settings"]["autoLoadMostRecentProject"] = true;
            doc["settings"]["favouriteLocations"] = {};

            // Save the file
            std::ofstream ofs(APP_SETTINGS_FILE_FULL_PATH);
            ofs << doc;            
        }
        return doc;
    }

    string Shared::getSettingsPath()
    {
        return APP_SETTINGS_PATH;
    }

    string Shared::getSettingsFileName()
    {
        return APP_SETTINGS_FILE_NAME;
    }

    string Shared::getSettingsFileFullPath()
    {
        return APP_SETTINGS_FILE_FULL_PATH;
    }

    int Shared::getDefaultFPS()
    {
#ifdef TARGET_OF_IOS
        return 30;
#else
        return 60;
#endif
    }

    float Shared::getDefaultScaling()
    {
#ifdef TARGET_OF_IOS
        return 1.5f;
#else
        return 1.0f;
#endif
    }

    int Shared::getVizDebug()
    {
        return mVizDebug;
    }

    void Shared::setVizDebug(int val)
    {
        mVizDebug = val;
    }

    void Shared::configure(
        float SETTING_User_Scale,
        string SETTING_language,
        int SETTING_userExperience,
        bool SETTING_useFbo,
        bool SETTING_showFps,
        string SETTING_absoluteFontPath,
        bool SETTING_autoLoadMostRecentProject,
        string SETTING_themeName)
    {

        cout << "Configuring Shared: " << SETTING_language << ", " << SETTING_absoluteFontPath << endl;

        viz = make_shared<Viz>(
            SETTING_User_Scale,
            SETTING_language,
            SETTING_userExperience,
            SETTING_useFbo,
            SETTING_showFps,
            SETTING_absoluteFontPath,
            SETTING_autoLoadMostRecentProject,
            SETTING_themeName);
    }

    /*
    methods that should only be called after Shared::configure is called
     */

    std::shared_ptr<Viz> Shared::getViz()
    {
        if (viz == nullptr)
        {
            cout << "***************************** ERROR ***************************** " << endl;
            cout << "Shared::configure was not called, MUST call it before using Shared::getViz()" << endl;
            cout << "***************************************************************** " << endl;
            viz = make_shared<Viz>(); // Use the defaults!!
            return nullptr;
        }
        return viz;
    }

    void Shared::setUserScale(float val)
    {
        getViz()->setUserScale(val);
    }

    void Shared::setUserExperience(float val)
    {
        getViz()->setUserExperience(val, false);
    }

    void Shared::setUserExperience(float val, bool isInitializing)
    {
        getViz()->setUserExperience(val, isInitializing);
    }

    void Shared::setUseFbo(bool useFbo)
    {
        getViz()->setUseFbo(useFbo);
    }

    void Shared::setShowFps(bool showFps)
    {
        getViz()->setShowFps(showFps);
    }

    void Shared::throttleUserExperience()
    {
        getViz()->throttleUserExperience(false);
    }

    void Shared::revertThrottleUserExperience()
    {
        getViz()->revertThrottleUserExperience();
    }

    void Shared::setResolutionMultiplier()
    {
        getViz()->setResolutionMultiplier();
    }

    void Shared::setUserFont(string absolutePath)
    {
        getViz()->setUserFont(absolutePath);
    }

    string Shared::getUserSpecifiedFont()
    {
        return getViz()->getUserSpecifiedFont();
    }

    string Shared::lang(string tag)
    {
        return getViz()->lang(tag);
    }

    void Shared::langLoadXML(string xml)
    {
        getViz()->langLoadXML(xml);
    }

    void Shared::langLoadFile(string filename)
    {
        getViz()->lang(filename);
    }

    string Shared::encodeForXML(string val)
    {
        ofStringReplace(val, "&", "&amp;");
        ofStringReplace(val, "<", "&lt;");
        ofStringReplace(val, ">", "&gt;");
        ofStringReplace(val, "\"", "&quot;");
        ofStringReplace(val, "\'", "&apos;");
        return val;
    }

    string Shared::encodeForJSON(string val)
    {
        ofStringReplace(val, "\"", "\""
                                   "");
        return val;
    }

    string Shared::getMachineId()
    {
        // Determined by hashing : << getMachineName() << getCpuHash() << getVolumeHash()
        return machineid::machineHash();
    }

    /***********************************************************
     XOR Process
     for both encryption/decryption processing
     Picked up from :
     https://www.daniweb.com/programming/software-development/threads/339958/public-key-encryption-private-key-decryption
     ***********************************************************/
    std::string Shared::XOR_Encryption(string toBeEncrypted, string sKey, bool interleaveMachineId)
    {
        string tmpKey = "";
        if (interleaveMachineId)
        {
            string machineId = Shared::getMachineId();
            for (int i = 0; i < machineId.size(); i++)
            {
                tmpKey += machineId[i];
                if (sKey.size() > i)
                    tmpKey += sKey[i];
            }

            if (sKey.size() > machineId.size())
            {
                string additional = sKey.substr(machineId.size(), sKey.size() - machineId.size());
                tmpKey += additional;
            }

            sKey = tmpKey;
        }

        string sEncrypted(toBeEncrypted);
        unsigned int iKey(sKey.length()), iIn(toBeEncrypted.length()), x(0);
        for (unsigned int i = 0; i < iIn; i++)
        {
            sEncrypted[i] = toBeEncrypted[i] ^ sKey[x];
            if (++x == iKey)
            {
                x = 0;
            }
        }
        return sEncrypted;
    }

    string Shared::Pretty_Bytes(uint64_t bytes)
    {
        const char *suffixes[7];
        suffixes[0] = "B";
        suffixes[1] = "KB";
        suffixes[2] = "MB";
        suffixes[3] = "GB";
        suffixes[4] = "TB";
        suffixes[5] = "PB";
        suffixes[6] = "EB";
        unsigned int s = 0; // which suffix to use
        double count = bytes;
        while (count >= 1024 && s < 7)
        {
            s++;
            count /= 1024;
        }

        char buf[100];
        if (count - floor(count) == 0.0)
            sprintf(buf, "%d %s", (int)count, suffixes[s]);
        else
            sprintf(buf, "%.1f %s", count, suffixes[s]);

        std::string buffAsStdStr = buf;
        return buffAsStdStr;
    }

    string Shared::removeFileExtension(const std::string &filename)
    {
        size_t lastdot = filename.find_last_of(".");
        if (lastdot == std::string::npos)
            return filename;
        return filename.substr(0, lastdot);
    }

    string Shared::safeFileName(std::string &filename)
    {
        string illegalChars = "\\/:?\"<>|[]";
        for (auto it = filename.begin() ; it < filename.end() ; ++it){
            bool found = illegalChars.find(*it) != string::npos;
            if(found){
                *it = ' ';
            }
        }
        return filename;
    }

    vector<string> Shared::tokenize(const string &str, const string &delim)
    {
        vector<string> tokens;

        size_t p0 = 0, p1 = string::npos;
        while (p0 != string::npos)
        {
            p1 = str.find_first_of(delim, p0);
            if (p1 != p0)
            {
                string token = str.substr(p0, p1 - p0);
                tokens.push_back(token);
            }
            p0 = str.find_first_not_of(delim, p1);
        }
        return tokens;
    }

    void Shared::appendXMLToTarget(TiXmlElement *source, ofxXmlSettings *target, bool recursive)
    {
        if (source == NULL)
            return;

        TiXmlElement *elm = source->ToElement();
        string tagName = elm->Value();

        int tagNum = target->addTag(tagName);

        // Value
        if (elm->GetText() != NULL)
            target->setValue(tagName, elm->GetText());

        // Attributes
        TiXmlAttribute *first = elm->FirstAttribute();
        if (first)
        {
            for (TiXmlAttribute *curr = first; curr != NULL; curr = curr->Next())
            {

                int ival = 0;
                double dval = 0.0;
                string attributeName = curr->Name();

                if (curr->QueryIntValue(&ival) == TIXML_SUCCESS)
                {
                    target->setAttribute(tagName, attributeName, ival, tagNum);
                }
                else if (curr->QueryDoubleValue(&dval) == TIXML_SUCCESS)
                {
                    target->setAttribute(tagName, attributeName, dval, tagNum);
                }
                else
                {
                    target->setAttribute(tagName, attributeName, curr->ValueStr(), tagNum);
                }
            }
        }

        target->pushTag(tagName);

        if (recursive)
        {
            for (TiXmlNode *child = source->FirstChild(); child; child = child->NextSibling())
            {
                TiXmlElement *childElm = child->ToElement();
                if (childElm != NULL)
                {
                    Shared::appendXMLToTarget(childElm, target, recursive);
                }
            }
        }

        target->popTag();
    }

    string Shared::getXMLAtTag(ofxXmlSettings *xml, string tag, int which)
    {
        if (!xml)
            return "";

        vector<string> tokens = tokenize(tag, ":");

        TiXmlHandle tagHandle = TiXmlHandle(&xml->doc);
        for (int x = 0; x < (int)tokens.size(); x++)
        {
            if (x < tokens.size() - 1)
                tagHandle = tagHandle.FirstChildElement(tokens.at(x));
            else
                tagHandle = tagHandle.ChildElement(tokens.at(x), which);
        }

        // once we've walked, let's get that value...
        TiXmlHandle valHandle = tagHandle.Child(0);

        string xmlOut = "";
        TiXmlPrinter printer;
        if (valHandle.ToNode())
        {
            valHandle.ToNode()->Accept(&printer);
            xmlOut = printer.CStr();
        }
        return xmlOut;
    }

    string &Shared::implode(const std::vector<std::string> &elems, char delim, std::string &s)
    {
        for (std::vector<std::string>::const_iterator ii = elems.begin(); ii != elems.end(); ++ii)
        {
            s += (*ii);
            if (ii + 1 != elems.end())
            {
                s += delim;
            }
        }

        return s;
    }

    string Shared::implode(const std::vector<std::string> &elems, char delim)
    {
        std::string s;
        return Shared::implode(elems, delim, s);
    }

    string Shared::base64EncodeString(ofBuffer tmpBuff)
    {
        stringstream ss;
        ss.str("");
        Poco::Base64Encoder encoder(ss);
        encoder << tmpBuff.getText();
        encoder.close();
        return ss.str();
    }

    void Shared::launchBrowser(const string uriStr)
    {
#ifdef TARGET_WIN32
        ShellExecuteA(nullptr, "open", uriStr.c_str(),
                      nullptr, nullptr, SW_SHOWNORMAL);
#endif

#ifdef TARGET_OSX
        // could also do with LSOpenCFURLRef
        string commandStr = "open \"" + uriStr + "\"";
        int ret = system(commandStr.c_str());
        if (ret != 0)
        {
            ofLogError("ofUtils") << "ofLaunchBrowser(): couldn't open browser, commandStr \"" << commandStr << "\"";
        }
#endif

#ifdef TARGET_LINUX
        string commandStr = "xdg-open \"" + uriStr + "\"";
        int ret = system(commandStr.c_str());
        if (ret != 0)
        {
            ofLogError("ofUtils") << "ofLaunchBrowser(): couldn't open browser, commandStr \"" << commandStr << "\"";
        }
#endif

#ifdef TARGET_OF_IOS
        ofxiOSLaunchBrowser(uriStr);
#endif

#ifdef TARGET_ANDROID
        ofxAndroidLaunchBrowser(uriStr);
#endif

#ifdef TARGET_EMSCRIPTEN
        ofLogError("ofUtils") << "ofLaunchBrowser() not implementeed in emscripten";
#endif
    }

}