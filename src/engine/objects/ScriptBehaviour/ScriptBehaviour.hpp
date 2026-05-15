#ifndef SCRIPTBEHAVIOUR_HPP
#define SCRIPTBEHAVIOUR_HPP

#include <string>

#include "../GameObject/GameObject.hpp"

class Scene;

class ScriptBehaviour final : public GameObject
{
    friend class Scene;

    protected:
        ScriptBehaviour(Scene *s, Transform t);
        ScriptBehaviour(Scene *s);

        ~ScriptBehaviour() override;

        void Update(double delta) override;
        void OnSceneLoad() override;

    public:
        std::string sourceCode;
        std::string scriptFilePath;
        bool enabled = true;

        GameObject *GetHost() const;
        void SetSource(const std::string &code);
        const std::string &GetSource() const;
        void SetScriptFile(const std::string &relativePath);
        const std::string &GetScriptFile() const;
};

#endif
