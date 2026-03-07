#ifndef NONRENDERABLE_HPP
#define NONRENDERABLE_HPP

#include "../GameObject/GameObject.hpp"

class Scene;

class NonRenderable : public GameObject
{
    friend class Scene;

    protected:
        NonRenderable(Scene *s, Transform t);
        NonRenderable(Scene *s);

        ~NonRenderable();

        void Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt) override;
};

#endif