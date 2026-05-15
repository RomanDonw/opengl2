#include "EditorState.hpp"

#include "SceneSerializer.hpp"

#include "engine/Input.hpp"
#include "engine/Logger.hpp"
#include "engine/Engine.hpp"
#include "engine/Scene.hpp"
#include "engine/Utils.hpp"
#include "engine/io/FileSystem.hpp"
#include "engine/objects/Camera/Camera.hpp"
#include "engine/objects/FreeplayCamera/FreeplayCamera.hpp"
#include "engine/objects/RigidBody/RigidBody.hpp"
#include "engine/objects/ScriptBehaviour/ScriptBehaviour.hpp"
#include "engine/render/RenderTarget.hpp"
#include "engine/script/ScriptEngine.hpp"

void EditorState::InitProject()
{
    project.Load(project.rootPath);
    project.EnsureLayout();
    scenePath = project.config.entryScene;
}

bool EditorState::LoadScene(const std::string &relativeScenePath)
{
    if (!scene) EnsureScene();
    scenePath = relativeScenePath;
    const std::string abs = project.Abs(scenePath);
    if (!SceneSerializer::Load(scene, abs, &project))
    {
        SceneSerializer::CreateDefaultScene(scene, &project);
        SceneSerializer::Save(scene, abs, &project);
    }
    EnsureEditorCamera();
    SetEditorVisualDefaults();
    ClearSelection();
    return true;
}

bool EditorState::SaveScene()
{
    if (!scene) return false;
    project.EnsureParentDirs(scenePath);
    return SceneSerializer::Save(scene, project.Abs(scenePath), &project);
}

bool EditorState::SaveProject()
{
    if (!scene) return false;
    SaveScene();
    scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (ScriptBehaviour *sb = dynamic_cast<ScriptBehaviour *>(obj))
        {
            if (!sb->GetScriptFile().empty())
                project.WriteTextFile(sb->GetScriptFile(), sb->GetSource());
        }
        return true;
    });
    return project.SaveConfig();
}

std::string EditorState::GetScriptPathFor(GameObject *obj) const
{
    if (!obj) return {};
    if (ScriptBehaviour *sb = const_cast<EditorState *>(this)->GetScriptFor(obj))
    {
        if (!sb->GetScriptFile().empty()) return sb->GetScriptFile();
    }
    return project.DefaultScriptPath(obj->displayName);
}

void EditorState::LoadScriptFromDisk()
{
    if (scriptPath.empty()) return;
    std::string text;
    if (project.ReadTextFile(scriptPath, &text)) scriptBuffer = text;
}

void EditorState::OnProjectFileDeleted(const std::string &relativePath)
{
    if (relativePath == scenePath)
    {
        scenePath.clear();
        ClearSelection();
    }
    if (relativePath == scriptPath)
    {
        scriptPath.clear();
        scriptBuffer.clear();
        scriptDirty = false;
    }
}

void EditorState::OnProjectFileRenamed(const std::string &fromRel, const std::string &toRel)
{
    if (fromRel == scenePath) scenePath = toRel;
    if (fromRel == scriptPath) scriptPath = toRel;

    if (scene)
    {
        scene->ForEachAllObjects([&](GameObject *obj) -> bool
        {
            if (ScriptBehaviour *sb = dynamic_cast<ScriptBehaviour *>(obj))
            {
                if (sb->GetScriptFile() == fromRel) sb->SetScriptFile(toRel);
            }
            return true;
        });
    }
}

void EditorState::SaveScriptToDisk()
{
    if (scriptPath.empty()) return;

    project.WriteTextFile(scriptPath, scriptBuffer);

    if (ScriptBehaviour *sb = GetScriptFor(selected))
    {
        sb->SetScriptFile(scriptPath);
        sb->SetSource(scriptBuffer);
    }
    scriptDirty = false;
}

void EditorState::EnsureScene()
{
    if (scene) return;

    if (!Engine::HasScene("editor")) Engine::CreateScene("editor");
    scene = Engine::GetScene("editor");
    Engine::SetCurrentScene("editor");

    editorCameraObject = scene->CreateObject<Camera>();
    editorCameraObject->displayName = "__EditorCamera__";
    editorCameraObject->hiddenInPlay = true;
    EnsureEditorCamera();
}

