#include "Entity.hpp"

#include <exception>
#include <stdexcept>

#include "../../Scene.hpp"
#include "../../ResourceManager.hpp"

#include "../../resources/Mesh.hpp"
#include "../../resources/Texture.hpp"
#include "../../resources/ShaderProgram.hpp"

#include "../../Utils.hpp"

// === PRIVATE ===

void Entity::constructor()
{
    rb = scene->world->createRigidBody(Utils::transformtorp3dtransform(transform));
    SetRigidBodyType(EntityRigidBodyType::STATIC);
}

// === PROTECTED ===

Entity::Entity(Scene *s, Transform t) : GameObject(s, t), Model(s, t) { constructor(); }
Entity::Entity(Scene *s) : GameObject(s), Model(s) { constructor(); }

Entity::~Entity() { scene->world->destroyRigidBody(rb); }

void Entity::OnGlobalTransformChanged()
{
    GameObject::OnGlobalTransformChanged();

    if (lockphystransupdate) { lockphystransupdate = false; return; }
    rb->setTransform(Utils::transformtorp3dtransform(GetGlobalTransform()));
}

void Entity::AfterUpdate()
{
    //GameObject::AfterUpdate();

    if (GetRigidBodyType() == EntityRigidBodyType::STATIC) return;

    Transform newt = Utils::rp3dtransformtotransform(rb->getTransform());
    lockphystransupdate = true;
    transform = Transform(newt.GetPosition(), newt.GetRotation(), transform.GetScale());
}

// === PUBLIC ===

size_t Entity::SetParent(GameObject *new_parent, bool save_global_pos)
{
    if (GetRigidBodyType() == EntityRigidBodyType::STATIC) return GameObject::SetParent(new_parent, save_global_pos);
    return -1;
}

EntityRigidBodyType Entity::GetRigidBodyType()
{
    switch (rb->getType())
    {
        case rp3d::BodyType::STATIC:
            return EntityRigidBodyType::STATIC;
        
        case rp3d::BodyType::DYNAMIC:
            return EntityRigidBodyType::DYNAMIC;

        default:
            throw std::runtime_error("unsupported type of rigidbody");
    }
}

void Entity::SetRigidBodyType(EntityRigidBodyType type)
{
    if (GetRigidBodyType() == type) return;

    switch (type)
    {
        case EntityRigidBodyType::DYNAMIC:
            rb->setType(rp3d::BodyType::DYNAMIC);

            GameObject::SetParent(nullptr, true); // deattach from parent, because dynamic physics body must be in world coordinate system.
            SetEnabledGravity(false);

            break;

        case EntityRigidBodyType::STATIC:
            rb->setType(rp3d::BodyType::STATIC);

            break;
    }
}

bool Entity::IsGravityEnabled() const { return rb->isGravityEnabled(); }
void Entity::SetEnabledGravity(bool state) { rb->enableGravity(state); }

glm::vec3 Entity::GetLinearVelocity() const { return Utils::rp3dvec3toglmvec3(rb->getLinearVelocity()); }
void Entity::SetLinearVelocity(glm::vec3 v) { rb->setLinearVelocity(Utils::glmvec3torp3dvec3(v)); }