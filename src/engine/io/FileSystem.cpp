#include "FileSystem.hpp"

#include "../Utils.hpp"

#include <filesystem>

namespace fs = std::filesystem;

bool FileSystem::Exists(const std::string &path) { return fs::exists(path); }

bool FileSystem::IsFile(const std::string &path) { return fs::is_regular_file(path); }

bool FileSystem::IsDirectory(const std::string &path) { return fs::is_directory(path); }

bool FileSystem::CreateDirectory(const std::string &path)
{
    std::error_code ec;
    return fs::create_directories(path, ec) || fs::is_directory(path);
}

bool FileSystem::Delete(const std::string &path)
{
    std::error_code ec;
    return static_cast<bool>(fs::remove_all(path, ec));
}

bool FileSystem::Copy(const std::string &from, const std::string &to)
{
    std::error_code ec;
    if (fs::is_directory(from))
        fs::copy(from, to, fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
    else
        fs::copy_file(from, to, fs::copy_options::overwrite_existing, ec);
    return !ec;
}

bool FileSystem::ReadText(const std::string &path, std::string *out)
{
    if (!out) return false;
    return Utils::ReadTextFile(path, out);
}

bool FileSystem::WriteText(const std::string &path, const std::string &content)
{
    return Utils::WriteTextFile(path, content);
}

bool FileSystem::AppendText(const std::string &path, const std::string &content)
{
    std::string existing;
    if (Exists(path)) ReadText(path, &existing);
    return WriteText(path, existing + content);
}

std::vector<std::string> FileSystem::List(const std::string &path)
{
    std::vector<std::string> result;
    if (!fs::is_directory(path)) return result;

    for (const auto &entry : fs::directory_iterator(path))
        result.push_back(entry.path().filename().string());

    return result;
}

std::string FileSystem::Join(const std::string &a, const std::string &b)
{
    return (fs::path(a) / b).string();
}

std::string FileSystem::GetExtension(const std::string &path)
{
    return fs::path(path).extension().string();
}

std::string FileSystem::GetFileName(const std::string &path)
{
    return fs::path(path).filename().string();
}
