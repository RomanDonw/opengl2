#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "../../../external/physics.hpp"
#include "../../../Transform.hpp"

#include "ColliderType.hpp"
#include "ColliderTransform.hpp"

class RigidBody;

class Collider // interface class.
{
    friend class RigidBody;
    friend class ColliderTransform;

    protected:
        rp3d::PhysicsCommon *phys;
        rp3d::RigidBody *physrb;
        RigidBody *wraprb;
        rp3d::Collider *collider = nullptr;

        Collider(rp3d::PhysicsCommon *phys, rp3d::RigidBody *physrb, RigidBody *wraprb, Transform t);
        virtual ~Collider();

        void OnTransformChanged();

        void linkphyscollwiththis();
        void unlinkphyscollfromthis();

    public:
        const ColliderType type = ColliderType::UNKNOWN;
        ColliderTransform transform;

        RigidBody *GetRigidBody() const;

        float GetBounciness() const;
        void SetBounciness(float bounciness);

        float GetFrictionCoefficient() const;
        void SetFrictionCoefficient(float coefficient);

        float GetMassDensity() const;
        void SetMassDensity(float density);

        bool GetIsTrigger() const;
        void SetIsTrigger(bool istrigger);

        unsigned short GetCollisionCategoryBits();
        void SetCollisionCategoryBits(unsigned short bits);

        unsigned short GetCollideWithMaskBits();
        void SetCollideWithMaskBits(unsigned short bits);
};

#endif