#ifndef AUDIODEVICE_HPP
#define AUDIODEVICE_HPP

#include "../external/openal.hpp"

class Engine;

class AudioDevice
{
    friend class Engine;

    private:
        ALCdevice *device;
        ALCcontext *context;

    public:
        AudioDevice(const ALchar *devname); // devname can be NULL/nullptr.
        ~AudioDevice();
};

#endif