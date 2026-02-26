#include "ResourceManager.hpp"

#include "resources/Mesh.hpp"
#include "resources/Texture.hpp"
#include "resources/ShaderProgram.hpp"

void ResourceManager::DeleteAll()
{
    
}

bool ResourceManager::HasMesh(std::string name) { return meshes.contains(name); }
Mesh *ResourceManager::CreateMesh(std::string name)
{
    if (HasMesh(name)) return nullptr;

    Mesh *ret = new Mesh();
    meshes.insert({name, ret});
    return ret;
}
Mesh *ResourceManager::GetMesh(std::string name)
{
    if (!HasMesh(name)) return nullptr;
    return meshes.at(name);
}
bool ResourceManager::DeleteMesh(std::string name)
{
    if (!HasMesh(name)) return false;
    meshes.erase(name);
    return true;
}

bool ResourceManager::HasTexture(std::string name) { return textures.contains(name); }
Texture *ResourceManager::CreateTexture(std::string name)
{
    if (HasTexture(name)) return nullptr;

    Texture *ret = new Texture();
    textures.insert({name, ret});
    return ret;
}
Texture *ResourceManager::GetTexture(std::string name)
{
    if (!HasTexture(name)) return nullptr;
    return textures.at(name);
}
bool ResourceManager::DeleteTexture(std::string name)
{
    if (!HasTexture(name)) return false;
    textures.erase(name);
    return true;
}

bool ResourceManager::HasShaderProgram(std::string name);
ShaderProgram *ResourceManager::CreateShaderProgram(std::string name); // can return nullptr.
ShaderProgram *ResourceManager::GetShaderProgram(std::string name); // can return nullptr.
bool ResourceManager::DeleteShaderProgram(std::string name);

bool ResourceManager::HasScene(std::string name);
Scene *ResourceManager::CreateScene(std::string name); // can return nullptr.
Scene *ResourceManager::GetScene(std::string name); // can return nullptr.
bool ResourceManager::DeleteScene(std::string name);