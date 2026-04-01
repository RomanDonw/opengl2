#include "Model.hpp"

#include "../../ResourceManager.hpp"
#include "../../render/SSBO.hpp"

#include "../../resources/Mesh.hpp"
#include "../../resources/Texture.hpp"
#include "../../resources/ShaderProgram.hpp"

Model::Model(Scene *s, Transform t) : GameObject(s, t) {}
Model::Model(Scene *s) : GameObject(s) {}

Model::~Model() {}

void Model::Render(const GameObjectRenderData *data)
{
    //GameObject::Render(data);

    if (!enableRender) return;

    ShaderProgram *sp = ResourceManager::GetShaderProgram(usedShaderProgram);
    if (!sp) return;

    sp->UseThisProgram();

    sp->SetUniformMatrix4x4("projection", *(data->proj));
    sp->SetUniformMatrix4x4("view", *(data->view));

    sp->SetUniformVector3("cameraPosition", data->camt->GetPosition());
    sp->SetUniformVector3("cameraRotation", glm::eulerAngles(data->camt->GetRotation())); // glm::eulerAngles is unstable.

    sp->SetUniformVector3("cameraFront", data->camt->GetFront());
    sp->SetUniformVector3("cameraUp", data->camt->GetUp());
    sp->SetUniformVector3("cameraRight", data->camt->GetRight());

    sp->SetUniformInteger("fogEnabled", (data->fog->enabled && data->fog->startDistance >= 0 && data->fog->endDistance > 0) ? GL_TRUE : GL_FALSE);
    sp->SetUniformFloat("fogStartDistance", data->fog->startDistance);
    sp->SetUniformFloat("fogEndDistance", data->fog->endDistance);
    sp->SetUniformVector3("fogColor", data->fog->color);

    sp->SetUniformVector3("ambientLight", *(data->ambientlight));

    data->pointlightsssbo->BindToSlot(0);
    sp->SetUniformInteger("pointLightsCount", data->pointlightscount);

    Transform globt = GetGlobalTransform();
    glm::mat4 mdl = globt.GetTransformationMatrix();

    for (Surface surface : surfaces)
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
            }
        }

        sp->SetUniformInteger("texture", 0);
        if (texture) texture->BindTexture();
        else glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        sp->SetUniformInteger("hasTexture", texture ? GL_TRUE : GL_FALSE);

        sp->SetUniformMatrix3x3("textureTransformation", surface.textureTransform.GetTransformationMatrix());

        sp->SetUniformMatrix4x4("model", mdl * surface.transform.GetTransformationMatrix());
        sp->SetUniformVector4("color", color * surface.color);

        if (enableDepthTest && surface.enableDepthTest) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
        mesh->RenderMesh();
    }
}