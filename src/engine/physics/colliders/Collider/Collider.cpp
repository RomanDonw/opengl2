#include "Collider.hpp"

#include "../../../Engine.hpp"
#include "../../../Utils.hpp"
#include "../../../Scene.hpp"
#include "../../../objects/RigidBody/RigidBody.hpp"

// === PROTECTED ===

Collider::Collider(rp3d::PhysicsCommon *phys, rp3d::RigidBody *physrb, RigidBody *wraprb, Transform t) : phys(phys), physrb(physrb), wraprb(wraprb), transform(this) { transform = t; }
Collider::~Collider() {}

void Collider::OnTransformChanged()
{
    if (!collider) return;

    collider->setLocalToBodyTransform(Utils::transformtorp3dtransform(transform));
}

void Collider::linkphyscollwiththis() { wraprb->scene->collslinks.insert({collider, this}); }
void Collider::unlinkphyscollfromthis() { wraprb->scene->collslinks.erase(collider); }

// === PUBLIC ===

RigidBody *Collider::GetRigidBody() const { return wraprb; }

float Collider::GetBounciness() const { return collider->getMaterial().getBounciness(); }
void Collider::SetBounciness(float bounciness) { collider->getMaterial().setBounciness(bounciness); }

float Collider::GetFrictionCoefficient() const { return collider->getMaterial().getFrictionCoefficient(); }
void Collider::SetFrictionCoefficient(float coefficient) { collider->getMaterial().setFrictionCoefficient(coefficient); }

float Collider::GetMassDensity() const { return collider->getMaterial().getMassDensity(); }
void Collider::SetMassDensity(float density) { collider->getMaterial().setMassDensity(density); }