#include "Scene.hpp"

#include <iterator>
#include <algorithm>
#include <exception>
#include <stdexcept>

#include "objects/GameObject/GameObject.hpp"
#include "objects/Camera/Camera.hpp"
#include "physics/SceneRaycastCallback.hpp"

#include "Utils.hpp"

// === PRIVATE ===

Scene::Scene()
{
    objects.insert({0, std::vector<GameObject *>()});

    world = Engine::phys->createPhysicsWorld();
}

Scene::~Scene()
{
    ForEachAllObjects([&](GameObject *obj) -> bool { DeleteObject(obj); return true; });

    if (Engine::GetScene(Engine::GetCurrentScene()) == this) Engine::SetCurrentScene("");

    Engine::phys->destroyPhysicsWorld(world);
}

void Scene::Update(double delta)
{
    ForEachAllObjects([&](GameObject *obj) -> bool { obj->Update(delta); return true; });

    world->update(delta);

    ForEachAllObjects([&](GameObject *obj) -> bool { obj->AfterUpdate(); return true; });
}

void Scene::Render()
{
    if (!currcam) return;

    glm::mat4 proj = currcam->GetProjectionMatrix(Engine::GetWindowSize());
    glm::mat4 view = currcam->GetViewMatrix();
    Transform globt = currcam->GetGlobalTransform();

    ForEachAllOrders([&](std::vector<GameObject *> layer) -> bool
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        for (GameObject *obj : layer) obj->Render(&proj, &view, &globt, &fog);
        return true;
    });
}

void Scene::OnSceneLoad() { ForEachAllObjects([&](GameObject *obj) -> bool { obj->OnSceneLoad(); return true; }); }
void Scene::OnSceneUnload() { ForEachAllObjects([&](GameObject *obj) -> bool { obj->OnSceneUnload(); return true; }); }

// === PUBLIC ===

// ============================================================================================================

bool Scene::HasObject(GameObject *obj)
{
    bool ret = false;
    ForEachAllObjects([&](GameObject *o) -> bool
    {
        ret = obj == o;
        return !ret;
    });
    return ret;
}

void Scene::DeleteObject(GameObject *obj)
{
    if (!HasObject(obj)) throw std::runtime_error("this scene doesn't have this object");

    std::vector<GameObject *> *group = &objects.at(obj->order);
    group->erase(std::remove(group->begin(), group->end(), obj), group->end());
    if (!group->size()) objects.erase(obj->order);

    delete obj;
}

// ============================================================================================================

int32_t Scene::GetObjectOrder(GameObject *obj) { return obj->order; }

void Scene::SetObjectOrder(GameObject *obj, int32_t order)
{
    if (!HasObject(obj)) throw std::runtime_error("this scene doesn't have this object");
    if (order == obj->order) return;

    std::vector<GameObject *> *group = &objects.at(obj->order);
    group->erase(std::remove(group->begin(), group->end(), obj), group->end());
    if (!group->size()) objects.erase(obj->order);

    if (!objects.contains(order)) objects.insert({order, std::vector<GameObject *>()});
    objects.at(order).push_back(obj);
    obj->order = order;
}

// ============================================================================================================

void Scene::ForEachAllObjects(std::function<bool (GameObject *)> callback)
{
    for (std::pair<int32_t, std::vector<GameObject *>> pair : objects)
    {
        for (GameObject *obj : pair.second) if (!callback(obj)) return;
    }
}

void Scene::ForEachAllOrders(std::function<bool (std::vector<GameObject *>)> callback)
{
    for (std::pair<int32_t, std::vector<GameObject *>> pair : objects) if (!callback(pair.second)) return;
}

// ============================================================================================================

Camera *Scene::GetCurrentCamera() { return currcam; }
void Scene::SetCurrentCamera(Camera *camera)
{
    if (!HasObject(camera)) throw std::runtime_error("this scene doesn't have this object");
    currcam = camera;
}

glm::vec3 Scene::GetGravity() { return Utils::rp3dvec3toglmvec3(world->getGravity()); }
void Scene::SetGravity(glm::vec3 v) { world->setGravity(Utils::glmvec3torp3dvec3(v)); }

bool Scene::Raycast(glm::vec3 start, glm::vec3 end, std::function<RaycastCallbackState (RaycastInfo)> callback)
{
    SceneRaycastCallback c(this, callback);
    rp3d::Ray ray(Utils::glmvec3torp3dvec3(start), Utils::glmvec3torp3dvec3(end));

    world->raycast(ray, &c);

    return c.hit;
}