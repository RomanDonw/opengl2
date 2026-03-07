#include <iostream>
#include <cstdio>
#include <filesystem>
#include <sstream>

#include "engine/Engine.hpp"

#include "engine/ResourceManager.hpp"
#include "engine/resources/Mesh.hpp"
#include "engine/resources/ShaderProgram.hpp"
#include "engine/resources/Texture.hpp"
#include "engine/Scene.hpp"

#include "engine/objects/GameObject/GameObject.hpp"
#include "engine/objects/Camera/Camera.hpp"

const unsigned short FPS = 100;

bool readtextfile(std::string filename, std::string *output);

int main()
{
    if (!Engine::Init(1200, 700)) return 1;

    GLFWwindow *window = Engine::GetWindow();

    ShaderProgram *sh = ResourceManager::CreateShaderProgram("default");

    {
        std::string log;
        std::string src;

        if (readtextfile("./res/default.v.glsl", &src)) printf("read vertex shader source\n");
        sh->LoadVertexShader(src);
        if (!sh->CompileVertexShader(&log)) std::cout << "Error compiling vertex shader: " << std::endl << log << std::endl;

        if (readtextfile("./res/default.f.glsl", &src)) printf("read fragment shader source\n");
        sh->LoadFragmentShader(src);
        if (!sh->CompileFragmentShader(&log)) std::cout << "Error compiling fragment shader: " << std::endl << log << std::endl;
        
        if (!sh->LinkShaderProgram(&log)) std::cout << "Error linking shader program: " << std::endl << log << std::endl;
    }

    if (ResourceManager::CreateMesh("crowbar_cyl")->LoadFromUCMESHFile("./res/models/cyl.ucmesh")) printf("loaded model crowbar_cyl\n");
    if (ResourceManager::CreateMesh("crowbar_head")->LoadFromUCMESHFile("./res/models/head.ucmesh")) printf("loaded model crowbar_head\n");

    if (ResourceManager::CreateTexture("crowbar_cyl")->LoadFromUCTEXFile("./res/textures/cyl.uctex")) printf("loaded texture crowbar_cyj\n");
    if (ResourceManager::CreateTexture("crowbar_head")->LoadFromUCTEXFile("./res/textures/cyl.uctex")) printf("loaded texture crowbar_head\n");

    Scene *s = ResourceManager::CreateScene("main");
    Engine::CurrentScene = "main";

    Camera *cam = s->CreateObject<Camera>();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0.1, 1);

    glfwSetTime(0);
    double prev_time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        double delta = glfwGetTime() - prev_time;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        if (delta >= 1.0 / FPS)
        {
            prev_time = glfwGetTime();

            Engine::Update(delta);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Engine::Render();

            glfwSwapBuffers(window);
        }

        glfwPollEvents();
    }

    Engine::Shutdown();

    return 0;
}

bool readtextfile(std::string filename, std::string *output)
{
    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    std::ostringstream oss;

    //if (!std::filesystem::is_regular_file(filename)) return false;

    FILE *f = fopen(filename.c_str(), "r");
    if (!f) return false;

    while (!feof(f))
    {
        size_t readbytes = fread(buffer, sizeof(char), BUFFER_SIZE, f);
        for (size_t i = 0; i < readbytes; i++) oss << buffer[i];
    }

    fclose(f);

    *output = oss.str();

    return true;
}