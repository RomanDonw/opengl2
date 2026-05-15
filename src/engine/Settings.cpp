#include "Settings.hpp"

#include "Utils.hpp"

#include <cctype>
#include <fstream>
#include <sstream>

void Settings::SetPath(const std::string &filePath) { path = filePath; }

const std::string &Settings::GetPath() { return path; }

WindowSettings &Settings::GetWindow() { return window; }

const WindowSettings &Settings::GetWindowConst() { return window; }

static std::string trim(const std::string &s)
{
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;

    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;

    return s.substr(start, end - start);
}

static bool parseBool(const std::string &value, bool &out)
{
    if (value == "1" || value == "true" || value == "TRUE" || value == "yes" || value == "on")
    {
        out = true;
        return true;
    }

    if (value == "0" || value == "false" || value == "FALSE" || value == "no" || value == "off")
    {
        out = false;
        return true;
    }

    return false;
}

bool Settings::Load()
{
    std::string text;
    if (!Utils::ReadTextFile(path, &text)) return false;

    std::istringstream stream(text);
    std::string line;

    while (std::getline(stream, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        const size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        const std::string key = trim(line.substr(0, eq));
        const std::string value = trim(line.substr(eq + 1));

        if (key == "title") window.title = value;
        else if (key == "width") window.width = static_cast<unsigned int>(std::stoul(value));
        else if (key == "height") window.height = static_cast<unsigned int>(std::stoul(value));
        else if (key == "gl_major") window.glMajor = std::stoi(value);
        else if (key == "gl_minor") window.glMinor = std::stoi(value);
        else if (key == "vsync") parseBool(value, window.vsync);
        else if (key == "resizable") parseBool(value, window.resizable);
        else if (key == "decorated") parseBool(value, window.decorated);
        else if (key == "fullscreen") parseBool(value, window.fullscreen);
        else if (key == "maximize") parseBool(value, window.maximizeOnStart);
        else if (key == "msaa") window.msaaSamples = std::stoi(value);
        else if (key == "min_width") window.minWidth = std::stoi(value);
        else if (key == "min_height") window.minHeight = std::stoi(value);
        else if (key == "target_fps") window.targetFPS = std::stoi(value);
    }

    return true;
}

bool Settings::Save()
{
    std::ostringstream oss;
    oss << "title=" << window.title << "\n";
    oss << "width=" << window.width << "\n";
    oss << "height=" << window.height << "\n";
    oss << "gl_major=" << window.glMajor << "\n";
    oss << "gl_minor=" << window.glMinor << "\n";
    oss << "vsync=" << (window.vsync ? 1 : 0) << "\n";
    oss << "resizable=" << (window.resizable ? 1 : 0) << "\n";
    oss << "decorated=" << (window.decorated ? 1 : 0) << "\n";
    oss << "fullscreen=" << (window.fullscreen ? 1 : 0) << "\n";
    oss << "maximize=" << (window.maximizeOnStart ? 1 : 0) << "\n";
    oss << "msaa=" << window.msaaSamples << "\n";
    oss << "min_width=" << window.minWidth << "\n";
    oss << "min_height=" << window.minHeight << "\n";
    oss << "target_fps=" << window.targetFPS << "\n";

    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()) return false;

    file << oss.str();
    return file.good();
}
