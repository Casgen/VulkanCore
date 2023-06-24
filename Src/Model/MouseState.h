#pragma once
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>

struct MouseState
{
    constexpr static glm::dvec2 zeroPos = glm::dvec2(0.0);
    constexpr static double epsilon = 1e-4;
    
    bool isMMBPressed:1 = false, isRMBPressed:1 = false, isLMBPressed:1 = false, isDragging = false;
    glm::ivec2 lastPosition, lastClickPosition = glm::ivec2(0);

    void Update()
    {
        isDragging = false;
        
        glm::dvec2 currPosition(0.0), lastPositionDouble(lastPosition);
        glfwGetCursorPos(glfwGetCurrentContext(), &currPosition.x, &currPosition.y);

        const glm::vec<2, bool> isDifferent = glm::notEqual(currPosition, lastPositionDouble);

        isDragging = (isDifferent.x || isDifferent.y) && isLMBPressed;

        lastPosition = currPosition;
        
    }
};
