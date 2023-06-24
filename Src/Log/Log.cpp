#include "Log.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <mutex>
#include <string>

namespace Log
{
    void Logger::Log(const ECategory &category, const ESeverity &severity, std::string message)
    {
        if (severity < m_Filter)
            return;

        std::string severityString, categoryString;

        switch (severity)
        {
        case ESeverity::Info:
            severityString = "[INFO]";
        case ESeverity::Verbose:
            severityString = "[VERBOSE]";
        case ESeverity::Warning:
            severityString = "[WARNING]";
        case ESeverity::Error:
            severityString = "[ERROR]";
        default:
            severityString = "[UNKNOWN]";
        }

        switch (category)
        {
        case ECategory::Window:
            categoryString = "[WINDOW]";
        case ECategory::Vulkan:
            categoryString = "[VULKAN]";
        case ECategory::Application:
            categoryString = "[APP]";
        case ECategory::Event:
            categoryString = "[EVENT]";
        case ECategory::Rendering:
            categoryString = "[RENDERING]";
        default:
            categoryString = "[UNKNOWN]";
        }

        auto timeStamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        printf("LOG - %s | %s | [Time: %s] - Description: %s", severityString.c_str(), categoryString.c_str(),
               ctime(&timeStamp), message.c_str());
    }

    void Logger::SetSeverityFilter(const ESeverity &severity)
    {
        m_Filter = severity;
    }

} // namespace Log
