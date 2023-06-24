#pragma once
#include <string>

#include "Event.h"

class KeyEvent : public Event
{
public:
    [[nodiscard]] int GetKeyCode() const {return m_KeyCode;}
    
protected:
    KeyEvent(int keyCode) : m_KeyCode(keyCode) {}
    int m_KeyCode;
};


class KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(int keyCode) : KeyEvent(keyCode) {}
    
    EVENT_CLASS_TYPE(KeyPressed)

    std::string GetName() override {return {"KeyPressedEvent - Key: " + std::to_string(m_KeyCode)};}
};


class KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}
    
    EVENT_CLASS_TYPE(KeyReleased)
    
    std::string GetName() override {return {"KeyReleasedEvent - Key: " + std::to_string(m_KeyCode)};}
};


class KeyRepeatedEvent : public KeyEvent
{
public:
    KeyRepeatedEvent(int keyCode) : KeyEvent(keyCode) {}
    
    EVENT_CLASS_TYPE(KeyRepeated)
    
    std::string GetName() override {return {"KeyRepeatedEvent - Key: " + std::to_string(m_KeyCode)};}
};
