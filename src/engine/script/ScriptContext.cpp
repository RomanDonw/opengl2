#include "ScriptContext.hpp"

#include "../Scene.hpp"
#include "../objects/GameObject/GameObject.hpp"

GameObject *ScriptContext::ResolveObject(const std::string &name) const
{
    if (name == "self" && self) return self;
    if (variables.contains(name)) return variables.at(name);

    if (!scene) return nullptr;

    GameObject *found = nullptr;
    scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (obj->displayName == name) { found = obj; return false; }
        return true;
    });
    return found;
}
