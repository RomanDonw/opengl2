#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "../../external/glm.hpp"

#include <string>

#include "../../Transform.hpp"

enum
{
    NoCulling = 0,
    BackFace = 1,
    FrontFace = 2,
    BothFaces = 3
} typedef FaceCullingType;

struct
{
    Transform transform = Transform();
    bool enableRender = true;
    glm::vec4 color = glm::vec4(1.0f);
    FaceCullingType culling = FaceCullingType::BackFace;

    std::string mesh = "";
    std::string texture = "";
} typedef Surface;

#endif