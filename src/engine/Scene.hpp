#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <concepts>

class Engine;
class ResourceManager;
class GameObject;
class Camera;

class Scene final
{
    friend class ResourceManager;
    friend class Engine;

    private:
        Scene();
        ~Scene();

        std::vector<GameObject *> objects;
        Camera *currcam = nullptr;

        void Render();
        void Update(double delta);

    public:
        template<std::derived_from<GameObject> T>
        bool HasObject(T *obj);

        template<std::derived_from<GameObject> T, typename... Args>
        T *CreateObject(Args&&... args);

        template<std::derived_from<GameObject> T>
        void DeleteObject(T *obj);

        Camera *GetCurrentCamera(); // can return nullptr.
        void SetCurrentCamera(Camera *cam);
};

#endif