#ifndef AUDIOEFFECTSLOT_HPP
#define AUDIOEFFECTSLOT_HPP

#include "../external/openal.hpp"

#include <vector>

#include "AudioEffectProperties.hpp"

class AudioSource;

class AudioEffectSlot
{
    friend class AudioSource;

    private:
        ALuint slot;
        
        std::vector<AudioSource *> attachedsources = std::vector<AudioSource *>();

    public:
        AudioEffectSlot();
        ~AudioEffectSlot();

        void ApplyEffect(AudioEffectProperties effect);

        void SetSlotGain(float gain);

        bool HasAttachedSource(AudioSource *source);
        std::vector<AudioSource *> GetAttachedSources();
        bool AddSource(AudioSource *source);
        bool RemoveSource(AudioSource *source);
};

#endif