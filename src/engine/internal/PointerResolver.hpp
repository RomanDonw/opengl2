#ifndef POINTERRESOLVER_HPP
#define POINTERRESOLVER_HPP

/*
    this class is will be used to resolve pointer/API links between
    instances of different classes, such as AudioClip & AudioSource (for ex. to sync deleted AudioSource/AudioClip).
*/

#include <unordered_map>
#include <vector>

class Engine;

class AudioClip;
class AudioSource;
class AudioEffectSlot;

class PointerResolver final
{
    friend class Engine;

    friend class AudioClip;
    friend class AudioSource;
    friend class AudioEffectSlot;

    private:
        static inline std::unordered_map<AudioClip, std::vector<AudioSource>> clipslinks;
};

#endif