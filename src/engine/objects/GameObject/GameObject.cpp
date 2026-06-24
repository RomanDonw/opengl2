#include "GameObject.hpp"

#include <algorithm>
#include <iterator>

#include "../../ResourceManager.hpp"
#include "../../resources/ShaderProgram.hpp"
#include "../../Scene.hpp"

// === PROTECTED ===

void GameObject::OnLocalTransformChanged() { OnGlobalTransformChanged(); }
void GameObject::OnParentTransformChanged() { OnGlobalTransformChanged(); }

void GameObject::OnGlobalTransformChanged()
{
    for (GameObject *obj : GetChildren()) obj->OnParentTransformChanged();
    for (GameObject *obj : GetShadowChildren()) obj->OnParentTransformChanged();
}

GameObject::GameObject(Scene *s, Transform t) : transform(this), scene(s) { transform = t; }
GameObject::GameObject(Scene *s) : transform(this), scene(s) {}

GameObject::~GameObject()
{
    SetParent(nullptr, false);
    for (GameObject *obj : GetChildren()) obj->SetParent(nullptr);
    for (GameObject *obj : GetShadowChildren()) scene->DeleteObject(obj);
}

void GameObject::Update(double delta) {}

void GameObject::AfterUpdate() {}

void GameObject::Render(const GameObjectRenderData *data) {}

void GameObject::OnSceneLoad() {}
void GameObject::OnSceneUnload() {}

std::unordered_set<GameObject *> GameObject::GetShadowChildren() const { return shadowchildren; }

bool GameObject::AddShadowChild(GameObject *child, bool saveglobalpos)
{
    if (!child || child == this || child->scene != scene) return false; // objects must be in the same scene.
    
    //child->SetParent(nullptr, saveglobalpos);
    if (child->parent)
    {
        if (child->isshadowchild) child->parent->shadowchildren.erase(this);
        else child->parent->children.erase(this);
    }

    shadowchildren.insert(child);
    child->isshadowchild = true;

    if (saveglobalpos) child->transform.SetTransformSilently(child->GetGlobalTransform().GlobalToLocal(GetGlobalTransform()));
    
    child->parent = this;
    child->OnParentTransformChanged();
    //if (saveglobalpos) child->OnLocalTransformChanged();
    return true;
}

// === PUBLIC ===

GameObject *GameObject::GetParent() const { return parent; }

bool GameObject::SetParent(GameObject *newparent, bool saveglobalpos)
{
    if (newparent == this || (newparent && (newparent->scene != scene))) return false; // objects must be in the same scene.
    
    if (parent)
    {
        if (isshadowchild) parent->shadowchildren.erase(this);
        else parent->children.erase(this);
    }

    if (newparent) newparent->children.insert(this);
    isshadowchild = false;

    if (saveglobalpos)
    {
        Transform globt = GetGlobalTransform();
        transform.SetTransformSilently(newparent ? globt.GlobalToLocal(newparent->GetGlobalTransform()) : globt);
    }

    parent = newparent;
    OnParentTransformChanged();
    //if (saveglobalpos) OnLocalTransformChanged();
    return true;
}

std::unordered_set<GameObject *> GameObject::GetChildren() const { return children; }

Scene *GameObject::GetScene() const { return scene; }

Transform GameObject::GetParentGlobalTransform() const
{ return parent ? parent->transform.LocalToGlobal(parent->GetParentGlobalTransform()) : Transform(); }

Transform GameObject::GetGlobalTransform() const
{ return ((Transform)transform).LocalToGlobal(GetParentGlobalTransform()); }