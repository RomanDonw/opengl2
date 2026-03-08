#include "ResourceManager.hpp"

#include "resources/Mesh.hpp"
#include "resources/Texture.hpp"
#include "resources/ShaderProgram.hpp"

// === PRIVATE ===

ResourceManager::~ResourceManager() { DeleteAll(); }

// === PUBLIC ===

void ResourceManager::DeleteAll()
{
    for (std::pair<std::string, Mesh *> pair : meshes) DeleteMesh(pair.first);
    for (std::pair<std::string, Texture *> pair : textures) DeleteTexture(pair.first);
    for (std::pair<std::string, ShaderProgram *> pair : shprogs) DeleteShaderProgram(pair.first);
}

// === Mesh ===

bool ResourceManager::HasMesh(std::string name) { return meshes.contains(name) && name.length() != 0; }

Mesh *ResourceManager::CreateMesh(std::string name)
{
    if (HasMesh(name) || name.length() == 0) return nullptr;

    Mesh *ret = new Mesh();
    meshes.insert({name, ret});
    return ret;
}

Mesh *ResourceManager::GetMesh(std::string name)
{
    if (!HasMesh(name) || name.length() == 0) return nullptr;
    return meshes.at(name);
}

bool ResourceManager::DeleteMesh(std::string name)
{
    if (!HasMesh(name) || name.length() == 0) return false;

    delete GetMesh(name);
    meshes.erase(name);
    return true;
}

// === Texture ===

bool ResourceManager::HasTexture(std::string name) { return textures.contains(name) && name.length() != 0; }

Texture *ResourceManager::CreateTexture(std::string name)
{
    if (HasTexture(name) || name.length() == 0) return nullptr;

    Texture *ret = new Texture();
    textures.insert({name, ret});
    return ret;
}

Texture *ResourceManager::GetTexture(std::string name)
{
    if (!HasTexture(name) || name.length() == 0) return nullptr;
    return textures.at(name);
}

bool ResourceManager::DeleteTexture(std::string name)
{
    if (!HasTexture(name) || name.length() == 0) return false;

    delete GetTexture(name);
    textures.erase(name);
    return true;
}

// === ShaderProgram ===

bool ResourceManager::HasShaderProgram(std::string name) { return shprogs.contains(name) && name.length() != 0; }

ShaderProgram *ResourceManager::CreateShaderProgram(std::string name)
{
    if (HasShaderProgram(name) || name.length() == 0) return nullptr;

    ShaderProgram *ret = new ShaderProgram();
    shprogs.insert({name, ret});
    return ret;
}

ShaderProgram *ResourceManager::GetShaderProgram(std::string name)
{
    if (!HasShaderProgram(name) || name.length() == 0) return nullptr;
    return shprogs.at(name);
}

bool ResourceManager::DeleteShaderProgram(std::string name)
{
    if (!HasShaderProgram(name) || name.length() == 0) return false;

    delete GetShaderProgram(name);
    shprogs.erase(name);
    return true;
}