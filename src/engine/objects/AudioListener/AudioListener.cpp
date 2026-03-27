#include "AudioListener.hpp"

#include "../../external/glm.hpp"

#include <exception>
#include <stdexcept>

#include "../../Engine.hpp"
#include "../../Scene.hpp"

struct
{
    glm::vec3 front;
    glm::vec3 up;
} typedef ALListenerOrientation;

void AudioListener::constructor()
{
    Scene *scene = GetScene();
    if (scene->hasAudioListener) throw std::runtime_error("one scene can have only one AudioListener class instance");
    scene->hasAudioListener = true;

    if (Engine::GetScene(Engine::GetCurrentScene()) != scene) freezed = true;
    updatelstpos();
}

void AudioListener::updatelstpos()
{
    if (freezed) return;

    Transform globt = GetGlobalTransform();
    alListenerfv(AL_POSITION, glm::value_ptr(globt.GetPosition()));

    ALListenerOrientation orient;
    orient.front = globt.GetFront();
    orient.up = globt.GetUp();
    alListenerfv(AL_ORIENTATION, (ALfloat *)&orient);
}

AudioListener::AudioListener(Scene *s, Transform t) : GameObject(s, t) { constructor(); }
AudioListener::AudioListener(Scene *s) : GameObject(s) { constructor(); }

AudioListener::~AudioListener() { GetScene()->hasAudioListener = false; }

void AudioListener::OnGlobalTransformChanged()
{
    GameObject::OnGlobalTransformChanged();

    updatelstpos();
}

void AudioListener::OnSceneLoad()
{
    freezed = false;
    updatelstpos();
}

void AudioListener::OnSceneUnload() { freezed = true; }