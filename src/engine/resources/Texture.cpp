#include "Texture.hpp"

#include <vector>
#include <filesystem>
#include <cstring>
#include <iterator>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// === PRIVATE ===

Texture::Texture() {}
Texture::~Texture() { DeleteTexture(); }

// === PUBLIC ===

bool Texture::HasTexture() { return hasTexture; }
bool Texture::BindTexture()
{
    if (!HasTexture()) return false;

    glBindTexture(GL_TEXTURE_2D, texture);

    return true;
}

bool Texture::DeleteTexture()
{
    if (!HasTexture()) return false;

    glDeleteTextures(1, &texture);
    hasTexture = false;

    return true;
}

bool Texture::LoadFromTextureFile(std::string filename)
{
    if (!std::filesystem::is_regular_file(filename)) return false;

    int width, height, channels;
    void *data = stbi_load(filename.c_str(), &width, &height, &channels, 4); // 4 = RGBA only;
    if (!data) return false;

    DeleteTexture();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    hasTexture = true;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    return true;
}

bool Texture::SetTextureIntParameter(GLenum param, GLint value)
{
    if (!HasTexture()) return false;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, param, value);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}