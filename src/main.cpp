#include <iostream>
#include <cstdio>
#include <filesystem>
#include <sstream>

#include "engine/Utils.hpp"
#include "engine/Engine.hpp"

#include "engine/ResourceManager.hpp"
#include "engine/resources/Mesh.hpp"
#include "engine/resources/ShaderProgram.hpp"
#include "engine/resources/Texture.hpp"
#include "engine/resources/AudioClip.hpp"
#include "engine/Scene.hpp"

#include "engine/objects/GameObject/GameObject.hpp"
#include "engine/objects/Camera/Camera.hpp"
#include "engine/objects/Entity/Entity.hpp"
#include "engine/objects/AudioSource/AudioSource.hpp"
#include "engine/objects/AudioListener/AudioListener.hpp"
#include "engine/objects/TemporaryAudioSource/TemporaryAudioSource.hpp"

#include "engine/audio/AudioDevice.hpp"
#include "engine/audio/AudioEffectProperties.hpp"
#include "engine/audio/AudioEffectSlot.hpp"

const unsigned short FPS = 100;

bool readtextfile(std::string filename, std::string *output);

void scrollCallback(GLFWwindow *w, double xoff, double yoff);
void updateCam(GLFWwindow *w, Camera *c, double delta, Entity *crowbar);

const unsigned short WWIDTH = 1280;
const unsigned short WHEIGHT = 720;

const glm::vec3 crowbar_rot = glm::vec3(-3.04567, -0.648789, 3.12098);

double lastX = WWIDTH / 2, lastY = WHEIGHT / 2;

struct
{
    float density = 1;
    float diffusion = 1;

    float gain = 0.32;
    float gainhf = 0.89;
    float gainlf = 0;

    float decaytime = 1.49;
    float decayhfratio = 0.83;
    float decaylfratio = 1;

    float reflectionsgain = 0.05;
    float reflectionsdelay = 0.007;

    float latereverbgain = 1.26;
    float latereverbdelay = 0.011;
    
    float echotime = 0.25;
    float echodepth = 0;

    float modulationtime = 0.25;
    float modulationdepth = 0;

    float airabsorptiongainhf = 0.994;

    float hfreference = 5000;
    float lfreference = 250;

    float roomrollofffactor = 0;

    bool decayhf_limit = true;
} typedef EAXReverbEffectSettings;

void applyeaxreverbeffecttoslot(const EAXReverbEffectSettings *setts, AudioEffectSlot *slot)
{
    AudioEffectProperties p;
    p.SetEffectType(AL_EFFECT_EAXREVERB);



    /*p.SetEffectFloat(AL_EAXREVERB_DECAY_TIME, 10);
    p.SetEffectFloat(AL_EAXREVERB_DENSITY, 0);
    p.SetEffectFloat(AL_EAXREVERB_DIFFUSION, 0);
    p.SetEffectFloat(AL_EAXREVERB_GAIN, 0.4);
    p.SetEffectFloat(AL_EAXREVERB_LATE_REVERB_DELAY, 0.08);

    p.SetEffectFloat(AL_EAXREVERB_GAINLF, 0.8);
    p.SetEffectFloat(AL_EAXREVERB_DECAY_LFRATIO, 0.8);

    p.SetEffectFloat(AL_EAXREVERB_ECHO_TIME, 0.25);
    p.SetEffectFloat(AL_EAXREVERB_ECHO_DEPTH, 0.5);

    p.SetEffectFloat(AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, 1);*/

    p.SetEffectFloat(AL_EAXREVERB_DENSITY, setts->density);
    p.SetEffectFloat(AL_EAXREVERB_DIFFUSION, setts->diffusion);

    p.SetEffectFloat(AL_EAXREVERB_GAIN, setts->gain);
    p.SetEffectFloat(AL_EAXREVERB_GAINHF, setts->gainhf);
    p.SetEffectFloat(AL_EAXREVERB_GAINLF, setts->gainlf);

    p.SetEffectFloat(AL_EAXREVERB_DECAY_TIME, setts->decaytime);
    p.SetEffectFloat(AL_EAXREVERB_DECAY_HFRATIO, setts->decayhfratio);
    p.SetEffectFloat(AL_EAXREVERB_DECAY_LFRATIO, setts->decaylfratio);

    p.SetEffectFloat(AL_EAXREVERB_REFLECTIONS_GAIN, setts->reflectionsgain);
    p.SetEffectFloat(AL_EAXREVERB_REFLECTIONS_DELAY, setts->reflectionsdelay);

    p.SetEffectFloat(AL_EAXREVERB_LATE_REVERB_GAIN, setts->latereverbgain);
    p.SetEffectFloat(AL_EAXREVERB_LATE_REVERB_DELAY, setts->latereverbdelay);

    p.SetEffectFloat(AL_EAXREVERB_ECHO_TIME, setts->echotime);
    p.SetEffectFloat(AL_EAXREVERB_ECHO_DEPTH, setts->echodepth);

    p.SetEffectFloat(AL_EAXREVERB_MODULATION_TIME, setts->modulationtime);
    p.SetEffectFloat(AL_EAXREVERB_MODULATION_DEPTH, setts->modulationdepth);

    p.SetEffectFloat(AL_EAXREVERB_AIR_ABSORPTION_GAINHF, setts->airabsorptiongainhf);

    p.SetEffectFloat(AL_EAXREVERB_HFREFERENCE, setts->hfreference);
    p.SetEffectFloat(AL_EAXREVERB_LFREFERENCE, setts->lfreference);

    p.SetEffectFloat(AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, setts->roomrollofffactor);

    p.SetEffectInt(AL_EAXREVERB_DECAY_HFLIMIT, setts->decayhf_limit ? AL_TRUE : AL_FALSE);

    slot->ApplyEffect(p);
}

