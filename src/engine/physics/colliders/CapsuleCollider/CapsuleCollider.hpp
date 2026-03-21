#ifndef CAPSULECOLLIDER_HPP
#define CAPSULECOLLIDER_HPP

#include "../../../external/glm.hpp"

#include "../Collider/Collider.hpp"

class RigidBody;


// this collider type doesn't support scale from transform property.
class CapsuleCollider final : public Collider
{
    friend class RigidBody;

    private:
        rp3d::CapsuleShape *shape;

        CapsuleCollider(rp3d::PhysicsCommon *phys, rp3d::RigidBody *physrb, RigidBody *wraprb, Transform t, float radius, float height);
        ~CapsuleCollider() override;

    public:
        const ColliderType type = ColliderType::CAPSULE;

        float GetRadius() const;
        void SetRadius(float radius);

        float GetHeight() const;
        void SetHeight(float height);
};

#endif