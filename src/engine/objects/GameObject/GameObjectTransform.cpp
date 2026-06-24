#include "GameObjectTransform.hpp"

#include "GameObject.hpp"

// === PRIVATE ===

GameObjectTransform::GameObjectTransform(GameObject *obj) : Transform(), object(obj) {}

void GameObjectTransform::OnTransformChanged()
{
    Transform::OnTransformChanged();
    
    if (locknotifyinggameobject) { locknotifyinggameobject = false; return; }
    object->OnLocalTransformChanged();
}

void GameObjectTransform::SetTransformSilently(Transform transform)
{
    locknotifyinggameobject = true;
    *this = transform;
}

GameObjectTransform *GameObjectTransform::operator=(Transform other)
{
    Transform::operator=(other);
    return this;
}