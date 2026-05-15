#include "LightingUniforms.hpp"

#include "../external/opengl.hpp"

#include <string>

void LightingUniforms::Apply(ShaderProgram *shader, const LightRenderSettings *lights)
{
    if (!shader || !lights) return;

    shader->SetUniformVector3("ambientColor", lights->ambientColor);
    shader->SetUniformInteger("hasDirectionalLight", lights->hasDirectional ? GL_TRUE : GL_FALSE);
    shader->SetUniformVector3("directionalLightDir", lights->directional.direction);
    shader->SetUniformVector3("directionalLightColor", lights->directional.color);
    shader->SetUniformFloat("directionalLightIntensity", lights->directional.intensity);
    shader->SetUniformInteger("pointLightCount", lights->pointLightCount);

    for (int i = 0; i < lights->pointLightCount; ++i)
    {
        const std::string idx = std::to_string(i);
        shader->SetUniformVector3("pointLightPos[" + idx + "]", lights->pointLights[i].position);
        shader->SetUniformVector3("pointLightColor[" + idx + "]", lights->pointLights[i].color);
        shader->SetUniformFloat("pointLightIntensity[" + idx + "]", lights->pointLights[i].intensity);
        shader->SetUniformFloat("pointLightRange[" + idx + "]", lights->pointLights[i].range);
    }
}
