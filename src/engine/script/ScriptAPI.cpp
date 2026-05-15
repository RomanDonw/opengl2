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

    glm::vec3 v3;
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

    return false;
}

static bool executeInputCall(const std::string &line)
{
    if (line.rfind("Input.key_down(", 0) == 0)
    {
        std::string key;
        if (!parseQuotedString(line, 14, key)) return false;
        int k = GLFW_KEY_UNKNOWN;
        if (key.size() == 1) k = key[0];
        else if (key == "Space") k = GLFW_KEY_SPACE;
        else if (key == "W") k = GLFW_KEY_W;
        else if (key == "A") k = GLFW_KEY_A;
        else if (key == "S") k = GLFW_KEY_S;
        else if (key == "D") k = GLFW_KEY_D;
        return Input::IsKeyDown(k);
    }

    if (line.rfind("Input.action_down(", 0) == 0)
    {
        std::string action;
        if (parseQuotedString(line, 17, action)) return Input::IsActionDown(action);
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

bool ScriptAPI::EvaluateCondition(ScriptContext &ctx, const std::string &condition)
{
    (void)ctx;
    const std::string expr = trim(condition);
    if (expr.rfind("Input.", 0) == 0) return executeInputCall(expr);
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

    if (executeSceneCall(ctx, line)) return true;
    if (executeInputCall(line)) return true;
    if (executeFSCall(line)) return true;
    if (executeChain(ctx, line)) return true;

    if (line.find('.') != std::string::npos && line.find('(') != std::string::npos)
        Logger::Warning("[Script] Unknown call: " + line);

    return false;
}
