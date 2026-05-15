#include "EditorProject.hpp"

#include "engine/Logger.hpp"
#include "engine/Utils.hpp"
#include "engine/io/FileSystem.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

static std::string trimLine(std::string s)
{
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t')) s.pop_back();
    size_t a = 0;
    while (a < s.size() && (s[a] == ' ' || s[a] == '\t')) ++a;
    return s.substr(a);
}

bool EditorProject::Load(const std::string &root)
{
    rootPath = root;
    EnsureLayout();

    const std::string cfgPath = Abs("project.json");
    std::string text;
    if (!Utils::ReadTextFile(cfgPath, &text))
    {
        Logger::Info("Creating default project.json at " + cfgPath);
        SaveConfig();
        return true;
    }

    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line))
    {
        line = trimLine(line);
        if (line.empty() || line[0] == '#') continue;

        const size_t sp = line.find(' ');
        if (sp == std::string::npos) continue;

        const std::string key = line.substr(0, sp);
        const std::string val = trimLine(line.substr(sp + 1));

        if (key == "name") config.name = val;
        else if (key == "entry_scene") config.entryScene = val;
        else if (key == "scripts_dir") config.scriptsDir = val;
        else if (key == "scenes_dir") config.scenesDir = val;
        else if (key == "docs_dir") config.docsDir = val;
        else if (key == "prefer_freeplay_camera") config.preferFreeplayCamera = val == "1" || val == "true";
        else if (key == "move_speed") config.defaultMoveSpeed = std::stof(val);
        else if (key == "look_speed") config.defaultLookSpeed = std::stof(val);
    }

    return true;
}

bool EditorProject::SaveConfig() const
{
    std::ostringstream oss;
    oss << "# Evelent Engine project config\n";
    oss << "name " << config.name << "\n";
    oss << "entry_scene " << config.entryScene << "\n";
    oss << "scripts_dir " << config.scriptsDir << "\n";
    oss << "scenes_dir " << config.scenesDir << "\n";
    oss << "docs_dir " << config.docsDir << "\n";
    oss << "prefer_freeplay_camera " << (config.preferFreeplayCamera ? 1 : 0) << "\n";
    oss << "move_speed " << config.defaultMoveSpeed << "\n";
    oss << "look_speed " << config.defaultLookSpeed << "\n";
    return WriteTextFile("project.json", oss.str());
}

void EditorProject::EnsureLayout() const
{
    fs::create_directories(rootPath);
    fs::create_directories(Abs(config.scenesDir));
    fs::create_directories(Abs(config.scriptsDir));
    fs::create_directories(Abs(config.docsDir));

    const std::string docs = DocsPath();
    if (!FileSystem::Exists(docs))
    {
        const std::string doc =
            "# SEScript Reference\n\n"
            "See Help -> Engine Documentation in the editor.\n\n"
            "func _ready() / func _update(dt)\n"
            "self.translate(x,y,z)  Scene.find(\"Name\")  Input.key_down(\"W\")\n"
            "FS.write(\"path\", \"text\")  log(\"msg\")\n";
        Utils::WriteTextFile(docs, doc);
    }
}

std::string EditorProject::Abs(const std::string &relative) const
{
    return FileSystem::Join(rootPath, relative);
}

std::string EditorProject::Rel(const std::string &absolute) const
{
    std::error_code ec;
    const fs::path rel = fs::relative(fs::path(absolute), fs::path(rootPath), ec);
    if (ec) return absolute;
    return rel.generic_string();
}

std::string EditorProject::DefaultScriptPath(const std::string &objectName) const
{
    return FileSystem::Join(config.scriptsDir, objectName + ".ses");
}

std::string EditorProject::DocsPath() const { return Abs(FileSystem::Join(config.docsDir, "SEScript.md")); }

std::vector<ProjectEntry> EditorProject::ListTree() const
{
    std::vector<ProjectEntry> out;
    if (!fs::exists(rootPath)) return out;

    for (auto it = fs::recursive_directory_iterator(rootPath); it != fs::recursive_directory_iterator(); ++it)
    {
        const fs::path p = it->path();
        std::error_code ec;
        const fs::path rel = fs::relative(p, rootPath, ec);
        if (ec) continue;

        const std::string relStr = rel.generic_string();
        if (relStr.find(".git") != std::string::npos) continue;

        ProjectEntry e;
        e.relativePath = relStr;
        e.name = p.filename().string();
        e.isDirectory = it->is_directory();
        out.push_back(e);
    }

    std::sort(out.begin(), out.end(), [](const ProjectEntry &a, const ProjectEntry &b)
    {
        if (a.isDirectory != b.isDirectory) return a.isDirectory > b.isDirectory;
        return a.relativePath < b.relativePath;
    });

    return out;
}

std::vector<std::string> EditorProject::ListByExtension(const std::string &subdir, const std::string &ext) const
{
    std::vector<std::string> out;
    const std::string dir = Abs(subdir);
    if (!fs::is_directory(dir)) return out;

    for (const auto &entry : fs::directory_iterator(dir))
    {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() == ext) out.push_back(Rel(entry.path().string()));
    }
    std::sort(out.begin(), out.end());
    return out;
}

bool EditorProject::ReadTextFile(const std::string &relative, std::string *out) const
{
    return Utils::ReadTextFile(Abs(relative), out);
}

bool EditorProject::WriteTextFile(const std::string &relative, const std::string &content) const
{
    EnsureParentDirs(relative);
    return Utils::WriteTextFile(Abs(relative), content);
}

bool EditorProject::EnsureParentDirs(const std::string &relativeFile) const
{
    const fs::path parent = fs::path(Abs(relativeFile)).parent_path();
    if (parent.empty()) return true;
    std::error_code ec;
    fs::create_directories(parent, ec);
    return !ec;
}

bool EditorProject::IsProtectedPath(const std::string &relativePath) const
{
    if (relativePath.empty()) return true;
    if (relativePath == "project.json") return true;
    return false;
}

bool EditorProject::CreateFolder(const std::string &relativeDir) const
{
    if (relativeDir.empty() || IsProtectedPath(relativeDir)) return false;
    std::error_code ec;
    fs::create_directories(Abs(relativeDir), ec);
    return !ec;
}

bool EditorProject::CreateFile(const std::string &relativeFile, const std::string &content) const
{
    if (relativeFile.empty() || IsProtectedPath(relativeFile)) return false;
    return WriteTextFile(relativeFile, content);
}

bool EditorProject::DeleteRelative(const std::string &relativePath) const
{
    if (IsProtectedPath(relativePath)) return false;
    const std::string abs = Abs(relativePath);
    if (!fs::exists(abs)) return false;
    std::error_code ec;
    if (fs::is_directory(abs)) return static_cast<bool>(fs::remove_all(abs, ec)) && !ec;
    return fs::remove(abs, ec) && !ec;
}

bool EditorProject::RenameRelative(const std::string &fromRel, const std::string &toRel) const
{
    if (IsProtectedPath(fromRel) || IsProtectedPath(toRel) || fromRel.empty() || toRel.empty()) return false;
    const std::string fromAbs = Abs(fromRel);
    const std::string toAbs = Abs(toRel);
    if (!fs::exists(fromAbs)) return false;
    EnsureParentDirs(toRel);
    std::error_code ec;
    fs::rename(fromAbs, toAbs, ec);
    return !ec;
}
