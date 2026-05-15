#include "SceneSerializer.hpp"

#include "EditorProject.hpp"
#include "engine/Engine.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/Scene.hpp"
#include "engine/Utils.hpp"
#include "engine/objects/Camera/Camera.hpp"
#include "engine/objects/FreeplayCamera/FreeplayCamera.hpp"
#include "engine/objects/Entity/Entity.hpp"
#include "engine/objects/Light/DirectionalLight.hpp"
#include "engine/objects/Light/PointLight.hpp"
#include "engine/objects/Model/Model.hpp"
#include "engine/objects/Model/Surface.hpp"
#include "engine/objects/ScriptBehaviour/ScriptBehaviour.hpp"
#include "engine/physics/CollisionLayers.hpp"
#include "engine/physics/colliders/colliders.hpp"
#include "engine/Logger.hpp"

#include <sstream>
#include <vector>

static bool isEditorInternalObject(GameObject *obj)
{
    return obj && obj->displayName == "__EditorCamera__";
}

static glm::quat quatFromSceneEuler(const glm::vec3 &degrees)
{
    return glm::angleAxis(glm::radians(degrees.y), glm::vec3(0.0f, 1.0f, 0.0f))
         * glm::angleAxis(glm::radians(degrees.x), glm::vec3(1.0f, 0.0f, 0.0f));
}

static std::string esc(const std::string &s)
{
    std::string out;
    for (char c : s)
    {
        if (c == '\\' || c == '"') out.push_back('\\');
        out.push_back(c);
    }
    return out;
}

static std::string typeNameOf(GameObject *obj)
{
    if (dynamic_cast<ScriptBehaviour *>(obj)) return "ScriptBehaviour";
    if (dynamic_cast<FreeplayCamera *>(obj)) return "FreeplayCamera";
    if (dynamic_cast<Camera *>(obj)) return "Camera";
    if (dynamic_cast<PointLight *>(obj)) return "PointLight";
    if (dynamic_cast<DirectionalLight *>(obj)) return "DirectionalLight";
    if (dynamic_cast<Entity *>(obj)) return "Entity";
    if (dynamic_cast<Model *>(obj)) return "Model";
    return "GameObject";
}

bool SceneSerializer::Save(Scene *scene, const std::string &path, const EditorProject *project)
{
    if (!scene) return false;

    std::ostringstream oss;
    oss << "# Evelent Engine scene\n";
    oss << "ambient " << scene->ambientLight.x << " " << scene->ambientLight.y << " " << scene->ambientLight.z << "\n";
    oss << "fog " << (scene->fog.enabled ? 1 : 0) << " " << scene->fog.startDistance << " " << scene->fog.endDistance
        << " " << scene->fog.color.x << " " << scene->fog.color.y << " " << scene->fog.color.z << "\n";

    scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (isEditorInternalObject(obj)) return true;
        if (dynamic_cast<ScriptBehaviour *>(obj)) return true;

        const Transform gt = obj->GetGlobalTransform();
        const glm::vec3 euler = glm::degrees(glm::eulerAngles(gt.GetRotation()));

        oss << "object \"" << esc(obj->displayName) << "\"\n";
        oss << "  type " << typeNameOf(obj) << "\n";
        oss << "  pos " << gt.GetPosition().x << " " << gt.GetPosition().y << " " << gt.GetPosition().z << "\n";
        oss << "  rot " << euler.x << " " << euler.y << " " << euler.z << "\n";
        oss << "  scale " << gt.GetScale().x << " " << gt.GetScale().y << " " << gt.GetScale().z << "\n";

        if (GameObject *p = obj->GetParent()) oss << "  parent \"" << esc(p->displayName) << "\"\n";

        if (Model *m = dynamic_cast<Model *>(obj))
        {
            if (!m->surfaces.empty())
            {
                oss << "  mesh " << m->surfaces[0].mesh << "\n";
                oss << "  texture " << m->surfaces[0].texture << "\n";
            }
            oss << "  color " << m->color.r << " " << m->color.g << " " << m->color.b << " " << m->color.a << "\n";
        }

        if (Entity *e = dynamic_cast<Entity *>(obj))
        {
            oss << "  body " << (e->GetRigidBodyType() == DYNAMIC ? "dynamic" : "kinematic") << "\n";
            oss << "  mass " << e->GetMass() << "\n";
        }

        if (Camera *c = dynamic_cast<Camera *>(obj)) oss << "  fov " << glm::degrees(c->FOV) << "\n";

        if (PointLight *pl = dynamic_cast<PointLight *>(obj))
        {
            oss << "  light_color " << pl->color.x << " " << pl->color.y << " " << pl->color.z << "\n";
            oss << "  light_intensity " << pl->intensity << "\n";
            oss << "  light_range " << pl->range << "\n";
        }

        if (DirectionalLight *dl = dynamic_cast<DirectionalLight *>(obj))
        {
            oss << "  light_color " << dl->color.x << " " << dl->color.y << " " << dl->color.z << "\n";
            oss << "  light_intensity " << dl->intensity << "\n";
        }

        for (GameObject *child : obj->GetChildren())
        {
            if (ScriptBehaviour *sb = dynamic_cast<ScriptBehaviour *>(child))
            {
                if (project && !sb->GetScriptFile().empty())
                {
                    project->WriteTextFile(sb->GetScriptFile(), sb->GetSource());
                    oss << "  script_file \"" << esc(sb->GetScriptFile()) << "\"\n";
                }
                else if (!sb->GetScriptFile().empty())
                {
                    oss << "  script_file \"" << esc(sb->GetScriptFile()) << "\"\n";
                }
                else
                {
                    oss << "  script_begin\n";
                    for (char c : sb->GetSource()) oss << c;
                    oss << "\n  script_end\n";
                }
            }
        }

        oss << "end\n";
        return true;
    });

    return Utils::WriteTextFile(path, oss.str());
}

