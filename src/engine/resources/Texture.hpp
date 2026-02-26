#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "../external/opengl.hpp"

#include <string>

class ResourceManager;

class Texture final
{
    friend class ResourceManager;

    private:
        bool hasTexture = false;
        GLuint texture;

        Texture();
        ~Texture();

    public:
        bool HasTexture();
        bool BindTexture();

        bool DeleteTexture();

        bool LoadFromUCTEXFile(std::string filename);

        bool SetTextureIntParameter(GLenum param, GLint value);
        void SetDefaultParametres();
        void SetLinearSmoothing();
};

#endif