#ifndef LIGHTINGUNIFORMS_HPP
#define LIGHTINGUNIFORMS_HPP

#include "../LightRenderSettings.hpp"
#include "../resources/ShaderProgram.hpp"

namespace LightingUniforms
{
    void Apply(ShaderProgram *shader, const LightRenderSettings *lights);
}

#endif
