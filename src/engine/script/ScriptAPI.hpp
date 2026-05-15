#ifndef SCRIPTAPI_HPP
#define SCRIPTAPI_HPP

#include "ScriptContext.hpp"

#include <string>

class ScriptAPI final
{
    public:
        static bool ExecuteLine(ScriptContext &ctx, const std::string &line);
        static bool EvaluateCondition(ScriptContext &ctx, const std::string &condition);
};

#endif
