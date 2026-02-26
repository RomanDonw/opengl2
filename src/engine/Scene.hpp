#ifndef SCENE_HPP
#define SCENE_HPP

class ResourceManager;

class Scene final
{
    friend class ResourceManager;

    private:
        Scene();
        ~Scene();

    public:
        void Render();
        void Update(double delta);
};

#endif