#include <iostream>

#include "opengl.hpp"
#include "openal.hpp"
#include "glm.hpp"

int main()
{
    GLFWwindow *w;
    {
        int ret = initOpenGL(&w, 1200, 700);
        if (ret != 0) return ret;
    }

    glfwTerminate();

    return 0;
}

