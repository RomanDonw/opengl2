#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warning,
    Error
};

class Logger final
{
    private:
        Logger() = delete;

        static inline LogLevel minLevel = LogLevel::Info;
        static inline bool timestamps = true;

        static const char *levelName(LogLevel level);
        static void write(LogLevel level, const std::string &message);

    public:
        static void SetMinLevel(LogLevel level);
        static void SetTimestamps(bool enabled);

        static void Trace(const std::string &message);
        static void Debug(const std::string &message);
        static void Info(const std::string &message);
        static void Warning(const std::string &message);
        static void Error(const std::string &message);
};

#endif
