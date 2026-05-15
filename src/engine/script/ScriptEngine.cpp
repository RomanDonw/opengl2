#include "ScriptEngine.hpp"

#include "ScriptAPI.hpp"
#include "ScriptContext.hpp"
#include "../objects/ScriptBehaviour/ScriptBehaviour.hpp"

#include <sstream>

static std::string trim(const std::string &s)
{
    size_t a = 0;
    size_t b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

CompiledScript ScriptEngine::parse(const std::string &source)
{
    CompiledScript out;
    enum class Block { None, Ready, Update, Fixed };
    Block block = Block::None;

    std::istringstream stream(source);
    std::string line;
    while (std::getline(stream, line))
    {
        const std::string t = trim(line);
        if (t.empty() || t[0] == '#') continue;

        if (t == "func ready():" || t == "func _ready():") { block = Block::Ready; continue; }
        if (t == "func update(dt):" || t == "func _update(dt):" || t == "func _process(dt):") { block = Block::Update; continue; }
        if (t == "func fixed_update(dt):" || t == "func _fixed_update(dt):") { block = Block::Fixed; continue; }

        if (block == Block::Ready) out.readyLines.push_back(t);
        else if (block == Block::Update) out.updateLines.push_back(t);
        else if (block == Block::Fixed) out.fixedLines.push_back(t);
    }
    return out;
}

void ScriptEngine::Compile(ScriptBehaviour *behaviour, const std::string &source)
{
    if (!behaviour) return;
    scripts[behaviour] = parse(source);
}

void ScriptEngine::Unregister(ScriptBehaviour *behaviour)
{
    if (!behaviour) return;
    scripts.erase(behaviour);
}

void ScriptEngine::ClearAll() { scripts.clear(); }

static GameObject *hostOf(ScriptBehaviour *behaviour)
{
    GameObject *host = behaviour->GetHost();
    return host ? host : behaviour;
}

static void runLines(ScriptBehaviour *behaviour, const std::vector<std::string> &lines, double delta)
{
    ScriptContext ctx;
    ctx.scene = behaviour->GetScene();
    ctx.self = hostOf(behaviour);
    ctx.behaviour = behaviour;
    ctx.deltaTime = static_cast<float>(delta);

    for (size_t i = 0; i < lines.size(); ++i)
    {
        const std::string &line = lines[i];
        if (line.rfind("if ", 0) == 0 && line.size() > 4 && line.back() == ':')
        {
            const std::string cond = trim(line.substr(3, line.size() - 4));
            if (!ScriptAPI::EvaluateCondition(ctx, cond) && i + 1 < lines.size()) ++i;
            continue;
        }
        ScriptAPI::ExecuteLine(ctx, line);
    }
}

void ScriptEngine::RunReady(ScriptBehaviour *behaviour)
{
    const auto it = scripts.find(behaviour);
    if (it == scripts.end()) return;
    runLines(behaviour, it->second.readyLines, 0.0);
}

void ScriptEngine::RunUpdate(ScriptBehaviour *behaviour, double delta)
{
    const auto it = scripts.find(behaviour);
    if (it == scripts.end()) return;
    runLines(behaviour, it->second.updateLines, delta);
}

void ScriptEngine::RunFixedUpdate(ScriptBehaviour *behaviour, double delta)
{
    const auto it = scripts.find(behaviour);
    if (it == scripts.end()) return;
    runLines(behaviour, it->second.fixedLines, delta);
}
