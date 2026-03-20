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
    SetRigidBodyType(RigidBodyType::STATIC);
}

// === PROTECTED ===

RigidBody::RigidBody(Scene *s, Transform t) : GameObject(s, t) { constructor(); }
RigidBody::RigidBody(Scene *s) : GameObject(s) { constructor(); }

RigidBody::~RigidBody() { scene->world->destroyRigidBody(rb); }

void RigidBody::OnGlobalTransformChanged()
{
    GameObject::OnGlobalTransformChanged();

    if (lockphystransupdate) { lockphystransupdate = false; return; }
    rb->setTransform(Utils::transformtorp3dtransform(GetGlobalTransform()));
}

void RigidBody::AfterUpdate()
{
    //GameObject::AfterUpdate();

    if (GetRigidBodyType() == RigidBodyType::STATIC) return;

    Transform newt = Utils::rp3dtransformtotransform(rb->getTransform());
    lockphystransupdate = true;
    transform = Transform(newt.GetPosition(), newt.GetRotation(), transform.GetScale());
}

// === PUBLIC ===

size_t RigidBody::SetParent(GameObject *new_parent, bool save_global_pos)
{
    if (GetRigidBodyType() == RigidBodyType::STATIC) return GameObject::SetParent(new_parent, save_global_pos);
    return -1;
}

RigidBodyType RigidBody::GetRigidBodyType() const
{
    switch (rb->getType())
    {
        case rp3d::BodyType::STATIC:
            return RigidBodyType::STATIC;
        
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

            GameObject::SetParent(nullptr, true); // deattach from parent, because dynamic physics body must be in world coordinate system.
            //SetGravityEnabled(false);

            break;

        case RigidBodyType::STATIC:
            rb->setType(rp3d::BodyType::STATIC);

            break;
    }
}

bool RigidBody::IsGravityEnabled() const { return rb->isGravityEnabled(); }
void RigidBody::SetGravityEnabled(bool state) { rb->enableGravity(state); }

glm::vec3 RigidBody::GetLinearVelocity() const { return Utils::rp3dvec3toglmvec3(rb->getLinearVelocity()); }
void RigidBody::SetLinearVelocity(glm::vec3 v) { rb->setLinearVelocity(Utils::glmvec3torp3dvec3(v)); }

float RigidBody::GetMass() const { return rb->getMass(); }
void RigidBody::SetMass(float mass) { rb->setMass(mass); }

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