#include "ScriptBehaviour.hpp"

#include "../../script/ScriptEngine.hpp"

ScriptBehaviour::ScriptBehaviour(Scene *s, Transform t) : GameObject(s, t)
{
    displayName = "Script";
    editorVisible = false;
}

ScriptBehaviour::ScriptBehaviour(Scene *s) : GameObject(s)
{
    displayName = "Script";
    editorVisible = false;
}

ScriptBehaviour::~ScriptBehaviour() { ScriptEngine::Unregister(this); }

GameObject *ScriptBehaviour::GetHost() const { return GetParent(); }

void ScriptBehaviour::SetSource(const std::string &code)
{
    sourceCode = code;
    ScriptEngine::Compile(this, sourceCode);
}

const std::string &ScriptBehaviour::GetSource() const { return sourceCode; }

void ScriptBehaviour::SetScriptFile(const std::string &relativePath) { scriptFilePath = relativePath; }

const std::string &ScriptBehaviour::GetScriptFile() const { return scriptFilePath; }

void ScriptBehaviour::OnSceneLoad()
{
    if (enabled) ScriptEngine::RunReady(this);
}

void ScriptBehaviour::Update(double delta)
{
    if (!enabled) return;
    ScriptEngine::RunUpdate(this, delta);
}
