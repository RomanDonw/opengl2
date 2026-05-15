#include "ObjectFactory.hpp"

#include "engine/Scene.hpp"
#include "engine/objects/Camera/Camera.hpp"
#include "engine/objects/FreeplayCamera/FreeplayCamera.hpp"
#include "engine/objects/Entity/Entity.hpp"
#include "engine/objects/Light/DirectionalLight.hpp"
#include "engine/objects/Light/PointLight.hpp"
#include "engine/objects/Model/Model.hpp"
#include "engine/objects/Model/Surface.hpp"
#include "engine/objects/ScriptBehaviour/ScriptBehaviour.hpp"
#include "engine/physics/CollisionLayers.hpp"
#include "engine/physics/colliders/colliders.hpp"

GameObject *ObjectFactory::Create(Scene *scene, const std::string &typeName, const std::string &displayName)
{
    if (!scene) return nullptr;

    GameObject *obj = nullptr;

    if (typeName == "Entity")
    {
        Entity *e = scene->CreateObject<Entity>();
        e->usedShaderProgram = "default";
        e->color = glm::vec4(0.8f, 0.8f, 0.8f, 1);
        e->SetRigidBodyType(DYNAMIC);
        e->SetMass(1);
        BoxCollider *c = e->AddCollider<BoxCollider>(Transform(), glm::vec3(1));
        c->SetCollideWithMaskBits(CollisionLayer::All);
        Surface s;
        s.mesh = "cube";
        e->surfaces.push_back(s);
        obj = e;
    }
    else if (typeName == "Camera")
    {
        obj = scene->CreateObject<Camera>();
    }
    else if (typeName == "FreeplayCamera")
    {
        FreeplayCamera *fp = scene->CreateObject<FreeplayCamera>();
        fp->transform.SetPosition(glm::vec3(0, 2, 6));
        obj = fp;
    }
    else if (typeName == "PointLight")
    {
        obj = scene->CreateObject<PointLight>();
    }
    else if (typeName == "DirectionalLight")
    {
        obj = scene->CreateObject<DirectionalLight>();
    }
    else if (typeName == "Empty")
    {
        obj = scene->CreateObject<Model>();
        obj->editorVisible = false;
    }
    else
    {
        Model *m = scene->CreateObject<Model>();
        m->usedShaderProgram = "default";
        Surface s;
        s.mesh = "cube";
        m->surfaces.push_back(s);
        obj = m;
    }

    if (obj) obj->displayName = displayName;
    return obj;
}
