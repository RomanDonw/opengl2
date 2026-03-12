#ifndef AUDIOCLIP_HPP
#define AUDIOCLIP_HPP

#include "../external/openal.hpp"

#include <string>
#include <vector>

class ResourceManager;
class AudioSource;

class AudioClip final
{
    friend class AudioSource;
    friend class ResourceManager;

    private:
        ALuint buffer;
        std::vector<AudioSource *> usedinsources;

        void addsrctovector(AudioSource *src);
        void delsrcfromvector(AudioSource *src);

        void updatebuff(ALenum type, const ALvoid *data, ALsizei size, ALsizei freq);

        AudioClip();
        ~AudioClip();

    public:
        bool LoadFromUCSOUNDFile(std::string filename);
};

#endif