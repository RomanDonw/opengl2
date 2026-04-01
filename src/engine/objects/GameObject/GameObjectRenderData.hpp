#ifndef GAMEOBJECTRENDERDATA_HPP
#define GAMEOBJECTRENDERDATA_HPP

#include "../../external/glm.hpp"

#include "../../FogRenderSettings.hpp"

class SSBO;
class Transform;

struct
{
    const glm::mat4 *proj;
    const glm::mat4 *view;

    const Transform *camt;
    const FogRenderSettings *fog;
    const glm::vec3 *ambientlight;

    const SSBO *pointlightsssbo;
    uint32_t pointlightscount;
} typedef GameObjectRenderData;

#endif