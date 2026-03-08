#include "Scene.hpp"

#include "objects/GameObject/GameObject.hpp"
#include "objects/Camera/Camera.hpp"
#include "Engine.hpp"

// === PRIVATE ===

Scene::Scene() {}
Scene::~Scene()
{
    for (GameObject *obj : objects) DeleteObject(obj);
    if (Engine::GetScene(Engine::GetCurrentScene()) == this) Engine::SetCurrentScene("");
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

void Scene::OnSceneLoad() {}
void Scene::OnSceneUnload() {}

// === PUBLIC ===

Camera *Scene::GetCurrentCamera() { return currcam; }
void Scene::SetCurrentCamera(Camera *camera) { currcam = camera; }
/*{
    if (!camera) { currcam = nullptr; return; }

}*/