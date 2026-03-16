#include "AudioEffectSlot.hpp"

#include <algorithm>

#include "AudioEffectProperties.hpp"
#include "../objects/AudioSource/AudioSource.hpp"

AudioEffectSlot::AudioEffectSlot()
{
    alGenAuxiliaryEffectSlots(1, &slot);
}

AudioEffectSlot::~AudioEffectSlot()
{
    for (AudioSource *src : attachedsources) RemoveSource(src);
    alDeleteAuxiliaryEffectSlots(1, &slot);
}

void AudioEffectSlot::ApplyEffect(AudioEffectProperties effect) { alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect.effect); }

void AudioEffectSlot::SetSlotGain(float gain) { alAuxiliaryEffectSlotf(slot, AL_EFFECTSLOT_GAIN, gain); }

bool AudioEffectSlot::HasAttachedSource(AudioSource *source) { return std::ranges::contains(attachedsources, source); }
std::vector<AudioSource *> AudioEffectSlot::GetAttachedSources() { return attachedsources; }
bool AudioEffectSlot::AddSource(AudioSource *source)
{
    if (HasAttachedSource(source)) return false;

    source->attachedslot = this;
    attachedsources.push_back(source);
    alSource3i(source->source, AL_AUXILIARY_SEND_FILTER, slot, 0, AL_FILTER_NULL);

    return true;
}
bool AudioEffectSlot::RemoveSource(AudioSource *source)
{
    if (!HasAttachedSource(source)) return false;

    source->attachedslot = nullptr;
    attachedsources.erase(std::remove(attachedsources.begin(), attachedsources.end(), source), attachedsources.end());
    alSource3i(source->source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);

    return true;
}