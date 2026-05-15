#ifndef SCRIPTENGINE_HPP
#define SCRIPTENGINE_HPP

#include <string>
#include <unordered_map>
#include <vector>

class ScriptBehaviour;

struct CompiledScript
{
    std::vector<std::string> readyLines;
    std::vector<std::string> updateLines;
    std::vector<std::string> fixedLines;
};

class ScriptEngine final
{
    private:
        ScriptEngine() = delete;

        static inline std::unordered_map<ScriptBehaviour *, CompiledScript> scripts;

        static CompiledScript parse(const std::string &source);
        static void executeLine(ScriptBehaviour *behaviour, const std::string &line, float dt);

    public:
        static void Compile(ScriptBehaviour *behaviour, const std::string &source);
        static void Unregister(ScriptBehaviour *behaviour);
        static void ClearAll();

        static void RunReady(ScriptBehaviour *behaviour);
        static void RunUpdate(ScriptBehaviour *behaviour, double delta);
        static void RunFixedUpdate(ScriptBehaviour *behaviour, double delta);
};

#endif
