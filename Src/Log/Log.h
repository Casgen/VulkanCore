#pragma once

#include <cstdint>
#include <csignal>
#include <fstream>
#include <stdexcept>

#ifdef _WIN32
#define DEBUGBREAK() DebugBreak()
#else
#define DEBUGBREAK() raise(SIGTRAP)
#endif

#define TRY_CATCH_BEGIN()                                                                                              \
    try                                                                                                                \
    {

#define TRY_CATCH_END()                                                                                                \
    }                                                                                                                  \
    catch (const vk::SystemError& err)                                                                                 \
    {                                                                                                                  \
        Logger::GetLogger()->Logger::Print(ECategory::SystemError, ESeverity::Error, err.what());                      \
        exit(-1);                                                                                                      \
    }                                                                                                                  \
    catch (std::exception & err)                                                                                       \
    {                                                                                                                  \
        Logger::GetLogger()->Logger::Print(ECategory::Exception, ESeverity::Error, err.what());                        \
        exit(-1);                                                                                                      \
    }                                                                                                                  \
    catch (...)                                                                                                        \
    {                                                                                                                  \
        Logger::GetLogger()->Logger::Print(ECategory::Unknown, ESeverity::Error, "Unknown error!");                    \
        exit(-1);                                                                                                      \
    }

enum class ESeverity : uint32_t
{
    Verbose = 0x00000001,
    Info = 0x00000010,
    Warning = 0x00000100,
    Error = 0x00001000,
    Fatal = 0x00010000,
};

enum class ECategory : uint8_t
{
    Window = 0x00,
    Vulkan = 0x01,
    Application = 0x02,
    Event = 0x03,
    Rendering = 0x04,
    Exception = 0x05,
    SystemError = 0x06,
    Unknown = 0x07,
    Shader = 0x08,
    Assimp = 0x09,
    Assert = 0x10,

    // Vulkan specific
    Validation = 0x0A,
    Performance = 0x0B,
    DeviceAddressBinding = 0x0C,
    General = 0x0D,
    Allocation = 0x0E
};

class Logger
{
  public:
    Logger(Logger& other) = delete;
    Logger(Logger&& other) = delete;
    ~Logger() = delete;

    void Printf(const ECategory& category, const ESeverity& severity, const char* format, ...);
    void Print(const ECategory& category, const ESeverity& severity, const char* message);

    void operator=(const Logger& other) = delete;
    void operator=(const Logger&& other) = delete;

    /**
     * @brief Sets a filter to only show logs from a certain severity level
     * @param severity - the level of severity. e.g. if you define ESeverity Error it will show logs with Error or
     * more severe ones.
     */
    static void SetSeverityFilter(const ESeverity& severity);

    /**
     * Obtains a Logger instance
     */
    static Logger* GetLogger();

  private:
    inline static ESeverity m_Filter = ESeverity::Info;

    Logger();

    FILE* outputLogFile;

    inline static Logger* m_Logger = nullptr;

    static const char* EvaluateSeverityString(ESeverity severity);
    static const char* EvaluateCategoryString(ECategory category);
};

#ifdef DEBUG
#define LOGF(Category, Severity, Format, ...)                                                                          \
    Logger::GetLogger()->Printf(ECategory::Category, ESeverity::Severity, Format, __VA_ARGS__);

#define LOG(Category, Severity, Format) Logger::GetLogger()->Print(ECategory::Category, ESeverity::Severity, Format);

#define LOG_AND_THROW(Category, Severity, Format)                                                                      \
    const char* message = Format;                                                                                      \
    Logger::GetLogger()->Print(ECategory::Category, ESeverity::Severity, message);                                     \
    throw std::runtime_error(message);
#else
#define LOGF(Category, Severity, Format, ...)
#define LOG(Category, Severity, Format)
#define LOG_AND_THROW(Category, Severity, Format)
#endif

#define ASSERT(Expression, Format)                                                                                     \
    if (!(Expression))                                                                                                 \
    {                                                                                                                  \
        Logger::GetLogger()->Print(ECategory::Assert, ESeverity::Fatal, Format);                                       \
        DEBUGBREAK();                                                                                                  \
    }

#define ASSERTF(Expression, Format, ...)                                                                               \
    if (!(Expression))                                                                                                 \
    {                                                                                                                  \
        Logger::GetLogger()->Printf(ECategory::Assert, ESeverity::Fatal, Format, __VA_ARGS__);                    \
        DEBUGBREAK();                                                                                                  \
    }
