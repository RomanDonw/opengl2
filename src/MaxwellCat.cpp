#include "MaxwellCat.hpp"

#include "engine/Scene.hpp"
#include "engine/objects/AudioSource/AudioSource.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/objects/Model/Model.hpp"

#include "engine/physics/colliders/SphereCollider/SphereCollider.hpp"
#include "engine/physics/colliders/BoxCollider/BoxCollider.hpp"

// === PRIVATE ===

MaxwellCat::MaxwellCat(Scene *s, Transform t) : GameObject(s, t), RigidBody(s, t) {}
MaxwellCat::MaxwellCat(Scene *s) : GameObject(s), RigidBody(s) {}

MaxwellCat::~MaxwellCat()
{
    Scene *scene = GetScene();
    for (GameObject *obj : GetChildren()) if (obj->tags.contains("Decal")) scene->DeleteObject(obj);
}

void MaxwellCat::AfterCreation()
{
    Scene *scene = GetScene();
    scene->SetObjectOrder(this, 64);

    SetRigidBodyType(DYNAMIC);
    SetMass(4.5);
    BoxCollider *coll = AddCollider<BoxCollider>(Transform({0, 0.2, 0}), glm::vec3(0.3, 0.4, 0.3));
    coll->SetBounciness(0.75);

    model = scene->CreateObject<Model>();
    AddShadowChild(model, false);
    model->transform.SetScale(glm::vec3(0.03));
    {
        model->usedShaderProgram = "default";
        Surface surf;

        surf.texture = "maxwellcat";
        surf.mesh = "maxwellcat";
        model->surfaces.push_back(surf);
    }

    src = scene->CreateObject<AudioSource>();
    AddShadowChild(src, false);

    src->SetSourceFloat(AL_GAIN, 0.3);
    src->SetSourceFloat(AL_REFERENCE_DISTANCE, 0);
    src->SetSourceFloat(AL_MAX_DISTANCE, 16);
    src->SetLooping(true);
    src->SetCurrentClip(ResourceManager::GetAudioClip("mus_maxwellcat"));
}

void MaxwellCat::Update(double delta)
{
    if (transform.GetPosition().y < -100) { GetScene()->DeleteObject(this); return; }

    if (src->GetState() == AudioSourceState::INIT) src->Play();

    model->transform.Rotate(glm::radians(glm::vec3(0.0f, 360.0f, 0.0f)) * glm::vec3(delta));
}