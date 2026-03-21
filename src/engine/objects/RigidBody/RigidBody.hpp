#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include "../../external/physics.hpp"

#include <vector>
#include <concepts>
#include <iterator>
#include <algorithm>
#include <type_traits>

#include "../../Engine.hpp"
#include "../GameObject/GameObject.hpp"
#include "RigidBodyType.hpp"

class Scene;
class Collider;

template <typename T>
concept ColliderConcept = std::derived_from<T, Collider> && !std::is_same_v<T, Collider>;

class RigidBody : virtual public GameObject
{
    friend class Scene;
    friend class Collider;

    private:
        bool lockphystransupdate = false; // this flag need to prevert double updating of physics transform when rigidbody`s transform updates after scene physics update.

        void constructor();

    protected:
        rp3d::RigidBody *rb;
        std::vector<Collider *> colliders;

        RigidBody(Scene *s, Transform t);
        RigidBody(Scene *s);

        ~RigidBody() override;

        void OnGlobalTransformChanged() override;
        void AfterUpdate() override;

    public:
        const GameObjectType type = GameObjectType::RIGIDBODY;

        RigidBodyType GetRigidBodyType() const;
        void SetRigidBodyType(RigidBodyType type);

        bool IsGravityEnabled() const;
        void SetGravityEnabled(bool state);

        glm::vec3 GetLinearVelocity() const;
        void SetLinearVelocity(glm::vec3 v);

        glm::vec3 GetAngularVelocity() const;
        void SetAngularVelocity(glm::vec3 v);

        float GetMass() const;
        void SetMass(float mass);

        glm::vec3 GetLinearLockAxisFactor() const;
        void SetLinearLockAxisFactor(glm::vec3 factor);

        glm::vec3 GetAngularLockAxisFactor() const;
        void SetAngularLockAxisFactor(glm::vec3 factor);

        // applies force in global (world)/local space at body`s center of mass.
        void ApplyLocalForceToCenterOfMass(glm::vec3 force);
        void ApplyGlobalForceToCenterOfMass(glm::vec3 force);

        // applies force in global (world)/local space to point of body in local space.
        void ApplyLocalForceAtLocalPoint(glm::vec3 force, glm::vec3 point);
        void ApplyGlobalForceAtLocalPoint(glm::vec3 force, glm::vec3 point);

        // applies force in global (world)/local space to point of body in global (world) space.
        void ApplyLocalForceAtGlobalPoint(glm::vec3 force, glm::vec3 point);
        void ApplyGlobalForceAtGlobalPoint(glm::vec3 force, glm::vec3 point);

        // applies torque in global (world)/local space.
        void ApplyLocalTorque(glm::vec3 torque);
        void ApplyGlobalTorque(glm::vec3 torque);

        // ============================

        bool HasCollider(Collider *c);

        template<ColliderConcept T, typename... Args>
        T *AddCollider(Transform offset, Args&&... args)
        {
            T *ret = new T(Engine::phys, rb, this, offset, std::forward<Args>(args)...);

            colliders.push_back(ret);

            return ret;
        }

        void RemoveCollider(Collider *c);

        Collider *GetCollider(size_t index); // can return nullptr.
        std::vector<Collider *> GetAllColliders();
};

#endif