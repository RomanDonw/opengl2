#include "EditorDocs.hpp"

#include "engine/external/imgui.hpp"

void EditorDocs::RenderWindow(bool *open)
{
    if (!ImGUI::Begin("Engine Documentation", open, ImGuiWindowFlags_None))
    {
        ImGUI::End();
        return;
    }

    ImGUI::TextWrapped("Evelent Engine — SEScript on objects with a ScriptBehaviour child.");
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
        ImGUI::Text("self.translate / set_position / set_rotation / set_scale");
        ImGUI::Text("self.look_at(x,y,z)  self.face_object(\"Name\")");
        ImGUI::Text("self.move_forward(dist)  move_right / move_up / move_back...");
        ImGUI::Text("self.rotate_y / rotate_x / rotate_z / rotate(x,y,z)");
        ImGUI::Text("self.set_parent(\"Name\")  detach_parent()");
        ImGUI::Text("self.add_tag / remove_tag / clear_tags()");
        ImGUI::Text("self.set_visible(1)  hide_in_play(1)");
        ImGUI::Text("RigidBody: set_velocity, apply_force, apply_impulse, stop");
        ImGUI::Text("Model: set_color, set_mesh(\"cube\"), set_shader, set_render_enabled");
        ImGUI::Text("Camera: set_fov, set_near, set_far, set_active_camera()");
        ImGUI::Text("Light: set_light_color, set_light_intensity, set_light_enabled, set_light_range");
        ImGUI::Text("var x = Scene.find(\"Name\")  var y = Scene.find_tag(\"tag\")");
    }

    if (ImGUI::CollapsingHeader("Scene"))
    {
        ImGUI::Text("Scene.spawn_entity / spawn_model / spawn_camera");
        ImGUI::Text("Scene.spawn_point_light / spawn_directional_light");
        ImGUI::Text("Scene.spawn_freeplay  Scene.set_camera");
        ImGUI::Text("Scene.set_gravity / set_ambient(r,g,b)");
        ImGUI::Text("Scene.set_fog_enabled(1)  set_fog_color  set_fog_distance");
    }

    if (ImGUI::CollapsingHeader("Input & conditions"))
    {
        ImGUI::Text("Input.key_down / key_pressed / key_released");
        ImGUI::Text("Input.mouse_down(0)  mouse_pressed  mouse_captured()");
        ImGUI::Text("Input.any_key_down()  action_down / action_pressed");
        ImGUI::Text("if self.has_tag(\"player\"):");
        ImGUI::Text("if self.distance_less(\"Enemy\", 5):");
        ImGUI::Text("if Math.random_chance(0.3):  Math.coin_flip()");
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
        ImGUI::Text("log / log_warn / log_error");
        ImGUI::Text("Console.show() / hide() / toggle()");
        ImGUI::Text("Engine.quit()");
        ImGUI::Text("Time.set_scale  Time.pause()  Time.resume()");
    }

    if (ImGUI::CollapsingHeader("Viewport gizmos (editor)"))
    {
        ImGUI::BulletText("Camera — cyan pyramid + arrow + frustum; green = active");
        ImGUI::BulletText("Point light — cross + range rings (object color)");
        ImGUI::BulletText("Directional — disc + arrow = light direction (-Z / front)");
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
        ImGUI::BulletText("Cameras & lights show direction icons in viewport (edit mode)");
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
