#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <unordered_map>
#include <string>

class Engine;
class Mesh;
class Texture;
class ShaderProgram;
class Scene;

class ResourceManager final
{
    private:
        static inline std::unordered_map<std::string, Mesh *> meshes = std::unordered_map<std::string, Mesh *>();
        static inline std::unordered_map<std::string, Texture *> textures = std::unordered_map<std::string, Texture *>();
        static inline std::unordered_map<std::string, ShaderProgram *> shprogs = std::unordered_map<std::string, ShaderProgram *>();
        static inline std::unordered_map<std::string, Scene *> scenes = std::unordered_map<std::string, Scene *>();
        //static inline std::unordered_map<std::string, AudioClip *> clips = std::unordered_map<std::string, AudioClip *>();

        ResourceManager() = delete;
        ~ResourceManager() = delete;

    public:
        void DeleteAll();

        static bool HasMesh(std::string name);
        static Mesh *CreateMesh(std::string name); // can return nullptr.
        static Mesh *GetMesh(std::string name); // can return nullptr.
        static bool DeleteMesh(std::string name);

        static bool HasTexture(std::string name);
        static Texture *CreateTexture(std::string name); // can return nullptr.
        static Texture *GetTexture(std::string name); // can return nullptr.
        static bool DeleteTexture(std::string name);

        static bool HasShaderProgram(std::string name);
        static ShaderProgram *CreateShaderProgram(std::string name); // can return nullptr.
        static ShaderProgram *GetShaderProgram(std::string name); // can return nullptr.
        static bool DeleteShaderProgram(std::string name);

        static bool HasScene(std::string name);
        static Scene *CreateScene(std::string name); // can return nullptr.
        static Scene *GetScene(std::string name); // can return nullptr.
        static bool DeleteScene(std::string name);
};

#endif