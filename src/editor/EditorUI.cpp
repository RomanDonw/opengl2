#include "EditorUI.hpp"

#include "EditorDocs.hpp"
#include "EditorGizmos.hpp"
#include "EditorMode.hpp"
#include "EditorScriptEditor.hpp"
#include "EditorState.hpp"
#include "ObjectFactory.hpp"
#include "SceneSerializer.hpp"

#include "engine/Console.hpp"
#include "engine/Engine.hpp"
#include "engine/Input.hpp"
#include "engine/Logger.hpp"
#include "engine/Scene.hpp"
#include "engine/Utils.hpp"
#include "engine/io/FileSystem.hpp"
#include "engine/external/imgui.hpp"
#include "engine/objects/Camera/Camera.hpp"
#include "engine/objects/FreeplayCamera/FreeplayCamera.hpp"
#include "engine/objects/Entity/Entity.hpp"
#include "engine/objects/Light/DirectionalLight.hpp"
#include "engine/objects/Light/PointLight.hpp"
#include "engine/objects/Model/Model.hpp"
#include "engine/objects/RigidBody/RigidBody.hpp"
#include "engine/objects/ScriptBehaviour/ScriptBehaviour.hpp"
#include "engine/physics/RaycastCallbackState.hpp"
#include "engine/physics/RaycastInfo.hpp"
#include "engine/render/RenderTarget.hpp"

#include <cstring>

namespace
{
    constexpr float kHierarchyW = 240.0f;
    constexpr float kInspectorW = 300.0f;
    constexpr float kScriptH = 220.0f;
    constexpr float kScriptDocW = 300.0f;
    constexpr float kToolbarH = 36.0f;

    struct EditorLayout
    {
        ImVec2 workPos;
        ImVec2 workSize;
        float topY;
    };

    EditorLayout calcLayout()
    {
        const ImGuiViewport *vp = ImGUI::GetMainViewport();
        EditorLayout l;
        l.workPos = vp->WorkPos;
        l.workSize = vp->WorkSize;
        l.topY = l.workPos.y;
        return l;
    }

    void dockWindow(const char *title, const ImVec2 &pos, const ImVec2 &size, bool *open = nullptr)
    {
        ImGUI::SetNextWindowPos(pos, ImGuiCond_Always);
        ImGUI::SetNextWindowSize(size, ImGuiCond_Always);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        if (open) ImGUI::Begin(title, open, flags);
        else ImGUI::Begin(title, nullptr, flags);
    }
}

static GameObject *createAndSelect(EditorState &state, const std::string &typeName, const std::string &displayName)
{
    GameObject *obj = ObjectFactory::Create(state.scene, typeName, displayName);
    if (obj) state.Select(obj);
    return obj;
}

