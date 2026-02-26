#include <iostream>

#include "opengl.hpp"
#include "openal.hpp"
#include "glm.hpp"

const unsigned short FPS = 60;

int main()
{
    if (!Engine::Init(1200, 700)) return 1;

    GLFWwindow *window = Engine::GetWindow();

    glfwSetTime(0);
    double prev_time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        double delta = glfwGetTime() - prev_time;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        if (delta >= 1.0 / FPS)
        {
            prev_time = glfwGetTime();

            glfwSwapBuffers(window);
        }

        glfwPollEvents();
    }

    Engine::Shutdown();

    return 0;
}

