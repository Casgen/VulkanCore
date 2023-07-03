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
            severityString = "INFO"; break;
        case ESeverity::Verbose:
            severityString = "VERBOSE"; break;
        case ESeverity::Warning:
            severityString = "WARNING"; break;
        case ESeverity::Error:
            severityString = "ERROR"; break;
        default:
            severityString = "UNKNOWN";
        }

        switch (category)
        {
        case ECategory::Window:
            categoryString = "WINDOW"; break;
        case ECategory::Vulkan:
            categoryString = "VULKAN"; break;
        case ECategory::Application:
            categoryString = "APP"; break;
        case ECategory::Event:
            categoryString = "EVENT"; break;
        case ECategory::Rendering:
            categoryString = "RENDERING"; break;
        case ECategory::Exception:
            categoryString = "EXCEPTION"; break;
        default:
            categoryString = "UNKNOWN";
        }

        //auto timeStamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        printf("Log: [%s | %s] Description: %s", severityString.c_str(), categoryString.c_str(), message.c_str());
    }

    void Logger::SetSeverityFilter(const ESeverity &severity)
    {
        m_Filter = severity;
    }

} // namespace Log
