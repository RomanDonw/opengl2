#include "MaxwellCat.hpp"

#include "engine/Scene.hpp"
#include "engine/objects/AudioSource/AudioSource.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/objects/Model/Model.hpp"

#include "engine/physics/colliders/SphereCollider/SphereCollider.hpp"
#include "engine/physics/colliders/BoxCollider/BoxCollider.hpp"

// === PRIVATE ===

void MaxwellCat::constructor()
{
    Scene *scene = GetScene();
    tags.insert("Maxwell the Cat");

    SetRigidBodyType(DYNAMIC);
    SetMass(4.5);
    //AddCollider<SphereCollider>(Transform(), 0.3);
    //SetAngularLockAxisFactor({0, 1, 0});
    AddCollider<BoxCollider>(Transform(), glm::vec3(0.3, 0.2, 0.3));

    model = scene->CreateObject<Model>();
    model->SetParent(this, false);
    model->transform.SetScale(glm::vec3(0.03));
    {
        model->usedShaderProgram = "default";
        Surface surf;

        surf.texture = "maxwellcat";
        surf.mesh = "maxwellcat";
        model->surfaces.push_back(surf);
    }

    src = scene->CreateObject<AudioSource>();
    src->SetParent(this, false);

    src->SetSourceFloat(AL_GAIN, 0.3);
    src->SetSourceFloat(AL_REFERENCE_DISTANCE, 0);
    src->SetSourceFloat(AL_MAX_DISTANCE, 16);
    src->SetLooping(true);
    src->SetCurrentClip(ResourceManager::GetAudioClip("mus_maxwellcat"));
}

MaxwellCat::MaxwellCat(Scene *s, Transform t) : GameObject(s, t), RigidBody(s, t) { constructor(); }
MaxwellCat::MaxwellCat(Scene *s) : GameObject(s), RigidBody(s) { constructor(); }

MaxwellCat::~MaxwellCat()
{
    GetScene()->DeleteObject(src);
    GetScene()->DeleteObject(model);

    for (GameObject *obj : GetChildren()) if (obj->tags.contains("Decal")) GetScene()->DeleteObject(obj);
}

void MaxwellCat::Update(double delta)
{
    if (transform.GetPosition().y < -100) { GetScene()->DeleteObject(this); return; }

    if (src->GetState() == AudioSourceState::INIT) src->Play();

    model->transform.Rotate(glm::radians(glm::vec3(0.0f, 360.0f, 0.0f)) * glm::vec3(delta));
}