void EditorState::EnsureEditorCamera()
{
    if (!scene) return;

    const bool valid = editorCameraObject && scene->HasObject(editorCameraObject);
    if (!valid)
    {
        editorCameraObject = scene->CreateObject<Camera>();
        editorCameraObject->displayName = "__EditorCamera__";
        editorCameraObject->hiddenInPlay = true;
        Logger::Info("Editor camera recreated");
    }

    if (!isPlaying && !freeplayViewportPreview) scene->SetCurrentCamera(editorCameraObject);
}

FreeplayCamera *EditorState::FindFreeplayCamera() const
{
    if (!scene) return nullptr;
    FreeplayCamera *found = nullptr;
    scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (FreeplayCamera *fp = dynamic_cast<FreeplayCamera *>(obj))
        {
            found = fp;
            return false;
        }
        return true;
    });
    return found;
}

void EditorState::SyncFlyFromFreeplay(FreeplayCamera *fp)
{
    if (!fp) return;
    const glm::vec3 pos = fp->GetGlobalTransform().GetPosition();
    flyCamera.FocusOn(pos);
}

void EditorState::ToggleFreeplayPreview()
{
    FreeplayCamera *fp = FindFreeplayCamera();
    if (!fp)
    {
        Logger::Warning("No FreeplayCamera in scene. Add one from the Add menu.");
        return;
    }

    freeplayViewportPreview = !freeplayViewportPreview;
    if (freeplayViewportPreview)
    {
        fp->moveSpeed = project.config.defaultMoveSpeed;
        fp->lookSpeed = project.config.defaultLookSpeed;
        fp->Activate();
        if (scene) scene->SetCurrentCamera(fp);
    }
    else
    {
        fp->Deactivate();
        EnsureEditorCamera();
        SyncEditorCameraObject();
    }
}

void EditorState::SyncFreeplayFromFly()
{
    FreeplayCamera *fp = FindFreeplayCamera();
    if (!fp || !freeplayViewportPreview) return;

    fp->transform.SetPosition(flyCamera.GetPosition());
    const glm::vec3 front = flyCamera.GetFront();
    const glm::mat4 look = glm::lookAt(glm::vec3(0.0f), front, glm::vec3(0.0f, 1.0f, 0.0f));
    fp->transform.SetRotation(glm::quat_cast(look));
    fp->SyncAnglesFromTransform();
    if (scene) scene->SetCurrentCamera(fp);
}

size_t EditorState::GetSceneObjectCount() const
{
    return scene ? scene->GetObjectCount() : 0;
}

glm::vec3 EditorState::GetSceneClearColor() const
{
    return scene ? scene->fog.color : glm::vec3(0.15f);
}

void EditorState::SetEditorVisualDefaults()
{
    if (!scene) return;
    scene->fog.enabled = false;
    if (scene->ambientLight.x < 0.25f) scene->ambientLight = glm::vec3(0.35f, 0.36f, 0.4f);
}

void EditorState::Select(GameObject *obj)
{
    if (obj && dynamic_cast<ScriptBehaviour *>(obj)) obj = obj->GetParent();
    selected = obj;

    if (!obj)
    {
        scriptBuffer.clear();
        scriptPath.clear();
        scriptDirty = false;
        return;
    }

    if (ScriptBehaviour *sb = GetScriptFor(obj))
    {
        scriptPath = GetScriptPathFor(obj);
        if (sb->GetScriptFile().empty()) sb->SetScriptFile(scriptPath);

        if (!sb->GetSource().empty()) scriptBuffer = sb->GetSource();
        else LoadScriptFromDisk();

        if (scriptBuffer.empty())
        {
            scriptBuffer =
                "# SEScript\n"
                "func _ready():\n"
                "    log(\"ready\")\n"
                "\n"
                "func _update(dt):\n"
                "    pass\n";
        }
        scriptDirty = false;
    }
    else
    {
        scriptBuffer.clear();
        scriptPath.clear();
        scriptDirty = false;
    }
}

