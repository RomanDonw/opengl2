#include "SSBO.hpp"

SSBO::SSBO()
{
    glGenBuffers(1, &buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 0, NULL, GL_DYNAMIC_DRAW); // clear buffer.
}

SSBO::~SSBO() { glDeleteBuffers(1, &buffer); }

void SSBO::BindToSlot(GLuint slot) const { glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, slot, 1, &buffer); }

void SSBO::SetBufferData(const void *data, size_t size, GLenum usage) { glNamedBufferData(buffer, size, data, usage); }