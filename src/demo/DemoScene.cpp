#include "DemoScene.hpp"

#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <sstream>

#include "Button.hpp"
#include "Decal.hpp"
#include "MaxwellCat.hpp"

#include "engine/Application.hpp"
#include "engine/Console.hpp"
#include "engine/DebugOverlay.hpp"
#include "engine/Engine.hpp"
#include "engine/Logger.hpp"
#include "engine/Timer.hpp"
#include "engine/Input.hpp"
#include "engine/Input.hpp"
#include "engine/Settings.hpp"
#include "engine/Window.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/Scene.hpp"
#include "engine/Time.hpp"
#include "engine/Utils.hpp"
#include "engine/audio/AudioEffectProperties.hpp"
#include "engine/external/imgui.hpp"
#include "engine/objects/AudioListener/AudioListener.hpp"
#include "engine/objects/AudioSource/AudioSource.hpp"
#include "engine/objects/Camera/Camera.hpp"
#include "engine/objects/Entity/Entity.hpp"
#include "engine/objects/Model/Model.hpp"
#include "engine/objects/Model/Surface.hpp"
#include "engine/objects/RigidBody/RigidBody.hpp"
#include "engine/objects/TemporaryAudioSource/TemporaryAudioSource.hpp"
#include "engine/objects/Light/DirectionalLight.hpp"
#include "engine/objects/Light/PointLight.hpp"
#include "engine/objects/ParticleEmitter/ParticleEmitter.hpp"
#include "engine/physics/CollisionLayers.hpp"
#include "engine/physics/RaycastCallbackState.hpp"
#include "engine/physics/RaycastInfo.hpp"
#include "engine/physics/colliders/colliders.hpp"
#include "engine/objects/RigidBody/RigidBody.hpp"
#include "engine/resources/AudioClip.hpp"
#include "engine/resources/Mesh.hpp"
#include "engine/resources/ShaderProgram.hpp"
#include "engine/resources/Texture.hpp"

