#include "Model.hpp"

#include "../../ResourceManager.hpp"
#include "../../render/LightingUniforms.hpp"

#include "../../resources/Mesh.hpp"
#include "../../resources/Texture.hpp"
#include "../../resources/ShaderProgram.hpp"

Model::Model(Scene *s, Transform t) : GameObject(s, t) {}
Model::Model(Scene *s) : GameObject(s) {}

Model::~Model() {}

void Model::Render(const GameObjectRenderData *data)
{
    if (!enableRender || !data || surfaces.empty()) return;

    ShaderProgram *sp = ResourceManager::GetShaderProgram(usedShaderProgram);
    if (!sp) return;

    sp->UseThisProgram();

    sp->SetUniformMatrix4x4("projection", *(data->proj));
    sp->SetUniformMatrix4x4("view", *(data->view));

    sp->SetUniformVector3("cameraPosition", data->cameraPosition);
    sp->SetUniformVector3("cameraRotation", data->cameraRotation);
    sp->SetUniformVector3("cameraFront", data->cameraFront);
    sp->SetUniformVector3("cameraUp", data->cameraUp);
    sp->SetUniformVector3("cameraRight", data->cameraRight);

    sp->SetUniformInteger("fogEnabled", (data->fog->enabled && data->fog->startDistance >= 0 && data->fog->endDistance > 0) ? GL_TRUE : GL_FALSE);
    sp->SetUniformFloat("fogStartDistance", data->fog->startDistance);
    sp->SetUniformFloat("fogEndDistance", data->fog->endDistance);
    sp->SetUniformVector3("fogColor", data->fog->color);

    LightingUniforms::Apply(sp, data->lights);

    const glm::mat4 modelMatrix = GetGlobalTransform().GetTransformationMatrix();

    for (const Surface &surface : surfaces)
    {
        if (!surface.enableRender) continue;

        Mesh *mesh = ResourceManager::GetMesh(surface.mesh);
        if (!mesh) continue;

        Texture *texture = ResourceManager::GetTexture(surface.texture);

        if (surface.culling == FaceCullingType::NoCulling) glDisable(GL_CULL_FACE);
        else
        {
            glEnable(GL_CULL_FACE);

            switch (surface.culling)
            {
                case FaceCullingType::BackFace:
                    glCullFace(GL_BACK);
                    break;

                case FaceCullingType::FrontFace:
                    glCullFace(GL_FRONT);
                    break;

                case FaceCullingType::BothFaces:
                    glCullFace(GL_FRONT_AND_BACK);
                    break;

                default:
                    break;
            }
        }

        sp->SetUniformInteger("texture", 0);
        if (texture) texture->BindTexture();
        else glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        sp->SetUniformInteger("hasTexture", texture ? GL_TRUE : GL_FALSE);

        sp->SetUniformMatrix3x3("textureTransformation", surface.textureTransform.GetTransformationMatrix());
        sp->SetUniformMatrix4x4("model", modelMatrix * surface.transform.GetTransformationMatrix());
        sp->SetUniformVector4("color", color * surface.color);

        if (enableDepthTest && surface.enableDepthTest) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);

        mesh->RenderMesh();
    }
}
