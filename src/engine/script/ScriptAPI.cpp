#include "ScriptAPI.hpp"

#include "../Console.hpp"
#include "../Engine.hpp"
#include "../Input.hpp"
#include "../Logger.hpp"
#include "../Scene.hpp"
#include "../Time.hpp"
#include "../Window.hpp"
#include "../io/FileSystem.hpp"
#include "../objects/Camera/Camera.hpp"
#include "../objects/FreeplayCamera/FreeplayCamera.hpp"
#include "../objects/Entity/Entity.hpp"
#include "../objects/Light/DirectionalLight.hpp"
#include "../objects/Light/Light.hpp"
#include "../objects/Light/PointLight.hpp"
#include "../objects/Model/Model.hpp"
#include "../objects/RigidBody/RigidBody.hpp"
#include "../objects/ScriptBehaviour/ScriptBehaviour.hpp"
#include "../physics/CollisionLayers.hpp"
#include "../physics/colliders/colliders.hpp"
#include "../resources/Mesh.hpp"
#include "../resources/ShaderProgram.hpp"
#include "../ResourceManager.hpp"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <sstream>

static std::string trim(const std::string &s)
{
    size_t a = 0;
    size_t b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

static std::string replaceAll(std::string s, const std::string &from, const std::string &to)
{
    if (from.empty()) return s;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos)
    {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

static float evalFloat(const std::string &exprIn, float dt)
{
    std::string expr = trim(replaceAll(exprIn, "dt", std::to_string(dt)));
    if (expr.empty()) return 0.0f;

    const size_t mul = expr.find('*');
    if (mul != std::string::npos)
    {
        const float a = std::stof(trim(expr.substr(0, mul)));
        const float b = std::stof(trim(expr.substr(mul + 1)));
        return a * b;
    }

    const size_t add = expr.find('+');
    if (add != std::string::npos)
    {
        const float a = std::stof(trim(expr.substr(0, add)));
        const float b = std::stof(trim(expr.substr(add + 1)));
        return a + b;
    }

    return std::stof(expr);
}

static bool parseQuotedString(const std::string &line, size_t start, std::string &out)
{
    const size_t q1 = line.find('"', start);
    if (q1 == std::string::npos) return false;
    const size_t q2 = line.find('"', q1 + 1);
    if (q2 == std::string::npos) return false;
    out = line.substr(q1 + 1, q2 - q1 - 1);
    return true;
}

static bool parseVec3Args(const std::string &args, float dt, glm::vec3 &out)
{
    std::stringstream ss(args);
    std::string part;
    float vals[3]{};
    int i = 0;
    while (std::getline(ss, part, ',') && i < 3)
    {
        vals[i++] = evalFloat(part, dt);
    }
    if (i < 3) return false;
    out = glm::vec3(vals[0], vals[1], vals[2]);
    return true;
}

static int parseKeyCode(const std::string &key)
{
    if (key.size() == 1) return key[0];
    if (key == "Space") return GLFW_KEY_SPACE;
    if (key == "W") return GLFW_KEY_W;
    if (key == "A") return GLFW_KEY_A;
    if (key == "S") return GLFW_KEY_S;
    if (key == "D") return GLFW_KEY_D;
    if (key == "Q") return GLFW_KEY_Q;
    if (key == "E") return GLFW_KEY_E;
    if (key == "Escape") return GLFW_KEY_ESCAPE;
    if (key == "Enter") return GLFW_KEY_ENTER;
    if (key == "Shift") return GLFW_KEY_LEFT_SHIFT;
    if (key == "Ctrl") return GLFW_KEY_LEFT_CONTROL;
    if (key == "Tab") return GLFW_KEY_TAB;
    if (key == "Up") return GLFW_KEY_UP;
    if (key == "Down") return GLFW_KEY_DOWN;
    if (key == "Left") return GLFW_KEY_LEFT;
    if (key == "Right") return GLFW_KEY_RIGHT;
    return GLFW_KEY_UNKNOWN;
}

static void moveGlobal(GameObject *obj, const glm::vec3 &delta)
{
    if (!obj) return;
    Transform gt = obj->GetGlobalTransform();
    gt.SetPosition(gt.GetPosition() + delta);
    if (GameObject *parent = obj->GetParent())
        obj->transform = gt.GlobalToLocal(parent->GetGlobalTransform());
    else
        obj->transform.SetPosition(gt.GetPosition());
}

static GameObject *findByTag(Scene *scene, const std::string &tag)
{
    if (!scene) return nullptr;
    GameObject *found = nullptr;
    scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (obj->tags.contains(tag)) { found = obj; return false; }
        return true;
    });
    return found;
}

