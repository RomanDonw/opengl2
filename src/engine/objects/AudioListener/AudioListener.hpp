#ifndef AUDIOLISTENER_HPP
#define AUDIOLISTENER_HPP

#include "../../external/openal.hpp"

#include "../GameObject/GameObject.hpp"
#include "../../Transform.hpp"

class Scene;

class AudioListener final : public GameObject
{
    friend class Scene;

    private:
        bool freezed = false;

        void constructor();
        void updatelstpos();

        AudioListener(Scene *s, Transform t);
        AudioListener(Scene *s);
        ~AudioListener() override;

        void OnGlobalTransformChanged() override;
        void OnSceneLoad() override;
        void OnSceneUnload() override;

    public:
        const GameObjectType type = GameObjectType::AUDIOLISTENER;
};

#endif