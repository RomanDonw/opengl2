#include "EditorDocs.hpp"

#include "engine/external/imgui.hpp"

void EditorDocs::RenderWindow(bool *open)
{
    if (!ImGUI::Begin("Engine Documentation", open, ImGuiWindowFlags_None))
    {
        ImGUI::End();
        return;
    }

    ImGUI::TextWrapped("SEScript — line-based scripting on objects with a ScriptBehaviour child.");
    ImGUI::Separator();

    if (ImGUI::CollapsingHeader("Script structure", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGUI::BulletText("func _ready(): — runs once when scene loads");
        ImGUI::BulletText("func _update(dt): — every frame");
        ImGUI::BulletText("func _fixed_update(dt): — physics step");
        ImGUI::BulletText("if Input.key_down(\"W\"): — next line runs when true");
        ImGUI::BulletText("# comment");
    }

    if (ImGUI::CollapsingHeader("Object (self / Scene.find)", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGUI::Text("self.translate(x, y, z)");
        ImGUI::Text("self.set_position(x, y, z)");
        ImGUI::Text("self.set_rotation(x, y, z)  # euler degrees");
        ImGUI::Text("self.set_scale(x, y, z)");
        ImGUI::Text("self.rotate_y(degrees)");
        ImGUI::Text("self.set_velocity(x, y, z)   # RigidBody");
        ImGUI::Text("self.apply_force(x, y, z)");
        ImGUI::Text("self.set_mass(value)");
        ImGUI::Text("self.set_dynamic() / self.set_kinematic()");
        ImGUI::Text("self.set_color(r, g, b, a)   # Model");
        ImGUI::Text("self.set_name(\"Name\")");
        ImGUI::Text("self.add_tag(\"tag\")");
        ImGUI::Text("self.delete()  /  self.destroy()");
        ImGUI::Text("var x = Scene.find(\"Name\")");
    }

    if (ImGUI::CollapsingHeader("Scene"))
    {
        ImGUI::Text("Scene.find(\"Name\")");
        ImGUI::Text("Scene.delete(\"Name\")");
        ImGUI::Text("Scene.spawn_entity(\"Name\")");
        ImGUI::Text("Scene.spawn_model(\"Name\")");
        ImGUI::Text("Scene.spawn_camera(\"Name\")");
        ImGUI::Text("Scene.spawn_freeplay(\"Name\")");
        ImGUI::Text("Scene.set_camera(\"Name\")");
        ImGUI::Text("Scene.set_gravity(x, y, z)");
    }

    if (ImGUI::CollapsingHeader("Input"))
    {
        ImGUI::Text("Input.key_down(\"W\")  — also A S D Space");
        ImGUI::Text("Input.action_down(\"MoveForward\")");
        ImGUI::Text("Input.get_axis(\"MoveX\")");
    }

    if (ImGUI::CollapsingHeader("FileSystem (FS)"))
    {
        ImGUI::Text("FS.write(\"path\", \"text\")");
        ImGUI::Text("FS.append(\"path\", \"text\")");
        ImGUI::Text("FS.delete(\"path\")");
        ImGUI::Text("FS.exists(\"path\")");
    }

    if (ImGUI::CollapsingHeader("Engine / Time"))
    {
        ImGUI::Text("log(\"message\")  /  print(\"message\")");
        ImGUI::Text("Engine.quit()");
        ImGUI::Text("Time.set_scale(1.0)");
    }

    if (ImGUI::CollapsingHeader("Project files"))
    {
        ImGUI::BulletText("project.json — config (entry scene, paths)");
        ImGUI::BulletText("scenes/*.scene — level data");
        ImGUI::BulletText("scripts/*.ses — external script files");
        ImGUI::BulletText("Save Project (Ctrl+S) — scene + scripts + config");
        ImGUI::BulletText("Project tab — browse and open scenes/scripts");
    }

    if (ImGUI::CollapsingHeader("Editor controls"))
    {
        ImGUI::BulletText("Viewport: RMB + WASD/E/Q — fly camera");
        ImGUI::BulletText("F — FreeplayCamera preview (edit only)");
        ImGUI::BulletText("Viewport: LMB — select / gizmo drag");
        ImGUI::BulletText("F — toggle Freeplay camera preview (edit mode)");
        ImGUI::BulletText("1/2/3/4 — Select / Move / Rotate / Scale gizmo");
        ImGUI::BulletText("Hierarchy RMB — context menu");
    }

    ImGUI::End();
}

void EditorDocs::RenderSidePanel()
{
    ImGUI::TextWrapped("SEScript");
    ImGUI::Separator();

    if (ImGUI::CollapsingHeader("Structure", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGUI::BulletText("func _ready():");
        ImGUI::BulletText("func _update(dt):");
        ImGUI::BulletText("if Input.key_down(\"W\"):");
        ImGUI::BulletText("# comment");
        ImGUI::BulletText("Tab = 4 spaces");
    }

    if (ImGUI::CollapsingHeader("self", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGUI::TextWrapped("self.translate(x,y,z)");
        ImGUI::TextWrapped("self.set_position(x,y,z)");
        ImGUI::TextWrapped("self.rotate_y(deg)");
        ImGUI::TextWrapped("self.delete()");
    }

    if (ImGUI::CollapsingHeader("Scene"))
    {
        ImGUI::TextWrapped("Scene.find(\"Name\")");
        ImGUI::TextWrapped("Scene.delete(\"Name\")");
        ImGUI::TextWrapped("Scene.spawn_entity(\"N\")");
        ImGUI::TextWrapped("Scene.set_camera(\"N\")");
    }

    if (ImGUI::CollapsingHeader("Input / FS"))
    {
        ImGUI::TextWrapped("Input.key_down(\"W\")");
        ImGUI::TextWrapped("FS.write(\"p\", \"t\")");
        ImGUI::TextWrapped("log(\"msg\")");
    }
}