static bool parseFloat1(const std::string &args, float dt, float &out)
{
    if (trim(args).empty()) return false;
    out = evalFloat(args, dt);
    return true;
}

static GameObject *parseFindObject(const std::string &line, ScriptContext &ctx)
{
    std::string name;
    if (line.rfind("Scene.find(", 0) == 0 && parseQuotedString(line, 10, name)) return ctx.ResolveObject(name);
    if (line.rfind("find(", 0) == 0 && parseQuotedString(line, 4, name)) return ctx.ResolveObject(name);
    return nullptr;
}

static bool applyObjectMethod(GameObject *obj, const std::string &method, const std::string &args, float dt)
{
    if (!obj) return false;

    if (method == "delete" || method == "destroy")
    {
        if (Scene *s = obj->GetScene()) s->DeleteObject(obj);
        return true;
    }

    if (method == "set_name")
    {
        std::string name;
        if (args.size() >= 2 && args.front() == '"' && parseQuotedString(args, 0, name))
            obj->displayName = name;
        else if (!trim(args).empty())
            obj->displayName = trim(args);
        return true;
    }

    if (method == "set_visible")
    {
        int v = 0;
        if (sscanf(args.c_str(), "%d", &v) == 1) obj->editorVisible = v != 0;
        return true;
    }

    if (method == "add_tag")
    {
        std::string tag;
        if (args.size() >= 2 && args.front() == '"' && parseQuotedString(args, 0, tag)) obj->tags.insert(tag);
        return true;
    }

    if (method == "remove_tag")
    {
        std::string tag;
        if (args.size() >= 2 && args.front() == '"' && parseQuotedString(args, 0, tag)) obj->tags.erase(tag);
        return true;
    }

    if (method == "clear_tags")
    {
        obj->tags.clear();
        return true;
    }

    if (method == "hide_in_play")
    {
        int v = 0;
        if (sscanf(args.c_str(), "%d", &v) == 1) obj->hiddenInPlay = v != 0;
        return true;
    }

    if (method == "detach_parent")
    {
        obj->SetParent(nullptr, true);
        return true;
    }

    if (method == "reset_rotation")
    {
        obj->transform.SetRotation(glm::quat(1, 0, 0, 0));
        return true;
    }

    if (method == "reset_scale")
    {
        obj->transform.SetScale(glm::vec3(1));
        return true;
    }

    if (method == "set_parent")
    {
        std::string parentName;
        if (args.size() >= 2 && args.front() == '"' && parseQuotedString(args, 0, parentName))
        {
            if (Scene *s = obj->GetScene())
            {
                GameObject *found = nullptr;
                s->ForEachAllObjects([&](GameObject *o) -> bool
                {
                    if (o->displayName == parentName) { found = o; return false; }
                    return true;
                });
                if (found) obj->SetParent(found, true);
            }
        }
        return true;
    }

    if (method == "face_object")
    {
        std::string targetName;
        if (args.size() >= 2 && args.front() == '"' && parseQuotedString(args, 0, targetName))
        {
            if (Scene *s = obj->GetScene())
            {
                GameObject *target = nullptr;
                s->ForEachAllObjects([&](GameObject *o) -> bool
                {
                    if (o->displayName == targetName) { target = o; return false; }
                    return true;
                });
                if (target)
                {
                    const glm::vec3 from = obj->GetGlobalTransform().GetPosition();
                    const glm::vec3 to = target->GetGlobalTransform().GetPosition();
                    const glm::vec3 dir = to - from;
                    if (glm::dot(dir, dir) > 0.0001f)
                        obj->transform.SetRotation(glm::quatLookAt(glm::normalize(dir), glm::vec3(0, 1, 0)));
                }
            }
        }
        return true;
    }

    if (method == "copy_transform")
    {
        std::string targetName;
        if (args.size() >= 2 && args.front() == '"' && parseQuotedString(args, 0, targetName))
        {
            if (Scene *s = obj->GetScene())
            {
                s->ForEachAllObjects([&](GameObject *o) -> bool
                {
                    if (o->displayName == targetName)
                    {
                        obj->transform = o->transform;
                        return false;
                    }
                    return true;
                });
            }
        }
        return true;
    }

    glm::vec3 v3;
    float f1 = 0;
    if (method == "set_position" || method == "set_pos")
    {
        if (!parseVec3Args(args, dt, v3)) return false;
        obj->transform.SetPosition(v3);
        return true;
    }

    if (method == "translate")
    {
        if (!parseVec3Args(args, dt, v3)) return false;
        obj->transform.Translate(v3);
        return true;
    }

    if (method == "set_rotation")
    {
        if (!parseVec3Args(args, dt, v3)) return false;
        obj->transform.SetRotation(glm::quat(glm::radians(v3)));
        return true;
    }

    if (method == "set_scale")
    {
        if (!parseVec3Args(args, dt, v3)) return false;
        obj->transform.SetScale(v3);
        return true;
    }

    if (method == "rotate_y")
    {
        float deg = evalFloat(args, dt);
        obj->transform.Rotate(glm::angleAxis(glm::radians(deg), glm::vec3(0, 1, 0)));
        return true;
    }

    if (method == "rotate" || method == "rotate_euler")
    {
        if (!parseVec3Args(args, dt, v3)) return false;
        obj->transform.Rotate(glm::quat(glm::radians(v3)));
        return true;
    }

    if (method == "rotate_x")
    {
        if (!parseFloat1(args, dt, f1)) return false;
        obj->transform.Rotate(glm::angleAxis(glm::radians(f1), glm::vec3(1, 0, 0)));
        return true;
    }

    if (method == "rotate_z")
    {
        if (!parseFloat1(args, dt, f1)) return false;
        obj->transform.Rotate(glm::angleAxis(glm::radians(f1), glm::vec3(0, 0, 1)));
        return true;
    }

    if (method == "look_at")
    {
        if (!parseVec3Args(args, dt, v3)) return false;
        const glm::vec3 from = obj->GetGlobalTransform().GetPosition();
        const glm::vec3 dir = v3 - from;
        if (glm::dot(dir, dir) > 0.0001f)
            obj->transform.SetRotation(glm::quatLookAt(glm::normalize(dir), glm::vec3(0, 1, 0)));
        return true;
    }

    if (method == "move_forward")
    {
        if (!parseFloat1(args, dt, f1)) return false;
        moveGlobal(obj, obj->GetGlobalTransform().GetFront() * f1);
        return true;
    }

    if (method == "move_right")
    {
        if (!parseFloat1(args, dt, f1)) return false;
        moveGlobal(obj, obj->GetGlobalTransform().GetRight() * f1);
        return true;
    }

    if (method == "move_up")
    {
        if (!parseFloat1(args, dt, f1)) return false;
        moveGlobal(obj, obj->GetGlobalTransform().GetUp() * f1);
        return true;
    }

    if (method == "move_back")
    {
        if (!parseFloat1(args, dt, f1)) return false;
        moveGlobal(obj, -obj->GetGlobalTransform().GetFront() * f1);
        return true;
    }

    if (method == "move_left")
    {
        if (!parseFloat1(args, dt, f1)) return false;
        moveGlobal(obj, -obj->GetGlobalTransform().GetRight() * f1);
        return true;
    }

    if (method == "move_down")
    {
        if (!parseFloat1(args, dt, f1)) return false;
        moveGlobal(obj, -obj->GetGlobalTransform().GetUp() * f1);
        return true;
    }

    if (RigidBody *rb = dynamic_cast<RigidBody *>(obj))
    {
        if (method == "set_velocity")
        {
            if (!parseVec3Args(args, dt, v3)) return false;
            rb->SetLinearVelocity(v3);
            return true;
        }
        if (method == "apply_force")
        {
            if (!parseVec3Args(args, dt, v3)) return false;
            rb->ApplyGlobalForceToCenterOfMass(v3);
            return true;
        }
        if (method == "set_mass")
        {
            rb->SetMass(evalFloat(args, dt));
            return true;
        }
        if (method == "set_dynamic") { rb->SetRigidBodyType(DYNAMIC); return true; }
        if (method == "set_kinematic") { rb->SetRigidBodyType(KINEMATIC); return true; }
        if (method == "apply_impulse")
        {
            if (!parseVec3Args(args, dt, v3)) return false;
            rb->ApplyGlobalForceToCenterOfMass(v3);
            return true;
        }
        if (method == "stop")
        {
            rb->SetLinearVelocity(glm::vec3(0));
            return true;
        }
    }

    if (Model *m = dynamic_cast<Model *>(obj))
    {
        if (method == "set_color")
        {
            float r = 1, g = 1, b = 1, a = 1;
            sscanf(args.c_str(), "%f,%f,%f,%f", &r, &g, &b, &a);
            m->color = glm::vec4(r, g, b, a);
            return true;
        }
        if (method == "set_mesh")
        {
            std::string mesh;
            if (args.size() >= 2 && args.front() == '"' && parseQuotedString(args, 0, mesh))
            {
                if (m->surfaces.empty()) m->surfaces.push_back(Surface());
                m->surfaces[0].mesh = mesh;
            }
            return true;
        }
        if (method == "set_shader")
        {
            std::string shader;
            if (args.size() >= 2 && args.front() == '"' && parseQuotedString(args, 0, shader))
                m->usedShaderProgram = shader;
            return true;
        }
        if (method == "set_render_enabled")
        {
            int v = 1;
            if (sscanf(args.c_str(), "%d", &v) == 1) m->enableRender = v != 0;
            return true;
        }
    }

    if (Camera *cam = dynamic_cast<Camera *>(obj))
    {
        if (method == "set_fov")
        {
            if (!parseFloat1(args, dt, f1)) return false;
            cam->FOV = glm::radians(f1);
            return true;
        }
        if (method == "set_near")
        {
            if (!parseFloat1(args, dt, f1)) return false;
            cam->nearDistance = f1;
            return true;
        }
        if (method == "set_far")
        {
            if (!parseFloat1(args, dt, f1)) return false;
            cam->farDistance = f1;
            return true;
        }
        if (method == "set_active_camera")
        {
            if (Scene *s = obj->GetScene()) s->SetCurrentCamera(cam);
            return true;
        }
    }

    if (FreeplayCamera *fp = dynamic_cast<FreeplayCamera *>(obj))
    {
        if (method == "activate") { fp->Activate(); return true; }
        if (method == "deactivate") { fp->Deactivate(); return true; }
        if (method == "set_move_speed")
        {
            if (!parseFloat1(args, dt, f1)) return false;
            fp->moveSpeed = f1;
            return true;
        }
        if (method == "set_look_speed")
        {
            if (!parseFloat1(args, dt, f1)) return false;
            fp->lookSpeed = f1;
            return true;
        }
    }

    if (Light *light = dynamic_cast<Light *>(obj))
    {
        if (method == "set_light_color" || method == "set_color")
        {
            if (!parseVec3Args(args, dt, v3)) return false;
            light->color = v3;
            return true;
        }
        if (method == "set_light_intensity" || method == "set_intensity")
        {
            if (!parseFloat1(args, dt, f1)) return false;
            light->intensity = f1;
            return true;
        }
        if (method == "set_light_enabled" || method == "set_enabled")
        {
            int v = 1;
            if (sscanf(args.c_str(), "%d", &v) == 1) light->enabled = v != 0;
            return true;
        }
    }

    if (PointLight *pl = dynamic_cast<PointLight *>(obj))
    {
        if (method == "set_light_range" || method == "set_range")
        {
            if (!parseFloat1(args, dt, f1)) return false;
            pl->range = f1;
            return true;
        }
    }

    return false;
}