static void renderMainBar(EditorState &state)
{
    if (!ImGUI::BeginMainMenuBar()) return;
    ImGUI::Text("Evelent Engine");
    ImGUI::Separator();

    if (ImGUI::BeginMenu("File"))
    {
        if (ImGUI::MenuItem("Save Project", "Ctrl+S")) state.SaveProject();
        if (ImGUI::MenuItem("Save Scene")) state.SaveScene();
        if (ImGUI::MenuItem("Reload Scene")) state.LoadScene(state.scenePath);
        if (ImGUI::MenuItem("New Default Scene"))
        {
            SceneSerializer::CreateDefaultScene(state.scene, &state.project);
            state.ClearSelection();
            state.EnsureEditorCamera();
            state.flyCamera.FocusOn(glm::vec3(0, 1, 0));
            state.SaveScene();
        }
        ImGUI::Separator();
        ImGUI::Text("Project: %s", state.project.config.name.c_str());
        ImGUI::Text("Scene: %s", state.scenePath.c_str());
        ImGUI::EndMenu();
    }

    if (ImGUI::BeginMenu("Add"))
    {
        if (ImGUI::MenuItem("Entity")) createAndSelect(state, "Entity", "Entity");
        if (ImGUI::MenuItem("Model")) createAndSelect(state, "Model", "Model");
        if (ImGUI::MenuItem("Camera")) createAndSelect(state, "Camera", "Camera");
        if (ImGUI::MenuItem("Freeplay Camera")) createAndSelect(state, "FreeplayCamera", "FreeplayCamera");
        if (ImGUI::MenuItem("Directional Light")) createAndSelect(state, "DirectionalLight", "Sun");
        if (ImGUI::MenuItem("Point Light")) createAndSelect(state, "PointLight", "Light");
        ImGUI::EndMenu();
    }

    if (ImGUI::BeginMenu("Help"))
    {
        if (ImGUI::MenuItem("Engine Documentation")) state.showDocs = true;
        ImGUI::EndMenu();
    }

    ImGUI::Separator();
    ImGUI::Text("Mode:");
    ImGUI::SameLine();

    if (ImGUI::RadioButton("Scene", state.activeTab == EditorTab::Scene))
    {
        state.EndPlay();
        state.activeTab = EditorTab::Scene;
    }
    ImGUI::SameLine();
    if (ImGUI::RadioButton("Script", state.activeTab == EditorTab::Script))
    {
        state.EndPlay();
        state.activeTab = EditorTab::Script;
    }
    ImGUI::SameLine();
    if (state.isPlaying)
    {
        ImGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.55f, 0.25f, 1));
        if (ImGUI::Button("Stop")) state.EndPlay();
        ImGUI::PopStyleColor();
        ImGUI::SameLine();
        ImGUI::TextColored(ImVec4(0.3f, 1, 0.4f, 1), "PLAYING");
    }
    else if (ImGUI::Button("Play")) state.BeginPlay();

    if (state.activeTab == EditorTab::Scene && !state.isPlaying)
    {
        ImGUI::SameLine();
        ImGUI::Separator();
        ImGUI::SameLine();
        ImGUI::Text("Gizmo:");
        ImGUI::SameLine();
        if (ImGUI::RadioButton("Sel##gz", state.gizmoMode == GizmoMode::Select)) state.gizmoMode = GizmoMode::Select;
        ImGUI::SameLine();
        if (ImGUI::RadioButton("Move##gz", state.gizmoMode == GizmoMode::Translate)) state.gizmoMode = GizmoMode::Translate;
        ImGUI::SameLine();
        if (ImGUI::RadioButton("Rot##gz", state.gizmoMode == GizmoMode::Rotate)) state.gizmoMode = GizmoMode::Rotate;
        ImGUI::SameLine();
        if (ImGUI::RadioButton("Scale##gz", state.gizmoMode == GizmoMode::Scale)) state.gizmoMode = GizmoMode::Scale;
    }

    ImGUI::EndMainMenuBar();
}

static std::string objectTypeName(GameObject *obj)
{
    if (dynamic_cast<FreeplayCamera *>(obj)) return "FreeplayCamera";
    if (dynamic_cast<Camera *>(obj)) return "Camera";
    if (dynamic_cast<PointLight *>(obj)) return "PointLight";
    if (dynamic_cast<DirectionalLight *>(obj)) return "DirectionalLight";
    if (dynamic_cast<Entity *>(obj)) return "Entity";
    if (dynamic_cast<Model *>(obj)) return "Model";
    return "Model";
}

static GameObject *duplicateObject(EditorState &state, GameObject *src)
{
    if (!src || !state.scene) return nullptr;

    GameObject *copy = ObjectFactory::Create(state.scene, objectTypeName(src), src->displayName + "_copy");
    if (!copy) return nullptr;

    copy->transform = src->transform;
    copy->tags = src->tags;

    if (Model *m = dynamic_cast<Model *>(src))
    {
        if (Model *mc = dynamic_cast<Model *>(copy))
        {
            mc->color = m->color;
            mc->surfaces = m->surfaces;
            mc->usedShaderProgram = m->usedShaderProgram;
        }
    }

    if (Entity *e = dynamic_cast<Entity *>(src))
    {
        if (Entity *ec = dynamic_cast<Entity *>(copy))
        {
            ec->SetRigidBodyType(e->GetRigidBodyType());
            ec->SetMass(e->GetMass());
        }
    }

    return copy;
}

