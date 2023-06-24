#pragma once
#include <string>

#include "Event.h"

class WindowResizedEvent : public Event
{
public:
    WindowResizedEvent(const unsigned int& width, const unsigned int height)
        : m_Width(width), m_Height(height)
    {
    }

    EVENT_CLASS_TYPE(WindowResized)

    std::string GetName() override
    {
        return {"WindowResizedEvent X: " + std::to_string(m_Width) + "Y: " + std::to_string(m_Height)};
    }

    [[nodiscard]] inline unsigned int GetWidth() const { return m_Width; }
    [[nodiscard]] inline unsigned int GetHeight() const { return m_Height; }

private:
    unsigned int m_Width, m_Height;
};

class WindowClosedEvent : public Event
{
public:
    WindowClosedEvent() = default;

    std::string GetName() override { return {"WindowClosedEvent"}; }

    EVENT_CLASS_TYPE(WindowClosed)
};