static GameObject *spawnEntity(Scene *scene, const std::string &name)
{
    Entity *e = scene->CreateObject<Entity>();
    e->displayName = name;
    e->usedShaderProgram = "default";
    e->color = glm::vec4(0.8f, 0.8f, 0.8f, 1);
    e->SetRigidBodyType(DYNAMIC);
    e->SetMass(1);
    BoxCollider *c = e->AddCollider<BoxCollider>(Transform(), glm::vec3(1));
    c->SetCollideWithMaskBits(CollisionLayer::All);
    Surface s;
    s.mesh = "cube";
    e->surfaces.push_back(s);
    return e;
}

static bool executeSceneCall(ScriptContext &ctx, const std::string &line)
{
    if (!ctx.scene) return false;

    std::string name;
    if (line.rfind("Scene.delete(", 0) == 0 && parseQuotedString(line, 12, name))
    {
        if (GameObject *obj = ctx.ResolveObject(name))
        {
            ctx.scene->DeleteObject(obj);
            ctx.variables.erase(name);
            return true;
        }
        return false;
    }

    if (line.rfind("Scene.spawn_entity(", 0) == 0 && parseQuotedString(line, 18, name))
    {
        ctx.variables[name] = spawnEntity(ctx.scene, name);
        return true;
    }

    if (line.rfind("Scene.spawn_model(", 0) == 0 && parseQuotedString(line, 17, name))
    {
        Model *m = ctx.scene->CreateObject<Model>();
        m->displayName = name;
        m->usedShaderProgram = "default";
        Surface s;
        s.mesh = "cube";
        m->surfaces.push_back(s);
        ctx.variables[name] = m;
        return true;
    }

    if (line.rfind("Scene.spawn_camera(", 0) == 0 && parseQuotedString(line, 18, name))
    {
        Camera *c = ctx.scene->CreateObject<Camera>();
        c->displayName = name;
        ctx.variables[name] = c;
        return true;
    }

    if (line.rfind("Scene.spawn_freeplay(", 0) == 0 && parseQuotedString(line, 20, name))
    {
        FreeplayCamera *fp = ctx.scene->CreateObject<FreeplayCamera>();
        fp->displayName = name;
        ctx.variables[name] = fp;
        return true;
    }

    if (line.rfind("Scene.set_gravity(", 0) == 0)
    {
        glm::vec3 g;
        const size_t p = line.find('(');
        if (parseVec3Args(line.substr(p + 1), ctx.deltaTime, g))
        {
            ctx.scene->SetGravity(g);
            return true;
        }
    }

    if (line.rfind("Scene.set_camera(", 0) == 0 && parseQuotedString(line, 16, name))
    {
        if (Camera *c = dynamic_cast<Camera *>(ctx.ResolveObject(name)))
        {
            ctx.scene->SetCurrentCamera(c);
            return true;
        }
    }

    if (line.rfind("Scene.spawn_point_light(", 0) == 0 && parseQuotedString(line, 21, name))
    {
        PointLight *pl = ctx.scene->CreateObject<PointLight>();
        pl->displayName = name;
        ctx.variables[name] = pl;
        return true;
    }

    if (line.rfind("Scene.spawn_directional_light(", 0) == 0 && parseQuotedString(line, 27, name))
    {
        DirectionalLight *dl = ctx.scene->CreateObject<DirectionalLight>();
        dl->displayName = name;
        ctx.variables[name] = dl;
        return true;
    }

    if (line.rfind("Scene.set_ambient(", 0) == 0)
    {
        glm::vec3 amb;
        const size_t p = line.find('(');
        if (parseVec3Args(line.substr(p + 1), ctx.deltaTime, amb))
        {
            ctx.scene->ambientLight = amb;
            return true;
        }
    }

    if (line.rfind("Scene.set_fog_enabled(", 0) == 0)
    {
        int v = 0;
        if (sscanf(line.c_str(), "Scene.set_fog_enabled(%d)", &v) == 1)
        {
            ctx.scene->fog.enabled = v != 0;
            return true;
        }
    }

    if (line.rfind("Scene.set_fog_color(", 0) == 0)
    {
        glm::vec3 c;
        const size_t p = line.find('(');
        if (parseVec3Args(line.substr(p + 1), ctx.deltaTime, c))
        {
            ctx.scene->fog.color = c;
            return true;
        }
    }

    if (line.rfind("Scene.set_fog_distance(", 0) == 0)
    {
        float start = 0, end = 0;
        const size_t p = line.find('(');
        if (sscanf(line.c_str() + p + 1, "%f,%f", &start, &end) == 2)
        {
            ctx.scene->fog.startDistance = start;
            ctx.scene->fog.endDistance = end;
            return true;
        }
    }

    return false;
}

