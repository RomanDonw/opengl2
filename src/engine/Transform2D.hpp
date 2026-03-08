#ifndef TRANSFORM2D_HPP
#define TRANSFORM2D_HPP

#include "external/glm.hpp"

#include <string>

class Transform2D
{
    protected:
        glm::vec2 position = glm::vec2(0.0f);
        float rotation = 0;
        glm::vec2 scale = glm::vec2(1.0f);

        virtual void OnTransformChanged();

    public:
        Transform2D(glm::vec2 pos, float rot, glm::vec2 scl);
        Transform2D(glm::vec2 pos, float rot);
        Transform2D(glm::vec2 pos);
        Transform2D();

        virtual ~Transform2D();

        Transform2D Copy();

        std::string ToString() const;

        // ================================

        glm::vec2 GetPosition() const;
        float GetRotation() const;
        glm::mat3 GetRotationMatrix() const;
        glm::vec2 GetScale() const;

        // ================================

        glm::mat3 GetTransformationMatrix() const;

        // ================================

        void SetPosition(glm::vec2 pos);
        void SetRotation(float rot);
        void SetScale(glm::vec2 scl);

        // ================================

        void Translate(glm::vec2 offset);
        void Rotate(float offset);
        void Scale(glm::vec2 offset);

        // ================================

        Transform2D *operator=(Transform2D other);
};

#endif