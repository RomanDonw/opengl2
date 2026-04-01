#include "SSBO.hpp"

SSBO::SSBO() { glGenBuffers(1, &buffer); }
SSBO::~SSBO() { glDeleteBuffers(1, &buffer); }

void SSBO::BindToSlot(GLuint slot) const { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, buffer); }

void SSBO::SetBufferData(void *data, size_t size, GLenum usage)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}