static bool executeInputCall(const std::string &line)
{
    if (line.rfind("Input.key_down(", 0) == 0)
    {
        std::string key;
        if (!parseQuotedString(line, 14, key)) return false;
        return Input::IsKeyDown(parseKeyCode(key));
    }

    if (line.rfind("Input.key_pressed(", 0) == 0 || line.rfind("Input.key_just_pressed(", 0) == 0)
    {
        std::string key;
        const size_t start = line.find('(');
        if (!parseQuotedString(line, start, key)) return false;
        return Input::IsKeyJustPressed(parseKeyCode(key));
    }

    if (line.rfind("Input.key_released(", 0) == 0 || line.rfind("Input.key_just_released(", 0) == 0)
    {
        std::string key;
        const size_t start = line.find('(');
        if (!parseQuotedString(line, start, key)) return false;
        return Input::IsKeyJustReleased(parseKeyCode(key));
    }

    if (line.rfind("Input.mouse_down(", 0) == 0)
    {
        int btn = 0;
        if (sscanf(line.c_str(), "Input.mouse_down(%d)", &btn) == 1) return Input::IsMouseButtonDown(btn);
    }

    if (line.rfind("Input.mouse_pressed(", 0) == 0)
    {
        int btn = 0;
        if (sscanf(line.c_str(), "Input.mouse_pressed(%d)", &btn) == 1) return Input::IsMouseButtonJustPressed(btn);
    }

    if (line.rfind("Input.mouse_captured(", 0) == 0) return Input::IsMouseCaptured();

    if (line.rfind("Input.any_key_down(", 0) == 0)
    {
        for (int k = GLFW_KEY_SPACE; k <= GLFW_KEY_LAST; ++k)
            if (Input::IsKeyDown(k)) return true;
        return false;
    }

    if (line.rfind("Input.action_down(", 0) == 0)
    {
        std::string action;
        if (parseQuotedString(line, 17, action)) return Input::IsActionDown(action);
    }

    if (line.rfind("Input.action_pressed(", 0) == 0)
    {
        std::string action;
        if (parseQuotedString(line, 20, action)) return Input::IsActionJustPressed(action);
    }

    if (line.rfind("Input.get_axis(", 0) == 0)
    {
        std::string axis;
        if (parseQuotedString(line, 14, axis)) return Input::GetAxis(axis) != 0.0f;
    }

    return false;
}

