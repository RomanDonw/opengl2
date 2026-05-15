#ifndef SCRIPTCONTEXT_HPP
#define SCRIPTCONTEXT_HPP

#include "../external/glm.hpp"

#include <string>
#include <unordered_map>

class Scene;
class GameObject;
class ScriptBehaviour;

class ScriptContext
{
    public:
        Scene *scene = nullptr;
        GameObject *self = nullptr;
        ScriptBehaviour *behaviour = nullptr;
        float deltaTime = 0.0f;

        std::unordered_map<std::string, GameObject *> variables;

        GameObject *ResolveObject(const std::string &name) const;
};

#endif
