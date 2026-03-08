#include "Transform2D.hpp"

#include <sstream>

// === PRIVATE ===

void Transform2D::OnTransformChanged() {}

// === PUBLIC ===

Transform2D::Transform2D(glm::vec2 pos, float rot, glm::vec2 scl) : position(pos), rotation(rot), scale(scl) { OnTransformChanged(); }

Transform2D::Transform2D(glm::vec2 pos, float rot) : position(pos), rotation(rot) { OnTransformChanged(); }

Transform2D::Transform2D(glm::vec2 pos) : position(pos) { OnTransformChanged(); }

Transform2D::Transform2D() { OnTransformChanged(); }

Transform2D::~Transform2D() {}

Transform2D Transform2D::Copy() { return *this; }

std::string Transform2D::ToString() const
{
    std::ostringstream ss;
    ss << "{pos:[" << position.x << "; " << position.y << "], rot: " << rotation << ", scl:[" << scale.x << "; " << scale.y << "]}";
    return ss.str();
}

glm::vec2 Transform2D::GetPosition() const { return position; }
float Transform2D::GetRotation() const { return rotation; }
glm::mat3 Transform2D::GetRotationMatrix() const { return glm::rotate(glm::mat3(1.0f), rotation); }
glm::vec2 Transform2D::GetScale() const { return scale; }

glm::mat3 Transform2D::GetTransformationMatrix() const
{ return glm::translate(glm::scale(glm::rotate(glm::translate(glm::mat3(1.0f), position + glm::vec2(0.5f)), rotation), scale), glm::vec2(-0.5f)); }

void Transform2D::SetPosition(glm::vec2 pos) { position = pos; OnTransformChanged(); }
void Transform2D::SetRotation(float rot) { rotation = rot; OnTransformChanged(); }
void Transform2D::SetScale(glm::vec2 scl) { scale = scl; OnTransformChanged(); }

void Transform2D::Translate(glm::vec2 offset) { position += offset; OnTransformChanged(); }
void Transform2D::Rotate(float offset) { rotation += offset; OnTransformChanged(); }
void Transform2D::Scale(glm::vec2 offset) { scale += offset; OnTransformChanged(); }

Transform2D *Transform2D::operator=(Transform2D other)
{
    position = other.position;
    rotation = other.rotation;
    scale = other.scale;

    OnTransformChanged();

    return this;
}