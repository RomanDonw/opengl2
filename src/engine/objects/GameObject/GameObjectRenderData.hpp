#ifndef GAMEOBJECTRENDERDATA_HPP
#define GAMEOBJECTRENDERDATA_HPP

#include "../../external/glm.hpp"

#include "../../FogRenderSettings.hpp"

class Transform;

struct GameObjectRenderData
{
    const glm::mat4 *proj = nullptr;
    const glm::mat4 *view = nullptr;
    const Transform *camt = nullptr;
    const FogRenderSettings *fog = nullptr;

    glm::vec3 cameraPosition = glm::vec3(0.0f);
    glm::vec3 cameraRotation = glm::vec3(0.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
};

#endif
