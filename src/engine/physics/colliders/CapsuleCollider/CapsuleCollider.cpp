#include "CapsuleCollider.hpp"

#include "../../../objects/RigidBody/RigidBody.hpp"
#include "../../../Utils.hpp"

// === PRIVATE ===

CapsuleCollider::CapsuleCollider(rp3d::PhysicsCommon *phys, rp3d::RigidBody *physrb, RigidBody *wraprb, Transform t, float radius, float height) : Collider(phys, physrb, wraprb, t)
{
    shape = phys->createCapsuleShape(radius, height);
    collider = physrb->addCollider(shape, Utils::transformtorp3dtransform(t));
    linkphyscollwiththis();
}

CapsuleCollider::~CapsuleCollider()
{
    unlinkphyscollfromthis();
    physrb->removeCollider(collider);
    phys->destroyCapsuleShape(shape);
}

// === PUBLIC ===

float CapsuleCollider::GetRadius() const { return shape->getRadius(); }
void CapsuleCollider::SetRadius(float radius) { shape->setRadius(radius); }

float CapsuleCollider::GetHeight() const { return shape->getHeight(); }
void CapsuleCollider::SetHeight(float height) { shape->setHeight(height); }