static bool executeFSCall(const std::string &line)
{
    std::string path;
    std::string text;

    if (line.rfind("FS.write(", 0) == 0)
    {
        const size_t c = line.find(',');
        if (c == std::string::npos) return false;
        if (!parseQuotedString(line, 8, path)) return false;
        if (!parseQuotedString(line, c, text)) return false;
        return FileSystem::WriteText(path, text);
    }

    if (line.rfind("FS.append(", 0) == 0)
    {
        const size_t c = line.find(',');
        if (c == std::string::npos) return false;
        if (!parseQuotedString(line, 9, path)) return false;
        if (!parseQuotedString(line, c, text)) return false;
        return FileSystem::AppendText(path, text);
    }

    if (line.rfind("FS.delete(", 0) == 0 && parseQuotedString(line, 9, path))
        return FileSystem::Delete(path);

    if (line.rfind("FS.exists(", 0) == 0 && parseQuotedString(line, 9, path))
        return FileSystem::Exists(path);

    return false;
}

static bool executeChain(ScriptContext &ctx, const std::string &line)
{
    const size_t dot = line.find('.');
    if (dot == std::string::npos) return false;

    const std::string head = trim(line.substr(0, dot));
    const std::string tail = trim(line.substr(dot + 1));

    GameObject *obj = nullptr;
    if (head == "self") obj = ctx.self;
    else if (head.rfind("Scene.find(", 0) == 0) obj = parseFindObject(head, ctx);
    else if (ctx.variables.contains(head)) obj = ctx.variables.at(head);
    else obj = ctx.ResolveObject(head);

    if (!obj) return false;

    const size_t paren = tail.find('(');
    if (paren == std::string::npos) return false;

    const std::string method = trim(tail.substr(0, paren));
    const size_t end = tail.rfind(')');
    const std::string args = end != std::string::npos ? tail.substr(paren + 1, end - paren - 1) : "";
    return applyObjectMethod(obj, method, args, ctx.deltaTime);
}

