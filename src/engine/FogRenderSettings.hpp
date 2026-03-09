#ifndef FOGRENDERSETTINGS_HPP
#define FOGRENDERSETTINGS_HPP

#include "external/glm.hpp"

struct
{
    bool enabled = false;
    float startDistance = 0;
    float endDistance = 0;
    glm::vec3 color = glm::vec3(0.0f);
} typedef FogRenderSettings;

#endif