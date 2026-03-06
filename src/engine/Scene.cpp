#include "Scene.hpp"

#include <exception>
#include <stdexcept>
#include <iterator>
#include <algorithm>

#include "objects/GameObject/GameObject.hpp"

// === PRIVATE ===

Scene::Scene() {}
Scene::~Scene()
{
    for (GameObject *obj : objects) DeleteObject(obj);
}

// === PUBLIC ===

template<std::derived_from<GameObject> T, typename... Args>
T *Scene::CreateObject(Args&&... args)
{
    T *ret = new T(std::forward<Args>(args)...);

    objects.push_back(ret);
    
    return ret;
}

template<std::derived_from<GameObject> T>
void Scene::DeleteObject(T *obj)
{
    if (!std::ranges::contains(objects, obj)) throw std::runtime_error("this scene doesn't have this object");

    objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());

    delete obj;
}

void Scene::Update(double delta)
{
    for (GameObject *obj : objects) obj->Update(delta);
}

void Scene::Render()
{
    for (GameObject *obj : objects) obj->Render();
}