#include "Button.hpp"

#include "engine/objects/TemporaryAudioSource/TemporaryAudioSource.hpp"
#include "engine/Scene.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/physics/colliders/BoxCollider/BoxCollider.hpp"

// === PRIVATE ===

void Button::constructor()
{
    surfaces.push_back(Surface());
    
    BoxCollider *coll = AddCollider<BoxCollider>(Transform(), glm::vec3(0.125, 0.2, 0.125));
    coll->SetIsTrigger(true);
}

Button::Button(Scene *s, Transform t) : GameObject(s, t), Entity(s, t) { constructor(); }
Button::Button(Scene *s) : GameObject(s), Entity(s) { constructor(); }

Button::~Button() {}

// === PUBLIC ===

bool Button::GetButtonState() const { return enabled; }

void Button::SetButtonState(bool state, bool playsfx)
{
    enabled = state;

    surfaces[0].mesh = model;
    surfaces[0].texture = state ? textureon : textureoff;

    if (playsfx)
    {
        TemporaryAudioSource *src = GetScene()->CreateObject<TemporaryAudioSource>();
        src->SetParent(this, false);
        src->SetSourceFloat(AL_GAIN, 0.7);
        src->SetSourceFloat(AL_REFERENCE_DISTANCE, 2);
        src->SetSourceFloat(AL_MAX_DISTANCE, 8);
        src->SetCurrentClip(ResourceManager::GetAudioClip(togglesfx));
        src->Play();
    }

    if (toggleCallback) toggleCallback(this, state);
}

void Button::ToggleButtonState(bool playsfx) { SetButtonState(!GetButtonState(), playsfx); }