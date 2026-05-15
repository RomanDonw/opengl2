#include "Scene.hpp"

#include <exception>
#include <stdexcept>

#include "objects/GameObject/GameObject.hpp"
#include "objects/Camera/Camera.hpp"
#include "objects/Light/Light.hpp"
#include "physics/SceneRaycastCallback.hpp"

#include "Utils.hpp"

Scene::Scene()
{
    objects.insert({0, std::unordered_set<GameObject *>()});
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

    glm::uvec2 scrsize = Engine::GetWindowSize();
    if (renderTargetSize.x > 0 && renderTargetSize.y > 0) scrsize = renderTargetSize;
    if (scrsize.x == 0 || scrsize.y == 0) return;

    const glm::mat4 proj = currcam->GetProjectionMatrix(scrsize);
    const glm::mat4 view = currcam->GetViewMatrix();
    const Transform camt = currcam->GetGlobalTransform();

    LightRenderSettings lights;
    lights.ambientColor = ambientLight;
    ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (Light *light = dynamic_cast<Light *>(obj)) light->CollectLight(lights);
        return true;
    });

    GameObjectRenderData data;
    data.proj = &proj;
    data.view = &view;
    data.camt = &camt;
    data.fog = &fog;
    data.lights = &lights;
    data.cameraPosition = camt.GetPosition();
    data.cameraRotation = glm::eulerAngles(camt.GetRotation());
    data.cameraFront = camt.GetFront();
    data.cameraUp = camt.GetUp();
    data.cameraRight = camt.GetRight();

    ForEachAllObjects([&](GameObject *obj) -> bool { obj->Render(&data); return true; });
}

void Scene::OnSceneLoad() { ForEachAllObjects([&](GameObject *obj) -> bool { obj->OnSceneLoad(); return true; }); }
void Scene::OnSceneUnload() { ForEachAllObjects([&](GameObject *obj) -> bool { obj->OnSceneUnload(); return true; }); }

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

    std::unordered_set<GameObject *> *group = &objects.at(obj->order);
    group->erase(obj);
    if (!group->size()) objects.erase(obj->order);

    delete obj;
}

int32_t Scene::GetObjectOrder(GameObject *obj) { return obj->order; }

void Scene::SetObjectOrder(GameObject *obj, int32_t order)
{
    if (!HasObject(obj)) throw std::runtime_error("this scene doesn't have this object");
    if (order == obj->order) return;

    std::unordered_set<GameObject *> *group = &objects.at(obj->order);
    group->erase(obj);
    if (!group->size()) objects.erase(obj->order);

    if (!objects.contains(order)) objects.insert({order, std::unordered_set<GameObject *>()});
    objects.at(order).insert(obj);
    obj->order = order;
}

void Scene::ForEachAllObjects(std::function<bool (GameObject *)> callback)
{
    for (const auto &pair : objects)
    {
        for (GameObject *obj : pair.second)
        {
            if (!callback(obj)) return;
        }
    }
}

void Scene::ForEachAllOrders(std::function<bool (std::unordered_set<GameObject *>)> callback)
{
    for (const auto &pair : objects)
    {
        if (!callback(pair.second)) return;
    }
}

Camera *Scene::GetCurrentCamera() { return currcam; }

void Scene::SetCurrentCamera(Camera *camera)
{
    if (!HasObject(camera)) throw std::runtime_error("this scene doesn't have this object");
    currcam = camera;
}

glm::vec3 Scene::GetGravity() { return Utils::rp3dvec3toglmvec3(world->getGravity()); }

void Scene::SetGravity(glm::vec3 v) { world->setGravity(Utils::glmvec3torp3dvec3(v)); }

size_t Scene::GetObjectCount()
{
    size_t count = 0;
    for (const auto &pair : objects) count += pair.second.size();
    return count;
}

std::vector<GameObject *> Scene::FindObjectsWithTag(const std::string &tag)
{
    std::vector<GameObject *> result;
    ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (obj->tags.contains(tag)) result.push_back(obj);
        return true;
    });
    return result;
}

GameObject *Scene::FindFirstObjectWithTag(const std::string &tag)
{
    GameObject *found = nullptr;
    ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (obj->tags.contains(tag))
        {
            found = obj;
            return false;
        }
        return true;
    });
    return found;
}

bool Scene::Raycast(glm::vec3 start, glm::vec3 end, std::function<RaycastCallbackState (RaycastInfo)> callback, unsigned short collidewithmaskbits)
{
    SceneRaycastCallback c(this, callback);
    rp3d::Ray ray(Utils::glmvec3torp3dvec3(start), Utils::glmvec3torp3dvec3(end));

    world->raycast(ray, &c, collidewithmaskbits);

    return c.hit;
}

void Scene::SetRenderTargetSize(unsigned int width, unsigned int height) { renderTargetSize = glm::uvec2(width, height); }

void Scene::ClearRenderTargetSize() { renderTargetSize = glm::uvec2(0); }
