#include "opengl.hpp"

#include <iostream>

int initOpenGL(GLFWwindow **window, unsigned short windowWidth, unsigned short windowHeight)
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW." << std::endl;
        return INITOPENGL_ERROR;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /*GLFWmonitor *m = glfwGetPrimaryMonitor();
    if (!m)
    {
        std::cout << "Can't find primary monitor. Program halted with error exit code." << std::endl;
        glfwTerminate();
        return 1;
    }*/

    //int width, height;
    //glfwGetMonitorWorkarea(m, NULL, NULL, &width, &height);

    GLFWwindow *w = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", NULL/*m*/, NULL);
    if (!window)
    {
        std::cout << "Can't initialize GLFW window. Program halted with error exit code." << std::endl;
        glfwTerminate();
        return INITOPENGL_ERROR;
    }
    glfwMakeContextCurrent(w);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD. Program halted with error exit code." << std::endl;

        glfwMakeContextCurrent(NULL);
        glfwTerminate();
        return INITOPENGL_ERROR;
    }

    *window = w;
    
    glViewport(0, 0, windowWidth, windowHeight);

    return INITOPENGL_SUCCESS;
}