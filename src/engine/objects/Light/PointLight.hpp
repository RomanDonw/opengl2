#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include "Light.hpp"

class Scene;

class PointLight final : public Light
{
    friend class Scene;

    protected:
        PointLight(Scene *s, Transform t);
        PointLight(Scene *s);

        ~PointLight() override;

    public:
        float range = 12.0f;

        void CollectLight(LightRenderSettings &settings) override;
};

#endif
