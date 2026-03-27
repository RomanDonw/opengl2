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

#include "engine/physics/colliders/colliders.hpp"

#include "engine/audio/AudioEffectProperties.hpp"
#include "engine/audio/AudioEffectSlot.hpp"

#include "Decal.hpp"
#include "MaxwellCat.hpp"

const unsigned short FPS = 100;

bool readtextfile(std::string filename, std::string *output);

void scrollCallback(GLFWwindow *w, double xoff, double yoff);
void updateCam(GLFWwindow *w, Camera *c, double delta, Model *crowbar);

const unsigned short WWIDTH = 1280;
const unsigned short WHEIGHT = 720;

const glm::vec3 crowbar_rot = glm::vec3(-3.04567, -0.648789, 3.12098);

double lastX = WWIDTH / 2, lastY = WHEIGHT / 2;
const float MOUSE_SENSITIVITY = 0.1;

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
    if (Engine::Init(WWIDTH, WHEIGHT, NULL) != SUCCESS) return 1;

    Engine::SetAudioDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    GLFWwindow *window = Engine::GetWindow();
    ImGUI::GetIO().IniFilename = nullptr;

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetCursorPos(window, Engine::GetWindowSize().x / 2.0f, Engine::GetWindowSize().y / 2.0f);

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

    if (ResourceManager::CreateTexture("crowbar_cyl")->LoadFromTextureFile("./res/textures/cyl.png")) printf("loaded texture crowbar_cyj\n");
    if (ResourceManager::CreateTexture("crowbar_head")->LoadFromTextureFile("./res/textures/head.png")) printf("loaded texture crowbar_head\n");

    if (ResourceManager::CreateMesh("cube")->LoadFromUCMESHFile("./res/models/cube.ucmesh")) printf("loaded model cube\n");
    if (ResourceManager::CreateMesh("hl1_reactor_demo")->LoadFromUCMESHFile("./res/models/hl1_reactor_demo.ucmesh")) printf("loaded model hl1_reactor_demo\n");

    if (ResourceManager::CreateMesh("sphere")->LoadFromUCMESHFile("./res/models/sphere.ucmesh")) puts("loaded model sphere");
    if (ResourceManager::CreateMesh("decal")->LoadFromUCMESHFile("./res/models/decal.ucmesh")) puts("loaded model decal");


    if (ResourceManager::CreateMesh("button_3")->LoadFromUCMESHFile("./res/models/buttons/3.ucmesh")) puts("loaded button 3 model");
    if (ResourceManager::CreateMesh("button_4")->LoadFromUCMESHFile("./res/models/buttons/4.ucmesh")) puts("loaded button 4 model");

    if (ResourceManager::CreateTexture("button_3_on")->LoadFromTextureFile("./res/textures/buttons/3_on.png")) puts("loaded button 3 on texture");
    if (ResourceManager::CreateTexture("button_3_off")->LoadFromTextureFile("./res/textures/buttons/3_off.png")) puts("loaded button 3 off texture");

    if (ResourceManager::CreateTexture("button_4_on")->LoadFromTextureFile("./res/textures/buttons/4_on.png")) puts("loaded button 4 on texture");
    if (ResourceManager::CreateTexture("button_4_off")->LoadFromTextureFile("./res/textures/buttons/4_off.png")) puts("loaded button 4 off texture");


    if (ResourceManager::CreateTexture("bullethole1")->LoadFromTextureFile("./res/textures/bullethole1.png")) puts("loaded bullethole1 texture");
    if (ResourceManager::CreateTexture("bullethole2")->LoadFromTextureFile("./res/textures/bullethole2.png")) puts("loaded bullethole2 texture");

    if (ResourceManager::CreateTexture("maxwellcat")->LoadFromTextureFile("./res/textures/maxwellcat.png")) puts("loaded maxwellcat texture");
    if (ResourceManager::CreateMesh("maxwellcat")->LoadFromUCMESHFile("./res/models/maxwellcat.ucmesh")) puts("loaded maxwellact model");

    AudioClip *zapsfx = ResourceManager::CreateAudioClip("zapsfx");
    if (zapsfx->LoadFromAudioFile("./res/sounds/zapmachine.wav")) printf("loaded zapmachine sound\n");

    AudioClip *alienbuildersfx = ResourceManager::CreateAudioClip("alienbuildersfx");
    if (alienbuildersfx->LoadFromAudioFile("./res/sounds/alien_builder.wav")) printf("loaded alienbuilder sound\n");

    AudioClip *freightmove1sfx = ResourceManager::CreateAudioClip("freightmove1sfx");
    if (freightmove1sfx->LoadFromAudioFile("./res/sounds/freightmove1.wav")) puts("loaded freightmove1 sound");

    AudioClip *hit1sfx = ResourceManager::CreateAudioClip("hit1sfx");
    if (hit1sfx->LoadFromAudioFile("./res/sounds/hit_1.wav")) puts("loaded hit_1 sound");

    AudioClip *hit2sfx = ResourceManager::CreateAudioClip("hit2sfx");
    if (hit2sfx->LoadFromAudioFile("./res/sounds/hit_2.wav")) puts("loaded hit_2 sound");

    AudioClip *misssfx = ResourceManager::CreateAudioClip("misssfx");
    if (misssfx->LoadFromAudioFile("./res/sounds/miss.wav")) puts("loaded miss sound");

    AudioClip *mus_maxwellcat = ResourceManager::CreateAudioClip("mus_maxwellcat");
    if (mus_maxwellcat->LoadFromAudioFile("./res/music/maxwellcat.ogg")) puts("loaded maxwellcat music");

    AudioClip *mus_mech8 = ResourceManager::CreateAudioClip("mus_mech8");
    if (mus_mech8->LoadFromAudioFile("./res/music/Mech8.ogg")) puts("loaded Mech8 music");

    Scene *s = Engine::CreateScene("main");
    Engine::SetCurrentScene("main");
    s->fog.enabled = true;
    s->fog.startDistance = 0;
    s->fog.endDistance = 32;
    s->fog.color = glm::vec3(106 / 255.0f, 117 / 255.0f, 129 / 255.0f);

    Camera *cam = s->CreateObject<Camera>();
    cam->FOV = glm::radians(70.0f);
    s->SetCurrentCamera(cam);

    AudioSource *music = s->CreateObject<AudioSource>();
    music->SetParent(cam, false);
    music->SetSourceFloat(AL_REFERENCE_DISTANCE, 1);
    music->SetSourceFloat(AL_MAX_DISTANCE, 1);
    music->SetSourceFloat(AL_GAIN, 0.2);
    music->SetLooping(true);

    AudioListener *listener = s->CreateObject<AudioListener>();
    listener->SetParent(cam, false);

    Model *ent = s->CreateObject<Model>();
    ent->SetParent(cam, false);
    ent->usedShaderProgram = "default";
    ent->transform = Transform(glm::vec3(0.0933556, -0.160361, -0.179554), crowbar_rot, glm::vec3(0.01));
    ent->enableDepthTest = false;
    s->SetObjectOrder(ent, 101);

    Model *cube = s->CreateObject<Model>(Transform({0, 0, -5}, glm::quat(glm::vec3(0)), glm::vec3(0.1)));
    cube->usedShaderProgram = "default";
    cube->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    Entity *cube2 = s->CreateObject<Entity>(Transform({-2.5, 0, -2.5}));
    cube2->usedShaderProgram = "default";
    cube2->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    cube2->SetRigidBodyType(DYNAMIC);
    cube2->SetGravityEnabled(true);
    cube2->SetMass(10);
    BoxCollider *cube2coll = cube2->AddCollider<BoxCollider>(Transform(), glm::vec3(1));
    cube2coll->SetFrictionCoefficient(1);

    Entity *ground = s->CreateObject<Entity>(Transform({-2.5, -10, -2.5}, glm::quat(glm::radians(glm::vec3(10, 0, 0)))));
    ground->transform.SetScale({10, 0.5, 10});
    ground->usedShaderProgram = "default";
    ground->color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    BoxCollider *groundc = ground->AddCollider<BoxCollider>(Transform(), glm::vec3(10, 0.5, 10));
    groundc->SetFrictionCoefficient(1);
    groundc->SetBounciness(0);

    AudioSource *groundsrc = s->CreateObject<AudioSource>();
    groundsrc->SetParent(ground, false);
    groundsrc->SetSourceFloat(AL_GAIN, 0.1);
    groundsrc->SetSourceFloat(AL_MAX_DISTANCE, 16);
    groundsrc->SetSourceFloat(AL_REFERENCE_DISTANCE, 10);
    groundsrc->SetLooping(true);
    groundsrc->SetCurrentClip(freightmove1sfx);

    Entity *sphere = s->CreateObject<Entity>(Transform({-2.5, 3, -2.5}));
    sphere->usedShaderProgram = "default";
    sphere->SetRigidBodyType(DYNAMIC);
    SphereCollider *sphcoll = sphere->AddCollider<SphereCollider>(Transform(), 1);
    sphcoll->SetFrictionCoefficient(1);

    Model *hl1_reactor_demo = s->CreateObject<Model>();
    hl1_reactor_demo->usedShaderProgram = "default";
    hl1_reactor_demo->transform.SetPosition({-15, 0, 0});

    AudioEffectSlot reverb = AudioEffectSlot();
    EAXReverbEffectSettings reverbsetts;
    applyeaxreverbeffecttoslot(&reverbsetts, &reverb);

    AudioSource *src = s->CreateObject<AudioSource>();
    reverb.AddSource(src);
    src->SetParent(cube2, false);
    src->SetCurrentClip(zapsfx);

    src->SetLooping(true);

    RigidBody *playerrb = s->CreateObject<RigidBody>(Transform({-2.5, 1, -2.5}));
    CapsuleCollider *playercoll = playerrb->AddCollider<CapsuleCollider>(Transform(), 0.5, 1.5);
    playercoll->SetFrictionCoefficient(0.5);
    playercoll->SetBounciness(0);
    playerrb->SetAngularLockAxisFactor({0, 1, 0});
    playerrb->SetRigidBodyType(DYNAMIC);
    playerrb->SetGravityEnabled(true);
    playerrb->SetMass(70);
    cam->SetParent(playerrb, false);
    cam->transform.SetPosition({0, 0.5, 0});


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
    ground->surfaces.push_back(surf);

    surf.mesh = "hl1_reactor_demo";
    surf.texture = "";
    surf.culling = NoCulling;
    hl1_reactor_demo->surfaces.push_back(surf);

    surf.mesh = "sphere";
    sphere->surfaces.push_back(surf);

    float speed = 1;
    bool captured = false;

    bool autoapplyeffect = false;
    float pitch = 1;
    float gain = 1;
    float refdist = 8;
    float maxdist = 32;

    groundsrc->Play();

    music->SetCurrentClip(mus_mech8);
    //music->Play();

    srand(time(NULL));

    bool hitclicked = false;
    unsigned long loops = 0;
    unsigned long ups = 0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(s->fog.color.x, s->fog.color.y, s->fog.color.z, 1);
    
    double prev_time = 0;
    double prev_sec = 0;
    glfwSetTime(0);
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

            if (glfwGetTime() >= prev_sec + 1)
            {
                ups = loops;
                loops = 0;
                prev_sec = glfwGetTime();
            }

            ent->surfaces[0].textureTransform.Translate(glm::vec2(0, delta * 0.5f));
            ent->surfaces[0].textureTransform.Rotate(0.1 * delta);

            ground->SetLinearVelocity({1, 0, 0});

            if (Engine::IsKeyPressed(GLFW_KEY_W)) playerrb->ApplyLocalForceToCenterOfMass(glm::vec3(0, 0, -1) * 10.0f * playerrb->GetMass());
            if (Engine::IsKeyPressed(GLFW_KEY_S)) playerrb->ApplyLocalForceToCenterOfMass(glm::vec3(0, 0, 1) * 10.0f * playerrb->GetMass());
            if (Engine::IsKeyPressed(GLFW_KEY_A)) playerrb->ApplyLocalForceToCenterOfMass(glm::vec3(-1, 0, 0) * 10.0f * playerrb->GetMass());
            if (Engine::IsKeyPressed(GLFW_KEY_D)) playerrb->ApplyLocalForceToCenterOfMass(glm::vec3(1, 0, 0) * 10.0f * playerrb->GetMass());

            if (Engine::IsKeyPressed(GLFW_KEY_SPACE)) playerrb->ApplyLocalForceToCenterOfMass(-s->GetGravity() * 2.0f * playerrb->GetMass());

            playerrb->SetAngularVelocity(glm::vec3(0));

            if (!ImGUI::GetIO().WantCaptureMouse && Engine::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
            {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

                float mxoff = -glm::radians((mouseX - lastX) * MOUSE_SENSITIVITY);
                float myoff = -glm::radians((mouseY - lastY) * MOUSE_SENSITIVITY);
                {
                    glm::quat delta_pitch = glm::angleAxis(myoff, glm::vec3(1, 0, 0));
                    glm::quat delta_yaw = glm::angleAxis(mxoff, glm::vec3(0, 1, 0));

                    glm::quat new_rotation = cam->transform.GetRotation() * delta_pitch;

                    glm::vec3 front = new_rotation * glm::vec3(0, 0, -1);
                    glm::vec3 front_xz = glm::normalize(glm::vec3(front.x, 0, front.z));

                    if (glm::dot(front, front_xz) >= glm::cos(glm::radians(60.0f))) cam->transform.SetRotation(new_rotation);
                    playerrb->transform.Rotate(delta_yaw);
                }

                lastX = mouseX;
                lastY = mouseY;
            }

            if (captured && !hitclicked && Engine::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
            {
                hitclicked = true;

                {
                    TemporaryAudioSource *tmpsrc = s->CreateObject<TemporaryAudioSource>();
                    tmpsrc->SetParent(cam, false);
                    tmpsrc->SetSourceFloat(AL_MAX_DISTANCE, 1);
                    tmpsrc->SetSourceFloat(AL_REFERENCE_DISTANCE, 1);
                    tmpsrc->SetSourceFloat(AL_GAIN, 0.3);
                    tmpsrc->SetCurrentClip(misssfx);
                    tmpsrc->Play();
                }
                
                Transform camt = cam->GetGlobalTransform();
                s->Raycast(camt.GetPosition(), camt.GetPosition() + camt.GetFront() * 2.0f, [&](RaycastInfo info) -> RaycastCallbackState
                {
                    std::cout << Utils::tostring(info.point) << std::endl;

                    //glm::vec3 rot = Utils::angles(-info.normal, glm::radians(45.0f));
                    const glm::vec3 forward = glm::vec3(0, 0, -1);

                    glm::quat rot = glm::angleAxis(glm::acos(glm::dot(info.normal, forward)), Utils::normalize(glm::cross(forward, info.normal)));
                    glm::vec3 offset = info.normal * (0.01f + ((rand() % 101) / 100.0f - 0.5f) * 0.005f);
                    Decal *d = s->CreateObject<Decal>(Transform(info.point + offset, rot, glm::vec3(0.3)), 60);
                    d->fadeoutstart = 50;
                    d->usedShaderProgram = "default";
                    s->SetObjectOrder(d, 1);
                    d->SetParent(info.rigidbody, true);
                    
                    Surface sf;
                    sf.mesh = "decal";
                    sf.texture = (rand() & 1) ? "bullethole1" : "bullethole2";
                    //s.texture = "crowbar_cyl";
                    d->surfaces.push_back(sf);

                    TemporaryAudioSource *tmpsrc = s->CreateObject<TemporaryAudioSource>();
                    tmpsrc->SetParent(cam, false);
                    tmpsrc->SetSourceFloat(AL_MAX_DISTANCE, 1);
                    tmpsrc->SetSourceFloat(AL_REFERENCE_DISTANCE, 1);
                    tmpsrc->SetSourceFloat(AL_GAIN, 0.3);
                    tmpsrc->SetCurrentClip((rand() % 2) ? hit1sfx : hit2sfx);
                    tmpsrc->Play();

                    info.rigidbody->ApplyGlobalForceAtGlobalPoint(-2000.0f * info.normal, info.point);

                    //if (info.rigidbody->tags.contains("Maxwell the Cat")) s->DeleteObject(info.rigidbody);

                    return STOP;
                });
            }
            else if ((captured && hitclicked && !Engine::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) || !captured)) hitclicked = false;

            glm::vec3 oldplpos = playerrb->GetGlobalTransform().GetPosition();

            Engine::Update(delta);

            glm::vec3 deltaplpos = playerrb->GetGlobalTransform().GetPosition() - oldplpos;

            static float cwoffset = 0;
            cwoffset += glm::length(Utils::normalize(deltaplpos)) * glm::radians(11.25f / 4);
            cwoffset = glm::mod(cwoffset, glm::radians(360.0f));

            ent->transform.SetRotation(crowbar_rot + glm::vec3(glm::sin(cwoffset) * 0.03f, 0.0f, 0.0f));

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Engine::Render();

            Engine::BeginRenderUI();

            {
                std::ostringstream oss;
                oss << "FPS: " << ups;
                ImGUI::GetForegroundDrawList()->AddText(ImVec2(10, 10), IM_COL32(255, 255, 255, 255), oss.str().c_str());
            }

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

            if (ImGUI::Button("Start music")) music->Play();

            if (ImGUI::Button("Spawn new TemporaryAudioSource"))
            {
                TemporaryAudioSource *tmpsrc = s->CreateObject<TemporaryAudioSource>();
                tmpsrc->SetParent(cam, false);

                tmpsrc->SetSourceFloat(AL_GAIN, 0.2);

                tmpsrc->SetCurrentClip(ResourceManager::GetAudioClip("zapsfx"));
                tmpsrc->Play();
            }

            //ImGUI::Checkbox("Yellow cube gravity", &enablegravity);

            //if (ImGUI::Button("Enable yellow cube gravity")) cube2->SetGravityEnabled(true);
            //if (ImGUI::Button("Disable yellow cube gravity")) cube2->SetGravityEnabled(false);

            if (ImGUI::Button("Reset yellow cube velocity")) cube2->SetLinearVelocity(glm::vec3(0.0f));

            if (ImGUI::Button("Spawn Maxwell the Cat")) s->CreateObject<MaxwellCat>(Transform(playerrb->transform.GetPosition()));

            ImGUI::End();

            Engine::EndRenderUI();

            glfwSwapBuffers(window);

            loops++;
        }
    }

    Engine::Shutdown();

    return 0;
}

