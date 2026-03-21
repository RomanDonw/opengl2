#include "RigidBody.hpp"

#include <exception>
#include <stdexcept>

#include "../../Scene.hpp"
#include "../../Utils.hpp"

#include "../../physics/colliders/Collider/Collider.hpp"

// === PRIVATE ===

void RigidBody::constructor()
{
    rb = scene->world->createRigidBody(Utils::transformtorp3dtransform(transform));
    SetRigidBodyType(RigidBodyType::KINEMATIC);
}

// === PROTECTED ===

RigidBody::RigidBody(Scene *s, Transform t) : GameObject(s, t) { constructor(); }
RigidBody::RigidBody(Scene *s) : GameObject(s) { constructor(); }

RigidBody::~RigidBody()
{
    for (Collider *c : colliders) RemoveCollider(c);
    scene->world->destroyRigidBody(rb);
}

// ==================================================================================

void RigidBody::OnGlobalTransformChanged()
{
    GameObject::OnGlobalTransformChanged();

    if (lockphystransupdate) { lockphystransupdate = false; return; }
    rb->setTransform(Utils::transformtorp3dtransform(GetGlobalTransform()));
}

void RigidBody::AfterUpdate()
{
    //GameObject::AfterUpdate();

    Transform newt = Utils::rp3dtransformtotransform(rb->getTransform()).GlobalToLocal(GetParentGlobalTransform());
    newt.SetScale(transform.GetScale());
    {
        lockphystransupdate = true;
        transform = newt;
    }
}

// ==================================================================================

// === PUBLIC ===

RigidBodyType RigidBody::GetRigidBodyType() const
{
    switch (rb->getType())
    {
        case rp3d::BodyType::KINEMATIC:
            return RigidBodyType::KINEMATIC;
        
        case rp3d::BodyType::DYNAMIC:
            return RigidBodyType::DYNAMIC;

        default:
            throw std::runtime_error("undefined type of rigidbody");
    }
}

void RigidBody::SetRigidBodyType(RigidBodyType type)
{
    if (GetRigidBodyType() == type) return;

    switch (type)
    {
        case RigidBodyType::DYNAMIC:
            rb->setType(rp3d::BodyType::DYNAMIC);
            break;

        case RigidBodyType::KINEMATIC:
            rb->setType(rp3d::BodyType::KINEMATIC);
            break;
    }
}

bool RigidBody::IsGravityEnabled() const { return rb->isGravityEnabled(); }
void RigidBody::SetGravityEnabled(bool state) { rb->enableGravity(state); }

glm::vec3 RigidBody::GetLinearVelocity() const { return Utils::rp3dvec3toglmvec3(rb->getLinearVelocity()); }
void RigidBody::SetLinearVelocity(glm::vec3 v) { rb->setLinearVelocity(Utils::glmvec3torp3dvec3(v)); }

glm::vec3 RigidBody::GetAngularVelocity() const { return Utils::rp3dvec3toglmvec3(rb->getAngularVelocity()); }
void RigidBody::SetAngularVelocity(glm::vec3 v) { rb->setAngularVelocity(Utils::glmvec3torp3dvec3(v)); }

float RigidBody::GetMass() const { return rb->getMass(); }
void RigidBody::SetMass(float mass) { rb->setMass(mass); }

glm::vec3 RigidBody::GetLinearLockAxisFactor() const { return Utils::rp3dvec3toglmvec3(rb->getLinearLockAxisFactor()); }
void RigidBody::SetLinearLockAxisFactor(glm::vec3 factor) { rb->setLinearLockAxisFactor(Utils::glmvec3torp3dvec3(glm::clamp(factor, glm::vec3(0), glm::vec3(1)))); }

glm::vec3 RigidBody::GetAngularLockAxisFactor() const { return Utils::rp3dvec3toglmvec3(rb->getAngularLockAxisFactor()); }
void RigidBody::SetAngularLockAxisFactor(glm::vec3 factor) { rb->setAngularLockAxisFactor(Utils::glmvec3torp3dvec3(glm::clamp(factor, glm::vec3(0), glm::vec3(1)))); }

void RigidBody::ApplyLocalForceToCenterOfMass(glm::vec3 force) { rb->applyLocalForceAtCenterOfMass(Utils::glmvec3torp3dvec3(force)); }
void RigidBody::ApplyGlobalForceToCenterOfMass(glm::vec3 force) { rb->applyWorldForceAtCenterOfMass(Utils::glmvec3torp3dvec3(force)); }

void RigidBody::ApplyLocalForceAtLocalPoint(glm::vec3 force, glm::vec3 point) { rb->applyLocalForceAtLocalPosition(Utils::glmvec3torp3dvec3(force), Utils::glmvec3torp3dvec3(point)); }
void RigidBody::ApplyGlobalForceAtLocalPoint(glm::vec3 force, glm::vec3 point) { rb->applyWorldForceAtLocalPosition(Utils::glmvec3torp3dvec3(force), Utils::glmvec3torp3dvec3(point)); }

void RigidBody::ApplyLocalForceAtGlobalPoint(glm::vec3 force, glm::vec3 point) { rb->applyLocalForceAtWorldPosition(Utils::glmvec3torp3dvec3(force), Utils::glmvec3torp3dvec3(point)); }
void RigidBody::ApplyGlobalForceAtGlobalPoint(glm::vec3 force, glm::vec3 point) { rb->applyWorldForceAtWorldPosition(Utils::glmvec3torp3dvec3(force), Utils::glmvec3torp3dvec3(point)); }

void RigidBody::ApplyLocalTorque(glm::vec3 torque) { rb->applyLocalTorque(Utils::glmvec3torp3dvec3(torque)); }
void RigidBody::ApplyGlobalTorque(glm::vec3 torque) { rb->applyWorldTorque(Utils::glmvec3torp3dvec3(torque)); }

// ==================================================================================

bool RigidBody::HasCollider(Collider *c) { return std::ranges::contains(colliders, c); }

void RigidBody::RemoveCollider(Collider *c)
{
    if (!HasCollider(c)) throw std::runtime_error("this rigidbody doesn't has this collider");
    colliders.erase(std::remove(colliders.begin(), colliders.end(), c), colliders.end());
    delete c;
}

Collider *RigidBody::GetCollider(size_t index)
{
    if (index >= colliders.size()) return nullptr;
    return colliders[index];
}

std::vector<Collider *> RigidBody::GetAllColliders() { return colliders; }