static void applyEAXReverbToSlot(const EAXReverbEffectSettings *setts, AudioEffectSlot *slot)
{
    AudioEffectProperties p;
    p.SetEffectType(AL_EFFECT_EAXREVERB);

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

bool DemoScene::loadResources()
{
    ShaderProgram *sh = ResourceManager::CreateShaderProgram("default");
    std::string log;
    std::string src;

    if (Utils::ReadTextFile("./res/default.v.glsl", &src)) printf("read vertex shader source\n");
    sh->LoadVertexShader(src);
    if (!sh->CompileVertexShader(&log)) std::cout << "Error compiling vertex shader: " << std::endl << log << std::endl;

    if (Utils::ReadTextFile("./res/default.f.glsl", &src)) printf("read fragment shader source\n");
    sh->LoadFragmentShader(src);
    if (!sh->CompileFragmentShader(&log)) std::cout << "Error compiling fragment shader: " << std::endl << log << std::endl;

    if (!sh->LinkShaderProgram(&log)) std::cout << "Error linking shader program: " << std::endl << log << std::endl;

    if (ResourceManager::CreateMesh("crowbar_cyl")->LoadFromObjFile("./res/models/cyl.obj")) printf("loaded model crowbar_cyl\n");
    if (ResourceManager::CreateMesh("crowbar_head")->LoadFromObjFile("./res/models/head.obj")) printf("loaded model crowbar_head\n");
    if (ResourceManager::CreateTexture("crowbar_cyl")->LoadFromTextureFile("./res/textures/cyl.png")) printf("loaded texture crowbar_cyj\n");
    if (ResourceManager::CreateTexture("crowbar_head")->LoadFromTextureFile("./res/textures/head.png")) printf("loaded texture crowbar_head\n");
    if (ResourceManager::CreateMesh("cube")->LoadFromObjFile("./res/models/cube.obj")) printf("loaded model cube\n");
    if (ResourceManager::CreateMesh("hl1_reactor_demo")->LoadFromObjFile("./res/models/hl1_reactor_demo.obj")) printf("loaded model hl1_reactor_demo\n");
    if (ResourceManager::CreateMesh("sphere")->LoadFromObjFile("./res/models/sphere.obj")) puts("loaded model sphere");
    if (ResourceManager::CreateMesh("decal")->LoadFromObjFile("./res/models/decal.obj")) puts("loaded model decal");
    if (ResourceManager::CreateMesh("button_3")->LoadFromObjFile("./res/models/buttons/3.obj")) puts("loaded button 3 model");
    if (ResourceManager::CreateMesh("button_4")->LoadFromObjFile("./res/models/buttons/4.obj")) puts("loaded button 4 model");
    if (ResourceManager::CreateTexture("button_3_on")->LoadFromTextureFile("./res/textures/buttons/3_on.png")) puts("loaded button 3 on texture");
    if (ResourceManager::CreateTexture("button_3_off")->LoadFromTextureFile("./res/textures/buttons/3_off.png")) puts("loaded button 3 off texture");
    if (ResourceManager::CreateTexture("button_4_on")->LoadFromTextureFile("./res/textures/buttons/4_on.png")) puts("loaded button 4 on texture");
    if (ResourceManager::CreateTexture("button_4_off")->LoadFromTextureFile("./res/textures/buttons/4_off.png")) puts("loaded button 4 off texture");
    if (ResourceManager::CreateTexture("bullethole1")->LoadFromTextureFile("./res/textures/bullethole1.png")) puts("loaded bullethole1 texture");
    if (ResourceManager::CreateTexture("bullethole2")->LoadFromTextureFile("./res/textures/bullethole2.png")) puts("loaded bullethole2 texture");
    if (ResourceManager::CreateTexture("maxwellcat")->LoadFromTextureFile("./res/textures/maxwellcat.png")) puts("loaded maxwellcat texture");
    if (ResourceManager::CreateMesh("maxwellcat")->LoadFromObjFile("./res/models/maxwellcat.obj")) puts("loaded maxwellcat model");

    if (ResourceManager::CreateAudioClip("zapsfx")->LoadFromAudioFile("./res/sounds/zapmachine.wav")) printf("loaded zapmachine sound\n");
    if (ResourceManager::CreateAudioClip("alienbuildersfx")->LoadFromAudioFile("./res/sounds/alien_builder.wav")) printf("loaded alienbuilder sound\n");
    if (ResourceManager::CreateAudioClip("freightmove1sfx")->LoadFromAudioFile("./res/sounds/freightmove1.wav")) puts("loaded freightmove1 sound");
    hit1sfx = ResourceManager::CreateAudioClip("hit1sfx");
    if (hit1sfx->LoadFromAudioFile("./res/sounds/hit_1.wav")) puts("loaded hit_1 sound");
    hit2sfx = ResourceManager::CreateAudioClip("hit2sfx");
    if (hit2sfx->LoadFromAudioFile("./res/sounds/hit_2.wav")) puts("loaded hit_2 sound");
    misssfx = ResourceManager::CreateAudioClip("misssfx");
    if (misssfx->LoadFromAudioFile("./res/sounds/miss.wav")) puts("loaded miss sound");
    if (ResourceManager::CreateAudioClip("button8sfx")->LoadFromAudioFile("./res/sounds/buttons/8.wav")) puts("loaded button 8 sound");
    if (ResourceManager::CreateAudioClip("mus_maxwellcat")->LoadFromAudioFile("./res/music/maxwellcat.ogg")) puts("loaded maxwellcat music");
    if (ResourceManager::CreateAudioClip("mus_mech8")->LoadFromAudioFile("./res/music/Mech8.ogg")) puts("loaded Mech8 music");

    return true;
}

void DemoScene::buildScene()
{
    scene = Engine::CreateScene("main");
    Engine::SetCurrentScene("main");

    scene->fog.enabled = true;
    scene->fog.startDistance = 0;
    scene->fog.endDistance = 32;
    scene->fog.color = glm::vec3(106 / 255.0f, 117 / 255.0f, 129 / 255.0f);
    scene->ambientLight = glm::vec3(0.08f, 0.09f, 0.12f);

    sunLight = scene->CreateObject<DirectionalLight>();
    sunLight->transform.SetRotation(glm::quat(glm::radians(glm::vec3(-35.0f, 45.0f, 0.0f))));
    sunLight->color = glm::vec3(1.0f, 0.95f, 0.85f);
    sunLight->intensity = 1.1f;
    sunLight->tags.insert("sun");

    camera = scene->CreateObject<Camera>();
    camera->FOV = glm::radians(70.0f);
    scene->SetCurrentCamera(camera);

    music = scene->CreateObject<AudioSource>();
    music->SetParent(camera, false);
    music->SetSourceFloat(AL_REFERENCE_DISTANCE, 1);
    music->SetSourceFloat(AL_MAX_DISTANCE, 1);
    music->SetSourceFloat(AL_GAIN, 0.2);
    music->SetLooping(true);
    music->SetCurrentClip(ResourceManager::GetAudioClip("mus_mech8"));

    AudioListener *listener = scene->CreateObject<AudioListener>();
    listener->SetParent(camera, false);

    crowbar = scene->CreateObject<Model>();
    crowbar->SetParent(camera, false);
    crowbar->usedShaderProgram = "default";
    crowbar->transform = Transform(glm::vec3(0.0933556f, -0.160361f, -0.179554f), crowbarRot, glm::vec3(0.01f));
    crowbar->enableDepthTest = false;
    scene->SetObjectOrder(crowbar, 101);

    cube = scene->CreateObject<Model>(Transform({0, 0, -5}, glm::quat(glm::vec3(0)), glm::vec3(0.1f)));
    cube->usedShaderProgram = "default";
    cube->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    cube2 = scene->CreateObject<Entity>(Transform({-2.5f, 0, -2.5f}));
    cube2->usedShaderProgram = "default";
    cube2->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    cube2->SetRigidBodyType(DYNAMIC);
    cube2->SetGravityEnabled(true);
    cube2->SetMass(10);
    BoxCollider *cube2coll = cube2->AddCollider<BoxCollider>(Transform(), glm::vec3(1));
    cube2coll->SetFrictionCoefficient(1);

    ground = scene->CreateObject<Entity>(Transform({-2.5f, -10, -2.5f}, glm::quat(glm::radians(glm::vec3(10, 0, 0)))));
    ground->transform.SetScale({10, 0.5f, 10});
    ground->usedShaderProgram = "default";
    ground->color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    BoxCollider *groundc = ground->AddCollider<BoxCollider>(Transform(), glm::vec3(10, 0.5f, 10));
    groundc->SetFrictionCoefficient(1);
    groundc->SetBounciness(0);
    groundc->SetCollisionCategoryBits(CollisionLayer::World);
    groundc->SetCollideWithMaskBits(CollisionLayer::All);

    groundSource = scene->CreateObject<AudioSource>();
    groundSource->SetParent(ground, false);
    groundSource->SetSourceFloat(AL_GAIN, 0.1f);
    groundSource->SetSourceFloat(AL_MAX_DISTANCE, 16);
    groundSource->SetSourceFloat(AL_REFERENCE_DISTANCE, 10);
    groundSource->SetLooping(true);
    groundSource->SetCurrentClip(ResourceManager::GetAudioClip("freightmove1sfx"));

    sphere = scene->CreateObject<Entity>(Transform({-2.5f, 3, -2.5f}));
    sphere->usedShaderProgram = "default";
    sphere->SetRigidBodyType(DYNAMIC);
    SphereCollider *sphcoll = sphere->AddCollider<SphereCollider>(Transform(), 1);
    sphcoll->SetFrictionCoefficient(1);
    sphcoll->SetCollisionCategoryBits(CollisionLayer::Default);
    sphcoll->SetCollideWithMaskBits(CollisionLayer::All);

    hl1Reactor = scene->CreateObject<Model>();
    hl1Reactor->usedShaderProgram = "default";
    hl1Reactor->transform.SetPosition({-15, 0, 0});

    applyEAXReverbToSlot(&reverbSettings, &reverb);

    zapSource = scene->CreateObject<AudioSource>();
    reverb.AddSource(zapSource);
    zapSource->SetParent(cube2, false);
    zapSource->SetCurrentClip(ResourceManager::GetAudioClip("zapsfx"));
    zapSource->SetLooping(true);

    zapLight = scene->CreateObject<PointLight>();
    zapLight->SetParent(cube2, false);
    zapLight->transform.SetPosition(glm::vec3(0, 0.5f, 0));
    zapLight->color = glm::vec3(0.3f, 0.7f, 1.0f);
    zapLight->intensity = 2.5f;
    zapLight->range = 10.0f;
    zapLight->tags.insert("interactive-light");

    sparks = scene->CreateObject<ParticleEmitter>(Transform({-2.5f, 0.5f, -2.5f}));
    sparks->spawnRate = 55.0f;
    sparks->velocityMin = glm::vec3(-0.3f, 1.5f, -0.3f);
    sparks->velocityMax = glm::vec3(0.3f, 3.5f, 0.3f);
    sparks->colorStart = glm::vec4(1.0f, 0.85f, 0.2f, 1.0f);
    sparks->colorEnd = glm::vec4(0.8f, 0.1f, 0.0f, 0.0f);
    sparks->tags.insert("fx");

    playerBody = scene->CreateObject<RigidBody>(Transform({-2.5f, 1, -2.5f}));
    CapsuleCollider *playercoll = playerBody->AddCollider<CapsuleCollider>(Transform(), 0.5f, 1.5f);
    playercoll->SetFrictionCoefficient(0.5f);
    playercoll->SetBounciness(0);
    playercoll->SetCollisionCategoryBits(CollisionLayer::Player);
    playercoll->SetCollideWithMaskBits(CollisionLayer::World | CollisionLayer::Interactable | CollisionLayer::Default | CollisionLayer::Projectile);
    playerBody->SetAngularLockAxisFactor({0, 1, 0});
    playerBody->SetRigidBodyType(DYNAMIC);
    playerBody->SetGravityEnabled(true);
    playerBody->SetMass(70);
    camera->SetParent(playerBody, false);
    camera->transform.SetPosition({0, 0.5f, 0});

    player.Init(scene, playerBody, camera, crowbar, crowbarRot);

    button = scene->CreateObject<Button>();
    button->SetParent(ground);
    button->transform.SetPosition({0, 2, 5});
    button->usedShaderProgram = "default";
    button->model = "button_4";
    button->textureoff = "button_4_off";
    button->textureon = "button_4_on";
    button->togglesfx = "button8sfx";
    button->SetButtonState(false, false);

    Surface surf;
    surf.mesh = "crowbar_cyl";
    surf.texture = "crowbar_cyl";
    crowbar->surfaces.push_back(surf);

    surf.mesh = "crowbar_head";
    surf.texture = "crowbar_head";
    crowbar->surfaces.push_back(surf);

    surf.mesh = "cube";
    surf.texture = "testcube";
    cube->surfaces.push_back(surf);
    cube2->surfaces.push_back(surf);
    ground->surfaces.push_back(surf);

    surf.mesh = "hl1_reactor_demo";
    surf.texture = "";
    surf.culling = NoCulling;
    hl1Reactor->surfaces.push_back(surf);

    surf.mesh = "sphere";
    sphere->surfaces.push_back(surf);
}

void DemoScene::SetApplication(Application *app) { application = app; }

bool DemoScene::Load()
{
    if (!loadResources()) return false;

    buildScene();

    srand(static_cast<unsigned>(time(nullptr)));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(scene->fog.color.x, scene->fog.color.y, scene->fog.color.z, 1);

    Logger::Info("Demo scene loaded, objects: " + std::to_string(scene->GetObjectCount()));

    Timer::Every(5.0, [this]()
    {
        if (zapLight) zapLight->intensity = zapLight->intensity > 1.5f ? 0.8f : 2.8f;
    });

    return true;
}

void DemoScene::updateButtonPlatform()
{
    if (button->GetButtonState())
    {
        if (groundSource->GetState() != AudioSourceState::PLAYING) groundSource->Play();
        ground->SetLinearVelocity({1, 0, 0});
    }
    else
    {
        if (groundSource->GetState() != AudioSourceState::PAUSED) groundSource->Pause();
        ground->SetLinearVelocity(glm::vec3(0));
    }
}

void DemoScene::updateCrowbarTexture(double delta)
{
    crowbar->surfaces[0].textureTransform.Translate(glm::vec2(0, static_cast<float>(delta * 0.5)));
    crowbar->surfaces[0].textureTransform.Rotate(0.1 * delta);
}

void DemoScene::handleFire()
{
    if (Input::IsMouseCaptured() && !fireHeld && Input::IsActionDown("Fire"))
    {
        fireHeld = true;

        bool swing = true;
        Transform camt = camera->GetGlobalTransform();

        scene->Raycast(camt.GetPosition(), camt.GetPosition() + camt.GetFront() * 2.5f, [&](RaycastInfo info) -> RaycastCallbackState
        {
            if (Button *btn = dynamic_cast<Button *>(info.rigidbody))
            {
                swing = false;
                btn->ToggleButtonState();
            }
            else
            {
                const glm::vec3 forward = glm::vec3(0, 0, -1);
                (void)dynamic_cast<MaxwellCat *>(info.rigidbody);

                glm::quat rot = glm::angleAxis(glm::acos(glm::dot(info.normal, forward)), Utils::normalize(glm::cross(forward, info.normal)));
                glm::vec3 offset = info.normal * (0.01f + ((rand() % 101) / 100.0f - 0.5f) * 0.005f);
                Decal *d = scene->CreateObject<Decal>(Transform(info.point + offset, rot, glm::vec3(0.3f)), 60);
                d->fadeoutstart = 50;
                d->usedShaderProgram = "default";
                scene->SetObjectOrder(d, 1);
                d->SetParent(info.rigidbody, true);

                Surface sf;
                sf.mesh = "decal";
                sf.texture = (rand() & 1) ? "bullethole1" : "bullethole2";
                d->surfaces.push_back(sf);

                TemporaryAudioSource *tmpsrc = scene->CreateObject<TemporaryAudioSource>();
                tmpsrc->SetParent(camera, false);
                tmpsrc->SetSourceFloat(AL_MAX_DISTANCE, 1);
                tmpsrc->SetSourceFloat(AL_REFERENCE_DISTANCE, 1);
                tmpsrc->SetSourceFloat(AL_GAIN, 0.3f);
                tmpsrc->SetCurrentClip((rand() % 2) ? hit1sfx : hit2sfx);
                tmpsrc->Play();

                info.rigidbody->ApplyGlobalForceAtGlobalPoint(-2000.0f * info.normal, info.point);
            }

            return STOP;
        });

        if (swing)
        {
            TemporaryAudioSource *tmpsrc = scene->CreateObject<TemporaryAudioSource>();
            tmpsrc->SetParent(camera, false);
            tmpsrc->SetSourceFloat(AL_MAX_DISTANCE, 1);
            tmpsrc->SetSourceFloat(AL_REFERENCE_DISTANCE, 1);
            tmpsrc->SetSourceFloat(AL_GAIN, 0.3f);
            tmpsrc->SetCurrentClip(misssfx);
            tmpsrc->Play();
        }
    }
    else if ((Input::IsMouseCaptured() && fireHeld && !Input::IsActionDown("Fire")) || !Input::IsMouseCaptured())
    {
        fireHeld = false;
    }
}

void DemoScene::applyReverbEffect()
{
    applyEAXReverbToSlot(&reverbSettings, &reverb);
}

void DemoScene::Update(double delta)
{
    if (Input::IsKeyJustPressed(GLFW_KEY_P) && application) application->SetPaused(!application->IsPaused());

    if (Time::GetTime() >= fpsTimer + 1)
    {
        displayedFps = frameCounter;
        DebugOverlay::SetFps(static_cast<unsigned int>(displayedFps));
        frameCounter = 0;
        fpsTimer = Time::GetTime();
    }

    updateCrowbarTexture(delta);
    updateButtonPlatform();

    player.ProcessInput(delta);
    const glm::vec3 posBefore = player.GetPosition();
    handleFire();
    Engine::Update(delta);
    player.SyncViewmodel(posBefore);
}

void DemoScene::renderWindowSettings()
{
    WindowSettings &ws = Engine::GetWindowSettings();

    static char titleBuffer[128] = "Evelent Engine";
    if (ImGUI::IsWindowAppearing()) strncpy(titleBuffer, ws.title.c_str(), sizeof(titleBuffer) - 1);

    ImGUI::InputText("Title", titleBuffer, sizeof(titleBuffer));
    ws.title = titleBuffer;

    int width = static_cast<int>(ws.width);
    int height = static_cast<int>(ws.height);
    ImGUI::InputInt("Width", &width, 0, 0);
    ImGUI::InputInt("Height", &height, 0, 0);
    if (width < ws.minWidth) width = ws.minWidth;
    if (height < ws.minHeight) height = ws.minHeight;
    ws.width = static_cast<unsigned int>(width);
    ws.height = static_cast<unsigned int>(height);

    ImGUI::Checkbox("VSync", &ws.vsync);
    ImGUI::Checkbox("Resizable", &ws.resizable);
    ImGUI::Checkbox("Decorated", &ws.decorated);
    ImGUI::Checkbox("Fullscreen", &ws.fullscreen);
    ImGUI::Checkbox("Maximize on start", &ws.maximizeOnStart);

    ImGUI::SliderInt("MSAA", &ws.msaaSamples, 0, 8);
    ImGUI::SliderInt("Min width", &ws.minWidth, 160, 1920);
    ImGUI::SliderInt("Min height", &ws.minHeight, 120, 1080);
    ImGUI::SliderInt("Target FPS", &ws.targetFPS, 30, 240);

    if (ImGUI::Button("Apply"))
    {
        Window::ApplyRuntimeSettings();
        Settings::GetWindow() = ws;
    }

    ImGUI::SameLine();

    if (ImGUI::Button("Save to settings.cfg"))
    {
        Settings::GetWindow() = ws;
        Settings::Save();
    }

    if (ImGUI::Button("Minimize")) Window::SetIconified(true);
    ImGUI::SameLine();
    if (ImGUI::Button("Restore")) Window::SetIconified(false);
    ImGUI::SameLine();
    if (ImGUI::Button("Focus")) Window::Focus();
}

void DemoScene::renderEnginePanel()
{
    ImGUI::Text("Scene objects: %zu", scene->GetObjectCount());
    ImGUI::Text("Tagged FX: %zu", scene->FindObjectsWithTag("fx").size());

    float timeScale = Time::GetTimeScale();
    if (ImGUI::SliderFloat("Time scale", &timeScale, 0.0f, 3.0f)) Time::SetTimeScale(timeScale);

    if (application)
    {
        const bool paused = application->IsPaused();
        ImGUI::Text("Paused: %s", paused ? "yes" : "no");

        if (ImGUI::Button("Toggle pause (P)")) application->SetPaused(!paused);
        ImGUI::SameLine();
        if (ImGUI::Button("Step frame")) application->StepOneFrame();
    }

}

void DemoScene::renderPhysicsPanel()
{
    glm::vec3 gravity = scene->GetGravity();
    if (ImGUI::DragFloat3("Gravity", &gravity.x, 0.1f, -50.0f, 50.0f)) scene->SetGravity(gravity);

    if (sphere)
    {
        if (ImGUI::Button("Throw sphere up")) sphere->SetLinearVelocity(glm::vec3(0, 8, 0));

        ImGUI::SameLine();

        if (ImGUI::Button("Reset sphere"))
        {
            sphere->SetLinearVelocity(glm::vec3(0));
            sphere->transform.SetPosition({-2.5f, 3, -2.5f});
        }
    }

    if (playerBody)
    {
        if (ImGUI::Button("Teleport player to origin"))
        {
            playerBody->transform.SetPosition({-2.5f, 1.5f, -2.5f});
            playerBody->SetLinearVelocity(glm::vec3(0));
        }
    }
}

void DemoScene::RenderUI()
{
    const glm::vec3 pos = player.GetPosition();
    std::ostringstream oss;
    oss << "X: " << pos.x << ", Y: " << pos.y << ", Z: " << pos.z;
    ImGUI::GetForegroundDrawList()->AddText(ImVec2(10, 10), IM_COL32(255, 255, 255, 255), oss.str().c_str());

    DebugOverlay::Render();
    Console::Render();

    ImGUI::SetNextWindowSize(ImVec2(420, 140), ImGuiCond_FirstUseEver);
    ImGUI::Begin("Engine");
    renderEnginePanel();
    ImGUI::End();

    ImGUI::SetNextWindowSize(ImVec2(420, 200), ImGuiCond_FirstUseEver);
    ImGUI::Begin("Physics");
    renderPhysicsPanel();
    ImGUI::End();

    ImGUI::SetNextWindowSize(ImVec2(420, 380), ImGuiCond_FirstUseEver);
    ImGUI::Begin("Window");
    renderWindowSettings();
    ImGUI::End();

    ImGUI::SetNextWindowSize(ImVec2(600, 620), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    ImGUI::Begin("EAX Reverb. settings");

    ImGUI::SliderFloat("Density", &reverbSettings.density, 0, 1);
    ImGUI::SliderFloat("Diffusion", &reverbSettings.diffusion, 0, 1);
    ImGUI::Separator();
    ImGUI::SliderFloat("Gain", &reverbSettings.gain, 0, 1);
    ImGUI::SliderFloat("Gain HF", &reverbSettings.gainhf, 0, 1);
    ImGUI::SliderFloat("Gain LF", &reverbSettings.gainlf, 0, 1);
    ImGUI::Separator();
    ImGUI::SliderFloat("Decay Time", &reverbSettings.decaytime, 0, 20);
    ImGUI::SliderFloat("Decay HF Ratio", &reverbSettings.decayhfratio, 0.1f, 2);
    ImGUI::SliderFloat("Decay LF Ratio", &reverbSettings.decaylfratio, 0.1f, 2);
    ImGUI::Separator();
    ImGUI::SliderFloat("Reflections Gain", &reverbSettings.reflectionsgain, 0, 3.16f);
    ImGUI::SliderFloat("Reflections Delay", &reverbSettings.reflectionsdelay, 0, 0.3f);
    ImGUI::Separator();
    ImGUI::SliderFloat("Late Reverb. Gain", &reverbSettings.latereverbgain, 0, 10);
    ImGUI::SliderFloat("Late Reverb. Delay", &reverbSettings.latereverbdelay, 0, 0.1f);
    ImGUI::Separator();
    ImGUI::SliderFloat("Echo Time", &reverbSettings.echotime, 0.075f, 0.25f);
    ImGUI::SliderFloat("Echo Depth", &reverbSettings.echodepth, 0, 1);
    ImGUI::Separator();
    ImGUI::SliderFloat("Modulation Time", &reverbSettings.modulationtime, 0.04f, 4);
    ImGUI::SliderFloat("Modulation Depth", &reverbSettings.modulationdepth, 0, 1);
    ImGUI::Separator();
    ImGUI::SliderFloat("HF Reference", &reverbSettings.hfreference, 1000, 20000);
    ImGUI::SliderFloat("LF Reference", &reverbSettings.lfreference, 20, 1000);
    ImGUI::Separator();
    ImGUI::SliderFloat("Room Rolloff Factor", &reverbSettings.roomrollofffactor, 0, 10);
    ImGUI::SliderFloat("Air Absorption Gain HF", &reverbSettings.airabsorptiongainhf, 0.892f, 1);
    ImGUI::Checkbox("Decay HF Limit", &reverbSettings.decayhf_limit);
    ImGUI::Separator();
    ImGUI::Checkbox("Auto apply effect", &autoApplyReverb);
    if (autoApplyReverb) applyReverbEffect();
    else if (ImGUI::Button("Apply effect")) applyReverbEffect();

    ImGUI::End();

    ImGUI::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_FirstUseEver);
    ImGUI::Begin("AudioSource control");

    if (ImGUI::Button("Play/Resume")) zapSource->Play();
    ImGUI::SameLine();
    if (ImGUI::Button("Pause")) zapSource->Pause();
    ImGUI::SameLine();
    if (ImGUI::Button("Stop")) zapSource->Stop();
    ImGUI::SameLine();
    if (ImGUI::Button("Rewind")) zapSource->Rewind();

    ImGUI::SliderFloat("Pitch", &pitch, 0, 2);
    ImGUI::SliderFloat("Gain", &gain, 0, 1);
    ImGUI::DragFloat("Reference Distance", &refdist, 0.1f, 0, std::numeric_limits<float>().max());
    ImGUI::DragFloat("Max Distance", &maxdist, 0.1f, 0, std::numeric_limits<float>().max());

    zapSource->SetSourceFloat(AL_PITCH, pitch);
    zapSource->SetSourceFloat(AL_GAIN, gain);
    zapSource->SetSourceFloat(AL_REFERENCE_DISTANCE, refdist);
    zapSource->SetSourceFloat(AL_MAX_DISTANCE, maxdist);

    ImGUI::End();

    ImGUI::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
    ImGUI::Begin("Test functions");

    if (ImGUI::Button("Start music")) music->Play();

    if (ImGUI::Button("Spawn new TemporaryAudioSource"))
    {
        TemporaryAudioSource *tmpsrc = scene->CreateObject<TemporaryAudioSource>();
        tmpsrc->SetParent(camera, false);
        tmpsrc->SetSourceFloat(AL_GAIN, 0.2f);
        tmpsrc->SetCurrentClip(ResourceManager::GetAudioClip("zapsfx"));
        tmpsrc->Play();
    }

    if (ImGUI::Button("Reset yellow cube velocity")) cube2->SetLinearVelocity(glm::vec3(0.0f));

    if (ImGUI::Button("Spawn Maxwell the Cat"))
        scene->CreateObject<MaxwellCat>(Transform(player.GetBody()->transform.GetPosition()));

    if (sparks && ImGUI::Button("Toggle sparks"))
    {
        sparks->playing = !sparks->playing;
        if (!sparks->playing) sparks->Clear();
    }

    ImGUI::End();
}

void DemoScene::OnFramePresented()
{
    frameCounter++;
}

unsigned long DemoScene::GetDisplayedFps() const
{
    return displayedFps;
}
