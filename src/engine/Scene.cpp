#include "Scene.hpp"

#include <iterator>
#include <algorithm>
#include <exception>
#include <stdexcept>

#include "objects/GameObject/GameObject.hpp"
#include "objects/Camera/Camera.hpp"
#include "physics/SceneRaycastCallback.hpp"
#include "objects/PointLight/PointLight.hpp"
#include "render/SSBO.hpp"

#include "Utils.hpp"

// === PRIVATE ===

Scene::Scene()
{
    objects.insert({0, std::unordered_set<GameObject *>()});

    world = Engine::phys->createPhysicsWorld();
}

Scene::~Scene()
{
    //ForEachAllObjects([&](GameObject *obj) -> bool { DeleteObject(obj); return true; });
    cleanupobjects(true);

    if (Engine::GetScene(Engine::GetCurrentScene()) == this) Engine::SetCurrentScene("");

    Engine::phys->destroyPhysicsWorld(world);
}

std::vector<GameObject *> Scene::getallobjectsflat()
{
    std::vector<GameObject *> ret;
    ForEachAllObjects([&](GameObject *obj) -> bool { ret.push_back(obj); return true; });
    return ret;
}
#include <cstdio>
void Scene::deleteobject(GameObject *obj)
{
    printf("obj type: %s\nobj->parent (1): %p\n", typeid(*obj).name(), obj->parent);

    obj->BeforeDeletion();

    printf("obj->parent (2): %p\n", obj->parent);
    
    std::unordered_set<GameObject *> *group = &objects.at(obj->order);
    group->erase(obj);
    if (!group->size()) objects.erase(obj->order);

    delete obj;

    putchar('\n');
}

void Scene::cleanupobjects(bool forcedeletion)
{
    std::vector<GameObject *> objs = getallobjectsflat();
    while (objs.size())
    {
        size_t delindex = 0;
        if (!forcedeletion)
        {
            bool hasdelcandidate = false;
            for (; delindex < objs.size(); delindex++) if (objs[delindex]->willbedeleted) { hasdelcandidate = true; break; }
            if (!hasdelcandidate) break;
        }

        deleteobject(objs.at(delindex));
        objs = getallobjectsflat();
    }
}

void Scene::Update(double delta)
{
    std::vector<GameObject *> objs = getallobjectsflat();

    for (GameObject *obj : objs) obj->Update(delta);

    world->update(delta);
    
    for (GameObject *obj : objs) obj->AfterUpdate();
    cleanupobjects(false);
}

void Scene::Render()
{
    if (!currcam) return;

    glm::mat4 proj = currcam->GetProjectionMatrix(Engine::GetWindowSize());
    glm::mat4 view = currcam->GetViewMatrix();
    Transform globt = currcam->GetGlobalTransform();
    
    std::vector<PointLightData> pointlightsdata;
    for (PointLight *light : pointlights) pointlightsdata.push_back(light->GetLightData());

    Engine::pointlightsssbo->SetBufferData(pointlightsdata.data(), pointlightsdata.size() * sizeof(PointLightData), GL_STREAM_DRAW);
    /*
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    */

    GameObjectRenderData data;
    data.proj = &proj;
    data.view = &view;
    data.camt = &globt;
    data.fog = &fog;
    data.pointlightsssbo = Engine::pointlightsssbo;
    data.pointlightscount = (uint32_t)(pointlights.size());
    data.ambientlight = &ambientlight;
    
    ForEachAllObjects([&](GameObject *obj) -> bool { obj->Render(&data); return true; });
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
    obj->willbedeleted = true;
}

// ============================================================================================================

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

// ============================================================================================================

void Scene::ForEachAllObjects(std::function<bool (GameObject *)> callback)
{
    for (std::pair<int32_t, std::unordered_set<GameObject *>> pair : objects)
    {
        for (GameObject *obj : pair.second) if (!callback(obj)) return;
    }
}

void Scene::ForEachAllOrders(std::function<bool (std::unordered_set<GameObject *>)> callback)
{
    for (std::pair<int32_t, std::unordered_set<GameObject *>> pair : objects) if (!callback(pair.second)) return;
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

bool Scene::Raycast(glm::vec3 start, glm::vec3 end, std::function<RaycastCallbackState (RaycastInfo)> callback, unsigned short collidewithmaskbits)
{
    SceneRaycastCallback c(this, callback);
    rp3d::Ray ray(Utils::glmvec3torp3dvec3(start), Utils::glmvec3torp3dvec3(end));

    world->raycast(ray, &c, collidewithmaskbits);

    return c.hit;
}