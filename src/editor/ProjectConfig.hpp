#ifndef PROJECTCONFIG_HPP
#define PROJECTCONFIG_HPP

#include <string>

struct ProjectConfig
{
    std::string name = "default";
    std::string entryScene = "scenes/main.scene";
    std::string scriptsDir = "scripts";
    std::string scenesDir = "scenes";
    std::string docsDir = "docs";
    bool preferFreeplayCamera = true;
    float defaultMoveSpeed = 10.0f;
    float defaultLookSpeed = 1.0f;
};

#endif
