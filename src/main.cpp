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
#include "engine/objects/Entity/Entity.hpp"

const unsigned short FPS = 100;

bool readtextfile(std::string filename, std::string *output);

void scrollCallback(GLFWwindow *w, double xoff, double yoff);
void updateCam(GLFWwindow *w, Camera *c, double delta, Entity *crowbar);

const unsigned short WWIDTH = 1200;
const unsigned short WHEIGHT = 700;

const glm::vec3 crowbar_rot = glm::vec3(-3.04567, -0.648789, 3.12098);

int main()
{
    if (!Engine::Init(WWIDTH, WHEIGHT)) return 1;

    GLFWwindow *window = Engine::GetWindow();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetCursorPos(window, WWIDTH / 2.0f, WHEIGHT / 2.0f);

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
    if (ResourceManager::CreateTexture("crowbar_head")->LoadFromUCTEXFile("./res/textures/head.uctex")) printf("loaded texture crowbar_head\n");

    if (ResourceManager::CreateMesh("cube")->LoadFromUCMESHFile("./res/models/cube.ucmesh")) printf("loaded model cube\n");

    Scene *s = Engine::CreateScene("main");
    Engine::SetCurrentScene("main");
    s->fog.enabled = true;
    s->fog.startDistance = 0;
    s->fog.endDistance = 8;
    s->fog.color = glm::vec3(106 / 255.0f, 117 / 255.0f, 129 / 255.0f);

    Camera *cam = s->CreateObject<Camera>();
    s->SetCurrentCamera(cam);

    Entity *ent = s->CreateObject<Entity>();
    ent->SetParent(cam, false);
    ent->usedShaderProgram = "default";
    ent->transform = Transform(glm::vec3(0.0933556, -0.160361, -0.179554), crowbar_rot, glm::vec3(0.01));

    Entity *cube = s->CreateObject<Entity>(Transform({0, 0, -5}, glm::quat(glm::vec3(0)), glm::vec3(0.1)));
    cube->usedShaderProgram = "default";
    cube->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    Surface surf;
    surf.mesh = "crowbar_cyl";
    surf.texture = "crowbar_cyl";
    ent->surfaces.push_back(surf);

    surf.mesh = "crowbar_head";
    surf.texture = "crowbar_head";
    ent->surfaces.push_back(surf);

    surf.mesh = "cube";
    surf.texture = "testcube";
    cube->surfaces.push_back(surf);

    glEnable(GL_DEPTH_TEST);
    glClearColor(s->fog.color.x, s->fog.color.y, s->fog.color.z, 1);

    float speed = 1;

    glfwSetTime(0);
    double prev_time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        double delta = glfwGetTime() - prev_time;

        if (Engine::IsKeyPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

        if (delta >= 1.0 / FPS)
        {
            prev_time = glfwGetTime();

            ent->surfaces[0].textureTransform.Translate(glm::vec2(0, delta * 0.5f));
            ent->surfaces[0].textureTransform.Rotate(0.1 * delta);

            updateCam(window, cam, delta, ent);

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

const float MOUSE_SENSITIVITY = 0.1;
const float DEFAULT_CAMERA_SPEED = 3;
float cameraSpeed = DEFAULT_CAMERA_SPEED;

void scrollCallback(GLFWwindow *w, double xoff, double yoff)
{
    cameraSpeed += yoff / 5;
    if (cameraSpeed < 0) cameraSpeed = 0;
}

void updateCam(GLFWwindow *window, Camera *cam, double delta, Entity *crowbar)
{
    static double lastX = WWIDTH / 2, lastY = WHEIGHT / 2;
    static float offset = 0;

    float speed;
    if (Engine::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) speed = cameraSpeed * 2.0;
    else if (Engine::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) speed = cameraSpeed / 2.0;
    else speed = cameraSpeed;

    Transform *t = &cam->transform;

    if (Engine::IsKeyPressed(GLFW_KEY_W)) { t->Translate(t->GetFront() * glm::vec3(speed * delta)); offset += 0.05; }
    if (Engine::IsKeyPressed(GLFW_KEY_S)) { t->Translate(-t->GetFront() * glm::vec3(speed * delta)); offset += 0.05; }
    if (Engine::IsKeyPressed(GLFW_KEY_A)) { t->Translate(-t->GetRight() * glm::vec3(speed * delta)); offset += 0.05; }
    if (Engine::IsKeyPressed(GLFW_KEY_D)) { t->Translate(t->GetRight() * glm::vec3(speed * delta)); offset += 0.05; }

    if (Engine::IsKeyPressed(GLFW_KEY_SPACE)) t->Translate(glm::vec3(0, speed * delta, 0));
    if (Engine::IsKeyPressed(GLFW_KEY_LEFT_ALT)) t->Translate(glm::vec3(0, -speed * delta, 0));

    if (Engine::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE)) cameraSpeed = DEFAULT_CAMERA_SPEED;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    float mxoff = -glm::radians((mouseX - lastX) * MOUSE_SENSITIVITY);
    float myoff = -glm::radians((mouseY - lastY) * MOUSE_SENSITIVITY);
    {
        glm::quat delta_pitch = glm::angleAxis(myoff, glm::vec3(1, 0, 0));
        glm::quat delta_yaw = glm::angleAxis(mxoff, glm::vec3(0, 1, 0));

        glm::quat new_rotation = delta_yaw * t->GetRotation() * delta_pitch;

        glm::vec3 front = new_rotation * glm::vec3(0, 0, -1);
        glm::vec3 front_xz = glm::normalize(glm::vec3(front.x, 0, front.z));

        if (glm::dot(front, front_xz) >= glm::cos(glm::radians(60.0f))) t->SetRotation(new_rotation);
        else t->Rotate(delta_yaw);
    }

    crowbar->transform.SetRotation(crowbar_rot + glm::vec3(glm::sin(offset) * 0.03f, 0.0f, 0.0f));
    //if (offset >= glm::radians(360.0f)) offs
    offset = glm::mod(offset, glm::radians(360.0f));

    lastX = mouseX;
    lastY = mouseY;
}

bool readtextfile(std::string filename, std::string *output)
{
    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    std::ostringstream oss;

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