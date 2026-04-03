#ifndef SSBO_HPP
#define SSBO_HPP

#include "../external/opengl.hpp"

class SSBO final
{
    private:
        GLuint buffer;
        size_t currsize;

    public:
        SSBO();
        ~SSBO();

        void SetBufferData(void *data, size_t size, GLenum usage);

        void BindToSlot(GLuint slot) const;
};

#endif