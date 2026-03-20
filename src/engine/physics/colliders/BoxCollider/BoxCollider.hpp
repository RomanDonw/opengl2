#ifndef BOXCOLLIDER_HPP
#define BOXCOLLIDER_HPP

#include "../../../external/glm.hpp"

#include "../Collider/Collider.hpp"

class RigidBody;

class BoxCollider final : public Collider
{
    friend class RigidBody;

    private:
        rp3d::BoxShape *shape;

        BoxCollider(rp3d::PhysicsCommon *phys, rp3d::RigidBody *physrb, RigidBody *wraprb, Transform t, glm::vec3 halfExtents);
        ~BoxCollider() override;

    public:
        const ColliderType type = ColliderType::BOX;

        glm::vec3 GetHalfExtents() const;
        void SetHalfExtents(glm::vec3 halfExtents);
};

#endif