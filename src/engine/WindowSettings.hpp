#ifndef WINDOWSETTINGS_HPP
#define WINDOWSETTINGS_HPP

#include <string>

struct WindowSettings
{
    std::string title = "SuperEngine";
    unsigned int width = 1280;
    unsigned int height = 720;
    int glMajor = 4;
    int glMinor = 6;
    bool vsync = true;
    bool resizable = true;
    bool decorated = true;
    bool fullscreen = false;
    bool maximizeOnStart = false;
    int msaaSamples = 0;
    int minWidth = 320;
    int minHeight = 240;
    int targetFPS = 100;
};

#endif
