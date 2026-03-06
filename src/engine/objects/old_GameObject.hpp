#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

class Scene;

class GameObject
{
    friend class Scene;

    private:
        virtual void OnLocalTransformChanged();
        virtual void OnParentTransformChanged();

        virtual void On

    public:

};

#endif