#include "Collider.hpp"

#include "../../../Engine.hpp"
#include "../../../Utils.hpp"

// === PROTECTED ===

Collider::Collider(rp3d::PhysicsCommon *phys, rp3d::RigidBody *physrb, RigidBody *wraprb, Transform t) : phys(phys), physrb(physrb), wraprb(wraprb), transform(this) { transform = t; }
Collider::~Collider() {}

void Collider::OnTransformChanged()
{
    if (!collider) return;

    collider->setLocalToBodyTransform(Utils::transformtorp3dtransform(transform));
}

// === PUBLIC ===

RigidBody *Collider::GetRigidBody() const { return wraprb; }