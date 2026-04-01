#include "PointLight.hpp"

#include "../../Scene.hpp"

// === PRIVATE ===

void PointLight::constructor() { GetScene()->pointlights.insert(this); }

PointLight::PointLight(Scene *s, Transform t) : GameObject(s, t) { constructor(); }
PointLight::PointLight(Scene *s) : GameObject(s) { constructor(); }

PointLight::~PointLight() { GetScene()->pointlights.erase(this); };

// === PUBLIC ===

PointLightData PointLight::GetLightData() const
{
    PointLightData ret;
    ret.position = transform.GetPosition();
    ret.radius = radius;
    ret.color = color;
    ret.flags = (enabled ? 0b1 : 0b0);
    return ret;
}