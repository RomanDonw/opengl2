#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "engine/objects/Entity/Entity.hpp"

#include <functional>

class Scene;

class Button final : public Entity
{
    friend class Scene;

    private:
        bool enabled;

        void constructor();

        Button(Scene *s, Transform t);
        Button(Scene *s);

        ~Button() override;

    public:
        std::string model;
        std::string textureon;
        std::string textureoff;
        std::string togglesfx;

        std::function<void (Button *, bool)> toggleCallback;

        bool GetButtonState() const;
        void SetButtonState(bool state, bool playsfx = true);
        void ToggleButtonState(bool playsfx = true);
};

#endif