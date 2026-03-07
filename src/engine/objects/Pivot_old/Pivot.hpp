#ifndef PIVOT_HPP
#define PIVOT_HPP

#include "../GameObject/GameObject.hpp"

class Scene;

class Pivot : public GameObject
{
    friend class Scene;

    protected:
        Pivot(Scene *s, Transform t);
        Pivot(Scene *s);

        ~Pivot();

        void Update(double delta) override;
        void Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt) override;

    public:
        const GameObjectType type = PIVOT;
};

#endif