static void renderHierarchyList(EditorState &state)
{
    if (state.scene)
    {
        state.scene->ForEachAllObjects([&](GameObject *obj) -> bool
        {
            if (dynamic_cast<ScriptBehaviour *>(obj)) return true;
            if (obj == state.editorCameraObject) return true;

            const bool selected = state.selected == obj;
            if (ImGUI::Selectable(obj->displayName.c_str(), selected))
            {
                state.Select(obj);
                if (!state.isPlaying) state.flyCamera.FocusOn(obj->GetGlobalTransform().GetPosition());
            }

            if (ImGUI::BeginPopupContextItem())
            {
                if (ImGUI::MenuItem("Focus in Viewport"))
                {
                    state.Select(obj);
                    state.flyCamera.FocusOn(obj->GetGlobalTransform().GetPosition());
                }
                if (ImGUI::MenuItem("Duplicate"))
                {
                    if (GameObject *dup = duplicateObject(state, obj)) state.Select(dup);
                }
                if (ImGUI::MenuItem("Delete"))
                {
                    if (state.selected == obj) state.ClearSelection();
                    state.scene->DeleteObject(obj);
                }
                ImGUI::EndPopup();
            }

            return true;
        });
    }

    if (ImGUI::BeginPopupContextWindow("hierarchy_bg", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
    {
        if (ImGUI::MenuItem("Create Entity")) createAndSelect(state, "Entity", "Entity");
        if (ImGUI::MenuItem("Create Model")) createAndSelect(state, "Model", "Model");
        if (ImGUI::MenuItem("Create Freeplay Camera")) createAndSelect(state, "FreeplayCamera", "FreeplayCamera");
        ImGUI::EndPopup();
    }
}

namespace
{
    enum class FileOpKind
    {
        None,
        CreateFile,
        CreateFolder,
        Rename
    };

    FileOpKind g_fileOpKind = FileOpKind::None;
    char g_fileOpName[256]{};
    std::string g_fileOpTarget;
    std::string g_fileOpFolder;
    std::string g_fileOpExtension;

    static void beginFileOp(FileOpKind kind, const std::string &folder, const std::string &ext, const std::string &target = {})
    {
        g_fileOpKind = kind;
        g_fileOpFolder = folder;
        g_fileOpExtension = ext;
        g_fileOpTarget = target;
        std::memset(g_fileOpName, 0, sizeof(g_fileOpName));
        if (kind == FileOpKind::Rename && !target.empty())
        {
            const size_t slash = target.find_last_of("/\\");
            std::string name = slash == std::string::npos ? target : target.substr(slash + 1);
            const size_t dot = name.find_last_of('.');
            if (dot != std::string::npos) name = name.substr(0, dot);
            std::strncpy(g_fileOpName, name.c_str(), sizeof(g_fileOpName) - 1);
        }
        ImGUI::OpenPopup("ProjectFileOp");
    }

    static std::string joinProjectPath(const std::string &folder, const std::string &name, const std::string &ext)
    {
        std::string file = name;
        if (!ext.empty() && (file.size() < ext.size() || file.substr(file.size() - ext.size()) != ext))
            file += ext;
        return FileSystem::Join(folder, file);
    }

    static void renderFileOpModal(EditorState &state)
    {
        if (g_fileOpKind == FileOpKind::None) return;

        ImVec2 center = ImGUI::GetMainViewport()->GetCenter();
        ImGUI::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (!ImGUI::BeginPopupModal("ProjectFileOp", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            return;

        if (g_fileOpKind == FileOpKind::CreateFile) ImGUI::Text("New file in %s", g_fileOpFolder.c_str());
        else if (g_fileOpKind == FileOpKind::CreateFolder) ImGUI::Text("New folder in %s", g_fileOpFolder.c_str());
        else if (g_fileOpKind == FileOpKind::Rename) ImGUI::Text("Rename %s", g_fileOpTarget.c_str());

        ImGUI::InputText("Name", g_fileOpName, sizeof(g_fileOpName));

        if (ImGUI::Button("OK", ImVec2(120, 0)))
        {
            const std::string name = g_fileOpName;
            if (!name.empty())
            {
                if (g_fileOpKind == FileOpKind::CreateFile)
                {
                    const std::string rel = joinProjectPath(g_fileOpFolder, name, g_fileOpExtension);
                    std::string content;
                    if (g_fileOpExtension == ".scene")
                        content = "# Evelent Engine scene\nambient 0.35 0.36 0.4\nfog 0 24 80 0.42 0.46 0.5\n";
                    else if (g_fileOpExtension == ".ses")
                        content = "# SEScript\nfunc _ready():\n    log(\"ready\")\n\nfunc _update(dt):\n    pass\n";
                    state.project.CreateFile(rel, content);
                }
                else if (g_fileOpKind == FileOpKind::CreateFolder)
                {
                    state.project.CreateFolder(joinProjectPath(g_fileOpFolder, name, ""));
                }
                else if (g_fileOpKind == FileOpKind::Rename)
                {
                    const size_t slash = g_fileOpTarget.find_last_of("/\\");
                    const std::string dir = slash == std::string::npos ? "" : g_fileOpTarget.substr(0, slash + 1);
                    const std::string ext = FileSystem::GetExtension(g_fileOpTarget);
                    const std::string toRel = joinProjectPath(dir, name, ext);
                    if (state.project.RenameRelative(g_fileOpTarget, toRel))
                        state.OnProjectFileRenamed(g_fileOpTarget, toRel);
                }
            }
            g_fileOpKind = FileOpKind::None;
            ImGUI::CloseCurrentPopup();
        }
        ImGUI::SameLine();
        if (ImGUI::Button("Cancel", ImVec2(120, 0)))
        {
            g_fileOpKind = FileOpKind::None;
            ImGUI::CloseCurrentPopup();
        }
        ImGUI::EndPopup();
    }

    static void projectFileContextMenu(EditorState &state, const std::string &path, bool canOpenScene, bool canOpenScript)
    {
        if (!ImGUI::BeginPopupContextItem()) return;

        if (canOpenScene && ImGUI::MenuItem("Open Scene")) state.LoadScene(path);
        if (canOpenScript && ImGUI::MenuItem("Open Script"))
        {
            state.scriptPath = path;
            state.LoadScriptFromDisk();
            state.activeTab = EditorTab::Script;
            state.scriptDirty = false;
        }
        if (ImGUI::MenuItem("Rename")) beginFileOp(FileOpKind::Rename, "", "", path);
        if (ImGUI::MenuItem("Delete") && !state.project.IsProtectedPath(path))
        {
            if (state.project.DeleteRelative(path)) state.OnProjectFileDeleted(path);
        }
        ImGUI::EndPopup();
    }

    static void renderProjectFolder(EditorState &state, const char *label, const std::string &folder, const char *ext,
        bool canOpenScene, bool canOpenScript, const char *newButtonLabel)
    {
        if (!ImGUI::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen)) return;

        if (ImGUI::Button(newButtonLabel))
            beginFileOp(FileOpKind::CreateFile, folder, ext);

        for (const std::string &path : state.project.ListByExtension(folder, ext))
        {
            const bool selected = (canOpenScene && path == state.scenePath) || (canOpenScript && path == state.scriptPath);
            if (ImGUI::Selectable(path.c_str(), selected))
            {
                if (ImGUI::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (canOpenScene) state.LoadScene(path);
                    else if (canOpenScript)
                    {
                        state.scriptPath = path;
                        state.LoadScriptFromDisk();
                        state.activeTab = EditorTab::Script;
                        state.scriptDirty = false;
                    }
                }
            }
            projectFileContextMenu(state, path, canOpenScene, canOpenScript);
        }

        ImGUI::TreePop();
    }

    static void renderProjectBrowser(EditorState &state)
    {
        ImGUI::Text("Evelent Engine");
        ImGUI::Text("Root: %s", state.project.rootPath.c_str());
        ImGUI::Separator();

        if (ImGUI::Button("New Folder"))
            beginFileOp(FileOpKind::CreateFolder, "", "");

        renderProjectFolder(state, "scenes", state.project.config.scenesDir, ".scene", true, false, "New Scene");
        renderProjectFolder(state, "scripts", state.project.config.scriptsDir, ".ses", false, true, "New Script");

        if (ImGUI::TreeNode("docs"))
        {
            const std::string docRel = FileSystem::Join(state.project.config.docsDir, "SEScript.md");
            if (ImGUI::Selectable(docRel.c_str())) state.showDocs = true;
            if (ImGUI::BeginPopupContextItem())
            {
                if (ImGUI::MenuItem("Open Docs")) state.showDocs = true;
                ImGUI::EndPopup();
            }
            ImGUI::TreePop();
        }

        ImGUI::Separator();
        if (ImGUI::Button("Open Docs")) state.showDocs = true;

        renderFileOpModal(state);
    }
}

static void renderLeftPanel(EditorState &state, const EditorLayout &layout, float bottomReserve)
{
    const ImVec2 pos(layout.workPos.x, layout.topY);
    const ImVec2 size(kHierarchyW, layout.workSize.y - bottomReserve);
    dockWindow("Browser", pos, size);

    if (ImGUI::BeginTabBar("left_tabs"))
    {
        if (ImGUI::BeginTabItem("Hierarchy")) { renderHierarchyList(state); ImGUI::EndTabItem(); }
        if (ImGUI::BeginTabItem("Project")) { renderProjectBrowser(state); ImGUI::EndTabItem(); }
        ImGUI::EndTabBar();
    }

    ImGUI::End();
}

static void renderInspector(EditorState &state, const EditorLayout &layout, float bottomReserve)
{
    const ImVec2 winPos(layout.workPos.x + layout.workSize.x - kInspectorW, layout.topY);
    const ImVec2 winSize(kInspectorW, layout.workSize.y - bottomReserve);
    dockWindow("Inspector", winPos, winSize);

    if (!state.selected)
    {
        ImGUI::TextWrapped("Select an object in Hierarchy or click in Viewport (LMB).");
        ImGUI::Separator();
        ImGUI::Text("Viewport: RMB + WASD");
        ImGUI::End();
        return;
    }

    char nameBuf[128];
    std::strncpy(nameBuf, state.selected->displayName.c_str(), sizeof(nameBuf) - 1);
    nameBuf[sizeof(nameBuf) - 1] = '\0';
    if (ImGUI::InputText("Name", nameBuf, sizeof(nameBuf))) state.selected->displayName = nameBuf;

    glm::vec3 position = state.selected->transform.GetPosition();
    glm::vec3 euler = glm::degrees(glm::eulerAngles(state.selected->transform.GetRotation()));
    glm::vec3 scl = state.selected->transform.GetScale();

    if (ImGUI::DragFloat3("Position", &position.x, 0.05f)) state.selected->transform.SetPosition(position);
    if (ImGUI::DragFloat3("Rotation", &euler.x, 0.5f)) state.selected->transform.SetRotation(glm::quat(glm::radians(euler)));
    if (ImGUI::DragFloat3("Scale", &scl.x, 0.02f, 0.01f, 100.0f)) state.selected->transform.SetScale(scl);

    if (Model *m = dynamic_cast<Model *>(state.selected))
    {
        ImGUI::ColorEdit4("Color", &m->color.r);
        if (!m->surfaces.empty())
        {
            char meshBuf[64]{};
            std::strncpy(meshBuf, m->surfaces[0].mesh.c_str(), sizeof(meshBuf) - 1);
            if (ImGUI::InputText("Mesh", meshBuf, sizeof(meshBuf))) m->surfaces[0].mesh = meshBuf;
        }
    }

    if (Entity *e = dynamic_cast<Entity *>(state.selected))
    {
        bool dynamic = e->GetRigidBodyType() == DYNAMIC;
        if (ImGUI::Checkbox("Dynamic", &dynamic)) e->SetRigidBodyType(dynamic ? DYNAMIC : KINEMATIC);
        float mass = e->GetMass();
        if (ImGUI::DragFloat("Mass", &mass, 0.1f, 0.01f, 10000.0f)) e->SetMass(mass);
    }

    if (Camera *cam = dynamic_cast<Camera *>(state.selected))
    {
        float fovDeg = glm::degrees(cam->FOV);
        if (ImGUI::DragFloat("FOV", &fovDeg, 0.5f, 10.0f, 120.0f)) cam->FOV = glm::radians(fovDeg);
        ImGUI::DragFloat("Near", &cam->nearDistance, 0.01f, 0.01f, 10.0f);
        ImGUI::DragFloat("Far", &cam->farDistance, 1.0f, 10.0f, 5000.0f);
        if (ImGUI::Button("Set as View Camera") && state.scene) state.scene->SetCurrentCamera(cam);
    }

    if (PointLight *pl = dynamic_cast<PointLight *>(state.selected))
    {
        ImGUI::Checkbox("Enabled", &pl->enabled);
        ImGUI::ColorEdit3("Light Color", &pl->color.x);
        ImGUI::DragFloat("Intensity", &pl->intensity, 0.05f, 0, 20);
        ImGUI::DragFloat("Range", &pl->range, 0.1f, 0.1f, 100);
    }

    if (DirectionalLight *dl = dynamic_cast<DirectionalLight *>(state.selected))
    {
        ImGUI::Checkbox("Enabled", &dl->enabled);
        ImGUI::ColorEdit3("Light Color", &dl->color.x);
        ImGUI::DragFloat("Intensity", &dl->intensity, 0.05f, 0, 20);
        ImGUI::TextWrapped("Arrow in viewport = light direction (local -Z / front).");
    }

    if (ImGUI::Button("Add / Edit Script"))
    {
        state.EnsureScript(state.selected, false);
        state.activeTab = EditorTab::Script;
    }

    if (ImGUI::Button("Delete Object") && state.scene)
    {
        GameObject *toDelete = state.selected;
        state.ClearSelection();
        state.scene->DeleteObject(toDelete);
    }

    ImGUI::End();
}

static void viewportPick(EditorState &state, const ImVec2 &mouse, const ImVec2 &size)
{
    if (!state.scene || size.x <= 0 || size.y <= 0) return;

    const float ndcX = (mouse.x / size.x) * 2.0f - 1.0f;
    const float ndcY = 1.0f - (mouse.y / size.y) * 2.0f;

    const glm::uvec2 vpSize(static_cast<unsigned>(size.x), static_cast<unsigned>(size.y));
    const glm::mat4 proj = state.flyCamera.GetProjectionMatrix(vpSize);
    const glm::mat4 view = state.flyCamera.GetViewMatrix();
    const glm::mat4 inv = glm::inverse(proj * view);

    const glm::vec4 pNear = inv * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    const glm::vec4 pFar = inv * glm::vec4(ndcX, ndcY, 1.0f, 1.0f);
    const glm::vec3 start = glm::vec3(pNear) / pNear.w;
    const glm::vec3 end = glm::vec3(pFar) / pFar.w;

    GameObject *hitObj = nullptr;
    state.scene->Raycast(start, end, [&](RaycastInfo info) -> RaycastCallbackState
    {
        hitObj = info.rigidbody;
        return STOP;
    });

    if (hitObj) state.Select(hitObj);
}

static glm::vec2 viewportMouseLocal(const ImVec2 &inner)
{
    const ImVec2 mp = ImGUI::GetMousePos();
    const ImVec2 wp = ImGUI::GetWindowPos();
    const ImVec2 cr = ImGUI::GetWindowContentRegionMin();
    return glm::vec2(mp.x - wp.x - cr.x, mp.y - wp.y - cr.y);
}

static void viewportGizmo(EditorState &state, const ImVec2 &inner, bool viewportHovered)
{
    if (!state.selected || state.isPlaying || !viewportHovered) return;

    const glm::vec2 mouse = viewportMouseLocal(inner);
    const bool lmb = ImGUI::IsMouseDown(ImGuiMouseButton_Left) && !Input::IsMouseCaptured();

    if (state.gizmoMode == GizmoMode::Select)
    {
        state.gizmoDragging = false;
        return;
    }

    if (ImGUI::IsMouseClicked(ImGuiMouseButton_Left) && !Input::IsMouseCaptured())
    {
        state.gizmoDragging = true;
        state.gizmoDragStart = mouse;
        state.gizmoStartPos = state.selected->transform.GetPosition();
        state.gizmoStartRot = state.selected->transform.GetRotation();
        state.gizmoStartScale = state.selected->transform.GetScale();
    }

    if (!state.gizmoDragging || !lmb)
    {
        state.gizmoDragging = false;
        return;
    }

    const glm::vec2 delta = mouse - state.gizmoDragStart;
    const glm::vec3 front = state.flyCamera.GetFront();
    const glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    const glm::vec3 up = glm::vec3(0, 1, 0);

    if (state.gizmoMode == GizmoMode::Translate)
    {
        const float scale = 0.02f;
        state.selected->transform.SetPosition(state.gizmoStartPos + right * delta.x * scale + up * (-delta.y) * scale);
    }
    else if (state.gizmoMode == GizmoMode::Rotate)
    {
        const glm::quat rotY = glm::angleAxis(glm::radians(delta.x * 0.5f), glm::vec3(0, 1, 0));
        const glm::quat rotX = glm::angleAxis(glm::radians(-delta.y * 0.5f), right);
        state.selected->transform.SetRotation(rotY * rotX * state.gizmoStartRot);
    }
    else if (state.gizmoMode == GizmoMode::Scale)
    {
        const float factor = 1.0f + delta.x * 0.01f;
        state.selected->transform.SetScale(state.gizmoStartScale * factor);
    }
}

static void renderViewport(EditorState &state, const EditorLayout &layout, float bottomReserve, double delta)
{
    const float centerW = layout.workSize.x - kHierarchyW - kInspectorW;
    const ImVec2 pos(layout.workPos.x + kHierarchyW, layout.topY);
    const ImVec2 size(centerW, layout.workSize.y - bottomReserve);

    ImGUI::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGUI::SetNextWindowSize(size, ImGuiCond_Always);
    ImGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGUI::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    const ImVec2 inner = ImGUI::GetContentRegionAvail();
    if (inner.x >= 32 && inner.y >= 32)
    {
        if (!state.viewportTarget) state.viewportTarget = new RenderTarget();
        state.viewportTarget->Resize(static_cast<unsigned>(inner.x), static_cast<unsigned>(inner.y));

        const bool viewportHovered = ImGUI::IsWindowHovered();
        const bool viewportFocused = ImGUI::IsWindowFocused();

        const bool rmbOrbit = ImGUI::IsMouseDown(ImGuiMouseButton_Right);
        const bool mmbPan = ImGUI::IsMouseDown(ImGuiMouseButton_Middle);
        const bool viewportCameraDrag = rmbOrbit || mmbPan;

        if (viewportHovered && (viewportCameraDrag || Input::IsMouseCaptured()))
        {
            ImGUI::SetNextFrameWantCaptureMouse(false);
            ImGUI::SetNextFrameWantCaptureKeyboard(false);
        }

        if (!state.isPlaying && viewportHovered)
        {
            if (viewportFocused)
            {
                if (ImGUI::IsKeyPressed(ImGuiKey_1)) state.gizmoMode = GizmoMode::Select;
                if (ImGUI::IsKeyPressed(ImGuiKey_2)) state.gizmoMode = GizmoMode::Translate;
                if (ImGUI::IsKeyPressed(ImGuiKey_3)) state.gizmoMode = GizmoMode::Rotate;
                if (ImGUI::IsKeyPressed(ImGuiKey_4)) state.gizmoMode = GizmoMode::Scale;
            }

            if (ImGUI::IsMouseClicked(ImGuiMouseButton_Left) && !Input::IsMouseCaptured())
            {
                if (state.gizmoMode == GizmoMode::Select)
                {
                    const glm::vec2 m = viewportMouseLocal(inner);
                    viewportPick(state, ImVec2(m.x, m.y), inner);
                }
            }
        }

        if (!state.isPlaying) viewportGizmo(state, inner, viewportHovered);

        const bool allowFly = state.isPlaying
            ? false
            : (viewportFocused || viewportHovered || Input::IsMouseCaptured());

        if (!state.isPlaying && viewportFocused && ImGUI::IsKeyPressed(ImGuiKey_F)) state.ToggleFreeplayPreview();

        FreeplayCamera *freeplay = state.FindFreeplayCamera();

        if (!state.isPlaying && state.freeplayViewportPreview && freeplay)
        {
            if (viewportHovered)
            {
                if (ImGUI::IsMouseClicked(ImGuiMouseButton_Right)) Input::SetMouseCaptured(true);
                if (ImGUI::IsMouseReleased(ImGuiMouseButton_Right)) Input::SetMouseCaptured(false);
                if (ImGUI::IsMouseDown(ImGuiMouseButton_Right) || Input::IsMouseCaptured())
                {
                    ImGUI::SetNextFrameWantCaptureMouse(false);
                    ImGUI::SetNextFrameWantCaptureKeyboard(false);
                }
            }

            freeplay->active = true;
            freeplay->moveSpeed = state.project.config.defaultMoveSpeed;
            freeplay->lookSpeed = state.project.config.defaultLookSpeed;
            freeplay->Update(delta);
            if (state.scene) state.scene->SetCurrentCamera(freeplay);
        }
        else if (!state.isPlaying)
        {
            EditorCameraInput camInput;
            if (allowFly && viewportHovered)
            {
                camInput.mouseDelta = glm::vec2(ImGUI::GetIO().MouseDelta.x, ImGUI::GetIO().MouseDelta.y);
                camInput.scrollY = Input::GetScrollDelta().y;
                camInput.orbiting = rmbOrbit;
                camInput.panning = mmbPan;
                Input::ClearScrollDelta();
            }
            state.flyCamera.Update(delta, allowFly, camInput);
            state.SyncEditorCameraObject();
        }

        if (state.scene)
        {
            state.scene->SetRenderTargetSize(static_cast<unsigned>(inner.x), static_cast<unsigned>(inner.y));

            const glm::vec3 clear = state.scene->fog.enabled ? state.scene->fog.color : glm::vec3(0.18f, 0.2f, 0.24f);
            state.viewportTarget->Bind();
            glClearColor(clear.r, clear.g, clear.b, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Engine::Render();
            state.viewportTarget->Unbind();
            state.scene->ClearRenderTargetSize();

            const glm::uvec2 winSize = Engine::GetWindowSize();
            glViewport(0, 0, static_cast<GLsizei>(winSize.x), static_cast<GLsizei>(winSize.y));
        }

        ImGUI::Image(static_cast<ImTextureID>(static_cast<intptr_t>(state.viewportTarget->GetColorTexture())), inner, ImVec2(0, 1), ImVec2(1, 0));
        if (!state.isPlaying)
            EditorGizmos::DrawViewportOverlay(state, ImGUI::GetItemRectMin(), inner);
    }
    else ImGUI::Text("Viewport too small");

    ImGUI::End();
    ImGUI::PopStyleVar();
}

static void renderScriptDocPanel(const EditorLayout &layout)
{
    const ImVec2 pos(layout.workPos.x + layout.workSize.x - kScriptDocW, layout.topY);
    const ImVec2 size(kScriptDocW, layout.workSize.y);
    dockWindow("Script Docs", pos, size);
    EditorDocs::RenderSidePanel();
    ImGUI::End();
}

static void renderScriptMode(EditorState &state, const EditorLayout &layout)
{
    renderLeftPanel(state, layout, 0);

    const float editorX = layout.workPos.x + kHierarchyW;
    const float editorW = layout.workSize.x - kHierarchyW - kScriptDocW;
    const ImVec2 editorPos(editorX, layout.topY);
    const ImVec2 editorSize(editorW, layout.workSize.y);
    EditorScriptEditor::Render(state, editorPos, editorSize);

    renderScriptDocPanel(layout);
}

void EditorUI::Render(EditorState &state, double delta)
{
    renderMainBar(state);

    if (state.showDocs) EditorDocs::RenderWindow(&state.showDocs);

    EditorLayout layout = calcLayout();

    if (state.activeTab == EditorTab::Script)
    {
        renderScriptMode(state, layout);
        return;
    }

    if (state.activeTab == EditorTab::Play || state.isPlaying)
    {
        const EditorLayout playLayout{layout.workPos, layout.workSize, layout.topY};
        renderViewport(state, playLayout, 0, delta);

        ImGUI::SetNextWindowPos(ImVec2(layout.workPos.x + 8, layout.topY + 8), ImGuiCond_Always);
        ImGUI::Begin("Play", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);
        if (ImGUI::Button("Stop Play")) state.EndPlay();
        ImGUI::End();
        return;
    }

    renderLeftPanel(state, layout, 0);
    renderInspector(state, layout, 0);
    renderViewport(state, layout, 0, delta);
}
