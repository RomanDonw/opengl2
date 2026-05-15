#include "DirectionalLight.hpp"

DirectionalLight::DirectionalLight(Scene *s, Transform t) : Light(s, t) {}

DirectionalLight::DirectionalLight(Scene *s) : Light(s) {}

DirectionalLight::~DirectionalLight() {}

void DirectionalLight::CollectLight(LightRenderSettings &settings)
{
    if (!enabled) return;

    settings.hasDirectional = true;
    settings.directional.direction = glm::normalize(GetGlobalTransform().GetFront());
    settings.directional.color = color;
    settings.directional.intensity = intensity;
}
