#ifndef ENITTY_HPP
#define ENTITY_HPP

#include "../Model/Model.hpp"
#include "../../external/physics.hpp"

class Scene;

enum
{
    STATIC = 0,
    DYNAMIC = 1
} typedef EntityRigidBodyType;

class Entity : public Model
{
    friend class Scene;

    private:
        bool lockphystransupdate = false; // this flag need to prevert double updating of physics transform when entity`s transform updates after scene physics update.

        void constructor();

    protected:
        rp3d::RigidBody *rb;

        Entity(Scene *s, Transform t);
        Entity(Scene *s);

        ~Entity() override;

        void OnGlobalTransformChanged() override;
        void AfterUpdate() override;

    public:
        const GameObjectType type = GameObjectType::ENTITY;

        size_t SetParent(GameObject *new_parent, bool save_global_pos = true) override;

        EntityRigidBodyType GetRigidBodyType();
        void SetRigidBodyType(EntityRigidBodyType type);

        // doesn't work with static (default) rigidbody type.

        bool IsGravityEnabled() const;
        void SetEnabledGravity(bool state);

        glm::vec3 GetLinearVelocity() const;
        void SetLinearVelocity(glm::vec3 v);
};

#endif