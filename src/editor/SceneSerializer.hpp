#ifndef SCENESERIALIZER_HPP
#define SCENESERIALIZER_HPP

#include <string>

class Scene;
class EditorProject;

class SceneSerializer final
{
    public:
        static bool Save(Scene *scene, const std::string &path, const EditorProject *project = nullptr);
        static bool Load(Scene *scene, const std::string &path, const EditorProject *project = nullptr);
        static bool CreateDefaultScene(Scene *scene, const EditorProject *project = nullptr);
};

#endif
