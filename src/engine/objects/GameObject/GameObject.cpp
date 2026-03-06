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

void GameObject::Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt)
{
    if (!enableRender) return;

    ShaderProgram *sp = ResourceManager::GetShaderProgram(usedShaderProgram);
    if (!sp) return;

    sp->UseThisProgram();

    sp->SetUniformMatrix4x4("projection", *proj);
    sp->SetUniformMatrix4x4("view", *view);

    sp->SetUniformVector3("cameraPosition", camt->GetPosition());
    sp->SetUniformVector3("cameraRotation", glm::eulerAngles(camt->GetRotation()));

    sp->SetUniformVector3("cameraFront", camt->GetFront());
    sp->SetUniformVector3("cameraUp", camt->GetUp());
    sp->SetUniformVector3("cameraRight", camt->GetRight());
}

// === PUBLIC ===

GameObject *GameObject::GetParent() { return parent; }

size_t GameObject::SetParent(GameObject *new_parent, bool save_global_pos)
{
    size_t index = -1; // yea, i know that size_t is an unsigned type, but vector can't contain 2^64 - 1 elements, so i can use that magic number as "special return code/value".
    if (new_parent == this) return -1;
    
    if (parent) parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
    if (new_parent)
    {
        new_parent->children.push_back(this);
        index = new_parent->children.size() - 1;
    }

    OnParentTransformChanged();

    if (save_global_pos)
    {
        Transform globt = GetGlobalTransform();
        transform = new_parent ? globt.GlobalToLocal(new_parent->GetGlobalTransform()) : globt;
    }

    parent = new_parent;
    return index;
}

Scene *GameObject::GetScene() { return scene; }

Transform GameObject::GetParentGlobalTransform()
{ return parent ? (Transform)parent->transform.LocalToGlobal(parent->GetParentGlobalTransform()) : Transform(); }

Transform GameObject::GetGlobalTransform()
{ return (Transform)transform.LocalToGlobal(GetParentGlobalTransform()); }