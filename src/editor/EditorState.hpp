#ifndef EDITORSTATE_HPP
#define EDITORSTATE_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "EditorMode.hpp"
#include "EditorCamera.hpp"
#include "EditorProject.hpp"
#include "engine/external/glm.hpp"

class Scene;
class GameObject;
class Camera;
class FreeplayCamera;
class RenderTarget;
class ScriptBehaviour;

struct ObjectSnapshot
{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec3 linearVelocity;
    bool hadRigidBody;
};

class EditorState final
{
    public:
        EditorTab activeTab = EditorTab::Scene;
        GizmoMode gizmoMode = GizmoMode::Select;
        bool isPlaying = false;
        bool freeplayViewportPreview = false;

        EditorProject project;
        Scene *scene = nullptr;
        Camera *editorCameraObject = nullptr;
        Camera *playCamera = nullptr;
        EditorCamera flyCamera;
        RenderTarget *viewportTarget = nullptr;

        GameObject *selected = nullptr;
        std::string scenePath;
        std::string scriptBuffer;
        std::string scriptPath;
        bool scriptDirty = false;

        std::unordered_map<GameObject *, ObjectSnapshot> playSnapshot;

        bool gizmoDragging = false;
        glm::vec2 gizmoDragStart{0.0f};
        glm::vec3 gizmoStartPos{0.0f};
        glm::quat gizmoStartRot{1, 0, 0, 0};
        glm::vec3 gizmoStartScale{1.0f};
        bool showDocs = false;

        void InitProject();
        bool LoadScene(const std::string &relativeScenePath);
        bool SaveScene();
        bool SaveProject();
        void SaveScriptToDisk();
        void LoadScriptFromDisk();
        std::string GetScriptPathFor(GameObject *obj) const;
        void OnProjectFileDeleted(const std::string &relativePath);
        void OnProjectFileRenamed(const std::string &fromRel, const std::string &toRel);

        void EnsureScene();
        void EnsureEditorCamera();
        FreeplayCamera *FindFreeplayCamera() const;
        void ToggleFreeplayPreview();
        void SyncFreeplayFromFly();
        void SyncFlyFromFreeplay(FreeplayCamera *fp);
        size_t GetSceneObjectCount() const;
        glm::vec3 GetSceneClearColor() const;
        void SetEditorVisualDefaults();
        void Select(GameObject *obj);
        void ClearSelection();
        ScriptBehaviour *GetScriptFor(GameObject *obj);
        ScriptBehaviour *EnsureScript(GameObject *obj);
        void BeginPlay();
        void EndPlay();
        void SyncEditorCameraObject();
};

#endif
