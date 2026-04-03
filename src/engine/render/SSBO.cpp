#include "SSBO.hpp"

SSBO::SSBO() { glGenBuffers(1, &buffer); }
SSBO::~SSBO() { glDeleteBuffers(1, &buffer); }

void SSBO::BindToSlot(GLuint slot) const { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, buffer); }

void SSBO::SetBufferData(void *data, size_t size, GLenum usage)
{
    if (currsize != size) { glNamedBufferData(buffer, size, data, usage); currsize = size; }
    else glNamedBufferSubData(buffer, 0, size, data);
}