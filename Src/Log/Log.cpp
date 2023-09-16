#include "Log.h"
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>
#include <string>

void Logger::Printf(const ECategory& category, const ESeverity& severity, const char* format, ...)
{
    if (severity < m_Filter)
        return;

    const char* severityString = EvaluateSeverityString(severity);
    const char* categoryString = EvaluateCategoryString(category);

    // auto timeStamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::cout << "Log: [" << severityString << " | " << categoryString << "] Description: ";

    va_list argptr;
    va_start(argptr, format);

    if (severity >= ESeverity::Error)
    {
        vfprintf(stderr, format, argptr);
    }
    else
    {
        vfprintf(stdout, format, argptr);
    }
    va_end(argptr);
    printf("\n");
}

void Logger::Print(const ECategory& category, const ESeverity& severity, const char* message)
{
    if (severity < m_Filter)
        return;

    const char* severityString = EvaluateSeverityString(severity);
    const char* categoryString = EvaluateCategoryString(category);

    // auto timeStamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    printf("Log: [%s | %s] Description: %s\n", severityString, categoryString, message);
}

void Logger::SetSeverityFilter(const ESeverity& severity)
{
    m_Filter = severity;
}

const char* Logger::EvaluateSeverityString(ESeverity severity)
{

    switch (severity)
    {
    case ESeverity::Info:
        return "INFO";
        break;
    case ESeverity::Verbose:
        return "VERBOSE";
        break;
    case ESeverity::Warning:
        return "WARNING";
        break;
    case ESeverity::Error:
        return "ERROR";
        break;
    case ESeverity::Fatal:
        return "FATAL";
        break;
    default:
        return "UNKNOWN";
    }
}

const char* Logger::EvaluateCategoryString(ECategory category)
{
    switch (category)
    {
    case ECategory::Window:
        return "WINDOW";
        break;
    case ECategory::Vulkan:
        return "VULKAN";
        break;
    case ECategory::Application:
        return "APP";
        break;
    case ECategory::Event:
        return "EVENT";
        break;
    case ECategory::Rendering:
        return "RENDERING";
        break;
    case ECategory::Exception:
        return "EXCEPTION";
        break;
    case ECategory::SystemError:
        return "SYSTEM ERROR";
        break;
    case ECategory::Unknown:
        return "UNKNOWN";
        break;
    case ECategory::Shader:
        return "SHADER";
        break;

    // Vulkan Specific
    case ECategory::Validation:
        return "VALIDATION";
        break;
    case ECategory::General:
        return "GENERAL";
        break;
    case ECategory::DeviceAddressBinding:
        return "DEVICE ADDRESS BINDING";
        break;
    case ECategory::Performance:
        return "PERFORMANCE";
        break;
    case ECategory::Allocation:
        return "ALLOCATION";
        break;
    default:
        return "UNCATEGORIZED";
    }
}
