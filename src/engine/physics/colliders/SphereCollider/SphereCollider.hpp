#ifndef SPHERECOLLIDER_HPP
#define SPHERECOLLIDER_HPP

#include "../../../external/glm.hpp"

#include "../Collider/Collider.hpp"

class RigidBody;

// this collider type doesn't support scaling from transform property.
class SphereCollider final : public Collider
{
    friend class RigidBody;

    private:
        rp3d::SphereShape *shape;

        SphereCollider(rp3d::PhysicsCommon *phys, rp3d::RigidBody *physrb, RigidBody *wraprb, Transform t, float radius);
        ~SphereCollider() override;

    public:
        float GetRadius() const;
        void SetRadius(float radius);
};

#endif