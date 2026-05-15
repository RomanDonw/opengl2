#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "../GameObject/GameObject.hpp"
#include "../../LightRenderSettings.hpp"

class Scene;

class Light : public GameObject
{
    friend class Scene;

    protected:
        Light(Scene *s, Transform t);
        Light(Scene *s);

        ~Light() override;

    public:
        bool enabled = true;
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;

        virtual void CollectLight(LightRenderSettings &settings) = 0;
};

#endif
