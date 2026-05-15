#ifndef OBJECTFACTORY_HPP
#define OBJECTFACTORY_HPP

#include <string>

class Scene;
class GameObject;

class ObjectFactory final
{
    public:
        static GameObject *Create(Scene *scene, const std::string &typeName, const std::string &displayName);
};

#endif