int main()
{
    if (!Engine::Init(WWIDTH, WHEIGHT)) return 1;

    GLFWwindow *window = Engine::GetWindow();
    ImGUI::GetIO().IniFilename = nullptr;

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetCursorPos(window, Engine::GetWindowSize().x / 2.0f, Engine::GetWindowSize().y / 2.0f);

    AudioDevice dev = AudioDevice(NULL);
    Engine::SetCurrentAudioDevice(&dev);

    Engine::SetAudioDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

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
    if (ResourceManager::CreateMesh("hl1_reactor_demo")->LoadFromUCMESHFile("./res/models/hl1_reactor_demo.ucmesh")) printf("loaded model hl1_reactor_demo\n");

    AudioClip *zapsfx = ResourceManager::CreateAudioClip("zapsfx");
    if (zapsfx->LoadFromUCSOUNDFile("./res/sounds/zapmachine.ucsound")) printf("loaded zapmachine sound\n");

    AudioClip *alienbuildersfx = ResourceManager::CreateAudioClip("alienbuildersfx");
    if (alienbuildersfx->LoadFromUCSOUNDFile("./res/sounds/alien_builder.ucsound")) printf("loaded alienbuilder sound\n");

    Scene *s = Engine::CreateScene("main");
    Engine::SetCurrentScene("main");
    s->fog.enabled = true;
    s->fog.startDistance = 0;
    s->fog.endDistance = 32;
    s->fog.color = glm::vec3(106 / 255.0f, 117 / 255.0f, 129 / 255.0f);

    Camera *cam = s->CreateObject<Camera>();
    s->SetCurrentCamera(cam);

    AudioListener *listener = s->CreateObject<AudioListener>();
    listener->SetParent(cam, false);

    Entity *ent = s->CreateObject<Entity>();
    ent->SetParent(cam, false);
    ent->usedShaderProgram = "default";
    ent->transform = Transform(glm::vec3(0.0933556, -0.160361, -0.179554), crowbar_rot, glm::vec3(0.01));
    s->SetObjectOrder(ent, 1);

    Entity *cube = s->CreateObject<Entity>(Transform({0, 0, -5}, glm::quat(glm::vec3(0)), glm::vec3(0.1)));
    cube->usedShaderProgram = "default";
    cube->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    Entity *cube2 = s->CreateObject<Entity>(Transform({-2.5, 0, -2.5}));
    cube2->usedShaderProgram = "default";
    cube2->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    cube2->SetRigidBodyType(DYNAMIC);
    //cube2->SetEnabledGravity(true);
    std::cout << cube2->SetParent(cube) << std::endl;

    Entity *hl1_reactor_demo = s->CreateObject<Entity>();
    hl1_reactor_demo->usedShaderProgram = "default";

    AudioEffectSlot reverb = AudioEffectSlot();
    EAXReverbEffectSettings reverbsetts;
    applyeaxreverbeffecttoslot(&reverbsetts, &reverb);

    AudioSource *src = s->CreateObject<AudioSource>();
    reverb.AddSource(src);
    src->SetParent(cube2, false);
    src->SetCurrentClip(zapsfx);

    src->SetLooping(true);
    //src->SetSourceFloat(AL_REFERENCE_DISTANCE, 8);
    //src->SetSourceFloat(AL_MAX_DISTANCE, 32);
    //src->SetSourceFloat(AL_GAIN, 1);

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
    cube2->surfaces.push_back(surf);

    surf.mesh = "hl1_reactor_demo";
    surf.texture = "";
    surf.culling = NoCulling;
    hl1_reactor_demo->surfaces.push_back(surf);

    glEnable(GL_DEPTH_TEST);
    glClearColor(s->fog.color.x, s->fog.color.y, s->fog.color.z, 1);

    float speed = 1;
    bool captured = false;

    bool autoapplyeffect = false;
    float pitch = 1;
    float gain = 1;
    float refdist = 8;
    float maxdist = 32;

    bool enabledgravity = false;

    src->Play();

    glfwSetTime(0);
    double prev_time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        double delta = glfwGetTime() - prev_time;

        if (Engine::IsKeyPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

        // capture mouse.
        if (Engine::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && !captured)
        {
            captured = true;

            glm::vec2 sz = Engine::GetWindowSize();
            glfwSetCursorPos(window, sz.x / 2.0f, sz.y / 2.0f);
            lastX = sz.x / 2.0f;
            lastY = sz.y / 2.0f;

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (!Engine::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && captured)
        {
            captured = false;

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        if (delta >= 1.0 / FPS)
        {
            prev_time = glfwGetTime();

            ent->surfaces[0].textureTransform.Translate(glm::vec2(0, delta * 0.5f));
            ent->surfaces[0].textureTransform.Rotate(0.1 * delta);

            //cube2->transform.Translate({1.0f * delta, 0, 1.0f * delta});

            updateCam(window, cam, delta, ent);

            //if (Engine::IsKeyPressed(GLFW_KEY_F)) src->Pause();
            //else if (src->GetState() != AudioSourceState::PLAYING) src->Play();

            Engine::Update(delta);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Engine::Render();

            Engine::BeginRenderUI();

            ImGUI::SetNextWindowSize(ImVec2(600, 620), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
            ImGUI::Begin("EAX Reverb. settings");
            //ImGUI::SetWindowSize(ImVec2(600, 620));

            ImGUI::SliderFloat("Density", &reverbsetts.density, 0, 1);
            ImGUI::SliderFloat("Diffusion", &reverbsetts.diffusion, 0, 1);

            ImGUI::Separator();

            ImGUI::SliderFloat("Gain", &reverbsetts.gain, 0, 1);
            ImGUI::SliderFloat("Gain HF", &reverbsetts.gainhf, 0, 1);
            ImGUI::SliderFloat("Gain LF", &reverbsetts.gainlf, 0, 1);

            ImGUI::Separator();

            ImGUI::SliderFloat("Decay Time", &reverbsetts.decaytime, 0, 20);
            ImGUI::SliderFloat("Decay HF Ratio", &reverbsetts.decayhfratio, 0.1, 2);
            ImGUI::SliderFloat("Decay LF Ratio", &reverbsetts.decaylfratio, 0.1, 2);

            ImGUI::Separator();

            ImGUI::SliderFloat("Reflections Gain", &reverbsetts.reflectionsgain, 0, 3.16);
            ImGUI::SliderFloat("Reflections Delay", &reverbsetts.reflectionsdelay, 0, 0.3);

            ImGUI::Separator();

            ImGUI::SliderFloat("Late Reverb. Gain", &reverbsetts.latereverbgain, 0, 10);
            ImGUI::SliderFloat("Late Reverb. Delay", &reverbsetts.latereverbdelay, 0, 0.1);

            ImGUI::Separator();

            ImGUI::SliderFloat("Echo Time", &reverbsetts.echotime, 0.075, 0.25);
            ImGUI::SliderFloat("Echo Depth", &reverbsetts.echodepth, 0, 1);

            ImGUI::Separator();

            ImGUI::SliderFloat("Modulation Time", &reverbsetts.modulationtime, 0.04, 4);
            ImGUI::SliderFloat("Modulation Depth", &reverbsetts.modulationdepth, 0, 1);

            ImGUI::Separator();

            ImGUI::SliderFloat("HF Reference", &reverbsetts.hfreference, 1000, 20000);
            ImGUI::SliderFloat("LF Reference", &reverbsetts.lfreference, 20, 1000);

            ImGUI::Separator();

            ImGUI::SliderFloat("Room Rolloff Factor", &reverbsetts.roomrollofffactor, 0, 10);
            ImGUI::SliderFloat("Air Absorption Gain HF", &reverbsetts.airabsorptiongainhf, 0.892, 1);
            ImGUI::Checkbox("Decay HF Limit", &reverbsetts.decayhf_limit);

            ImGUI::Separator();

            ImGUI::Checkbox("Auto apply effect", &autoapplyeffect);
            if (autoapplyeffect) applyeaxreverbeffecttoslot(&reverbsetts, &reverb);
            else if (ImGUI::Button("Apply effect")) applyeaxreverbeffecttoslot(&reverbsetts, &reverb);

            ImGUI::End();

            ImGUI::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_FirstUseEver);
            ImGUI::Begin("AudioSource control");

            if (ImGUI::Button("Play/Resume")) src->Play();
            ImGUI::SameLine();
            if (ImGUI::Button("Pause")) src->Pause();
            ImGUI::SameLine();
            if (ImGUI::Button("Stop")) src->Stop();
            ImGUI::SameLine();
            if (ImGUI::Button("Rewind")) src->Rewind();

            ImGUI::SliderFloat("Pitch", &pitch, 0, 2);
            ImGUI::SliderFloat("Gain", &gain, 0, 1);
            ImGUI::DragFloat("Reference Distance", &refdist, 0.1, 0, std::numeric_limits<float>().max());
            ImGUI::DragFloat("Max Distance", &maxdist, 0.1, 0, std::numeric_limits<float>().max());

            src->SetSourceFloat(AL_PITCH, pitch);
            src->SetSourceFloat(AL_GAIN, gain);
            src->SetSourceFloat(AL_REFERENCE_DISTANCE, refdist);
            src->SetSourceFloat(AL_MAX_DISTANCE, maxdist);

            ImGUI::End();

            ImGUI::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
            ImGUI::Begin("Test functions");

            if (ImGUI::Button("Spawn new TemporaryAudioSource"))
            {
                TemporaryAudioSource *tmpsrc = s->CreateObject<TemporaryAudioSource>();
                tmpsrc->SetParent(cam, false);

                tmpsrc->SetSourceFloat(AL_GAIN, 0.2);

                tmpsrc->SetCurrentClip(ResourceManager::GetAudioClip("zapsfx"));
                tmpsrc->Play();
            }

            //ImGUI::Checkbox("Yellow cube gravity", &enabledgravity);
            //cube2->SetEnabledGravity(enabledgravity);

            if (ImGUI::Button("Reset yellow cube velocity")) cube2->SetLinearVelocity(glm::vec3(0.0f));

            ImGUI::End();

            Engine::EndRenderUI();

            glfwSwapBuffers(window);
        }
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
    static float cwoffset = 0;

    float speed;
    if (Engine::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) speed = cameraSpeed * 2.0;
    else if (Engine::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) speed = cameraSpeed / 2.0;
    else speed = cameraSpeed;

    Transform *t = &cam->transform;
    glm::vec3 old_pos = t->GetPosition();

    if (Engine::IsKeyPressed(GLFW_KEY_W)) { t->Translate(t->GetFront() * glm::vec3(speed * delta)); }
    if (Engine::IsKeyPressed(GLFW_KEY_S)) { t->Translate(-t->GetFront() * glm::vec3(speed * delta)); }
    if (Engine::IsKeyPressed(GLFW_KEY_A)) { t->Translate(-t->GetRight() * glm::vec3(speed * delta)); }
    if (Engine::IsKeyPressed(GLFW_KEY_D)) { t->Translate(t->GetRight() * glm::vec3(speed * delta)); }

    if (Engine::IsKeyPressed(GLFW_KEY_SPACE)) t->Translate(glm::vec3(0, speed * delta, 0));
    if (Engine::IsKeyPressed(GLFW_KEY_LEFT_ALT)) t->Translate(glm::vec3(0, -speed * delta, 0));

    glm::vec3 deltapos = t->GetPosition() - old_pos;
    cwoffset += glm::length(Utils::normalize(deltapos)) * glm::radians(11.25f / 4);
    cwoffset = glm::mod(cwoffset, glm::radians(360.0f));

    if (Engine::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE)) cameraSpeed = DEFAULT_CAMERA_SPEED;

    if (!ImGUI::GetIO().WantCaptureMouse && Engine::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
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

        lastX = mouseX;
        lastY = mouseY;
    }

    crowbar->transform.SetRotation(crowbar_rot + glm::vec3(glm::sin(cwoffset) * 0.03f, 0.0f, 0.0f));
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