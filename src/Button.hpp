#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "engine/external/glm.hpp"
#include "engine/objects/Entity/Entity.hpp"

#include <functional>

class Scene;
class PointLight;

class Button final : public Entity
{
    friend class Scene;

    private:
        bool enabled;

        PointLight *light;

        void constructor();

        Button(Scene *s, Transform t);
        Button(Scene *s);

        ~Button() override;

    public:
        std::string model;
        std::string textureon;
        std::string textureoff;
        std::string togglesfx;

        bool lightstateon = false;
        glm::vec3 lightcoloron = glm::vec3(0);
        bool lightstateoff = false;
        glm::vec3 lightcoloroff = glm::vec3(0);

        std::function<void (Button *, bool)> toggleCallback;

        bool GetButtonState() const;
        void SetButtonState(bool state, bool playsfx = true);
        void ToggleButtonState(bool playsfx = true);
};

#endif