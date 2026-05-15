#ifndef RENDERTARGET_HPP
#define RENDERTARGET_HPP

#include "../external/glm.hpp"
#include "../external/opengl.hpp"

class RenderTarget final
{
    private:
        GLuint fbo = 0;
        GLuint colorTex = 0;
        GLuint depthRbo = 0;
        glm::uvec2 size = glm::uvec2(0);

    public:
        ~RenderTarget();
        bool Resize(unsigned int width, unsigned int height);
        void Bind();
        void Unbind();
        glm::uvec2 GetSize() const;
        GLuint GetColorTexture() const;
};

#endif