const float DEFAULT_CAMERA_SPEED = 3;
float cameraSpeed = DEFAULT_CAMERA_SPEED;

void scrollCallback(GLFWwindow *w, double xoff, double yoff)
{
    cameraSpeed += yoff / 5;
    if (cameraSpeed < 0) cameraSpeed = 0;
}

void updateCam(GLFWwindow *window, Camera *cam, double delta, Model *crowbar)
{
    static float cwoffset = 0;

    float speed;
    if (Engine::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) speed = cameraSpeed * 2.0;
    else if (Engine::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) speed = cameraSpeed / 2.0;
    else speed = cameraSpeed;

    Transform *t = &cam->transform;
    glm::vec3 old_pos = t->GetPosition();

    //if (Engine::IsKeyPressed(GLFW_KEY_W)) { t->Translate(t->GetFront() * glm::vec3(speed * delta)); }
    //if (Engine::IsKeyPressed(GLFW_KEY_S)) { t->Translate(-t->GetFront() * glm::vec3(speed * delta)); }
    //if (Engine::IsKeyPressed(GLFW_KEY_A)) { t->Translate(-t->GetRight() * glm::vec3(speed * delta)); }
    //if (Engine::IsKeyPressed(GLFW_KEY_D)) { t->Translate(t->GetRight() * glm::vec3(speed * delta)); }

    //if (Engine::IsKeyPressed(GLFW_KEY_SPACE)) t->Translate(glm::vec3(0, speed * delta, 0));
    //if (Engine::IsKeyPressed(GLFW_KEY_LEFT_ALT)) t->Translate(glm::vec3(0, -speed * delta, 0));

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