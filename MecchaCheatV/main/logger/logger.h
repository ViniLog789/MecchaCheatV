#pragma once
#include <mutex>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <cstdio>
#include <memory>
#pragma warning(disable: 4005)
#define LOG_INFO(...)
#define LOG_ERROR(...)
#include "Includes.h"
using namespace MecchaCheatV::Globals;

namespace MecchaCheatV
{
    class Logger
    {
    public:
        enum class Level : uint8_t
        {
            Call,
            Debug,
            Info,
            Warning,
            Error,
            Hooks,
            UInfo,
            UWarning,
            UError,
            RPC,
            Release
        };
        explicit Logger(Level minLevel = Level::Call);
        ~Logger();
        void ShutdownConsole();

        template<typename... Args>
        void Log(const Level level, Args&&... args)
        {
            std::string message = FormatMessage(std::forward<Args>(args)...);
            ActualLog(level, message);
        }

        void ActualLog(Level level, std::string_view message);
        void LogRelease(WORD color, std::string_view message);

        template<typename... Args>
        static std::string FormatMessage(const char* format, Args&&... args)
        {
            if (format == nullptr)
                return "<null format>";

            std::string formatStr(format);

            if (ContainsPrintfFormat(formatStr))
                return PrintfFormat(format, std::forward<Args>(args)...);
            else
                return ConcatenateFormat(format, std::forward<Args>(args)...);
        }

    private:
        const Level MinLevel;
        bool ConsoleExists;
        HANDLE HConsole;
        std::mutex LogMutex;
        std::ofstream FileOut;
        std::string LogFilePath;
        static constexpr std::string_view LevelToString(Level level);
        static constexpr WORD LevelToColor(Level level);
        static std::string GetTimestamp();
        bool InitializeLogDirectory();
        FILE* StdoutFile;
        FILE* StderrFile;

        static bool ContainsPrintfFormat(const std::string& format);

        template<typename T>
        static std::string ConvertToString(T&& arg)
        {
            std::ostringstream ss;
            ss << std::forward<T>(arg);
            return ss.str();
        }

        template<typename T>
        static std::string FormatMessage(T&& arg)
        {
            if constexpr (std::is_pointer_v<std::decay_t<T>>)
            {
                if (arg == nullptr)
                    return "<nullptr>";
            }
            return ConvertToString(std::forward<T>(arg));
        }

        template<typename... Args>
        static std::string PrintfFormat(const char* format, Args&&... args)
        {
            if (format == nullptr)
                return "<null format>";

            int size = std::snprintf(nullptr, 0, format, std::forward<Args>(args)...);
            if (size < 0)
                return std::string("[snprintf error] ") + (format ? format : "<null>");

            std::string result(size + 1, '\0');
            std::snprintf(&result[0], size + 1, format, std::forward<Args>(args)...);
            result.pop_back();
            return result;
        }

        template<typename First, typename... Rest>
        static std::string ConcatenateFormat(First&& first, Rest&&... rest)
        {
            std::ostringstream ss;
            ss << std::forward<First>(first);

            if constexpr (sizeof...(rest) > 0)
                ((ss << " " << std::forward<Rest>(rest)), ...);

            return ss.str();
        }
    };

    extern Logger* logger;
}

#define LOG_CALL(...) \
    if (MecchaCheatV::logger && IsCalledLogs) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::Call, __VA_ARGS__)
#define LOG_DEBUG(...) \
    if (MecchaCheatV::logger && IsDebugging && IsCalledLogs) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::Debug, __VA_ARGS__)
#define LOG_INFO(...) \
    if (MecchaCheatV::logger) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::Info, __VA_ARGS__)
#define LOG_HOOKS(...) \
    if (MecchaCheatV::logger && IsDebugging) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::Hooks, __VA_ARGS__)
#define LOG_WARN(...) \
    if (MecchaCheatV::logger) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::Warning, __VA_ARGS__)
#define LOG_ERROR(...) \
    if (MecchaCheatV::logger) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::Error, __VA_ARGS__)
#define LOG_CALL_UPDATE(...) \
    if (MecchaCheatV::logger && IsDebugging && IsUpdateCalledLogs) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::Call, __VA_ARGS__)
#define LOG_RPC(...) \
    if (MecchaCheatV::logger && IsDebugging && IsRPCLogs) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::RPC, __VA_ARGS__)
#define LOG_RELEASE(color, ...) \
    if (MecchaCheatV::logger) \
        MecchaCheatV::logger->LogRelease(color, MecchaCheatV::Logger::FormatMessage(__VA_ARGS__))

#define LOG_UNITY(...) \
    if (MecchaCheatV::logger && IsDebugging) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::UInfo, __VA_ARGS__)
#define LOG_UNITY_WARN(...) \
    if (MecchaCheatV::logger && IsDebugging) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::UWarning, __VA_ARGS__)
#define LOG_UNITY_ERROR(...) \
    if (MecchaCheatV::logger) \
        MecchaCheatV::logger->Log(MecchaCheatV::Logger::Level::UError, __VA_ARGS__)