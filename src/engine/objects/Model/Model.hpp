#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>

#include "../GameObject/GameObject.hpp"
#include "Surface.hpp"

class Scene;

class Model : virtual public GameObject
{
    friend class Scene;

    protected:
        Model(Scene *s, Transform t);
        Model(Scene *s);

        ~Model() override;

        void Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt, const FogRenderSettings *fog) override;

    public:
        const GameObjectType type = GameObjectType::MODEL;

        bool enableRender = true;
        bool enableDepthTest = true;
        std::string usedShaderProgram = "";
        glm::vec4 color = glm::vec4(1.0f);
        std::vector<Surface> surfaces = std::vector<Surface>();
};

#endif