static bool evaluateSelfCondition(ScriptContext &ctx, const std::string &expr)
{
    if (!ctx.self) return false;

    if (expr.rfind("self.has_tag(", 0) == 0)
    {
        std::string tag;
        if (!parseQuotedString(expr, 12, tag)) return false;
        return ctx.self->tags.contains(tag);
    }

    if (expr.rfind("self.distance_less(", 0) == 0)
    {
        std::string otherName;
        float maxDist = 0;
        const size_t comma = expr.find(',');
        if (comma == std::string::npos) return false;
        if (!parseQuotedString(expr, 20, otherName)) return false;
        maxDist = evalFloat(trim(expr.substr(comma + 1)), ctx.deltaTime);
        if (GameObject *other = ctx.ResolveObject(otherName))
        {
            const float d = glm::length(
                ctx.self->GetGlobalTransform().GetPosition() - other->GetGlobalTransform().GetPosition());
            return d < maxDist;
        }
        return false;
    }

    if (expr.rfind("self.distance_greater(", 0) == 0)
    {
        std::string otherName;
        float minDist = 0;
        const size_t comma = expr.find(',');
        if (comma == std::string::npos) return false;
        if (!parseQuotedString(expr, 22, otherName)) return false;
        minDist = evalFloat(trim(expr.substr(comma + 1)), ctx.deltaTime);
        if (GameObject *other = ctx.ResolveObject(otherName))
        {
            const float d = glm::length(
                ctx.self->GetGlobalTransform().GetPosition() - other->GetGlobalTransform().GetPosition());
            return d > minDist;
        }
        return false;
    }

    if (expr == "self.is_moving()")
    {
        if (RigidBody *rb = dynamic_cast<RigidBody *>(ctx.self))
            return glm::length(rb->GetLinearVelocity()) > 0.05f;
        return false;
    }

    return false;
}

