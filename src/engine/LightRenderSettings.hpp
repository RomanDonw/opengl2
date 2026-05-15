#ifndef LIGHTRENDERSETTINGS_HPP
#define LIGHTRENDERSETTINGS_HPP

#include "external/glm.hpp"

struct PointLightData
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float range = 10.0f;
};

struct DirectionalLightData
{
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
};

struct LightRenderSettings
{
    static constexpr int MAX_POINT_LIGHTS = 8;

    glm::vec3 ambientColor = glm::vec3(0.12f);
    bool hasDirectional = false;
    DirectionalLightData directional{};
    int pointLightCount = 0;
    PointLightData pointLights[MAX_POINT_LIGHTS]{};
};

#endif
