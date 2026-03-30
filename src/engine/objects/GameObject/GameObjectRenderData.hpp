#ifndef GAMEOBJECTRENDERDATA_HPP
#define GAMEOBJECTRENDERDATA_HPP

#include "../../external/glm.hpp"

#include "../../FogRenderSettings.hpp"

class Transform;

struct
{
    const glm::mat4 *proj;
    const glm::mat4 *view;
    const Transform *camt;
    const FogRenderSettings *fog;
} typedef GameObjectRenderData;

#endif