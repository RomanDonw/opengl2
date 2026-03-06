#include "Scene.hpp"

#include <exception>
#include <stdexcept>
#include <iterator>
#include <algorithm>

#include "objects/GameObject/GameObject.hpp"
#include "objects/Camera/Camera.hpp"
#include "Engine.hpp"

// === PRIVATE ===

Scene::Scene() {}
Scene::~Scene()
{
    for (GameObject *obj : objects) DeleteObject(obj);
}

void Scene::Update(double delta)
{
    for (GameObject *obj : objects) obj->Update(delta);
}

void Scene::Render()
{
    if (!currcam) return;

    glm::mat4 proj = currcam->GetProjectionMatrix(Engine::GetWindowSize());
    glm::mat4 view = currcam->GetViewMatrix();

    for (GameObject *obj : objects) obj->Render(&proj, &view, &currcam->transform);
}

// === PUBLIC ===

template<std::derived_from<GameObject> T>
bool Scene::HasObject(T *obj) { return std::ranges::contains(objects, obj); }

template<std::derived_from<GameObject> T, typename... Args>
T *Scene::CreateObject(Args&&... args)
{
    T *ret = new T(this, std::forward<Args>(args)...);

    objects.push_back(ret);
    
    return ret;
}

template<std::derived_from<GameObject> T>
void Scene::DeleteObject(T *obj)
{
    if (!HasObject(obj)) throw std::runtime_error("this scene doesn't have this object");

    objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());

    delete obj;
}

Camera *Scene::GetCurrentCamera() { return currcam; }
void Scene::SetCurrentCamera(Camera *camera) { currcam = camera; }
/*{
    if (!camera) { currcam = nullptr; return; }

}*/