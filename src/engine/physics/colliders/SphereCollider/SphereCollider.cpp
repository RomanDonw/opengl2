#include "SphereCollider.hpp"

#include "../../../objects/RigidBody/RigidBody.hpp"
#include "../../../Utils.hpp"

// === PRIVATE ===

SphereCollider::SphereCollider(rp3d::PhysicsCommon *phys, rp3d::RigidBody *physrb, RigidBody *wraprb, Transform t, float radius) : Collider(phys, physrb, wraprb, t)
{
    shape = phys->createSphereShape(radius);
    collider = physrb->addCollider(shape, Utils::transformtorp3dtransform(t));
    linkphyscollwiththis();
}

SphereCollider::~SphereCollider()
{
    unlinkphyscollfromthis();
    physrb->removeCollider(collider);
    phys->destroySphereShape(shape);
}

// === PUBLIC ===

float SphereCollider::GetRadius() const { return shape->getRadius(); }
void SphereCollider::SetRadius(float radius) { shape->setRadius(radius); }