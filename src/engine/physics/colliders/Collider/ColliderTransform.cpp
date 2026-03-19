#include "ColliderTransform.hpp"

#include "Collider.hpp"

// === PRIVATE ===

ColliderTransform::ColliderTransform(Collider *c) : Transform(), collider(c) {}

void ColliderTransform::OnTransformChanged()
{
    Transform::OnTransformChanged();
    
    collider->OnTransformChanged();
}

ColliderTransform *ColliderTransform::operator=(Transform other)
{
    Transform::operator=(other);
    return this;
}