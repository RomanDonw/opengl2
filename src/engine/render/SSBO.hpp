#ifndef SSBO_HPP
#define SSBO_HPP

#include "../external/opengl.hpp"

class SSBO final
{
    private:
        GLuint buffer;

    public:
        SSBO();
        ~SSBO();

        void SetBufferData(const void *data, size_t size, GLenum usage);

        void BindToSlot(GLuint slot) const;
};

#endif