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

    #if 0
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
    #endif

    return true;
}

#if 0
bool AudioClip::LoadFromUCSOUNDFile(std::string filename)
{
    if (!std::filesystem::is_regular_file(filename)) return false;

    FILE *f = fopen(filename.c_str(), "rb");
    if (!f) return false;

    char sig[7];
    fread(&sig, sizeof(char), 7, f);
    if (feof(f) || strncmp(sig, "UCSOUND", 7)) { fclose(f); return false; }

    uint16_t version;
    fread(&version, sizeof(version), 1, f);
    if (feof(f) || version != 0) { fclose(f); return false; }

    uint8_t type;
    fread(&type, sizeof(type), 1, f);
    if (feof(f)) { fclose(f); return false; }

    ALenum altype;
    switch (type)
    {
        case 0: // mono 8 bit/sample (unsigned 8-bit).
            altype = AL_FORMAT_MONO8;
            break;

        case 1: // mono 16 bit/sample (signed 16-bit).
            altype = AL_FORMAT_MONO16;
            break;

        case 2: // stereo 8 bit/sample (unsigned 8-bit).
            altype = AL_FORMAT_STEREO8;
            break;

        case 3: // stereo 16 bit/sample (signed 16-bit).
            altype = AL_FORMAT_STEREO16;
            break;

        default:
            fclose(f);
            return false;
    }

    uint16_t frequency;
    fread(&frequency, sizeof(frequency), 1, f);
    if (feof(f)) { fclose(f); return false; }

    std::vector<uint8_t> data = std::vector<uint8_t>();
    while (true)
    {
        uint8_t byte;
        fread(&byte, sizeof(byte), 1, f);
        if (feof(f)) break;

        data.push_back(byte);
    }

    updatebuff(altype, data.data(), data.size(), frequency);

    fclose(f);
    return true;
}
#endif