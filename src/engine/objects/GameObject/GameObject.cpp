#include "GameObject.hpp"

#include <algorithm>
#include <iterator>

#include "../../ResourceManager.hpp"
#include "../../resources/ShaderProgram.hpp"

// === PRIVATE ===

void GameObject::OnLocalTransformChanged() { OnGlobalTransformChanged(); }
void GameObject::OnParentTransformChanged() { OnGlobalTransformChanged(); }

void GameObject::OnGlobalTransformChanged() { for (GameObject *obj : children) obj->OnParentTransformChanged(); }

GameObject::GameObject(Scene *s, Transform t) : transform(this), scene(s) { transform = t; }
GameObject::GameObject(Scene *s) : transform(this), scene(s) {}

GameObject::~GameObject()
{
    SetParent(nullptr, false);
    for (GameObject *obj : children) obj->SetParent(nullptr);
}

void GameObject::Update(double delta) {}

void GameObject::AfterUpdate() {}

void GameObject::Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt, const FogRenderSettings *fog) {}

void GameObject::OnSceneLoad() {}
void GameObject::OnSceneUnload() {}

// === PUBLIC ===

GameObject *GameObject::GetParent() { return parent; }

size_t GameObject::SetParent(GameObject *newparent, bool saveglobalpos)
{
    size_t index = -1; // yea, i know that size_t is an unsigned type, but vector can't contain 2^64 - 1 elements, so i can use that magic number as "special return code/value".
    if (newparent == this) return -1;
    if (newparent && (newparent->scene != scene)) return -1; // objects must be in the same scene.
    
    if (parent) parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
    if (newparent)
    {
        newparent->children.push_back(this);
        index = newparent->children.size() - 1;
    }

    OnParentTransformChanged();

    if (saveglobalpos)
    {
        Transform globt = GetGlobalTransform();
        transform = newparent ? globt.GlobalToLocal(newparent->GetGlobalTransform()) : globt;
    }

    parent = newparent;
    return index;
}

Scene *GameObject::GetScene() { return scene; }

Transform GameObject::GetParentGlobalTransform()
{ return parent ? (Transform)parent->transform.LocalToGlobal(parent->GetParentGlobalTransform()) : Transform(); }

Transform GameObject::GetGlobalTransform()
{ return (Transform)transform.LocalToGlobal(GetParentGlobalTransform()); }