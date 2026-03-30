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

        void Render(const GameObjectRenderData *data) override;

    public:
        bool enableRender = true;
        bool enableDepthTest = true;
        std::string usedShaderProgram = "";
        glm::vec4 color = glm::vec4(1.0f);
        std::vector<Surface> surfaces = std::vector<Surface>();
};

#endif