#ifndef POINTLIGHTDATA_HPP
#define POINTLIGHTDATA_HPP

#include "../../external/glm.hpp"

struct
{
    alignas(16) glm::vec3 position;
    float radius;

    alignas(16) glm::vec3 color;
    uint32_t flags; // bit 0 - enabled.
} typedef PointLightData;

#endif