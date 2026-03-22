#include "Model.hpp"

#include "../../ResourceManager.hpp"

#include "../../resources/Mesh.hpp"
#include "../../resources/Texture.hpp"
#include "../../resources/ShaderProgram.hpp"

Model::Model(Scene *s, Transform t) : GameObject(s, t) {}
Model::Model(Scene *s) : GameObject(s) {}

Model::~Model() {}

void Model::Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt, const FogRenderSettings *fog)
{
    //GameObject::Render(proj, view, camt, fog);

    if (!enableRender) return;

    ShaderProgram *sp = ResourceManager::GetShaderProgram(usedShaderProgram);
    if (!sp) return;

    sp->UseThisProgram();

    sp->SetUniformMatrix4x4("projection", *proj);
    sp->SetUniformMatrix4x4("view", *view);

    sp->SetUniformVector3("cameraPosition", camt->GetPosition());
    sp->SetUniformVector3("cameraRotation", glm::eulerAngles(camt->GetRotation())); // glm::eulerAngles is unstable.

    sp->SetUniformVector3("cameraFront", camt->GetFront());
    sp->SetUniformVector3("cameraUp", camt->GetUp());
    sp->SetUniformVector3("cameraRight", camt->GetRight());

    sp->SetUniformInteger("fogEnabled", (fog->enabled && fog->startDistance >= 0 && fog->endDistance > 0) ? GL_TRUE : GL_FALSE);
    sp->SetUniformFloat("fogStartDistance", fog->startDistance);
    sp->SetUniformFloat("fogEndDistance", fog->endDistance);
    sp->SetUniformVector3("fogColor", fog->color);

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