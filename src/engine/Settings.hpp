#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "WindowSettings.hpp"

#include <string>

class Settings final
{
    private:
        Settings() = delete;

        static inline std::string path = "settings.cfg";
        static inline WindowSettings window;

    public:
        static void SetPath(const std::string &filePath);
        static const std::string &GetPath();

        static WindowSettings &GetWindow();
        static const WindowSettings &GetWindowConst();

        static bool Load();
        static bool Save();
};

#endif
