#include "AudioSource.hpp"

#include "../../resources/AudioClip.hpp"
#include "../../Engine.hpp"
#include "../../audio/AudioEffectSlot.hpp"

// === PRIVATE ===

void AudioSource::constructor()
{
    alGenSources(1, &source);

    alSourcei(source, AL_BUFFER, 0);
    SetLooping(false);

    if (Engine::GetScene(Engine::GetCurrentScene()) != GetScene()) locked = true;
}

void AudioSource::updatesrcpos()
{
    Transform globt = GetGlobalTransform();
    alSourcefv(source, AL_POSITION, glm::value_ptr(globt.GetPosition()));
}

void AudioSource::rewind() { alSourceRewind(source); }

// ========================================================================================================================

AudioSource::AudioSource(Scene *s, Transform t) : GameObject(s, t) { constructor(); }
AudioSource::AudioSource(Scene *s) : GameObject(s) { constructor(); }

AudioSource::~AudioSource()
{
    SetCurrentClip(nullptr);
    if (attachedslot) attachedslot->RemoveSource(this);

    alDeleteSources(1, &source);
}

// ========================================================================================================================

void AudioSource::OnGlobalTransformChanged()
{
    GameObject::OnGlobalTransformChanged();

    updatesrcpos();
}

void AudioSource::OnSceneLoad()
{
    GameObject::OnSceneLoad();

    locked = false;
    if (freezed) { Play(); freezed = false; }
}

void AudioSource::OnSceneUnload()
{
    GameObject::OnSceneUnload();

    if (GetState() == PLAYING) { Pause(); freezed = true; }
    locked = true;
}

// ========================================================================================================================

// === PUBLIC ===

AudioEffectSlot *AudioSource::GetAttachedSlot() { return attachedslot; }

AudioClip *AudioSource::GetCurrentClip() { return currclip; }

void AudioSource::SetCurrentClip(AudioClip *clip)
{
    if (clip == currclip) return;

    if (currclip) currclip->delsrcfromvector(this);
    if (clip)
    {
        clip->addsrctovector(this);
        alSourcei(source, AL_BUFFER, clip->buffer);
    }
    else alSourcei(source, AL_BUFFER, 0);

    currclip = clip;
}

void AudioSource::SetSourceFloat(ALenum option, float value) { alSourcef(source, option, value); }

AudioSourceState AudioSource::GetState()
{
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);

    switch (state)
    {
        case AL_INITIAL:
            return INIT;

        case AL_PLAYING:
            return PLAYING;

        case AL_PAUSED:
            return PAUSED;

        case AL_STOPPED:
            return STOPPED;
    }
    return UNDEFINED;
}

bool AudioSource::IsLooped() { return looped; }
void AudioSource::SetLooping(bool loop)
{
    looped = loop;
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void AudioSource::Play()
{
    if (locked) return;
    updatesrcpos();
    alSourcePlay(source);
}
void AudioSource::Stop() { if (locked) return; alSourceStop(source); }
void AudioSource::Pause() { if (locked) return; alSourcePause(source); }
void AudioSource::Rewind() { if (locked) return; rewind(); }