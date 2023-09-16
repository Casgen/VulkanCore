#pragma once
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>

struct MouseState
{
    constexpr static glm::dvec2 zeroPos = glm::dvec2(0.0);
    constexpr static double epsilon = 1e-4;

    bool m_IsMMBPressed : 1, m_IsRMBPressed : 1, m_IsLMBPressed : 1, m_IsDragging : 1;
    glm::ivec2 m_LastPosition, m_LastClickPosition = glm::ivec2(0);

    MouseState() : m_IsMMBPressed(false), m_IsRMBPressed(false), m_IsLMBPressed(false), m_IsDragging(false)
    {
    }

    void Update()
    {
        m_IsDragging = false;

        glm::dvec2 currPosition(0.0), lastPositionDouble(m_LastPosition);
        glfwGetCursorPos(glfwGetCurrentContext(), &currPosition.x, &currPosition.y);

        const glm::vec<2, bool> isDifferent = glm::notEqual(currPosition, lastPositionDouble);

        m_IsDragging = (isDifferent.x || isDifferent.y) && m_IsLMBPressed;

        m_LastPosition = currPosition;
    }
};
