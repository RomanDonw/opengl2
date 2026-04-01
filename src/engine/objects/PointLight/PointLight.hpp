#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include "../../external/glm.hpp"

#include "../GameObject/GameObject.hpp"
#include "PointLightData.hpp"

class Scene;

class PointLight final : public GameObject
{
    friend class Scene;

    private:
        void constructor();

        PointLight(Scene *s, Transform t);
        PointLight(Scene *s);

        ~PointLight() override;

    public:
        bool enabled = true;
        glm::vec3 color = glm::vec3(0.0f);
        float radius;

        PointLightData GetLightData() const;
};

#endif