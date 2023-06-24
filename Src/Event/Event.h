#pragma once

#define EVENT_CLASS_TYPE(type)  static EventType GetStaticType() {return EventType::type;} \
                                [[nodiscard]] virtual EventType GetEventType() const override {return GetStaticType();}

#include <functional>
#include <string>

enum class EventType
{
    KeyPressed, KeyReleased, KeyRepeated,
    MouseBtnPressed, MouseBtnReleased, MouseBtnRepeated, MouseScrolled,
    MouseMoved, WindowClosed, WindowResized
};

class Event
{
    friend class EventDispatcher;
    
public:
    [[nodiscard]] virtual EventType GetEventType() const = 0;
    virtual std::string GetName() = 0;
    
protected:
    bool m_Handled = false;
};


class EventDispatcher
{
public:
    EventDispatcher(Event& event) : m_Event(event) {}

    template <typename T>
    bool Dispatch(std::function<bool(T&)> func) {

        if (m_Event.GetEventType() == T::GetStaticType())
        {
            m_Event.m_Handled = func(*static_cast<T*>(&m_Event));
            return true;
        }
        return false;
    }
    
private:
    Event& m_Event;
    
};