bool ScriptAPI::EvaluateCondition(ScriptContext &ctx, const std::string &condition)
{
    const std::string expr = trim(condition);
    if (expr.rfind("Input.", 0) == 0) return executeInputCall(expr);
    if (expr.rfind("self.", 0) == 0) return evaluateSelfCondition(ctx, expr);
    if (expr.rfind("FS.", 0) == 0) return executeFSCall(expr);

    if (expr == "Time.paused()") return Time::GetTimeScale() < 0.001f;
    if (expr == "Time.playing()") return Time::GetTimeScale() > 0.001f;

    if (expr.rfind("Math.random_chance(", 0) == 0)
    {
        float chance = 0.5f;
        sscanf(expr.c_str(), "Math.random_chance(%f)", &chance);
        return (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) < chance;
    }

    if (expr.rfind("Math.coin_flip(", 0) == 0) return (std::rand() & 1) != 0;

    return false;
}

bool ScriptAPI::ExecuteLine(ScriptContext &ctx, const std::string &lineIn)
{
    const std::string line = trim(lineIn);
    if (line.empty() || line[0] == '#') return true;
    if (line == "pass") return true;

    if (line.rfind("var ", 0) == 0)
    {
        const size_t eq = line.find('=');
        if (eq == std::string::npos) return false;
        const std::string varName = trim(line.substr(4, eq - 4));
        const std::string expr = trim(line.substr(eq + 1));
        if (expr.rfind("Scene.find(", 0) == 0)
        {
            ctx.variables[varName] = parseFindObject(expr, ctx);
            return true;
        }
        if (expr.rfind("Scene.find_tag(", 0) == 0 && ctx.scene)
        {
            std::string tag;
            if (parseQuotedString(expr, 14, tag))
            {
                ctx.variables[varName] = findByTag(ctx.scene, tag);
                return true;
            }
        }
        return false;
    }

    if (line.rfind("log(", 0) == 0 || line.rfind("print(", 0) == 0)
    {
        std::string msg;
        if (parseQuotedString(line, 0, msg))
        {
            Logger::Info("[Script] " + msg);
            Console::Log(msg);
            return true;
        }
    }

    if (line == "Engine.quit()")
    {
        Window::SetShouldClose(true);
        return true;
    }

    if (line.rfind("Time.set_scale(", 0) == 0)
    {
        float s = evalFloat(line.substr(15), ctx.deltaTime);
        Time::SetTimeScale(s);
        return true;
    }

    if (line == "Time.pause()") { Time::SetTimeScale(0); return true; }
    if (line == "Time.resume()") { Time::SetTimeScale(1); return true; }

    if (line.rfind("Time.set_fixed(", 0) == 0)
    {
        float dt = evalFloat(line.substr(15), ctx.deltaTime);
        Time::SetFixedDeltaTime(dt);
        return true;
    }

    if (line.rfind("log_error(", 0) == 0)
    {
        std::string msg;
        if (parseQuotedString(line, 0, msg))
        {
            Logger::Error("[Script] " + msg);
            Console::LogError(msg);
            return true;
        }
    }

    if (line.rfind("log_warn(", 0) == 0)
    {
        std::string msg;
        if (parseQuotedString(line, 0, msg))
        {
            Logger::Warning("[Script] " + msg);
            Console::Log(msg);
            return true;
        }
    }

    if (line == "Console.show()") { Console::SetVisible(true); return true; }
    if (line == "Console.hide()") { Console::SetVisible(false); return true; }
    if (line == "Console.toggle()") { Console::Toggle(); return true; }

    if (executeSceneCall(ctx, line)) return true;
    if (executeInputCall(line)) return true;
    if (executeFSCall(line)) return true;
    if (executeChain(ctx, line)) return true;

    if (line.find('.') != std::string::npos && line.find('(') != std::string::npos)
        Logger::Warning("[Script] Unknown call: " + line);

    return false;
}