static GameObject *findByName(Scene *scene, const std::string &name)
{
    GameObject *found = nullptr;
    scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (obj->displayName == name) { found = obj; return false; }
        return true;
    });
    return found;
}

bool SceneSerializer::Load(Scene *scene, const std::string &path, const EditorProject *project)
{
    if (!scene) return false;

    std::string text;
    if (!Utils::ReadTextFile(path, &text))
    {
        Logger::Warning("Scene file not found: " + path);
        return false;
    }

    Logger::Info("Loading scene: " + path);

    std::vector<GameObject *> created;
    scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (!isEditorInternalObject(obj)) created.push_back(obj);
        return true;
    });
    for (GameObject *obj : created) scene->DeleteObject(obj);

    std::istringstream stream(text);
    std::string line;
    GameObject *current = nullptr;
    bool inScript = false;
    std::ostringstream scriptBuf;
    std::string pendingParent;
    std::string pendingName;

    auto finishObject = [&]()
    {
        if (!current) return;
        if (!pendingParent.empty())
        {
            if (GameObject *p = findByName(scene, pendingParent)) current->SetParent(p, false);
            pendingParent.clear();
        }
        if (FreeplayCamera *fp = dynamic_cast<FreeplayCamera *>(current)) fp->SyncAnglesFromTransform();
        current = nullptr;
    };

    while (std::getline(stream, line))
    {
        if (line.empty() || line[0] == '#') continue;

        if (line.rfind("ambient ", 0) == 0)
        {
            sscanf(line.c_str(), "ambient %f %f %f", &scene->ambientLight.x, &scene->ambientLight.y, &scene->ambientLight.z);
            continue;
        }

        if (line.rfind("fog ", 0) == 0)
        {
            int enabled = 0;
            sscanf(line.c_str(), "fog %d %f %f %f %f %f", &enabled, &scene->fog.startDistance, &scene->fog.endDistance,
                &scene->fog.color.x, &scene->fog.color.y, &scene->fog.color.z);
            scene->fog.enabled = enabled != 0;
            continue;
        }

        if (line.rfind("object \"", 0) == 0)
        {
            finishObject();
            const size_t q1 = line.find('"');
            const size_t q2 = line.find('"', q1 + 1);
            pendingName = line.substr(q1 + 1, q2 - q1 - 1);
            current = scene->CreateObject<Model>();
            current->displayName = pendingName;
            continue;
        }

        if (line == "end") { finishObject(); continue; }

        if (!current) continue;

        if (line.rfind("  type ", 0) == 0)
        {
            const std::string type = line.substr(7);
            scene->DeleteObject(current);
            if (type == "Entity") current = scene->CreateObject<Entity>();
            else if (type == "FreeplayCamera") current = scene->CreateObject<FreeplayCamera>();
            else if (type == "Camera") current = scene->CreateObject<Camera>();
            else if (type == "PointLight") current = scene->CreateObject<PointLight>();
            else if (type == "DirectionalLight") current = scene->CreateObject<DirectionalLight>();
            else current = scene->CreateObject<Model>();
            current->displayName = pendingName;
            continue;
        }

        if (inScript)
        {
            if (line == "  script_end")
            {
                inScript = false;
                ScriptBehaviour *sb = scene->CreateObject<ScriptBehaviour>();
                sb->SetParent(current, false);
                sb->SetSource(scriptBuf.str());
                scriptBuf.str("");
            }
            else scriptBuf << line << "\n";
            continue;
        }

        if (line.rfind("  script_file \"", 0) == 0)
        {
            const size_t q1 = line.find('"');
            const size_t q2 = line.find('"', q1 + 1);
            if (q1 != std::string::npos && q2 != std::string::npos)
            {
                const std::string rel = line.substr(q1 + 1, q2 - q1 - 1);
                ScriptBehaviour *sb = scene->CreateObject<ScriptBehaviour>();
                sb->SetParent(current, false);
                sb->SetScriptFile(rel);
                std::string code;
                if (project && project->ReadTextFile(rel, &code)) sb->SetSource(code);
                else sb->SetSource("# missing script file: " + rel + "\nfunc _ready():\n    pass\n");
            }
            continue;
        }

        if (line == "  script_begin") { inScript = true; continue; }

        glm::vec3 pos, rot, scale;
        if (line.rfind("  pos ", 0) == 0)
        {
            sscanf(line.c_str(), "  pos %f %f %f", &pos.x, &pos.y, &pos.z);
            current->transform.SetPosition(pos);
        }
        else if (line.rfind("  rot ", 0) == 0)
        {
            sscanf(line.c_str(), "  rot %f %f %f", &rot.x, &rot.y, &rot.z);
            current->transform.SetRotation(quatFromSceneEuler(rot));
        }
        else if (line.rfind("  scale ", 0) == 0)
        {
            sscanf(line.c_str(), "  scale %f %f %f", &scale.x, &scale.y, &scale.z);
            current->transform.SetScale(scale);
        }
        else if (line.rfind("  parent \"", 0) == 0)
        {
            const size_t q1 = line.find('"');
            const size_t q2 = line.find('"', q1 + 1);
            pendingParent = line.substr(q1 + 1, q2 - q1 - 1);
        }
        else if (line.rfind("  mesh ", 0) == 0)
        {
            if (Model *m = dynamic_cast<Model *>(current))
            {
                Surface s;
                s.mesh = line.substr(8);
                m->surfaces.push_back(s);
            }
        }
        else if (line.rfind("  texture ", 0) == 0)
        {
            if (Model *m = dynamic_cast<Model *>(current); m && !m->surfaces.empty())
                m->surfaces[0].texture = line.substr(10);
        }
        else if (line.rfind("  color ", 0) == 0)
        {
            if (Model *m = dynamic_cast<Model *>(current))
            {
                float colR = 1, colG = 1, colB = 1, colA = 1;
                sscanf(line.c_str(), "  color %f %f %f %f", &colR, &colG, &colB, &colA);
                m->color = glm::vec4(colR, colG, colB, colA);
                m->usedShaderProgram = "default";
            }
        }
        else if (line.rfind("  body ", 0) == 0)
        {
            if (Entity *e = dynamic_cast<Entity *>(current))
            {
                e->SetRigidBodyType(line.find("dynamic") != std::string::npos ? DYNAMIC : KINEMATIC);
                BoxCollider *c = e->AddCollider<BoxCollider>(Transform(), glm::vec3(1));
                c->SetCollisionCategoryBits(CollisionLayer::Default);
                c->SetCollideWithMaskBits(CollisionLayer::All);
            }
        }
        else if (line.rfind("  mass ", 0) == 0)
        {
            float mass = 1;
            sscanf(line.c_str(), "  mass %f", &mass);
            if (Entity *e = dynamic_cast<Entity *>(current)) e->SetMass(mass);
        }
        else if (line.rfind("  fov ", 0) == 0)
        {
            float fov = 70;
            sscanf(line.c_str(), "  fov %f", &fov);
            if (Camera *c = dynamic_cast<Camera *>(current)) c->FOV = glm::radians(fov);
        }
    }

    Logger::Info("Scene loaded, objects: " + std::to_string(scene->GetObjectCount()));
    return true;
}