void EditorState::ClearSelection()
{
    selected = nullptr;
    scriptBuffer.clear();
    scriptPath.clear();
    scriptDirty = false;
}

ScriptBehaviour *EditorState::GetScriptFor(GameObject *obj)
{
    if (!obj) return nullptr;
    for (GameObject *child : obj->GetChildren())
    {
        if (ScriptBehaviour *sb = dynamic_cast<ScriptBehaviour *>(child)) return sb;
    }
    return nullptr;
}

ScriptBehaviour *EditorState::EnsureScript(GameObject *obj)
{
    if (!obj || !scene) return nullptr;
    if (ScriptBehaviour *existing = GetScriptFor(obj)) return existing;

    ScriptBehaviour *sb = scene->CreateObject<ScriptBehaviour>();
    sb->SetParent(obj, false);
    sb->SetScriptFile(project.DefaultScriptPath(obj->displayName));
    sb->SetSource(
        "# SEScript\n"
        "func _ready():\n"
        "    log(\"ready\")\n"
        "\n"
        "func _update(dt):\n"
        "    if Input.key_down(\"W\"):\n"
        "        self.translate(0, 0, -4 * dt)\n");
    Select(obj);
    return sb;
}

void EditorState::BeginPlay()
{
    if (!scene || isPlaying) return;

    SaveScene();

    playSnapshot.clear();
    scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        ObjectSnapshot snap;
        snap.position = obj->transform.GetPosition();
        snap.rotation = obj->transform.GetRotation();
        snap.scale = obj->transform.GetScale();
        snap.hadRigidBody = false;
        snap.linearVelocity = glm::vec3(0);
        if (RigidBody *rb = dynamic_cast<RigidBody *>(obj))
        {
            snap.hadRigidBody = true;
            snap.linearVelocity = rb->GetLinearVelocity();
        }
        playSnapshot[obj] = snap;
        return true;
    });

    if (FreeplayCamera *fp = FindFreeplayCamera()) fp->Deactivate();

    playCamera = nullptr;
    scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (dynamic_cast<FreeplayCamera *>(obj)) return true;
        if (Camera *c = dynamic_cast<Camera *>(obj))
        {
            if (obj != editorCameraObject && obj->displayName != "__EditorCamera__")
            {
                if (!playCamera || obj->displayName == "MainCamera") playCamera = c;
            }
        }
        return true;
    });

    if (playCamera) scene->SetCurrentCamera(playCamera);

    isPlaying = true;
    activeTab = EditorTab::Play;

    const std::string sceneName = Engine::GetCurrentScene();
    Engine::SetCurrentScene("");
    Engine::SetCurrentScene(sceneName);
}

void EditorState::EndPlay()
{
    if (!scene || !isPlaying) return;

    Input::SetMouseCaptured(false);

    for (const auto &pair : playSnapshot)
    {
        GameObject *obj = pair.first;
        if (!obj) continue;
        const ObjectSnapshot &snap = pair.second;
        obj->transform.SetPosition(snap.position);
        obj->transform.SetRotation(snap.rotation);
        obj->transform.SetScale(snap.scale);
        if (snap.hadRigidBody)
        {
            if (RigidBody *rb = dynamic_cast<RigidBody *>(obj))
                rb->SetLinearVelocity(snap.linearVelocity);
        }
    }

    playSnapshot.clear();
    isPlaying = false;
    freeplayViewportPreview = false;
    activeTab = EditorTab::Scene;
    if (FreeplayCamera *fp = FindFreeplayCamera()) fp->Deactivate();
    EnsureEditorCamera();
}

void EditorState::SyncEditorCameraObject()
{
    EnsureEditorCamera();
    if (!editorCameraObject) return;
    editorCameraObject->transform.SetPosition(flyCamera.GetPosition());
    const glm::vec3 front = flyCamera.GetFront();
    const glm::mat4 look = glm::lookAt(glm::vec3(0.0f), front, glm::vec3(0.0f, 1.0f, 0.0f));
    editorCameraObject->transform.SetRotation(glm::quat_cast(look));
}
