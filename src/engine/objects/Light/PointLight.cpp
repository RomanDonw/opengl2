#include "PointLight.hpp"

PointLight::PointLight(Scene *s, Transform t) : Light(s, t) {}

PointLight::PointLight(Scene *s) : Light(s) {}

PointLight::~PointLight() {}

void PointLight::CollectLight(LightRenderSettings &settings)
{
    if (!enabled || settings.pointLightCount >= LightRenderSettings::MAX_POINT_LIGHTS) return;

    PointLightData &pl = settings.pointLights[settings.pointLightCount++];
    pl.position = GetGlobalTransform().GetPosition();
    pl.color = color;
    pl.intensity = intensity;
    pl.range = range > 0 ? range : 0.001f;
}
