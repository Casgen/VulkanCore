#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>

namespace Log
{
    enum class ESeverity : uint8_t
    {
        Verbose = 0x0,
        Info = 0x1,
        Warning = 0x2,
        Error = 0x3,
        Fatal = 0x4,
    };

    enum class ECategory : uint8_t
    {
        Window = 0x0,
        Vulkan = 0x1,
        Application = 0x2,
        Event = 0x3,
        Rendering = 0x4,
        Exception = 0x5,
    };

    class Logger
    {

      public:
        Logger(Logger &other) = delete;
        ~Logger() = delete;

        static void Log(const ECategory &category, const ESeverity &severity, std::string message);

        void operator=(const Logger &other) = delete;

        /**
         * @brief Sets a filter to only show logs from a certain severity level
         * @param severity - the level of severity. e.g. if you define ESeverity Error it will show logs with Error or
         * more severe ones.
         */
        static void SetSeverityFilter(const ESeverity &severity);


      private:
        inline static ESeverity m_Filter = ESeverity::Info;
    };

} // namespace Log


#ifdef DEBUG
#define LOG(Category, Severity, Message) Log::Logger::Log(Log::ECategory::Category, Log::ESeverity::Severity, Message);
#else
#define LOG(Category, Severity, Message)
#endif
