#include "AudioDevice.hpp"

#include <exception>
#include <stdexcept>
#include <string>
#include <sstream>

#include "../Engine.hpp"

AudioDevice::AudioDevice(const ALchar *devname)
{
    device = alcOpenDevice(devname);
    if (!device)
    {
        std::ostringstream oss;
        oss << "failed to open device \"" << devname << "\"";
        throw std::runtime_error(oss.str());
    }

    context = alcCreateContext(device, NULL);
    if (!context)
    {
        alcCloseDevice(device);

        std::ostringstream oss;
        oss << "failed to create context of device \"" << devname << "\"";
        throw std::runtime_error(oss.str());
    }
}

AudioDevice::~AudioDevice()
{
    if (Engine::GetCurrentAudioDevice() == this) Engine::SetCurrentAudioDevice(nullptr);

    alcDestroyContext(context);
    alcCloseDevice(device);
}