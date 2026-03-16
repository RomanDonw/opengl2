#include "TemporaryAudioSource.hpp"

#include "../../Scene.hpp"

// === PRIVATE ===

TemporaryAudioSource::TemporaryAudioSource(Scene *s, Transform t) : AudioSource(s, t) {}
TemporaryAudioSource::TemporaryAudioSource(Scene *s) : AudioSource(s) {}

TemporaryAudioSource::~TemporaryAudioSource() {}

void TemporaryAudioSource::Update(double delta)
{
    AudioSource::Update(delta);

    if (GetState() == AudioSourceState::STOPPED) scene->DeleteObject(this);
}