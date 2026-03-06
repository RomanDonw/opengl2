#ifndef NONRENDERABLE_HPP
#define NONRENDERABLE_HPP

#include "../GameObject/GameObject.hpp"

class NonRenderable : public GameObject
{
    protected:
        NonRenderable(Scene *s, Transform t);
        NonRenderable(Scene *s);

        ~NonRenderable();

        void Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt) override;
};

#endif