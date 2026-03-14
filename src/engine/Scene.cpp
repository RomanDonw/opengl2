#include "Scene.hpp"

#include "objects/GameObject/GameObject.hpp"
#include "objects/Camera/Camera.hpp"
#include "Engine.hpp"

// === PRIVATE ===

Scene::Scene() { objects.insert({0, std::vector<GameObject *>()}); }
Scene::~Scene()
{
    ForEachAllObjects([&](GameObject *obj) -> bool { DeleteObject(obj); return true; });

    if (Engine::GetScene(Engine::GetCurrentScene()) == this) Engine::SetCurrentScene("");
}

void Scene::Update(double delta) { ForEachAllObjects([&](GameObject *obj) -> bool { obj->Update(delta); return true; }); }
void Scene::Render()
{
    if (!currcam) return;

    glm::mat4 proj = currcam->GetProjectionMatrix(Engine::GetWindowSize());
    glm::mat4 view = currcam->GetViewMatrix();

    ForEachAllOrders([&](std::vector<GameObject *> layer) -> bool
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        for (GameObject *obj : layer) obj->Render(&proj, &view, &currcam->transform, &fog);
        return true;
    });
}

void Scene::OnSceneLoad() { ForEachAllObjects([&](GameObject *obj) -> bool { obj->OnSceneLoad(); return true; }); }
void Scene::OnSceneUnload() { ForEachAllObjects([&](GameObject *obj) -> bool { obj->OnSceneUnload(); return true; }); }

// === PUBLIC ===

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

Camera *Scene::GetCurrentCamera() { return currcam; }
void Scene::SetCurrentCamera(Camera *camera)
{
    if (!HasObject(camera)) throw std::runtime_error("this scene doesn't have this object");
    currcam = camera;
}