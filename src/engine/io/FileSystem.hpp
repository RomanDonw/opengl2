#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>

namespace FileSystem
{
    bool Exists(const std::string &path);
    bool IsFile(const std::string &path);
    bool IsDirectory(const std::string &path);

    bool CreateDirectory(const std::string &path);
    bool Delete(const std::string &path);
    bool Copy(const std::string &from, const std::string &to);

    bool ReadText(const std::string &path, std::string *out);
    bool WriteText(const std::string &path, const std::string &content);
    bool AppendText(const std::string &path, const std::string &content);

    std::vector<std::string> List(const std::string &path);
    std::string Join(const std::string &a, const std::string &b);
    std::string GetExtension(const std::string &path);
    std::string GetFileName(const std::string &path);
}

#endif
