#ifndef COLLIDERTRANSFORM_HPP
#define COLLIDERTRANSFORM_HPP

#include "../../../Transform.hpp"

class Collider;

class ColliderTransform : public Transform
{
    friend class Collider;

    private:
        Collider *collider;

        void OnTransformChanged() override;

        ColliderTransform(Collider *c);

    public:
        ColliderTransform *operator=(Transform other);
};

#endif