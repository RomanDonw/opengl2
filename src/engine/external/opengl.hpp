#ifndef OPENGL_HPP
#define OPENGL_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define INITOPENGL_SUCCESS 0
#define INITOPENGL_ERROR 1

int initOpenGL(GLFWwindow **window, unsigned short windowWidth, unsigned short windowHeight);

#endif