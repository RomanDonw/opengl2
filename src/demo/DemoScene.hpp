#ifndef DEMOSCENE_HPP
#define DEMOSCENE_HPP

#include "engine/external/glm.hpp"
#include "engine/external/openal.hpp"
#include "engine/audio/AudioEffectSlot.hpp"
#include "engine/objects/PlayerController/PlayerController.hpp"

class Application;
class RigidBody;

class Scene;
class Camera;
class Model;
class Entity;
class AudioSource;
class Button;
class AudioClip;
class DirectionalLight;
class PointLight;
class ParticleEmitter;

struct EAXReverbEffectSettings
{
    float density = 1;
    float diffusion = 1;
    float gain = 0.32f;
    float gainhf = 0.89f;
    float gainlf = 0;
    float decaytime = 1.49f;
    float decayhfratio = 0.83f;
    float decaylfratio = 1;
    float reflectionsgain = 0.05f;
    float reflectionsdelay = 0.007f;
    float latereverbgain = 1.26f;
    float latereverbdelay = 0.011f;
    float echotime = 0.25f;
    float echodepth = 0;
    float modulationtime = 0.25f;
    float modulationdepth = 0;
    float airabsorptiongainhf = 0.994f;
    float hfreference = 5000;
    float lfreference = 250;
    float roomrollofffactor = 0;
    bool decayhf_limit = true;
};

class DemoScene
{
    private:
        static inline const glm::vec3 crowbarRot = glm::vec3(-3.04567f, -0.648789f, 3.12098f);

        Application *application = nullptr;
        Scene *scene = nullptr;
        PlayerController player;

        Camera *camera = nullptr;
        RigidBody *playerBody = nullptr;
        Model *crowbar = nullptr;
        Model *cube = nullptr;
        Entity *cube2 = nullptr;
        Entity *ground = nullptr;
        Entity *sphere = nullptr;
        Model *hl1Reactor = nullptr;
        AudioSource *groundSource = nullptr;
        AudioSource *zapSource = nullptr;
        AudioSource *music = nullptr;
        Button *button = nullptr;
        DirectionalLight *sunLight = nullptr;
        PointLight *zapLight = nullptr;
        ParticleEmitter *sparks = nullptr;

        AudioClip *hit1sfx = nullptr;
        AudioClip *hit2sfx = nullptr;
        AudioClip *misssfx = nullptr;

        AudioEffectSlot reverb;
        EAXReverbEffectSettings reverbSettings;
        bool autoApplyReverb = false;
        float pitch = 1;
        float gain = 1;
        float refdist = 8;
        float maxdist = 32;

        bool fireHeld = false;
        unsigned long frameCounter = 0;
        unsigned long displayedFps = 0;
        double fpsTimer = 0;

        bool loadResources();
        void buildScene();
        void updateButtonPlatform();
        void updateCrowbarTexture(double delta);
        void handleFire();
        void applyReverbEffect();
        void renderWindowSettings();
        void renderEnginePanel();
        void renderPhysicsPanel();

    public:
        void SetApplication(Application *app);
        bool Load();
        void Update(double delta);
        void RenderUI();
        void OnFramePresented();

        unsigned long GetDisplayedFps() const;
};

#endif
