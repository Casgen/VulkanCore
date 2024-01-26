#include "Log.h"
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <string>

Logger::Logger() {
    outputLogFile = fopen("Log.txt", "a+");
    
    fprintf(outputLogFile, "Logger Initialized: \n\n");
    fflush(outputLogFile);
}

void Logger::Printf(const ECategory& category, const ESeverity& severity, const char* format, ...)
{
    if (severity < m_Filter)
        return;

    const char* severityString = EvaluateSeverityString(severity);
    const char* categoryString = EvaluateCategoryString(category);

    // auto timeStamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    printf("Log: [%s | %s] Description: ", severityString, categoryString);
    fprintf(outputLogFile,"Log: [%s | %s] Description: ", severityString, categoryString);



    va_list argptr;
    va_start(argptr, format);

    // the va_arg pointer has to be copied, because in the first vprintf function it invalidates the pointer.
    va_list argptrFile;
    va_copy(argptrFile, argptr);

    if (severity >= ESeverity::Error)
    {
        vfprintf(stderr, format, argptr);
        vfprintf(outputLogFile, format, argptrFile);
    }
    else
    {
        vfprintf(stdout, format, argptr);
        vfprintf(outputLogFile, format, argptrFile);
    }

    fprintf(outputLogFile, "\n");
    fflush(outputLogFile);

    printf("\n");


    va_end(argptr);
    va_end(argptrFile);

}

void Logger::Print(const ECategory& category, const ESeverity& severity, const char* message)
{
    if (severity < m_Filter)
        return;

    const char* severityString = EvaluateSeverityString(severity);
    const char* categoryString = EvaluateCategoryString(category);

    printf("Log: [%s | %s] Description: %s\n", severityString, categoryString, message);
    fprintf(outputLogFile,"Log: [%s | %s] Description: %s\n", severityString, categoryString, message);
    fflush(outputLogFile);
}

void Logger::SetSeverityFilter(const ESeverity& severity)
{
    m_Filter = severity;
}
Logger* Logger::GetLogger() {

    if (m_Logger == nullptr) {
        m_Logger = new Logger();
    }

    return m_Logger;

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
