#ifndef EDITORPROJECT_HPP
#define EDITORPROJECT_HPP

#include "ProjectConfig.hpp"

#include <string>
#include <vector>

struct ProjectEntry
{
    std::string relativePath;
    std::string name;
    bool isDirectory = false;
};

class EditorProject final
{
    public:
        std::string rootPath = "projects/default";
        ProjectConfig config;

        bool Load(const std::string &root);
        bool SaveConfig() const;
        void EnsureLayout() const;

        std::string Abs(const std::string &relative) const;
        std::string Rel(const std::string &absolute) const;

        std::string DefaultScriptPath(const std::string &objectName) const;
        std::string DocsPath() const;

        std::vector<ProjectEntry> ListTree() const;
        std::vector<std::string> ListByExtension(const std::string &subdir, const std::string &ext) const;

        bool ReadTextFile(const std::string &relative, std::string *out) const;
        bool WriteTextFile(const std::string &relative, const std::string &content) const;
        bool EnsureParentDirs(const std::string &relativeFile) const;

        bool CreateFolder(const std::string &relativeDir) const;
        bool CreateFile(const std::string &relativeFile, const std::string &content = "") const;
        bool DeleteRelative(const std::string &relativePath) const;
        bool RenameRelative(const std::string &fromRel, const std::string &toRel) const;
        bool IsProtectedPath(const std::string &relativePath) const;
};

#endif
