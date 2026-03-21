#include "BoxCollider.hpp"

#include "../../../objects/RigidBody/RigidBody.hpp"
#include "../../../Utils.hpp"

// === PRIVATE ===

BoxCollider::BoxCollider(rp3d::PhysicsCommon *phys, rp3d::RigidBody *physrb, RigidBody *wraprb, Transform t, glm::vec3 halfExtents) : Collider(phys, physrb, wraprb, t)
{
    shape = phys->createBoxShape(Utils::glmvec3torp3dvec3(halfExtents));
    collider = physrb->addCollider(shape, Utils::transformtorp3dtransform(t));
    linkphyscollwiththis();
}

BoxCollider::~BoxCollider()
{
    unlinkphyscollfromthis();
    physrb->removeCollider(collider);
    phys->destroyBoxShape(shape);
}

// === PUBLIC ===

glm::vec3 BoxCollider::GetHalfExtents() const { return Utils::rp3dvec3toglmvec3(shape->getHalfExtents()); }
void BoxCollider::SetHalfExtents(glm::vec3 halfExtents) { shape->setHalfExtents(Utils::glmvec3torp3dvec3(halfExtents)); }