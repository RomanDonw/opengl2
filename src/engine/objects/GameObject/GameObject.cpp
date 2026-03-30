#include "GameObject.hpp"

#include <algorithm>
#include <iterator>

#include "../../ResourceManager.hpp"
#include "../../resources/ShaderProgram.hpp"

// === PROTECTED ===

void GameObject::OnLocalTransformChanged() { OnGlobalTransformChanged(); }
void GameObject::OnParentTransformChanged() { OnGlobalTransformChanged(); }

void GameObject::OnGlobalTransformChanged() { for (GameObject *obj : GetChildren()) obj->OnParentTransformChanged(); }

GameObject::GameObject(Scene *s, Transform t) : transform(this), scene(s) { transform = t; }
GameObject::GameObject(Scene *s) : transform(this), scene(s) {}

GameObject::~GameObject()
{
    SetParent(nullptr, false);
    for (GameObject *obj : GetChildren()) obj->SetParent(nullptr);
}

void GameObject::Update(double delta) {}

void GameObject::AfterUpdate() {}

void GameObject::Render(const GameObjectRenderData *data) {}

void GameObject::OnSceneLoad() {}
void GameObject::OnSceneUnload() {}

// === PUBLIC ===

GameObject *GameObject::GetParent() const { return parent; }

bool GameObject::SetParent(GameObject *newparent, bool saveglobalpos)
{
    if (newparent == this) return false;
    if (newparent && (newparent->scene != scene)) return false; // objects must be in the same scene.
    
    if (parent) parent->children.erase(this);
    if (newparent) newparent->children.insert(this);

    OnParentTransformChanged();

    if (saveglobalpos)
    {
        Transform globt = GetGlobalTransform();
        transform = newparent ? globt.GlobalToLocal(newparent->GetGlobalTransform()) : globt;
    }

    parent = newparent;
    return true;
}

std::unordered_set<GameObject *> GameObject::GetChildren() const { return children; }

Scene *GameObject::GetScene() const { return scene; }

Transform GameObject::GetParentGlobalTransform()
{ return parent ? parent->transform.LocalToGlobal(parent->GetParentGlobalTransform()) : Transform(); }

Transform GameObject::GetGlobalTransform()
{ return transform.LocalToGlobal(GetParentGlobalTransform()); }