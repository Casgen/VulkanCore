#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>

enum class ESeverity : uint32_t
{
    Verbose = 0x0000001,
    Info = 0x00000010,
    Warning = 0x00000100,
    Error = 0x000001000,
    Fatal = 0x00010000,
};

enum class ECategory : uint8_t
{
    Window = 0x0,
    Vulkan = 0x1,
    Application = 0x2,
    Event = 0x3,
    Rendering = 0x4,
    Exception = 0x5,

    // Vulkan specific
    Validation = 0x6,
    Performance = 0x7,
    DeviceAddressBinding = 0x8,
    General = 0x9
};

class Logger
{
  public:

    Logger(Logger& other) = delete;
    ~Logger() = delete;

    static void Printf(const ECategory& category, const ESeverity& severity, const char* format, ...);
    static void Print(const ECategory& category, const ESeverity& severity, const char* message);

    void operator=(const Logger& other) = delete;

    /**
     * @brief Sets a filter to only show logs from a certain severity level
     * @param severity - the level of severity. e.g. if you define ESeverity Error it will show logs with Error or
     * more severe ones.
     */
    static void SetSeverityFilter(const ESeverity& severity);

  private:
    inline static ESeverity m_Filter = ESeverity::Info;

    static const char* EvaluateSeverityString(ESeverity severity);
    static const char* EvaluateCategoryString(ECategory category);
};

#ifdef DEBUG
#define LOGF(Category, Severity, Format, ...) Logger::Printf(ECategory::Category, ESeverity::Severity, Format, __VA_ARGS__);
#define LOG(Category, Severity, Format) Logger::Print(ECategory::Category, ESeverity::Severity, Format);
#else
#define LOGF(Category, Severity, Format, ...)
#define LOG(Category, Severity, Format)
#endif
