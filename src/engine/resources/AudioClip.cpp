#include "AudioClip.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <algorithm>
#include <iterator>

#include <sndfile.h>

#include "../objects/AudioSource/AudioSource.hpp"

// === PRIVATE ===

void AudioClip::addsrctovector(AudioSource *src) { usedinsources.push_back(src); }
void AudioClip::delsrcfromvector(AudioSource *src) { usedinsources.erase(std::remove(usedinsources.begin(), usedinsources.end(), src), usedinsources.end()); }

void AudioClip::updatebuff(ALenum type, const ALvoid *data, ALsizei size, ALsizei freq)
{
    for (AudioSource *src : usedinsources) src->rewind();
    alBufferData(buffer, type, data, size, freq);
}

AudioClip::AudioClip() { alGenBuffers(1, &buffer); }
AudioClip::~AudioClip()
{
    for (AudioSource *src : usedinsources) src->SetCurrentClip(nullptr);

    alDeleteBuffers(1, &buffer);
}

// === PUBLIC ===

bool AudioClip::LoadFromAudioFile(std::string filename)
{
    if (!std::filesystem::is_regular_file(filename)) return false;

    SF_INFO info;
    SNDFILE *sf = sf_open(filename.c_str(), SFM_READ, &info);
    if (!sf) return false;

    ALenum format;
    if (info.channels == 1) format = AL_FORMAT_MONO16;
    else if (info.channels == 2) format = AL_FORMAT_STEREO16;
    else { sf_close(sf); return false; }

    std::vector<short> data = std::vector<short>(info.frames * info.channels);
    sf_read_short(sf, data.data(), data.size());
    sf_close(sf);

    updatebuff(format, data.data(), data.size() * sizeof(short), info.samplerate);

    return true;
}