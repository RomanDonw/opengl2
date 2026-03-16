#ifndef AUDIOSOURCE_HPP
#define AUDIOSOURCE_HPP

#include "../../external/openal.hpp"

#include "../GameObject/GameObject.hpp"
#include "../../Transform.hpp"

class Scene;
class AudioClip;
class AudioEffectSlot;
class TemporaryAudioSource;

enum
{
    UNDEFINED = 0,
    INIT = 1,
    PLAYING = 2,
    PAUSED = 3,
    STOPPED = 4
} typedef AudioSourceState;

class AudioSource : public GameObject
{
    friend class Scene;
    friend class AudioClip;
    friend class AudioEffectSlot;
    friend class TemporaryAudioSource;

    private:
        ALuint source;
        bool looped;
        bool locked = false;
        bool freezed = false;
        
        AudioClip *currclip = nullptr;
        AudioEffectSlot *attachedslot = nullptr;

        void constructor();
        void updatesrcpos();
        void rewind();

        AudioSource(Scene *s, Transform t);
        AudioSource(Scene *s);
        ~AudioSource() override;

        void OnGlobalTransformChanged() override;
        void OnSceneLoad() override;
        void OnSceneUnload() override;

    public:
        const GameObjectType type = GameObjectType::AUDIOSOURCE;

        void SetSourceFloat(ALenum option, float value);

        bool IsLooped();
        void SetLooping(bool loop);

        AudioSourceState GetState();
        AudioEffectSlot *GetAttachedSlot();

        AudioClip *GetCurrentClip();
        void SetCurrentClip(AudioClip *clip);

        void Play();
        void Pause();
        void Stop();
        void Rewind();
};

#endif