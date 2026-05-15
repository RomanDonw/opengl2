#ifndef DIRECTIONALLIGHT_HPP
#define DIRECTIONALLIGHT_HPP

#include "Light.hpp"

class Scene;

class DirectionalLight final : public Light
{
    friend class Scene;

    protected:
        DirectionalLight(Scene *s, Transform t);
        DirectionalLight(Scene *s);

        ~DirectionalLight() override;

    public:
        void CollectLight(LightRenderSettings &settings) override;
};

#endif
