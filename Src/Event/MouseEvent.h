#pragma once
#include "glm/vec2.hpp"
#include <string>

#include "Event.h"

class MouseButtonEvent : public Event {
public:
  [[nodiscard]] int GetKeyCode() const { return m_KeyCode; }

protected:
  MouseButtonEvent(int keyCode) : m_KeyCode(keyCode) {}

  std::string GetName() override {
    return {"MouseButtonEvent - Button: " + std::to_string(m_KeyCode)};
  }

  int m_KeyCode;
};

class MouseButtonPressedEvent : public MouseButtonEvent {
public:
  MouseButtonPressedEvent(int keyCode) : MouseButtonEvent(keyCode) {}

  EVENT_CLASS_TYPE(MouseBtnPressed)

  std::string GetName() override {
    return {"MouseButtonPressedEvent - Button: " + std::to_string(m_KeyCode)};
  }
};

class MouseButtonReleasedEvent : public MouseButtonEvent {
public:
  MouseButtonReleasedEvent(int keyCode) : MouseButtonEvent(keyCode) {}

  EVENT_CLASS_TYPE(MouseBtnReleased)

  std::string GetName() override {
    return {"MouseButtonReleasedEvent - Button: " + std::to_string(m_KeyCode)};
  }
};

class MouseButtonRepeatedEvent : public MouseButtonEvent {
public:
  MouseButtonRepeatedEvent(int keyCode) : MouseButtonEvent(keyCode) {}

  EVENT_CLASS_TYPE(MouseBtnRepeated)

  std::string GetName() override {
    return {"MouseButtonRepeatedEvent - Button: " + std::to_string(m_KeyCode)};
  }
};

class MouseMovedEvent : public Event {
public:
  MouseMovedEvent(const float &xPos, const float &yPos) : m_Pos(xPos, yPos) {}

  EVENT_CLASS_TYPE(MouseMoved)

  std::string GetName() override {
    return {"MouseMovec X: " + std::to_string(m_Pos.x) +
            " Y:" + std::to_string(m_Pos.y)};
  }

  [[nodiscard]] glm::ivec2 GetPos() const { return m_Pos; }

private:
  glm::ivec2 m_Pos = glm::ivec2(0);
};

class MouseScrolledEvent : public Event {
public:
  MouseScrolledEvent(int xOffset, int yOffset)
      : m_XOffset(xOffset), m_YOffset(yOffset) {}

  EVENT_CLASS_TYPE(MouseScrolled)

  std::string GetName() override {
    return {"MouseButtonScrolled X Offset: " + std::to_string(m_XOffset) +
            " Y Offset: " + std::to_string(m_YOffset)};
  }

  [[nodiscard]] int GetXOffset() const { return m_XOffset; }
  [[nodiscard]] int GetYOffset() const { return m_YOffset; }

private:
  int m_XOffset, m_YOffset = 0;
};
