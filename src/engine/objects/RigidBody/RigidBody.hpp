#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include "../../external/physics.hpp"

#include "../GameObject/GameObject.hpp"
#include "RigidBodyType.hpp"

class Scene;

class RigidBody : virtual public GameObject
{
    friend class Scene;

    private:
        bool lockphystransupdate = false; // this flag need to prevert double updating of physics transform when rigidbody`s transform updates after scene physics update.

        void constructor();

    protected:
        rp3d::RigidBody *rb;

        RigidBody(Scene *s, Transform t);
        RigidBody(Scene *s);

        ~RigidBody() override;

        void OnGlobalTransformChanged() override;
        void AfterUpdate() override;

    public:
        const GameObjectType type = GameObjectType::RIGIDBODY;

        size_t SetParent(GameObject *new_parent, bool save_global_pos = true) override;

        RigidBodyType GetRigidBodyType();
        void SetRigidBodyType(RigidBodyType type);

        bool IsGravityEnabled() const;
        void SetEnabledGravity(bool state);

        glm::vec3 GetLinearVelocity() const;
        void SetLinearVelocity(glm::vec3 v);
};

#endif