bool SceneSerializer::CreateDefaultScene(Scene *scene, const EditorProject *project)
{
    if (!scene) return false;

    Logger::Info("Creating default scene");

    std::vector<GameObject *> all;
    scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (!isEditorInternalObject(obj)) all.push_back(obj);
        return true;
    });
    for (GameObject *obj : all) scene->DeleteObject(obj);

    scene->ambientLight = glm::vec3(0.35f, 0.36f, 0.4f);
    scene->fog.enabled = false;
    scene->fog.startDistance = 24;
    scene->fog.endDistance = 80;
    scene->fog.color = glm::vec3(0.42f, 0.46f, 0.5f);

    DirectionalLight *sun = scene->CreateObject<DirectionalLight>();
    sun->displayName = "Sun";
    sun->transform.SetRotation(glm::quat(glm::radians(glm::vec3(-40.0f, 35.0f, 0.0f))));
    sun->color = glm::vec3(1.0f, 0.95f, 0.85f);
    sun->intensity = 1.2f;

    Entity *ground = scene->CreateObject<Entity>(Transform(glm::vec3(0, -0.5f, 0)));
    ground->displayName = "Ground";
    ground->transform.SetScale(glm::vec3(12, 1, 12));
    ground->usedShaderProgram = "default";
    ground->color = glm::vec4(0.45f, 0.48f, 0.52f, 1);
    ground->SetRigidBodyType(KINEMATIC);
    BoxCollider *gc = ground->AddCollider<BoxCollider>(Transform(), glm::vec3(12, 1, 12));
    gc->SetCollisionCategoryBits(CollisionLayer::World);
    gc->SetCollideWithMaskBits(CollisionLayer::All);
    Surface gs;
    gs.mesh = "cube";
    ground->surfaces.push_back(gs);

    Entity *pillar = scene->CreateObject<Entity>(Transform(glm::vec3(4, 1.5f, 0)));
    pillar->displayName = "Pillar";
    pillar->usedShaderProgram = "default";
    pillar->transform.SetScale(glm::vec3(1, 3, 1));
    pillar->color = glm::vec4(0.85f, 0.45f, 0.2f, 1);
    pillar->SetRigidBodyType(KINEMATIC);
    BoxCollider *pc = pillar->AddCollider<BoxCollider>(Transform(), glm::vec3(1, 3, 1));
    pc->SetCollideWithMaskBits(CollisionLayer::All);
    Surface ps;
    ps.mesh = "cube";
    pillar->surfaces.push_back(ps);

    Entity *sphere = scene->CreateObject<Entity>(Transform(glm::vec3(-3, 2, 2)));
    sphere->displayName = "Ball";
    sphere->usedShaderProgram = "default";
    sphere->color = glm::vec4(0.9f, 0.2f, 0.25f, 1);
    sphere->SetRigidBodyType(DYNAMIC);
    sphere->SetMass(2);
    SphereCollider *sc = sphere->AddCollider<SphereCollider>(Transform(), 0.6f);
    sc->SetCollideWithMaskBits(CollisionLayer::All);
    Surface ss;
    ss.mesh = "sphere";
    sphere->surfaces.push_back(ss);

    Entity *cube = scene->CreateObject<Entity>(Transform(glm::vec3(0, 1.5f, 0)));
    cube->displayName = "PlayerCube";
    cube->usedShaderProgram = "default";
    cube->color = glm::vec4(0.2f, 0.65f, 1.0f, 1);
    cube->SetRigidBodyType(DYNAMIC);
    cube->SetMass(5);
    BoxCollider *cc = cube->AddCollider<BoxCollider>(Transform(), glm::vec3(1));
    cc->SetCollideWithMaskBits(CollisionLayer::All);
    Surface cs;
    cs.mesh = "cube";
    cube->surfaces.push_back(cs);

    FreeplayCamera *fp = scene->CreateObject<FreeplayCamera>(Transform(glm::vec3(0, 2, 8)));
    fp->displayName = "FreeplayCamera";
    fp->moveSpeed = project ? project->config.defaultMoveSpeed : 10.0f;
    fp->lookSpeed = project ? project->config.defaultLookSpeed : 1.0f;
    fp->SyncAnglesFromTransform();

    ScriptBehaviour *sb = scene->CreateObject<ScriptBehaviour>();
    sb->SetParent(cube, false);
    const std::string scriptPath = project ? project->DefaultScriptPath("PlayerCube") : "scripts/PlayerCube.ses";
    const std::string scriptSrc =
        "# PlayerCube\n"
        "func _ready():\n"
        "    log(\"PlayerCube ready\")\n"
        "\n"
        "func _update(dt):\n"
        "    if Input.key_down(\"W\"):\n"
        "        self.translate(0, 0, -4 * dt)\n";
    sb->SetScriptFile(scriptPath);
    sb->SetSource(scriptSrc);
    if (project) project->WriteTextFile(scriptPath, scriptSrc);

    return true;
}
