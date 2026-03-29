#ifndef TEMPORARYAUDIOSOURCE_HPP
#define TEMPORARYAUDIOSOURCE_HPP

#include "../AudioSource/AudioSource.hpp"

class Scene;

class TemporaryAudioSource final : public AudioSource
{
    friend class Scene;

    private:
        TemporaryAudioSource(Scene *s, Transform t);
        TemporaryAudioSource(Scene *s);

        ~TemporaryAudioSource() override;

        void Update(double delta) override;
};

#endif