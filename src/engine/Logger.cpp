#include "Logger.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>

const char *Logger::levelName(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        default: return "?";
    }
}

void Logger::write(LogLevel level, const std::string &message)
{
    if (static_cast<int>(level) < static_cast<int>(minLevel)) return;

    std::ostream &out = level >= LogLevel::Warning ? std::cerr : std::cout;

    if (timestamps)
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t t = std::chrono::system_clock::to_time_t(now);
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::tm local{};
#if defined(_WIN32)
        localtime_s(&local, &t);
#else
        localtime_r(&t, &local);
#endif

        out << std::put_time(&local, "%H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count() << ' ';
    }

    out << '[' << levelName(level) << "] " << message << std::endl;
}

void Logger::SetMinLevel(LogLevel level) { minLevel = level; }

void Logger::SetTimestamps(bool enabled) { timestamps = enabled; }

void Logger::Trace(const std::string &message) { write(LogLevel::Trace, message); }

void Logger::Debug(const std::string &message) { write(LogLevel::Debug, message); }

void Logger::Info(const std::string &message) { write(LogLevel::Info, message); }

void Logger::Warning(const std::string &message) { write(LogLevel::Warning, message); }

void Logger::Error(const std::string &message) { write(LogLevel::Error, message); }
